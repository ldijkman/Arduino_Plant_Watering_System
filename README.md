# Arduino Plant Watering System
Arduino Plant Watering System

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

timed duration of watering

pauzetime after watering, give water time to soak into soil

watering counter, maximum number of watering a day

software reboot, asm volatile (" jmp 0"); at 23:59:59 to reset wateringcounter and fresh millis();

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

<img src="https://www.codemacs.com/post.images/16204626.1.webp">

https://www.codemacs.com/raspberrypi/howtoo/soil-moisture-sensor-waterproofing.9450461.htm

a Penny for Sharing My Thoughts?

http://www.paypal.me/LDijkman
