# Custom PCB Badge firmware

Firmware for the RP2040 badge in this KiCad project – a 10×5 grid of SK6812MINI
RGB pixels, a row of 10 THT LEDs, 3 buttons, and an ST25DV04K NFC tag that can
be rewritten over I2C.

Inspired by Overglade-Badges, but written to match **this board's exact
netlist** (the LED chain order here is different, and there is no e-ink).

## What it does

| Mode (long-press SW4 to cycle) | What happens |
|---|---|
| 0. **Flappy** | Flappy-bird on the LED grid. SW3 = flap, SW4 = restart. Speeds up as you score. |
| 1. **Pong** | 2-player pong, first to 5. SW3 = left paddle, SW4 = right paddle. |
| 2. **NFC** | Pick one of 3 payloads with SW3 (URL / text / vCard), write it to the ST25DV tag with SW4. Green frame = tag present, red = not detected. Green sweep = write verified OK. |
| 3. **Demo** | Rainbow across the whole grid. |

**SW2** toggles the 10 THT LEDs at any time, in any mode – it's wired straight
to GPIO13, independent of the games. **SW1 = BOOTSEL** (reset/upload button,
handled by the RP2040 ROM, never touched by the firmware).

The NFC tag behaves like a normal Type-5 NDEF tag: after a write, tapping it to
a phone opens the URL, shows the text, or offers the contact card.

## The easy way — drag & drop (no Arduino IDE)

A ready-to-flash build is included in the parent folder:

**`../badge-firmware.uf2`**

1. Hold **BOOTSEL** (SW1) and plug in the USB-C cable → a drive called
   `RPI-RP2` appears.
2. Drag `badge-firmware.uf2` onto that drive.
3. The badge reboots into Flappy. Done.

To reflash later, replug USB while holding BOOTSEL and drop the file again.

The .uf2 was built with arduino-cli (RP2040 core 6.3.1, Adafruit NeoPixel
1.15.5) for Generic RP2040: 16 MB flash (no FS), W25Q boot stage, 12 MHz
crystal, 200 MHz CPU, Pico SDK USB stack. On this machine you can rebuild it
with:

```bash
./arduino-cli.exe compile --fqbn \
  "rp2040:rp2040:generic:flash=16777216_0,boot2=boot2_w25q080_2_padded_checksum" \
  badge-firmware --output-dir badge-uf2
```

## Manual route (Arduino IDE)

1. Install the **arduino-pico** core (Boards Manager → "Raspberry Pi RP2040
   boards" by Earle Philhower) and the **Adafruit NeoPixel** library.
2. Board: **Generic RP2040**, and in the board menu:
   - Flash Size: 16MB (W25Q128 SPI flash)
   - Crystal Frequency: **12 MHz**
   - USB Stack: Pico SDK
3. Hold BOOTSEL (SW1), plug in USB, release → RPI-RP2 drive appears.
4. Upload `badge-firmware.ino` (must sit in a folder named `badge-firmware`,
   already the case here).

## Pin map (from the KiCad netlist)

| Signal | GPIO | Notes |
|---|---|---|
| SK6812 data in | 20 | chain of 50; last pixel's DOUT loops back to GPIO21 (unused) |
| SW3 (button A) | 10 | to GND, internal pull-up |
| SW4 (button B) | 11 | to GND, internal pull-up |
| SW2 (THT toggle) | 12 | to GND, internal pull-up |
| THT LED row | 13 | HIGH = all 10 THT LEDs on (shared current-limit resistor) |
| ST25DV SDA | 28 | external 4k7 pull-ups on the board |
| ST25DV SCL | 29 | |

LED chain order → grid position is encoded in the `GRID[5][10]` table, traced
from the schematic data chain D49 → D50 → … → D98. If the badge looks mirrored
when you hold it, set `GRID_MIRROR` to 1 and re-upload.

## Customizing the NFC payloads

Edit the three strings near the top of `badge-firmware.ino`:

```cpp
static const char *NFC_PAYLOAD_URL   = "https://hackclub.com";
static const char *NFC_PAYLOAD_TEXT  = "Hello to whoever taps this badge!";
static const char *NFC_PAYLOAD_VCARD = "BEGIN:VCARD\nVERSION:3.0\nFN:YOUR NAME...";
```

URI prefixes are auto-detected (`https://`, `http://www.`, `tel:`, `mailto:` …)
and encoded compactly per the NDEF spec. Writes are verified by read-back; the
password session re-opens between attempts if the first write fails. Firmware
size per payload is capped at 254 bytes (TLV short form) but the payload
themselves are clamped to 200 bytes each — plenty for a URL, a message, and a
contact card on the 512-byte ST25DV04K.

## Files

- `badge-firmware.ino` — all modes, buttons, LED grid, THT row, mode switching
- `badge-types.h` — enums/structs (keeps Arduino prototype generation happy)
- `st25dv.h` — minimal self-contained ST25DV driver (read / write / ping /
  password session); no external NFC library needed, works over plain `Wire`

## Notes

- BRIGHTNESS is set to 48 so a full-grid animation stays comfortably within a
  USB-C power budget (~160 mA). Raise it if you power from something bigger.
- `st25dv.h` was written against the ST25DV datasheet and ST's own driver:
  data EEPROM at I2C 0x53, system registers at 0x57, 2-byte big-endian
  register pointers, password register at 0x0900, session flag at I2C_SSO_DYN.
