# Arduino Plant Watering System
Arduino Plant Watering System
 Arduino Garden watering
 Arduino crop irrigation
 
Advanced Irrigation System

Advanced, Intelligent, Smart, Automated, Automatic

https://youtu.be/1jKAxLyOY_s

+/- 10 Dollar Automated Plant Watering Computer on Soil Moisture

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

standalone system, calibrate sensors possible

software reboot, asm volatile (" jmp 0"); at 23:59:59 to reset wateringcounter and fresh millis(); and erase days wateringtimelog

------

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

a Penny for Sharing My Thoughts?

http://www.paypal.me/LDijkman
