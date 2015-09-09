# MyMultiSensorChild

A multi sensor board that I will plug into my other board MyTinyCamel

Size : 43.2x23.8 (mm)
Components size 0805.

####Sensors footprints : 
- NTC thermistor
- D203S Pir motion 
- SI7021 Temperature/humidity s
- BMP180 Pressure 
- TSL2561 Ambiant luminosity and IR
- APDS-9960 : Ambiant luminosity / RGB / Gesture
- VEML6070 : UV 

####Features : 
- P-Mosfet : Enable/disable Pir motion 
- all others sensors can be soldered except : you need to choose pir or gesture sensors as they share same emplacement
- if PIR choosen, Arduino pin D6 is PIR output. P-mosfet enable/disable VCC PIR. 
- if Gesture choosen, then D6 is APDS-9960 interrupt. and P-mosfet enable/disable IR Led for gesture recognition. So you can disable gesture control but keep Lux/RGB measurement.


####Jumper settings :
todo description

####MYSXConnector 1.5 : 
- 1 :	VIN 
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



####TODO : 

...

Note : it is still in dev. so not tested. 



####Links, reference and license : 

Copyright Scalz (2015). released under the [CERN Open Hardware Licence v1.2](http://ohwr.org/cernohl)


