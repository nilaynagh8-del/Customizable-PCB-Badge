# Customizable PCB-Badge
A customizable PCB badge with built in NFC & games!
<img width="1255" height="852" alt="image" src="https://github.com/user-attachments/assets/d85c4293-f76b-41fe-8fee-80f73b4314b0" />
# Features
- LED Matrix for fun games like flappy bird or pong
- E-ink display module for displaying ID information
- NFC tag for haring info like websites, etc.
- where the NFC tag directs you to can be changed on you're computer through a USB-C connection
- Fun copper and silkscreen art
- 16 broken out GPIO pins
- 16MB of flash storage for the rp2040
- power only needed when playing games or changing the image on the E-ink display
---
# Designing the PCB
the PCB is pretty compact and tightly packed together with many components, which forced me to make this PCB 6 layers. I used 2 GND planes, one on the top layer and another on the bottom. For the NFC antenna I used a plugin to generate me one with copper wire routing to make it part of the PCB itself so it doesn't require an external antenna. here are some photos of the design below.

_Main MCU Components_
<img width="1091" height="771" alt="image" src="https://github.com/user-attachments/assets/9556927a-44c7-4b97-bffe-3ce0f246568f" />


_LEDs and buttons for games_
<img width="1011" height="714" alt="image" src="https://github.com/user-attachments/assets/ffa58b05-dafc-45c6-bdb4-92c2cd5b878e" />


_E-ink Display_
<img width="1222" height="866" alt="image" src="https://github.com/user-attachments/assets/b04b0e05-d9b7-4dee-ae6e-33678ee295d6" />

_NFC tag components_
<img width="1223" height="865" alt="image" src="https://github.com/user-attachments/assets/e288bcb1-6aea-4548-b873-41ec2c29e3b5" />

_LED for fun lightin on the front_
<img width="1221" height="866" alt="image" src="https://github.com/user-attachments/assets/de1a3ed0-e789-4145-90bc-0c3042287f8a" />

_Final PCB Design_
<img width="866" height="492" alt="image" src="https://github.com/user-attachments/assets/18a55a25-3d09-4926-ae46-372828000735" />

_PCB front and back_
<img width="1148" height="808" alt="image" src="https://github.com/user-attachments/assets/f63fc9c6-ac1c-489e-8c58-c314ccd64b7e" />

<img width="1103" height="769" alt="image" src="https://github.com/user-attachments/assets/fe409e7e-8956-4667-8cc8-ee81ebbda3b0" />
---
#Cus


# Software


---
# BOM
|#Part       |#Price          |
|------------|----------------|
| **1x rp2040**     | PIANO: C3 (48) · MIRROR: C3 · DRUMS: C3     |
| **1x W25Q128JVS** | PIANO: D#3 (51) · MIRROR: F3 · DRUMS: F3    |
| **1x USB-C 2.0 receptacle**    | PIANO: C3 (48) · MIRROR: C3 · DRUMS: C3     |
| **50x SK6812MINI led**    | PIANO: D#3 (51) · MIRROR: F3 · DRUMS: F3    |
| **1x ST25DV04K-IER8C3**    | PIANO: C3 (48) · MIRROR: C3 · DRUMS: C3    |
| **6x 5mm THT LED**    | PIANO: D#3 (51) · MIRROR: F3 · DRUMS: F3    |
| **3x PTS645Sx43SMTR92 switches**    | PIANO: C3 (48) · MIRROR: C3 · DRUMS: C3    |
| **1x KMR2 switch**    | PIANO: D#3 (51) · MIRROR: F3 · DRUMS: F3    |
| **14x 0402 0.1uf capacitors**    | PIANO: C3 (48) · MIRROR: C3 · DRUMS: C3    |
| **3x 0603 10uf capacitors**    | PIANO: D#3 (51) · MIRROR: F3 · DRUMS: F3    |
| **1x NCP1117ST33T3G**    | PIANO: C3 (48) · MIRROR: C3 · DRUMS: C3    |
| **2x 5.1k 0402 resistors**    | PIANO: D#3 (51) · MIRROR: F3 · DRUMS: F3    |
| **2x 27 ohm 0402 resistors**    | PIANO: C3 (48) · MIRROR: C3 · DRUMS: C3    |
| **2x 1k  0402 resistors**    | PIANO: D#3 (51) · MIRROR: F3 · DRUMS: F3    |
| **1x 10k  0402 resistors**    | PIANO: C3 (48) · MIRROR: C3 · DRUMS: C3    |
| **2x 4.7k  0402 resistors**    | PIANO: D#3 (51) · MIRROR: F3 · DRUMS: F3    |
| **3x PTS645Sx43SMTR92 switches**    | PIANO: C3 (48) · MIRROR: C3 · DRUMS: C3    |
| **1x KMR2 switch**    | PIANO: D#3 (51) · MIRROR: F3 · DRUMS: F3    |
| **14x 0402 0.1uf capacitors**    | PIANO: C3 (48) · MIRROR: C3 · DRUMS: C3    |
| **3x 0603 10uf capacitors**    | PIANO: D#3 (51) · MIRROR: F3 · DRUMS: F3    |
