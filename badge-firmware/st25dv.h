// ============================================================
// st25dv.h — minimal ST25DV (I2C NFC EEPROM) driver
// Target: ST25DV04K (512 bytes user memory, 4-byte blocks)
// Must be called after Wire.begin() + Wire.setClock(400000).
//
// Protocol details follow the ST25DV datasheet + ST's own
// (stm32duino/ST25DV) driver + SparkFun ST25DV64KC library:
//   * user memory     @ 7-bit I2C address 0x53
//   * system/registers @ 7-bit I2C address 0x57
//   * memory / register pointers are 2 bytes, big-endian:
//       START | 0xA6/0xAE | addr_hi | addr_lo | data... | STOP
//   * I2C password register = 0x0900 (system address).
//     Session-open data = <pwd (reversed order)> 0x09 <pwd>,
//     17 bytes total. Default factory password = 8x 0x00.
//   * Dynamic register I2C_SSO_DYN = 0x2004, bit0 = session open.
// ============================================================

#pragma once
#include <Arduino.h>
#include <Wire.h>

#define ST25DV_I2C_DATA 0x53
#define ST25DV_I2C_SYS  0x57
#define ST25DV_MEM_SIZE 512

#define ST25DV_REG_I2C_PWD    0x0900
#define ST25DV_REG_I2C_SSO_DYN 0x2004

// ---- low level ---------------------------------------------------

static bool _st25dv_read(uint8_t devAddr, uint16_t mem, uint8_t *dst, uint16_t len) {
  if (len == 0) return true;
  Wire.beginTransmission(devAddr);
  Wire.write((uint8_t)(mem >> 8));
  Wire.write((uint8_t)(mem & 0xFF));
  if (Wire.endTransmission(false) != 0) return false;

  uint16_t done = 0;
  while (done < len) {
    uint8_t chunk = len - done;
    if (chunk > 32) chunk = 32;              // stay inside Wire's RX buffer
    uint8_t got = Wire.requestFrom(devAddr, chunk);
    if (got != chunk) return false;          // avoids ambiguity, exact match only
    for (uint8_t i = 0; i < chunk; i++) dst[done++] = Wire.read();
  }
  return true;
}

static bool _st25dv_write(uint8_t devAddr, uint16_t mem, const uint8_t *src, uint16_t len) {
  if (len == 0) return true;
  uint16_t done = 0;
  while (done < len) {
    uint8_t chunk = len - done;
    if (chunk > 16) chunk = 16;              // ST25DV supports page write; keep small
    Wire.beginTransmission(devAddr);
    uint16_t a = mem + done;
    Wire.write((uint8_t)(a >> 8));
    Wire.write((uint8_t)(a & 0xFF));
    for (uint8_t i = 0; i < chunk; i++) Wire.write(src[done + i]);
    if (Wire.endTransmission() != 0) return false;
    done += chunk;
    delay(5);                                 // EEPROM tWDP write-cycle time
  }
  return true;
}

// ---- public API --------------------------------------------------
// Read `len` bytes from user memory at `mem` (0x0000..0x01FF for the 4K part).
inline bool st25dv_read(uint16_t mem, uint8_t *dst, uint16_t len) {
  return _st25dv_read(ST25DV_I2C_DATA, mem, dst, len);
}

// Write `len` bytes at user-memory address `mem`.
inline bool st25dv_write(uint16_t mem, const uint8_t *src, uint16_t len) {
  return _st25dv_write(ST25DV_I2C_DATA, mem, src, len);
}

// Quick heart-beat: read the 4-byte CC block.
inline bool st25dv_ping() {
  uint8_t cc[4];
  if (!st25dv_read(0x0000, cc, 4)) return false;
  return true; // any read success is enough of a heartbeat
}

// Present the 8-byte password to open the I2C security session.
// Factory default = all zeros, in which case byte order doesn't matter.
inline bool st25dv_openSession(const uint8_t pwd[8]) {
  uint8_t rev[8];
  for (uint8_t i = 0; i < 8; i++) rev[i] = pwd[7 - i]; // driver-style reversed order
  Wire.beginTransmission(ST25DV_I2C_SYS);
  Wire.write((uint8_t)(ST25DV_REG_I2C_PWD >> 8));
  Wire.write((uint8_t)(ST25DV_REG_I2C_PWD & 0xFF));
  for (uint8_t i = 0; i < 8; i++) Wire.write(rev[i]);
  Wire.write(0x09);            // verification marker
  for (uint8_t i = 0; i < 8; i++) Wire.write(rev[i]);
  return Wire.endTransmission() == 0;
}

// Check the session-open flag by reading I2C_SSO_DYN (bit0).
inline bool st25dv_sessionOpen() {
  uint8_t s = 0;
  if (!_st25dv_read(ST25DV_I2C_SYS, ST25DV_REG_I2C_SSO_DYN, &s, 1)) return false;
  return (s & 0x01) != 0;
}
