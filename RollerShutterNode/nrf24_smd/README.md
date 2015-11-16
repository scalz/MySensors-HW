# RollerShutterNode (NRF24L01+ smd version)


It is a board for controlling rollershutter (0-100%) and detecting end stops with ACS712 for current sense of motor. It can be used as a relay board too.
You can connect 3 push buttons (UP/DOWN/STOP) and/or external i2c device (for tactile, gesture...). 
It can control 12-220V motors.
I will use it with Mysensors lib.

Size : 47x48 (mm)
Components size 0805.

Note : it is still in dev.

####General spec
- Atmel328p TQFP
- NRF24L01+ smd footprint
- ATSHA204A authentication footprint
- eeprom footprint for OTA
- ACS712 5Amp footprint
- 2x 10Amp relays : 1xNO G5Q-1A-5VDC and 1xNO-NC G5Q-1-EU-5VDC driven by dual relay driver
- AC/DC Stepdown 220V-5V : HILINK_HLK-PM01 protected by varistor & fuse
- 5mm pitch screw terminals
- I2C connector for optional tactile sensor chip

####Arduino Pin description
- A0 : ADC ACS712
- A3 : ATSHA204A data
- A2 : Down Relay
- A1 : Up relay
- I2C connector : SDA, SCL, D3, D4
- A7 : CTN thermistor to check temperature under Hilink Stepdown

####Connectors 
- FTDI for bootloading
- AVRSPI for programming Atmel328p
- D3 : optional IO
- D4 : optional IO
- D6 : Up button
- D7 : Down button
- D5 : Stop button
- gnd

####AC Connector description
- N : Neutral (Neutre)
- L : Live (Phase)
- 12_220 : optional. If using 220v motor, you need to tie it with Source on Motor Connector


####Motor Connector description
- Up : Motor Up 
- Down : Motor Down
- Source : 12-24v motor source. If using 220v motor, tie it with 12_220 on AC Connector

### Board overview  
<img src="https://raw.githubusercontent.com/scalz/MySensors-HW/development/RollerShutterNode/img/top_overview1.png" alt="Top">    

<img src="https://raw.githubusercontent.com/scalz/MySensors-HW/development/RollerShutterNode/img/bottom_overview.png" alt="Bottom">&nbsp; 


####TODO  
- sketch for rollershutter (buttons, calibration functions, controlling 0-100%...). 
- add Bill of Materials

####Notes
- there was an error on v1.0. I did so much changes in routing that I missed to change something on acs. So the spst relay is always on. too bad.
This is fixed in v1.1

Note : it is still in dev. 


####Links, reference and license 
Copyright Scalz (2015). released under the [CERN Open Hardware Licence v1.2](http://ohwr.org/cernohl)



