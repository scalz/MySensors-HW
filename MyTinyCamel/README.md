# MyTinyCamel

Based on my previous PowerManagement. I have miniaturized it.

Size : 49.5x23.8 (mm)
Components size 0805.

####Node sensors with power management features : 
- power through VBAT directly
- power through 3V Dc booster
- mode ultra low power ( < 1uA in sleep mode) needs the onboard 3VDC booster
- power through MCP1703 for VIN < 16V
- Mosfet for radio
- Mosfet for sensors
- Atmel 328p
- two DC boosters 3V and 5V
- onboard MCP1703 regulator
- supervisory circuit (2v threshold) 
- 3 optional i2c connectors. Each one has its 3v (through mosfet), 5V, GND
- MYSXconnector 1.5 to plug external boards/sensors
- rfm69hw/nrf24 smd footprints
- ATSHA for authentication
- external eeprom for OTA
- optional onnboard LED
- onboard optional i2c pullups
- optional pullups for IO
- Smd jumpers.


####Jumper settings :
- JP1 : VIN selection (power directly from VIN/VBAT | 3V DC Booster | MCP1703 
- JP2 : if JP1 is set VIN/VBAT, so you can choose if you want VIN/VBAT only, or if you want to use MCP1703
- JP3 : if JP1 is set on 3VDC Booster, you can enable storage capacitor for ultra low power mode 
- JP4 : enable/disable 5VDC Booster
- JP5 : if set, enable voltage supervisor control for ultra low power mode
- JP6 : choose if you want to power external eeprom through mosfet VCC sensors or through regulated VCC3 directly
- JP7 : choose if you want to power radios through mosfet VCC RF or through regulated VCC3 directly
- JP8 : choose INT2 Mode : INT2 or Interrupt on 2V supervisor (if 2v, wake up arduino, enable dc booster to charge C1) 
- JP9 : how 3VDC booster is enabled. By Supervisor/atmel pins or by VIN for continuous mode

####Atmel Pinout :
- A1 : Output : Enable/disable RF VCC 
- A2 : ADC Input used for reading battery voltage (<5v)
- D3 : optional INT2 or for voltage supervisor control
- D4 : Output : Enable/disable Sensors 3V Rail 
- D5 : Output : enable/disable DC Boost 3v

####MYSXConnector 1.5 : 
- 1 :	VIN connected to -> batt vs dc booster vs MCP1703 3.3vreg (max16v)
- 2 :	VCC3
- 3 :	VCCIO
- 4 :	GND
- 5 :	TX
- 6 :	RX
- 7 :	A6
- 8 :	A7
- 9 :	D2
- 10 :	D3
- 11 :	D5
- 12 :	D6
- 13 :	SCL
- 14 :	SDA
- 15 :	A0
- 16 :	D8
- 17 :	MOSI
- 18 :	MISO
- 19 :	SCK
- 20 :	D7


### Board overview  
<img src="https://raw.githubusercontent.com/scalz/MySensors-HW/development/MyTinyCamel/img/Top_overview_en.png" alt="Top">    

<img src="https://raw.githubusercontent.com/scalz/MySensors-HW/development/MyTinyCamel/img/Bottom_overview_en.png" alt="Bottom">&nbsp; 


TODO : 
- settings description
- some coding
...

Note : it is still in dev. so not tested. I am waiting for the pcbs.



Links, reference and license : 

Copyright scalz (2015). released under the [CERN Open Hardware Licence v1.2](http://ohwr.org/cernohl)

This board is inspired from the great work of :
- Patrick Fallberg : https://github.com/fallberg/MySensorsNode

Copyright Patrick Fallberg (2015). CERN Open Hardware Licence v1.2](http://ohwr.org/cernohl)

- Charles-Henri Hallard : https://hallard.me/category/ulpnode/
 
Copyright Charles-Henri Hallard (2014). [Attribution-ShareAlike CC BY-SA](https://creativecommons.org/licenses/)


