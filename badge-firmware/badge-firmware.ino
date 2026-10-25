// ============================================================================
//  Custom PCB Badge firmware  —  RP2040-QFN-56 + W25Q128 + 12MHz XIN + USB-C
//
//  Hardware map (extracted from the KiCad netlist):
//    * 50x SK6812MINI (on B.Cu) in a 10 x 5 grid, data chain starts at GPIO20
//      (the chain's last pixel DOUT loops back to GPIO21 — unused here)
//    * SW3 (game button A)  -> GPIO10  (pressed = LOW, INPUT_PULLUP)
//    * SW4 (game button B)  -> GPIO11  (pressed = LOW, INPUT_PULLUP)
//    * SW2 (THT LED toggle) -> GPIO12  (pressed = LOW, INPUT_PULLUP)
//    * D1..D10 THT LED row via shared 70R    -> GPIO13 (HIGH = all 10 ON)
//    * SW1 = BOOTSEL button — handled by the chip ROM, never used by code
//    * ST25DV04K NFC: SDA = GPIO28, SCL = GPIO29 (external 4k7 pullups)
//
//  Modes (cycle with long-press SW4, 0.8 s):
//    0. Flappy  — SW3 to flap, SW4 to restart
//    1. Pong    — two players, SW3 = paddle left, SW4 = paddle right
//    2. NFC     — flash one of 3 NDEF payloads (URL / text / vCard) to the tag
//    3. Demo    — rainbow
//
//  SW2 toggles the 10 THT LEDs at any time, in any mode.
//
//  Arduino IDE -> Tools -> Board = "Generic RP2040" (arduino-pico core)
//    Flash Size: 16MB, Crystal Frequency: 12 MHz, USB Stack: Pico SDK
//  Requires the Adafruit NeoPixel library (Library Manager).
// ============================================================================

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include "st25dv.h"
#include "badge-types.h"

// ---------------------------------------------------------------------------
// EDIT THESE — NFC payloads flashed to the tag in NFC mode
// ---------------------------------------------------------------------------
static const char *NFC_PAYLOAD_URL   = "https://hackclub.com";
static const char *NFC_PAYLOAD_TEXT  = "Hello to whoever taps this badge!";
static const char *NFC_PAYLOAD_VCARD =
    "BEGIN:VCARD\nVERSION:3.0\nFN:YOUR NAME\nTITLE:Hardware tinkerer\nEND:VCARD";

// ---------------------------------------------------------------------------
// Pins / constants
// ---------------------------------------------------------------------------
#define PIN_BTN_A     10         // SW3 — main action button
#define PIN_BTN_B     11         // SW4 — secondary action + mode switch
#define PIN_BTN_THT   12         // SW2 — toggle the THT LED row
#define PIN_THT      13          // drives D1..D10 THT anodes (HIGH = on)
#define PIN_PIXELS   20          // SK6812MINI chain data in
#define PIN_SDA      28          // ST25DV NFC
#define PIN_SCL      29

#define NUM_PIXELS   50
#define GRID_ROWS    5
#define GRID_COLS    10
#define BRIGHTNESS   48          // keeps full-grid animation under ~160 mA

#define BTN_DEBOUNCE_MS 25
#define BTN_LONG_MS     800

// If the badge looks left/right mirrored when you hold it, set this to 1.
#define GRID_MIRROR  0

// ---------------------------------------------------------------------------
// Chain index of every (row, col) of the 10x5 pixel patch,
// derived from the schematic data-out chain of D49..D98.
// ---------------------------------------------------------------------------
static const uint8_t GRID[GRID_ROWS][GRID_COLS] = {
  { 0,  1,  2, 26, 38, 14,  8, 20, 32, 44},   // row 0 (top, D49..D58)
  { 3, 27, 39, 15,  9, 21, 33, 45,  4, 28},
  {40, 16, 10, 22, 34, 46,  5, 29, 41, 17},
  {11, 23, 35, 47,  6, 30, 42, 18, 12, 24},
  {36, 48,  7, 31, 43, 19, 13, 25, 37, 49},   // row 4 (bottom)
};

// ---------------------------------------------------------------------------
// Pixels
// ---------------------------------------------------------------------------
static Adafruit_NeoPixel strip(NUM_PIXELS, PIN_PIXELS, NEO_GRB + NEO_KHZ800);

static inline uint8_t chainIndex(uint8_t r, uint8_t c) {
#if GRID_MIRROR
  c = GRID_COLS - 1 - c;
#endif
  return GRID[r][c];
}

static inline void setPix(int8_t r, int8_t c, uint32_t color) {
  if (r < 0 || r >= GRID_ROWS || c < 0 || c >= GRID_COLS) return;
  strip.setPixelColor(chainIndex(r, c), color);
}

static void clearGrid() { strip.clear(); }

static void fillAll(uint32_t color) {
  for (uint16_t i = 0; i < NUM_PIXELS; i++) strip.setPixelColor(i, color);
}

static void showFill(uint32_t color) { fillAll(color); strip.show(); }

static inline uint32_t C(uint8_t r, uint8_t g, uint8_t b) { return strip.Color(r, g, b); }
#define COL_RED    C(255,   0,   0)
#define COL_GREEN  C(  0, 255,   0)
#define COL_BLUE   C(  0,  60, 255)
#define COL_CYAN   C(  0, 255, 255)
#define COL_YELLOW C(255, 220,   0)
#define COL_WHITE  C(200, 200, 200)
#define COL_ORANGE C(255, 120,   0)
#define COL_PURPLE C(180,   0, 255)

// Brief blocking color sweep — used on startup and mode change only.
static void wipe(uint32_t color, uint16_t stepMs = 6) {
  clearGrid();
  for (uint8_t r = 0; r < GRID_ROWS; r++)
    for (uint8_t c = 0; c < GRID_COLS; c++) {
      setPix(r, c, color);
      strip.show();
      delay(stepMs);
    }
}

// ---------------------------------------------------------------------------
// Buttons — press / long-press edge events, polled every loop()
// ---------------------------------------------------------------------------
static BtnState sBtnA, sBtnB, sBtnT;

// per-tick one-shot events
static bool evAPress, evARelease, evALong;
static bool evBPress, evBRelease, evBLong;
static bool evTPress;

static void btnUpdate(BtnState &b, bool &press, bool &release, bool &longEv) {
  bool now = (digitalRead(b.pin) == LOW);   // pressed = LOW
  uint32_t t = millis();

  if (now != b.debounced && (t - b.lastChange) >= BTN_DEBOUNCE_MS) {
    b.debounced = now;
    b.lastChange = t;
    if (now) { b.pressStart = t; b.longFired = false; press = true; }
    else     { release = true; }
  } else if (now && !b.longFired && (t - b.pressStart) >= BTN_LONG_MS) {
    b.longFired = true;
    longEv = true;
  }
}

static void pollButtons() {
  btnUpdate(sBtnA, evAPress, evARelease, evALong);
  btnUpdate(sBtnB, evBPress, evBRelease, evBLong);
  bool rel, lng;   // unused
  btnUpdate(sBtnT, evTPress, rel, lng);
  evARelease = evBRelease = false;  // consumed implicitly (no callers yet)
}

// ---------------------------------------------------------------------------
// Modes
// ---------------------------------------------------------------------------
static Mode mode = M_FLAPPY;
static void enterMode(Mode m);

// =======================  FLAPPY  ==========================================
static const uint8_t BIRD_COL = 1;
static const uint8_t GAP_ROWS = 2;

static Pipe pipes[3];
static uint32_t flappyLast = 0;
static uint16_t flappyTickMs = 170;
static uint8_t flappyCountdown = 2;
static int8_t birdRow = 2;
static uint8_t flappyScore = 0;
static bool flappyDead = false;
static uint32_t dieT = 0;

static void flappyReset() {
  birdRow = 2; flappyScore = 0; flappyTickMs = 170; flappyDead = false;
  flappyCountdown = 2;
  for (uint8_t i = 0; i < 3; i++) { pipes[i].col = -1; pipes[i].gapTop = 0; pipes[i].scored = false; }
}

static void flappySpawn() {
  for (uint8_t i = 0; i < 3; i++)
    if (pipes[i].col == -1) {
      pipes[i].col = GRID_COLS;                // off-screen right, scrolls in
      pipes[i].gapTop = random(0, GRID_ROWS - GAP_ROWS + 1);
      pipes[i].scored = false;
      return;
    }
}

static void renderFlappy() {
  clearGrid();
  for (uint8_t i = 0; i < 3; i++) {
    Pipe &p = pipes[i];
    if (p.col < 0 || p.col >= GRID_COLS) continue;
    for (uint8_t r = 0; r < GRID_ROWS; r++)
      if (r < p.gapTop || r >= p.gapTop + GAP_ROWS) setPix(r, p.col, COL_RED);
  }
  if (!flappyDead && birdRow >= 0) setPix(birdRow, BIRD_COL, COL_WHITE);
  strip.show();
}

static void flappyDie() {
  flappyDead = true;
  dieT = millis();
}

static void flappyTick() {
  birdRow++;                                    // gravity
  if (birdRow >= GRID_ROWS) { flappyDie(); renderFlappy(); return; }

  for (uint8_t i = 0; i < 3; i++) if (pipes[i].col >= -1) pipes[i].col--;

  flappyCountdown--;
  if (flappyCountdown == 0) {
    flappySpawn();
    flappyCountdown = 3 + random(3);
  }

  for (uint8_t i = 0; i < 3; i++) {
    Pipe &p = pipes[i];
    if (p.col != BIRD_COL) continue;
    if (birdRow < (int8_t)p.gapTop || birdRow >= (int8_t)(p.gapTop + GAP_ROWS)) flappyDie();
    else if (!p.scored) {
      p.scored = true;
      flappyScore++;
      if (flappyTickMs > 110) flappyTickMs -= 2;
    }
  }
  renderFlappy();
}

static void showFlappyScore() {
  // score as a green bar on the middle row, 1 pixel per point (10 max)
  clearGrid();
  uint8_t n = flappyScore > 10 ? 10 : flappyScore;
  for (uint8_t c = 0; c < n; c++) setPix(2, c, COL_GREEN);
  setPix(2, GRID_COLS - 1, COL_WHITE);          // "restart" hint pixel
  strip.show();
}

static void loopFlappy() {
  if (!flappyDead) {
    if (evBPress) { flappyReset(); renderFlappy(); return; }  // restart
    if (evAPress && birdRow > 0) birdRow--;                   // flap
    if (millis() - flappyLast >= flappyTickMs) {
      flappyLast = millis();
      flappyTick();
    }
    return;
  }

  // dead: red blink, then score bar; any button restarts
  if (millis() - dieT < 900) {
    if ((millis() - dieT) % 300 < 150) showFill(COL_RED);
    else { clearGrid(); strip.show(); }
  } else {
    showFlappyScore();
    if (evAPress || evBPress || evTPress) flappyReset();
  }
}

static void enterFlappy() { flappyReset(); }

// =======================  PONG  ============================================
static uint8_t padA = 1, padB = 1;             // top row of each 2-row paddle
static int8_t bx = 4, by = 2, bvx = 1, bvy = 1;
static uint8_t pScoreA = 0, pScoreB = 0;
static uint16_t pongTickMs = 150;
static uint32_t pongLast = 0;
static bool pointPause = false;
static uint32_t pointT = 0;
static bool pongOver = false;

static void pongServe() {
  bx = 4;
  by = random(1, GRID_ROWS - 1);
  bvx = (random(2) == 0) ? 1 : -1;
  bvy = (random(2) == 0) ? 1 : -1;
}

static void pongReset() {
  padA = padB = 1; pScoreA = pScoreB = 0; pongTickMs = 150;
  pointPause = false; pongOver = false;
  pongServe();
}

static void renderScore() {
  clearGrid();
  for (uint8_t c = 0; c < pScoreA && c < 4; c++) setPix(2, c, COL_GREEN);
  for (uint8_t i = 0; i < pScoreB && i < 4; i++) setPix(2, GRID_COLS - 1 - i, COL_CYAN);
  setPix(2, 4, COL_YELLOW);
  setPix(2, 5, COL_YELLOW);
  strip.show();
}

static void renderPong() {
  clearGrid();
  setPix(padA, 0, COL_GREEN);
  setPix(padA + 1, 0, COL_GREEN);
  setPix(padB, GRID_COLS - 1, COL_CYAN);
  setPix(padB + 1, GRID_COLS - 1, COL_CYAN);
  setPix(by, bx, COL_WHITE);
  strip.show();
}

static void pongTick() {
  bx += bvx;
  by += bvy;

  if (by < 0) { by = 0; bvy = 1; }
  if (by > GRID_ROWS - 1) { by = GRID_ROWS - 1; bvy = -1; }

  // left wall -> paddle A
  if (bx <= 0) {
    if (by == (int8_t)padA || by == (int8_t)(padA + 1)) {
      bvx = 1;
      bx = 0;
      if (pongTickMs > 110) pongTickMs -= 2;
    } else {
      pScoreB++;
      pointT = millis(); pointPause = true;
      if (pScoreB >= 5) { showFill(COL_CYAN); delay(600); pongOver = true; return; }
      renderScore();
      bvx = 1;
      return;
    }
  }
  // right wall -> paddle B
  else if (bx >= GRID_COLS - 1) {
    if (by == (int8_t)padB || by == (int8_t)(padB + 1)) {
      bvx = -1;
      bx = GRID_COLS - 1;
      if (pongTickMs > 110) pongTickMs -= 2;
    } else {
      pScoreA++;
      pointT = millis(); pointPause = true;
      if (pScoreA >= 5) { showFill(COL_GREEN); delay(600); pongOver = true; return; }
      renderScore();
      bvx = -1;
      return;
    }
  }
  renderPong();
}

static void loopPong() {
  if (evAPress) {
    padA = (padA == 0) ? GRID_ROWS - 2 : padA - 1;   // wrap at top
    if (!pointPause && !pongOver) renderPong();
  }
  if (evBPress) {
    padB = (padB == 0) ? GRID_ROWS - 2 : padB - 1;
    if (!pointPause && !pongOver) renderPong();
  }

  if (pongOver) {
    // winner color pulses; any button resets the match
    if ((millis() / 250) % 2 == 0) showFill(pScoreA >= 5 ? COL_GREEN : COL_CYAN);
    else { clearGrid(); strip.show(); }
    if (evAPress || evBPress) pongReset();
    return;
  }

  if (pointPause) {
    if (millis() - pointT > 900) { pointPause = false; pongServe(); renderPong(); }
  } else if (millis() - pongLast >= pongTickMs) {
    pongLast = millis();
    pongTick();
  }
}

static void enterPong() { pongReset(); }

// =======================  NFC  NDEF builder  ==============================
// ST25DV user memory, Type-5 TLV layout (per ST AN4911 / NDEF spec):
//   0x0000: CC          = E1 40 40 00
//   0x0004: NDEF TLV    = 03 <len> <message bytes> FE   (len <= 254)
// One single-record message:
//   URI   : D1 01 <plen> 55 <uriCode> <uri tail>
//   TEXT  : D1 01 <plen> 54 02 'e' 'n' <text>
//   vCard : D2 <typeLen> <plen> "text/vcard" <payload>

// ============== enum NfcKind moved to top of file ==========================
static uint8_t pickUriPrefix(const char *url, const char **tail) {
  static const struct { const char *p; uint8_t code; } tbl[] = {
    {"https://www.", 0x02},
    {"http://www.",  0x01},
    {"https://",     0x04},
    {"http://",      0x03},
    {"tel:",         0x05},
    {"mailto:",      0x06},
  };
  for (const auto &e : tbl)
    if (strncmp(url, e.p, strlen(e.p)) == 0) { *tail = url + strlen(e.p); return e.code; }
  *tail = url;
  return 0x00;
}

// Fills a 260-byte frame (CC + TLV + terminator + zero fill) to burn at 0x0000.
static uint16_t buildNdefFrame(NfcKind kind, uint8_t *dst) {
  uint8_t msg[240];
  uint16_t mlen = 0;
  uint16_t plen;

  if (kind == NFC_URL) {
    const char *tail;
    uint8_t code = pickUriPrefix(NFC_PAYLOAD_URL, &tail);
    size_t tlen = strlen(tail);
    if (tlen > 200) tlen = 200;                 // keep inside the msg buffer
    plen = 1 + tlen;
    msg[mlen++] = 0xD1; msg[mlen++] = 0x01; msg[mlen++] = (uint8_t)plen;
    msg[mlen++] = 'U';  msg[mlen++] = code;
    memcpy(&msg[mlen], tail, tlen);
    mlen += tlen;
  } else if (kind == NFC_TEXT) {
    size_t tlen = strlen(NFC_PAYLOAD_TEXT);
    if (tlen > 200) tlen = 200;
    plen = 1 + 2 + tlen;        // status byte + "en" + text = text payload
    msg[mlen++] = 0xD1; msg[mlen++] = 0x01; msg[mlen++] = (uint8_t)plen;
    msg[mlen++] = 'T';  msg[mlen++] = 0x02;
    msg[mlen++] = 'e';  msg[mlen++] = 'n';
    memcpy(&msg[mlen], NFC_PAYLOAD_TEXT, tlen);
    mlen += tlen;
  } else {                      // NFC_VCARD — media-type record (TNF 2)
    const char *ctype = "text/vcard";
    size_t clen = strlen(ctype);
    size_t vlen = strlen(NFC_PAYLOAD_VCARD);
    if (vlen > 200) vlen = 200;
    plen = vlen;
    msg[mlen++] = 0xD2; msg[mlen++] = (uint8_t)clen; msg[mlen++] = (uint8_t)plen;
    memcpy(&msg[mlen], ctype, clen);
    mlen += clen;
    memcpy(&msg[mlen], NFC_PAYLOAD_VCARD, vlen);
    mlen += vlen;
  }

  if (mlen > 254) mlen = 254;   // TLV short-form length limit

  memset(dst, 0, 260);
  dst[0] = 0xE1; dst[1] = 0x40; dst[2] = 0x40; dst[3] = 0x00;  // Type-5 CC
  dst[4] = 0x03;                // NDEF message TLV
  dst[5] = (uint8_t)mlen;
  memcpy(&dst[6], msg, mlen);
  dst[6 + mlen] = 0xFE;         // TLV terminator
  return 6 + mlen + 1;
}

// =======================  NFC WRITER  ======================================
// NDEF payloads are CYCLED with SW3, WRITTEN with SW4.
static bool nfcTagOK = false;
static NfcKind nfcIdx = NFC_URL;

static void nfcRender() {
  clearGrid();
  uint32_t frameCol = nfcTagOK ? COL_GREEN : COL_RED;   // green = tag detected
  for (uint8_t c = 0; c < GRID_COLS; c++) { setPix(0, c, frameCol); setPix(GRID_ROWS - 1, c, frameCol); }
  for (uint8_t r = 1; r < GRID_ROWS - 1; r++) { setPix(r, 0, frameCol); setPix(r, GRID_COLS - 1, frameCol); }

  // selection: 1 / 2 / 3 center pixels — cyan, yellow, purple
  static const uint32_t kSelCol[3] = {COL_CYAN, COL_YELLOW, COL_PURPLE};
  for (uint8_t i = 0; i < (uint8_t)nfcIdx + 1; i++) setPix(2, 3 + i, kSelCol[(uint8_t)nfcIdx]);
  strip.show();
}

static void enterNfc() {
  nfcIdx = NFC_URL;
  nfcTagOK = st25dv_ping();                     // heartbeat check on the i2c bus
  nfcRender();
}

static void nfcWriteSelected() {
  uint8_t frame[260];
  uint16_t n = buildNdefFrame(nfcIdx, frame);

  showFill(COL_ORANGE);                         // orange = writing...

  bool ok = false;
  uint8_t check[260];
  for (int attempt = 0; attempt < 2 && !ok; attempt++) {
    if (st25dv_write(0x0000, frame, n)) {
      memset(check, 0, sizeof(check));
      ok = st25dv_read(0x0000, check, n) && (memcmp(check, frame, n) == 0);
    }
    if (!ok) {                                   // retry after opening a session
      static const uint8_t zeros[8] = {0};
      st25dv_openSession(zeros);
    }
  }
  nfcTagOK = st25dv_ping();

  // green sweep on success, red blink on failure
  if (ok) {
    clearGrid();
    for (uint8_t r = 0; r < GRID_ROWS; r++)
      for (uint8_t c = 0; c < GRID_COLS; c++) { setPix(r, c, COL_GREEN); strip.show(); delay(8); }
  } else {
    for (int i = 0; i < 4; i++) {
      showFill(COL_RED);
      delay(150);
      clearGrid(); strip.show(); delay(150);
    }
  }
  nfcRender();
}

static void loopNfc() {
  if (evAPress) {
    nfcIdx = (NfcKind)((nfcIdx + 1) % 3);
    nfcRender();
  }
  if (evBPress) nfcWriteSelected();
}

// =======================  DEMO  ============================================
static uint32_t demoLast = 0;
static uint16_t demoPhase = 0;

static void loopDemo() {
  if (millis() - demoLast < 40) return;
  demoLast = millis();
  demoPhase += 3;
  for (uint8_t r = 0; r < GRID_ROWS; r++)
    for (uint8_t c = 0; c < GRID_COLS; c++)
      strip.setPixelColor(chainIndex(r, c),
                          strip.ColorHSV(demoPhase + (r + c) * 2600, 255, 255));
  strip.show();
}

static void enterDemo() { demoPhase = 0; demoLast = 0; }

// =======================  mode switch  =====================================
static void enterMode(Mode m) {
  mode = m;
  wipe(COL_BLUE, 3);
  switch (m) {
    case M_FLAPPY: enterFlappy(); break;
    case M_PONG:   enterPong();   break;
    case M_NFC:    enterNfc();    break;
    case M_DEMO:   enterDemo();   break;
    default: break;
  }
  // first frame of the new mode
  if (m == M_FLAPPY) renderFlappy();
  else if (m == M_PONG) renderPong();
  else if (m == M_NFC) nfcRender();
  // M_DEMO draws itself on the next loop iteration
}

// ---------------------------------------------------------------------------
void setup() {
  // buttons pull their GPIO to GND when pressed -> internal pullups
  pinMode(PIN_BTN_A,   INPUT_PULLUP);
  pinMode(PIN_BTN_B,   INPUT_PULLUP);
  pinMode(PIN_BTN_THT, INPUT_PULLUP);
  sBtnA.pin = PIN_BTN_A;
  sBtnB.pin = PIN_BTN_B;
  sBtnT.pin = PIN_BTN_THT;

  pinMode(PIN_THT, OUTPUT);
  digitalWrite(PIN_THT, LOW);                    // THT LEDs start off

  Wire.setSDA(PIN_SDA);
  Wire.setSCL(PIN_SCL);
  Wire.begin();
  Wire.setClock(400000UL);

  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  clearGrid();
  strip.show();

  wipe(COL_GREEN);
  randomSeed(micros());                          // boot-time noise

  enterMode(M_FLAPPY);
}

static bool thtOn = false;                       // SW2 toggle state

void loop() {
  pollButtons();

  // SW2 always toggles the THT LED row, whatever the mode.
  if (evTPress) {
    thtOn = !thtOn;
    digitalWrite(PIN_THT, thtOn ? HIGH : LOW);
  }

  if (evBLong) {
    enterMode((Mode)((mode + 1) % N_MODES));
  } else {
    switch (mode) {
      case M_FLAPPY: loopFlappy(); break;
      case M_PONG:   loopPong();   break;
      case M_NFC:    loopNfc();    break;
      case M_DEMO:   loopDemo();   break;
      default: break;
    }
  }

  // events are one-shot per loop tick
  evAPress = evALong = false;
  evBPress = evBLong = false;
  evTPress = false;
}
