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
# How to Use
| Mode (long-press SW4 to cycle) | What happens |
|---|---|
| 1. **Flappy** | Flappy-bird on the LED grid. SW3 = flap, SW4 = restart. Speeds up as you score. |
| 2. **Pong** | 2-player pong, first to 5. SW3 = left paddle, SW4 = right paddle. |
| 3. **NFC** | Pick one of 3 payloads with SW3 (URL / text / vCard), write it to the ST25DV tag with SW4. Green frame = tag present, red = not detected. Green sweep = write verified OK. |
| 4. **Demo** | Rainbow across the whole grid. |

SW2 toggles the 6 THT LEDs at any time, in any mode – it's wired straight
to GPIO13.

The NFC tag behaves like a normal Type-5 NDEF tag: after a write, tapping it to
a phone opens the URL, shows the text, or offers the contact card.

**Customizing the NFC payloads**

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

_LED for fun lighting on the front_
<img width="1221" height="866" alt="image" src="https://github.com/user-attachments/assets/de1a3ed0-e789-4145-90bc-0c3042287f8a" />

_Final PCB Design_

<img width="866" height="492" alt="image" src="https://github.com/user-attachments/assets/18a55a25-3d09-4926-ae46-372828000735" />


_PCB front and back_
<img width="1148" height="808" alt="image" src="https://github.com/user-attachments/assets/f63fc9c6-ac1c-489e-8c58-c314ccd64b7e" />

<img width="1103" height="769" alt="image" src="https://github.com/user-attachments/assets/fe409e7e-8956-4667-8cc8-ee81ebbda3b0" />



# Software
The software of this device is pretty simple all you have to do is hold the BOOTSEL button on the board and drop the the [Badge-firmware.ino.uf2](https://github.com/nilaynagh8-del/Customizable-PCB-Badge/blob/main/badge-uf2/badge-firmware.ino.uf2) onto the device. You will then be able to upload you're ID and play games etc.

---
# BOM
|#Part       |#Price          |
|------------|----------------|
| **1x [rp2040](https://www.digikey.com/en/products/detail/raspberry-pi/SC0914(13)/14306010)**     | $0.76     |
| **1x [W25Q128JVS](https://www.digikey.com/en/products/detail/winbond-electronics/W25Q128JVSIQ/5803943)** | $5.99   |
| **1x [USB-C 2.0 receptacle](https://www.digikey.com/en/products/detail/adam-tech/USB-C20-S-RA-CS16A-BK-TR/25701126?gclsrc=aw.ds&gad_source=1&gad_campaignid=20678579980&gbraid=0AAAAADrbLlhqkQkBmy0GhcLsBGkQxSWwA&gclid=Cj0KCQjwrvHWBhDKARIsABmp7h3_4yny3H3xYeCTKnwiGQ5gFquwQlYKCZFCwDuY89yn7uL39px1PEEaAn94EALw_wcB)**    | $0.74  |
| **50x[ SK6812MINI LEDS](https://keeb.io/products/sk-6812-mini-e-rgb-leds-12-pack?srsltid=AYCX9vJHpHxhBnKpgdmcaYFWbcuSSWfHne0xMPR1KBHh8PzNO1_xWtn2)**    | $11.98    |
| **1x [ST25DV04K-IER8C3](https://www.digikey.com/en/products/detail/stmicroelectronics/ST25DV04K-IER6T3/6691418?gclsrc=aw.ds&gad_source=1&gad_campaignid=24216345527&gbraid=0AAAAADrbLli4CTTxePQYnJ5C6afwPKjL2&gclid=Cj0KCQjwrvHWBhDKARIsABmp7h1KdPLxHsMZuuaStQSl7R8vMIvJZ-5TIm2zWNzuZK7JSieiJ8zBqwcaAgxCEALw_wcB)**    | $1.10  |
| **6x [5mm THT LED](https://www.digikey.com/en/products/detail/rohm-semiconductor/SLR-56VC3F/636991?gclsrc=aw.ds&gad_source=1&gad_campaignid=20747733577&gbraid=0AAAAADrbLlibUioa_7C1IycEGCDz0hbTg&gclid=Cj0KCQjwrvHWBhDKARIsABmp7h10cfeflSGWLhXNBAp4XJWPd0hMV-nY00NDLkXkriuKhSCe2A7cvd8aAgY_EALw_wcB)**    | $3.30 |
| **3x [PTS645Sx43SMTR92 Switches](https://www.digikey.com/en/products/detail/wec/PTS645SK43SMTR92LFS/27533114?gclsrc=aw.ds&gad_source=4&gad_campaignid=23033451739&gbraid=0AAAAADrbLlh-8VarSu1z8cuQMuamoVxbl&gclid=Cj0KCQjwrvHWBhDKARIsABmp7h1Eo3e9FToyXaLy61q9kBlwUjuhkKtIDu0mgIWiZOV7CFqwBuzqiGIaAisyEALw_wcB)**    | $1.08|
| **1x [KMR2 switch](https://www.digikey.com/en/products/detail/c-k/KMR221G-LFS/550472?gclsrc=aw.ds&gad_source=1&gad_campaignid=20243136172&gbraid=0AAAAADrbLlj0U90TogABbZu38P156Ls3i&gclid=Cj0KCQjwrvHWBhDKARIsABmp7h1JYl6i5VjO5_h80ofNfqxeWJDCjkMDnMpPE0zijLod7d3QZ9AqAecaAiA4EALw_wcB)**    | $0.67 |
| **14x[ 0402 0.1uf capacitors](https://www.digikey.com/en/products/detail/murata-electronics/GRM155C81E104KA12D/6605993?gclsrc=aw.ds&gad_source=4&gad_campaignid=20509818233&gbraid=0AAAAADrbLlis9pHS9r0m2ZhlQP6IInvQA&gclid=Cj0KCQjwrvHWBhDKARIsABmp7h07aUFT4WDWhllQ61V_xpzCK5QXNSUuiS_rN4leZ1_dai8NjVI8rQsaAgjsEALw_wcB)**    | $1.40    |
| **3x [0603 10uf capacitors](https://www.digikey.com/en/products/detail/murata-electronics/GRT188R60J106ME13J/13904886?gclsrc=aw.ds&gad_source=1&gad_campaignid=20509818233&gbraid=0AAAAADrbLlhW4hUYZwqaRXxXOI1TvvnLC&gclid=Cj0KCQjwrvHWBhDKARIsABmp7h1m43mU9iI4nKG-fnOtnU_kfMY8bhoRzj5BhlG1TujyhKJdcvszKPgaAnMzEALw_wcB)**    | $0.63 |
| **4x [0402 15pf  capacitors](https://www.digikey.com/en/products/detail/yageo/CC0402FRNPO9BN150/5195051)**    | $0.52    |
| **1x [NCP1117ST33T3G](https://www.digikey.com/en/products/detail/onsemi/NCP1117ST33T3G/1749063)**    | $0.65    |
| **2x [5.1k 0402 resistors](https://www.digikey.com/en/products/detail/yageo/RC0402FR-075K1L/726624)**    | $0.20    |
| **2x [27 ohm 0402 resistors](https://www.digikey.com/en/products/detail/yageo/RC0402FR-0727RL/5280946)**    | $0.20    |
| **2x [1k  0402 resistors](https://www.digikey.com/en/products/detail/yageo/RC0402FR-071KL/726513)**    | $0.20    |
| **1x [10k  0402 resistors](https://www.digikey.com/en/products/detail/vishay-dale/RCA040210K0FKED/4946751)**    | $0.11 |
| **2x [4.7k  0402 resistors](https://www.digikey.com/en/products/detail/yageo/RC0402JR-074K7L/726477)**    | $0.10 |
| **1x [470 ohm  0402 resistors](https://www.digikey.com/en/products/detail/yageo/RC0402FR-07470RP/4935284)**    | $0.11 |
| **1x [E-ink Display]([https://www.digikey.com/en/products/detail/yageo/RC0402FR-07470RP/4935284](https://www.amazon.com/2-13inch-Display-HAT-Two-Color-Raspberry/dp/B07Z1WYRQH/ref=sr_1_11?dib=eyJ2IjoiMSJ9.gn6VLudgU0EyGmCtn6ouiN-En84h8hGdmjGuK1-66Xws_QcTkqImKZ7qe8NUiArwhwJk9JiOutgNeIz5cPmB0ZbkMG4yFjYDLpp9DKV0toxfU2H8bp0QUR760J-i1QKpJPyA7FF0PKqdo7Qt2GiVY_zAQcLqV49bpBDsulBlm3EtA--w29KZabezZMWkaSrQsFDFI53NmnQb6Gi7EZZHmbqM53_gEwxUkAk3kj63XW4.dg_95URBlxNDf7plGtIUSDxYDGLFnQ1UMweHCZRH_n8&dib_tag=se&keywords=e-ink+display&qid=1792900939&sr=8-11))**    | $23.03 |
| **Total Price of Parts (excluding PCB)**  | $52.77 |

# Disclaimers & Credits
This project is open source. If anything you make out of this does not work or has any issues, I sincerely apologize, but at the end of the day that's on you. Big shout out to Hack Club for giving me the opportunity to make this project. I would also like to thank **Kai Pereira**, as my project is heavily inspired his PCB badge in many ways.

