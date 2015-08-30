# MyTinyCamel

Based on my previous PowerManagement. I have miniaturized it.

Size : 49.5x23.8 (mm)
Components size 0805.

Node sensors with power management features : 
- power through VBAT directly
- power through 3V Dc booster
- mode ultra low power ( < 1uA in sleep mode)
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


Jumper settings :
- JP1 : VIN selection (power directly from VIN/VBAT | 3V DC Booster | MCP1703 
- JP2 : if JP1 is set VIN/VBAT, so you can choose if you want VIN/VBAT only, or if you want to use MCP1703
- JP3 : if JP1 is set on 3VDC Booster, you can enable storage capacitor for ultra low power mode 
- JP4 : enable/disable 5VDC Booster
- JP5 : if set, enable voltage supervisor control for ultra low power mode
- JP6 : choose if you want to power external eeprom through mosfet VCC sensors or through regulated VCC3 directly
- JP7 : choose if you want to power radios through mosfet VCC RF or through regulated VCC3 directly
- JP8 : choose INT2 Mode : INT2 or Interrupt on 2V supervisor (if 2v, wake up arduino, enable dc booster to charge C1) 

Atmel Pinout :
- A1 : Output : Enable/disable RF VCC 
- A2 : ADC Input used for reading battery voltage (<5v)
- D3 : optional INT2 or for voltage supervisor control
- D4 : Output : Enable/disable Sensors 3V Rail 
- D5 : Output : enable/disable DC Boost 3v

MYSXConnector 1.5 : 
1:	VIN connected to -> batt vs dc booster vs MCP1703 3.3vreg (max16v)
2:	VCC3
3:	VCCIO
4:	gnd
5:	tx
6:	rx
7:	a6
8:	a7
9:	d2
10:	d3
11:	d5
12:	d6
13:	scl
14:	sda
15:	A0
16:	d8
17:	mosi
18:	miso
19:	sck
20:	d7


How it will work :

1) Basic Mode : You can enable/disable functions, dc boosters, supervisor, power directly from battery... See jumpers and connectors description

2) Ulpnode mode : Enable JP1, JP2, JP3, JP5 supervisor

To get ultra low power, it needs : 
- don't use watchdog so you can save uA
- to wake up, use pin change interrupt
- no bod during sleep mode
- lower voltage frequency. So, BOD=1.8V, sleep mode freq : 1-4Mhz.
So, to be able to wake up each period, when supervisor < 2v, it toggles interrupt EN_BOOST, so dc Booster 3v starts to charge C1, and arduino wakes up. As the supervisor will maintain interrupt while vcc<2v, you can maintain interrupt by enabling D5 output Arduino. Then do your tasks (tune frequ clock, enable rf and sensors power, readings...), then tune freq for sleeping, disable D5 and you can put arduino in sleep mode again. C1 will discharge from 3.3v to <2v (need to test C1 value to know discharge time). And again...

it is possible to monitor battery voltage. But be careful, as I didn't want to consume more power with traditional divider, and have not enough free output to disable by mosfet such divider, 
I have choosen to use simple ADC read through resistor. Batt voltage must be <= vcc arduino to be able to read. I added protection for eeprom.

### Board overview  
<img src="https://raw.githubusercontent.com/scalz/MySensors-HW/development/MyTinyCamel/img/Top_overview_en.png" alt="Top">    

<img src="https://raw.githubusercontent.com/scalz/MySensors-HW/development/MyTinyCamel/img/Bottom_overview_en.png" alt="Bottom">&nbsp; 


Note : it is still in dev. so not tested. I will make some changes I think.



Links, reference and license : 

Copyright scalz (2015). released under the [CERN Open Hardware Licence v1.2](http://ohwr.org/cernohl)

- For https://github.com/fallberg/MySensorsNode,

Copyright Patrick Fallberg (2015). CERN Open Hardware Licence v1.2](http://ohwr.org/cernohl)

- For https://hallard.me/category/ulpnode/, 

Copyright Charles-Henri Hallard (2014). [Attribution-ShareAlike CC BY-SA](https://creativecommons.org/licenses/)


