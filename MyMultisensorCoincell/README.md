# MyMultisensorCoincell

I'm happy to share my first project using coincell. This is a Multisensors board optimized for low power. 
It will be able (I hope!) to detect motion, temperature, humidity, ambiant light for 2-3 years min, powered by a CR2032 coin cell; my goals! 
I will use it with Mysensors lib.	
	
Inspired from ST and TI appnotes. I thank them for documentation.
	
Note : This is a rev 1.0...and a work in progress as I am now waiting for pcbs.


####General spec

- PCB size : 30mmx49mm
- Very low power multisensor board powered by Coincell or vin=<3.3V.
- Motion detection sensor : LHI968. Motion detection can be disabled if not needed all the day, and save more battery.
- Temperature/Humidity sensor : SI7021
- Ambiant light : OPT3001
- MCU : Atmel 328p (Internal RC or external)
- Power supply : direct battery no regulator, Coincell, pads to connect other 3.3v batt like 2xAA/AAA. It's designed to be thin, so coincell or AAA is better..
- Security : ATSHA204A for signing/authentication
- Reverse polarity protection
- Radio supported : RFM69CW 
- Connectors : AVRSPI, FTDI
- One optional LED for indicator on D7
- One optional LED for motion trigger indicator (mainly for debug purpose, there are also so smd probe tests points for PIR circuit)

### Overview 
 
<img src="https://raw.githubusercontent.com/scalz/MySensors-HW/development/MyMultisensorCoincell/img/top_view.jpg" alt="Top view">    

<img src="https://raw.githubusercontent.com/scalz/MySensors-HW/development/MyMultisensorCoincell/img/bottom_view.png" alt="Bottom_view view">

<img src="https://raw.githubusercontent.com/scalz/MySensors-HW/development/MyMultisensorCoincell/img/custombox_1.png" alt="Custombox 1">

<img src="https://raw.githubusercontent.com/scalz/MySensors-HW/development/MyMultisensorCoincell/img/custombox_2.png" alt="Custombox 2">&nbsp;

####Notes : About power consumption, what can we expect???

Few simple maths because it can vary for multiple reasons (brand, internal resistance etc.). I won't explain all here, 
- but briefly we can't expect to get the whole 230mAh of the coincell. 200mAh of usable energy sounds more reasonable (for a good quality, or it could be less). 
- rough "kind" estimation of power consumption during sleep mode : 5-6uA in sleep mode with PIR enabled. No need to disable the radio (sleep mode is lowpower & listenmode is lowpower too).
- RFM69 : 30mA in radio transmit during says 40ms (can be more time with signing...) but it does not spend this whole 40ms in TX transmit, plus if using ATC mode it should use less power to transmit, so..it's a little bit negative estimation. no matter. 
- 10 wakeups/hour. 

That gives us a theoretical estimation of 2.33 years. Not so bad..But, the node could last a lot more than these 2.33years, if :
- very important : the software is well optimized. For instance, to manage sleep power consumption during blind time etc.. or if you disable the motion detection when you don't need it
- Another very important point : a good quality coincell. Bad, clone coincell can be 50% less usable energy. You won't notice it at the beginning, but you will notice it later ;) And it's almost sure to notice this if power supply is not optimized. Here we are lucky, it's optimized ;) but there is still (unfortunately) a big difference between coincell brands. Like the adage says..We have what we pay for..Of course I will try both, for curiosity!


####Known issues 


####TODO 
- assemble board 
- sketch
- print custom box


####Links, reference and license 
Copyright scalz (2016). released under the [CERN Open Hardware Licence v1.2](http://ohwr.org/cernohl)



