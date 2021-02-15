# Arduino Plant Watering System
Arduino Plant Watering System

+/- 10 Dollar Automated Plant Watering Computer on Soil Moisture

Arduino Nano, Uno or other Arduino device

2 analog inputs for 2 Capacitive Soil Moisture Sensors 

1 output for watering start (Pump and/or Valve)

i2c for RTC and LCD


2 Capacitive Soil Moisture Sensors 

4x20 i2c LCD

DS3231 RTC RealTimeClock

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


http://Paypal.Me/LDijkman
