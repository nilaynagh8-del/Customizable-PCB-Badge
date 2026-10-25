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
| **1x [rp2040](https://www.digikey.com/en/products/detail/raspberry-pi/SC0914(13)/14306010)**     | PIANO: C3 (48) · MIRROR: C3 · DRUMS: C3     |
| **1x [W25Q128JVS](https://www.digikey.com/en/products/detail/winbond-electronics/W25Q128JVSIQ/5803943)** | PIANO: D#3 (51) · MIRROR: F3 · DRUMS: F3    |
| **1x [USB-C 2.0 receptacle](https://www.digikey.com/en/products/detail/adam-tech/USB-C20-S-RA-CS16A-BK-TR/25701126?gclsrc=aw.ds&gad_source=1&gad_campaignid=20678579980&gbraid=0AAAAADrbLlhqkQkBmy0GhcLsBGkQxSWwA&gclid=Cj0KCQjwrvHWBhDKARIsABmp7h3_4yny3H3xYeCTKnwiGQ5gFquwQlYKCZFCwDuY89yn7uL39px1PEEaAn94EALw_wcB)**    | PIANO: C3 (48) · MIRROR: C3 · DRUMS: C3     |
| **50x[ SK6812MINI LEDS](https://keeb.io/products/sk-6812-mini-e-rgb-leds-12-pack?srsltid=AYCX9vJHpHxhBnKpgdmcaYFWbcuSSWfHne0xMPR1KBHh8PzNO1_xWtn2)**    | PIANO: D#3 (51) · MIRROR: F3 · DRUMS: F3    |
| **1x [ST25DV04K-IER8C3](https://www.digikey.com/en/products/detail/stmicroelectronics/ST25DV04K-IER6T3/6691418?gclsrc=aw.ds&gad_source=1&gad_campaignid=24216345527&gbraid=0AAAAADrbLli4CTTxePQYnJ5C6afwPKjL2&gclid=Cj0KCQjwrvHWBhDKARIsABmp7h1KdPLxHsMZuuaStQSl7R8vMIvJZ-5TIm2zWNzuZK7JSieiJ8zBqwcaAgxCEALw_wcB)**    | PIANO: C3 (48) · MIRROR: C3 · DRUMS: C3    |
| **6x [5mm THT LED](https://www.digikey.com/en/products/detail/rohm-semiconductor/SLR-56VC3F/636991?gclsrc=aw.ds&gad_source=1&gad_campaignid=20747733577&gbraid=0AAAAADrbLlibUioa_7C1IycEGCDz0hbTg&gclid=Cj0KCQjwrvHWBhDKARIsABmp7h10cfeflSGWLhXNBAp4XJWPd0hMV-nY00NDLkXkriuKhSCe2A7cvd8aAgY_EALw_wcB)**    | PIANO: D#3 (51) · MIRROR: F3 · DRUMS: F3    |
| **3x [PTS645Sx43SMTR92 Switches](https://www.digikey.com/en/products/detail/wec/PTS645SK43SMTR92LFS/27533114?gclsrc=aw.ds&gad_source=4&gad_campaignid=23033451739&gbraid=0AAAAADrbLlh-8VarSu1z8cuQMuamoVxbl&gclid=Cj0KCQjwrvHWBhDKARIsABmp7h1Eo3e9FToyXaLy61q9kBlwUjuhkKtIDu0mgIWiZOV7CFqwBuzqiGIaAisyEALw_wcB)**    | PIANO: C3 (48) · MIRROR: C3 · DRUMS: C3    |
| **1x [KMR2 switch](https://www.digikey.com/en/products/detail/c-k/KMR221G-LFS/550472?gclsrc=aw.ds&gad_source=1&gad_campaignid=20243136172&gbraid=0AAAAADrbLlj0U90TogABbZu38P156Ls3i&gclid=Cj0KCQjwrvHWBhDKARIsABmp7h1JYl6i5VjO5_h80ofNfqxeWJDCjkMDnMpPE0zijLod7d3QZ9AqAecaAiA4EALw_wcB)**    | PIANO: D#3 (51) · MIRROR: F3 · DRUMS: F3    |
| **14x [0402 0.1uf capacitors]([url](https://www.digikey.com/en/products/detail/murata-electronics/GRM155C81E104KA12D/6605993?gclsrc=aw.ds&gad_source=4&gad_campaignid=20509818233&gbraid=0AAAAADrbLlis9pHS9r0m2ZhlQP6IInvQA&gclid=Cj0KCQjwrvHWBhDKARIsABmp7h07aUFT4WDWhllQ61V_xpzCK5QXNSUuiS_rN4leZ1_dai8NjVI8rQsaAgjsEALw_wcB))**    | PIANO: C3 (48) · MIRROR: C3 · DRUMS: C3    |
| **3x [0603 10uf capacitors](https://www.digikey.com/en/products/detail/murata-electronics/GRT188R60J106ME13J/13904886?gclsrc=aw.ds&gad_source=1&gad_campaignid=20509818233&gbraid=0AAAAADrbLlhW4hUYZwqaRXxXOI1TvvnLC&gclid=Cj0KCQjwrvHWBhDKARIsABmp7h1m43mU9iI4nKG-fnOtnU_kfMY8bhoRzj5BhlG1TujyhKJdcvszKPgaAnMzEALw_wcB)**    | PIANO: D#3 (51) · MIRROR: F3 · DRUMS: F3    |
| **4x [0402 15pf  capacitor](https://www.digikey.com/en/products/detail/yageo/CC0402FRNPO9BN150/5195051)s**    | PIANO: C3 (48) · MIRROR: C3 · DRUMS: C3    |
| **1x [NCP1117ST33T3G](https://www.digikey.com/en/products/detail/onsemi/NCP1117ST33T3G/1749063)**    | PIANO: C3 (48) · MIRROR: C3 · DRUMS: C3    |
| **2x [5.1k 0402 resistors](https://www.digikey.com/en/products/detail/yageo/RC0402FR-075K1L/726624)**    | PIANO: D#3 (51) · MIRROR: F3 · DRUMS: F3    |
| **2x [27 ohm 0402 resistors](https://www.digikey.com/en/products/detail/yageo/RC0402FR-0727RL/5280946)**    | PIANO: C3 (48) · MIRROR: C3 · DRUMS: C3    |
| **2x [1k  0402 resistors](https://www.digikey.com/en/products/detail/yageo/RC0402FR-071KL/726513)**    | PIANO: D#3 (51) · MIRROR: F3 · DRUMS: F3    |
| **1x [10k  0402 resistors](https://www.digikey.com/en/products/detail/vishay-dale/RCA040210K0FKED/4946751)**    | |
| **2x [4.7k  0402 resistors](https://www.digikey.com/en/products/detail/yageo/RC0402JR-074K7L/726477)**    | |
| **1x [70 ohm  0402 resistors](https://www.digikey.com/en/products/detail/yageo/RC0402FR-07470RP/4935284)**    |  |

