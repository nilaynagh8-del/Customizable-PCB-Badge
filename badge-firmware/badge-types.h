// badge-types.h — type declarations shared by badge-firmware.ino.
// They live in a header so Arduino's generated function prototypes
// always see them before they are used.
#pragma once

enum Mode : uint8_t { M_FLAPPY, M_PONG, M_NFC, M_DEMO, N_MODES };

enum NfcKind : uint8_t { NFC_URL = 0, NFC_TEXT = 1, NFC_VCARD = 2 };

struct BtnState {
  uint8_t pin;
  bool debounced = false;
  uint32_t lastChange = 0;
  uint32_t pressStart = 0;
  bool longFired = false;
};

struct Pipe {
  int8_t col;
  uint8_t gapTop;
  bool scored;
};
