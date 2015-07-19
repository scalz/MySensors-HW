# PowerManagementMysensors

I mixed Anticimex(Patrick Fallberg) SensorsNode (in dev) with functionalities of ulpnode, to fit my needs. All credits to them for concepts and idea (links below). Here is my version still in dev.

Size : 50x44 (mm)

PowerManagementMysensors : nrf24/RFM69

PowerManagementMysensors2 : nrf24 smd module/RFM69

- Arduino pro mini connector

- C1 : Storage capacitor if using Ultra Low power Node 
- JP1, JP2 : Power by VIN, Bypass DC Boost 3v and capa c1 
- JP3 : enable/disable DC Booster +5V 
- JP4 : enable/disable 2v supervisor and its interrupt 
- JP5 : choose INT2 Mode : INT2 or Interrupt on 2V supervisor (if 2v, wake up arduino, enable dc booster to charge C1) 
- JP6 : Radio CSN Pin NRF or RFM69 
- JP7 : choose INT1 mode : INT1 or INT NRF connection

- A0 PIN : ADC Input used for reading C1 voltage 
- A1 PIN : Output : Enable/disable RF VCC 
- A2 PIN : ADC Input used for reading battery voltage (<5v)
- D4 PIN : Output : Enable/disable Sensors 3V Rail 
- D5 PIN : Output : enable/disable DC Boost 3v

Connectors :
- +3V Sensors
- +5V
- GND
- J1 : free. connected to TX
- J2 : free. connected to RX
- J3 : free. connected to D3 INT2 (jumper config)
- J4 : free. connected to D6. PWM
- J5 : free. connected to A6. ADC1
- J6 : free. connected to A7. ADC2
- J7 : I2C1 (A4 SDA, A5 SCL)
- J8 : I2C2 (A4 SDA, A5 SCL)
- J9 : free. SPI CSN. connected to D7.
- J10 : free. connected to D2. INT1 (jumper config)

- FLASH for OTA
- ATSHA204A authentication
- NRF24 or RFM69 footprint 
- AVRSPI connector
- I2C pull ups on board

How it will work :

1) Basic Mode : You can enable/disable functions, dc boosters, supervisor, power directly from battery... See jumpers and connectors description

2) Ulpnode mode : Enable JP1, JP2, JP4, JP5 supervisor

To get ultra low power, it needs : 
- not use watchdog so you can save uA
- to wake up, use pin change interrupt
- no bod during sleep mode
- lower voltage frequency. So, BOD=1.8V, sleep mode freq : 1-4Mhz.
So, to be able to wake up each period, when supervisor < 2v, it toggles interrupt EN_BOOST, so dc Booster 3v starts to charge C1, and arduino wakes up. As the supervisor will maintain interrupt while vcc<2v, you can maintain interrupt by enabling D5 output Arduino and check the C1 voltage level by reading A0 arduino. Then do your tasks (tune frequ clock, enable rf and sensors power, readings...), then tune freq for sleeping, disable D5 and you can put arduino in sleep mode again. C1 will discharge from 3.3v to <2v (need to test C1 value to know discharge time). And again...

it is possible to monitor battery voltage. But be careful, as I didn't want to consume more power with traditional divider, and have not enough free output to disable by mosfet such divider, 
I have choosen to use simple ADC read through resistor. Batt voltage must be <= vcc arduino to be able to read. I added protection for eeprom.

Note : it is still in dev. so not tested. I will make some changes I think.



TODO : 
- add gerbers for Seeed, Itead, and OSH fabhouses.
- custom bootloader


Links, reference and license : 

Copyright scalz (2015). [Attribution-ShareAlike CC BY-SA](https://creativecommons.org/licenses/)

- For https://github.com/fallberg/MySensorsNode,

Copyright Patrick Fallberg (2015). All designs are released under the [CERN Open Hardware Licence v1.2](http://ohwr.org/cernohl)

- For https://hallard.me/category/ulpnode/, 

Copyright Charles-Henri Hallard (2014). [Attribution-ShareAlike CC BY-SA](https://creativecommons.org/licenses/)

