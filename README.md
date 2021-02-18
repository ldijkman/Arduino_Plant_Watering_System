# Arduino Plant Watering System
Arduino Plant Watering System

https://youtu.be/1jKAxLyOY_s

+/- 10 Dollar Automated Plant Watering Computer on Soil Moisture

Arduino Nano, Uno or other Arduino device

2 analog inputs A0 A1 for 2 Capacitive Soil Moisture Sensors (Capacitive soil sensor is less corrosion sensitive)

1 output D13 for watering start (Pump and/or Valve) (output D13 is also onboard LED)

i2c for DS3231 RTC RealTimeClock and 4x20 i2c LCD

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


https://github.com/ldijkman/Arduino_Plant_Watering_System/blob/main/Arduino_Plant_Watering_System.ino

https://youtu.be/1jKAxLyOY_s


http://Paypal.Me/LDijkman

#
# Future:

Rotary encoder menu 

change and save parameters to EEPROM to make it an independant stand alone system

reuse parts of code from

http://www.sticker.tk/forum/index.php?action=view&id=299

or

http://www.sticker.tk/forum/index.php?action=view&id=296


less then a dollar Capacitive Soil Moisture Sensor on aliexpress

Capacitive is better corrosive resistant

<img src="https://cdn-reichelt.de/bilder/web/xxl_ws/A300/CAP-SHYG_1.png">

#
# 3D Printer:

https://www.google.com/search?q=3d+print+moisture+sensor

https://www.codemacs.com/raspberrypi/howtoo/soil-moisture-sensor-waterproofing.9450461.htm

<img src="https://www.codemacs.com/post.images/16204626.1.webp">

https://www.thingiverse.com/thing:2952434

<img src="https://cdn.thingiverse.com/renders/cb/96/fb/c5/82/576137ff660eed9ba9b4aad358fdf76c_preview_featured.jpg">

