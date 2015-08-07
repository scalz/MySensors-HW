# RollerShutterNode SMD version


It is a board for controlling rollershutter (0-100%) and detecting end stops.. It have two relays (UP and DOWN) and ACS712 for current sense of motor. 
You can connect 3 push buttons. This board can be used as dual relay board too, with current sense. It can control 12-220V motors.


Note : it is still in dev.


- Atmel328p TQFP
- NRF24 smd footprint
- ATSHA204A authentication footprint
- eeprom footprint for OTA
- ACS712 footprint
- 2x 5Amp relays : OMRON G6D-1A-ASI protected by TVS diode & small capacitor
- ac dc stepdown 220V-5V : HILINK_HLK-PM01 protected by varistor & ptc fuse
- 5mm pitch screw terminals
- I2C connector for optional tactile sensor chip
- 47mm x 48mm

Arduino Pin description :
- A0 : ADC ACS712
- A3 : ATSHA204A data
- D6 : Down Relay
- D7 : Up relay
- I2C connector : SDA, SCL, D3, D4

DC connectors : 
- SPI for programming Atmel328p
- FTDI for bootloading
- A1 : Up button
- A2 : Down button
- D5 : Stop button

AC Connector description :
- N : Neutral (Neutre)
- L : Live (Phase)
- 12_220 : optional. If using 220v motor, you need to tie it with Source on Motor Connector


Motor Connector description :
- Up : Up relay for motor
- Down : Down relay for motor
- Source : 12-24v motor source. If using 220v motor, tie it with 12_220 on AC Connector





In progress : 
- sketch for rollershutter (buttons, calibration functions, controlling 0-100%). 
- add Bill of Materials
- add gerbers for Seeed, Itead, and OSH fabhouses.


OPEN SOURCE


Copyright Scalz (2015). [Attribution-ShareAlike CC BY-SA](https://creativecommons.org/licenses/)



