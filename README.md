# Arduino Plant Watering System
## Perfect Moisture
Arduino Plant Watering System
 Arduino Garden watering
 Arduino crop irrigation
 
Advanced Irrigation System

Advanced, Intelligent, Smart, Automated, Automatic

https://youtu.be/1jKAxLyOY_s

----

javascript csv chart viewer of a piece of 

recieved CSV file logged by arduino plant watering http://www.arduino.tk/csv_chart.html

-----

+/- 10 Dollar Automated Plant Watering MicroController / Computer on Soil Moisture

Arduino Nano or Uno

2 analog inputs A0 A3 for 2 Capacitive Soil Moisture Sensors (Capacitive soil sensor is less corrosion sensitive)

1 output D13 for watering start (Pump and/or Valve) (output D13 is also onboard LED)

i2c for DS3231 RTC RealTimeClock 

i2c for 4x20 i2c LCD

start time, end time, let plants sleep at night, no watering at night

start watering on average startvalue in % of 2 Capacitive Soil Moisture Sensors 

Better NOT put the sensors in 1 pot they may influence eachother if in 1 pot

------------

start watering on soil moisture sensor setpoint, setpoint changeable in rotary encoder menu, and saved to eeprom

timed duration of watering from 2 seconds to ... seconds

pauzetime after watering, give water time to soak into soil, from 1 minute to ... minutes

watering counter, maximum number of watering a day, upto watering 90 jobs (eeprom steps 10 from 100 to 1000, 1023 eeprom limit)

watering jobs start times saved to eeprom, max 90 a day in LOG (erased at 23:59:59)  

watering start hour, end hour

parameters / settings changeable and saved in eeprom adress 0 to 90

standalone system, calibrate and adjust sensors possible in rotary encoder menu, settings saved to eeprom

software reboot, asm volatile (" jmp 0"); at 23:59:59 to reset wateringcounter and fresh millis(); and erase days wateringtimelog

NO PC needed to change parameters / settings / calibrate sensors

------

Low Cost, Low Power Consumption.

-------

i do not know if my code is 49-50 day millis(); overflow safe so thats why i do a software reboot

&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

GNU General Public License,

which basically means that you may freely copy, change, and distribute it,

but you may not impose any restrictions on further distribution,

and you must make the source code available.

&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&


https://github.com/ldijkman/Arduino_Plant_Watering_System/blob/main/Menu_test_arduino_plant_watering.ino

https://youtu.be/1jKAxLyOY_s



#
# The Future is now:

Rotary encoder menu 

Change and save parameters settings to EEPROM

Calibrate sensors settings, all without PC

an independant stand alone real plant watering system 

Low cost, Low power consumption

The First?

https://github.com/ldijkman/Arduino_Plant_Watering_System/blob/main/Menu_test_arduino_plant_watering.ino

#
# Code compiles different on Mega2560 with SD-Card
## now Creates Graphics like professional Growers have

arduino nano / uno out of progmem space, mega has more, so added SD-Card reader/writer(with levelconverter?!)

on mega2560 possible to write a lot of variables to SD-Card LOGfile CSV each second

a comma seperated txt file https://github.com/ldijkman/Arduino_Plant_Watering_System/blob/main/CSV_received.CSV

with a header on line 1 wich describes the fields

wich can be viewed in spreadsheat editor 

Or 

i like Analog Flavor BeSpice Wave http://www.analogflavor.com/en/bespice/bespice-wave/

BeSpice Wave == Perfect *.CSV to Graphic View (on my Raspberry pi400)

https://github.com/ldijkman/Arduino_Plant_Watering_System/blob/main/csv_recieved.png

i do a countdown on wateringduration timer and pauzetimer, easy to see remaining time on LCD display

------

serialdata to file for realtime plot

sometimes there is some old text before the wat should be the first header line

do not know yet how to get rit of this, some old buffer?

add a 1 dollar for ESP8266 for RealTime serial WIFI monitor or download SD-card CSV LOG file ???

or add a 2 dollar W5500 ethernet module for hardwired localarea net connection or data log download and realtime graphic monitoring

-------


#
# Sense:

less then a dollar Capacitive Soil Moisture Sensor on aliexpress

Capacitive is better corrosive resistant

<img src="https://cdn-reichelt.de/bilder/web/xxl_ws/A300/CAP-SHYG_1.png">

#
# 3D Printer:

not mine!

https://www.codemacs.com/raspberrypi/howtoo/soil-moisture-sensor-waterproofing.9450461.htm

STL file for your slic3r / 3d printer https://github.com/ldijkman/Arduino_Plant_Watering_System/blob/main/www.codemacs.com_downloads_smsensor.cover.stl 

<img src="https://www.codemacs.com/post.images/16204626.1.webp">

https://www.codemacs.com/raspberrypi/howtoo/soil-moisture-sensor-waterproofing.9450461.htm

#
# Arduino NANO ProtoType Layout setup:

Proto PCB https://nl.aliexpress.com/item/32997887790.html

1pcs DIY 10*15CM Blauw Single Side Prototype Paper PCB Universal Experiment Matrix Circuit Board 10x15CM Voor Arduino

<img src="https://github.com/ldijkman/Arduino_Plant_Watering_System/blob/main/arduino_plant_watering_nano_prototype_2.jpg" width="30%" heigth="30%"><img src="https://github.com/ldijkman/Arduino_Plant_Watering_System/blob/main/arduino_plant_watering_nano_prototype_1.jpg" width="30%" heigth="30%">

parts / modules on photo:

DS3231 RTC realtimeclock

i2c backpack for / and  2004 / 20x4 LCD

Arduino NANO

Rotary push button encoder KY-040

---------

a Penny for Sharing My Thoughts?

http://www.paypal.me/LDijkman

---------
