// working on     SD card Logging for mega 2560 board (not enough progmem space on nano /uno)
// working on     creates a comma seperated log file datestamp .txt on SD card each day
// on a mega2560 allmost everything / every change is logged to sdcard
//
//*****************************************************************************
// BUT HOW do a visualize such a LOG file online or in a spreadsheet editor???
//
// if you drag the log .txt file from sdcard to here https://www.csvplot.com/
// they will make a chart
// https://github.com/ldijkman/Arduino_Plant_Watering_System/blob/main/csvplot.com_log_txt_mega2560.jpg
// drag menu items on left to x or y
//
// if you know a good (simple) online/ofline chart maker to view (all) the data
// let me know??? https://m.facebook.com/luberth.dijkman
//
// sudo apt-get install kst        install on my linux raspberry pi400
// kst2                            command to start it = kst2 / it is also in startmenu Education
// kst-plot has a ton / load of options
// and is fast with big data
// https://miscircuitos.com/using-kst-plot-visualize-data-real-time/
// https://kst-plot.kde.org/
// https://kst-plot.kde.org/video_tutorials/
// WOW!
// plot data from serial port in realtime https://miscircuitos.com/plot-real-time-signal-coming-arduino/
//
//***********************************************************************
//
// Arduino Advanced Automated Plant Watering System, StandAlone, Low Cost, Low Power Consumption
// Copyright 2021 Dirk Luberth Dijkman Bangert 30 1619GJ Andijk The Netherlands
/*
  &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
  Arduino Nano / Uno / Mega2560
  Arduino Automated Plant Watering System, automatic irrigation system
  Arduino Advanced Automated Plant Watering System, StandAlone, Low Cost, Low Power Consumption
  Copyright 2021 Dirk Luberth Dijkman Bangert 30 1619GJ Andijk The Netherlands
  https://m.facebook.com/luberth.dijkman
  https://github.com/ldijkman/Arduino_Plant_Watering_System
  https://github.com/ldijkman/Arduino_Plant_Watering_System/discussions
  https://youtu.be/1jKAxLyOY_s
  GNU General Public License,
  which basically means that you may freely copy, change, and distribute it,
  but you may not impose any restrictions on further distribution,
  and you must make the source code available.
  https://www.gnu.org/licenses
  used code for rotary encoder from:
   Robust Rotary encoder reading
   Copyright John Main - best-microcontroller-projects.com
   https://www.best-microcontroller-projects.com/rotary-encoder.html
  http://Paypal.Me/LDijkman
  All above must be included in any redistribution
  &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
*/

//
// it all started with a Question from Jo
// Jo de Martelaer Arduino UNO / NANO WaterGift
// Boomkwekerij Hortus Conclusus
// https://www.hortusconclusus.be/
// Catalogus => https://www.hortusconclusus.be/collections
//
// Arduino PlantWatering Examples i have seen on the internet are to simple, overwatering, no pause, no limmits, no timebounds, less feedback,
// no standalone system ability to change parameters / settings, Calibrate sensors
//
// Jo Says has problems with sensors influencing eachother?
// https://www.youtube.com/c/HortusconclususBe/videos
// maybe use i2c analog 4 channel 16bit https://www.google.com/search?q=aliexpress+ads1115
// is the ads1115 multiplexing like arduino?
// or use 2 single channel i2c analog to digital converters with a different i2c adress, No Noise?
// future maybe 2x 1 channel 12bit? MCP4725 I2C, both with different i2c adres
// https://www.google.com/search?q=aliexpress+MCP4725
//
// What is maximum sensor wire length for analog? for i2c?
// soil-watch sells 5v analog with 20meter cables
// (No grounded shield?, unshielded?=does that make sense) https://pino-tech.eu/soilwatch10/
//
//
// with irrigation drippers / drip emmiters you have more control of amount of water per time!?
// different types liters/per hour
// white tyleen hose less heat / polution, better not use black, do not use transparent
// use an extra dripper in a measurecan to monitor/check water given each day (empty can each day)
//
//
// DS3231 RTC realtimeclock connected to +5vdc GND SCL SDA
// 4x20 i2c LCD connected to +5vdc GND SCL SDA
//
// Cheap dollar capacitive soil moisture sensors are in the Khz range
// more expensive capacitive soil moisture sensors are  in the +/-75Mhz range (mineral/contemination transperency?)
// but it isnt the 555 timer replacement that Justifys the 30x and up price
// https://www.google.com/search?q=LTC6905+soil+moisture
//
// 2 capacitive soil moisture sensors connected to analog A0 and A3
//
// Pump and or valve output D13 (wich is also onboard LED)
//
// rotarybutton_SW 2           // input D2 rotary encoder SW
// CLK 3                       // input D3 rotary encoder CLK
// DATA 4                      // input D4 rotary encoder DT
// and connect rotary encoder to +5vdc and GND to 0vdc, -, min, GND, ground, or whatever they call it


// parts list:
// Uno            https://www.google.com/search?q=aliexpress+arduino+uno
// or a nano  1 dollar?
// Nano           https://www.google.com/search?q=aliexpress+arduino+nano
// RTC DS3231     https://www.google.com/search?q=aliexpress+RTC+DS3231
// i2c LCD 20x4   https://www.google.com/search?q=aliexpress+LCD+20x4+i2c
// 2 sensors      https://www.google.com/search?q=aliexpress+aduino+capacitive+soil+moisture+V2
// capacitive soil moisture sensor, most are V1.2 there is a V2, maybe better?
// dupont wires or better solder wires for permanent device
// relays https://www.google.com/search?q=aliexpress+WeMos+relais+Shield
//
// for mega sd card log https://www.google.com/search?q=aliexpress+Micro+SD+card+mini+TF+card+reader+module+SPI+interfaces+for+arduino+with+level+converter+chip

// other sensors https://www.tindie.com/products/miceuz/i2c-soil-moisture-sensor/#product-name

// https://github.com/ldijkman/Arduino_Plant_Watering_System/discussions


// future SD Card log  // not enough space nano uno == 105%    for mega 2560 or mega pro mini
#if (defined(__AVR_ATmega2560__))
#include <SPI.h>
#include <SD.h>
/*
  i used an SPI TFT LCD SD-Card reader, maybe not safe 5v / 3.3v ???????? !!!!!!!! !!!!!
  https://diyi0t.com/sd-card-arduino-esp8266-esp32/
  https://www.google.com/search?q=aliexpress+Micro+SD+card+mini+TF+card+reader+module+SPI+interfaces+for+arduino+with+level+converter+chip
  SPI bus pins:
  mega pin i/o 50 = MISO
  mega pin i/o 51 = MOSI
  mega pin i/o 52 = CLK   (on SPI TFT LCD == SCK)
  mega pin i/o 53 = CS
  i used an SPI TFT LCD SD-Card reader, maybe not safe 5v / 3.3v ???????? !!!!!!!! !!!!!
*/
const int chipSelect = 53;

#endif
// future SD Card log // not enough space nano uno == 105%    for mega 2560 or mega pro mini



#include "RTClib.h"                   // https://github.com/adafruit/RTClib

RTC_DS3231 rtc;

#include <EEPROM.h>

#include <LiquidCrystal_I2C.h>       // https://github.com/marcoschwartz/LiquidCrystal_I2C/archive/master.zip

LiquidCrystal_I2C lcd(0x27, 20, 4);  // Configure LiquidCrystal_I2C library with 0x27 address, 20 columns and 4 rows

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

int start_hour = 9;                                 // starttime 9 o clock in the morning
int end_hour = 21;                                 // endtime  21 o clock in te evening
signed long watering_duration = 60;                        // in seconds = 60 seconds
signed long pauze_after_watering = 1 ;                     // in minutes = 1 minute
signed long watergifttimer;
signed long pauzetimer;
signed long startpauzetimer;
signed long starttime;

String lastwatering;

unsigned long previousMillis = 0;
unsigned long currentMillis;

int moistureforstartwatering = 30;                      // if smaller als 30% start watering

int dry_sensor_one = 795;                                   // my sensor value Dry in air   795
int wet_sensor_one = 435;                                   // my sensor value wet in water 435

int dry_sensor_two = 795;                                   // my sensor value Dry in air   795
int wet_sensor_two = 435;                                   // my sensor value wet in water 435

int sense1;
int sense2;

long Read_A0 = 0;
long Read_A3 = 0;
int dummy_read;

int averageinprocent;                                      // average of 2 sensors in %

byte watergiftcounter = 0;

byte ValveStatus = 0;

byte second_now;
byte last_second;

byte maximumaantalbeurtenperdag = 8;

byte Calibrate_Sensors = 1;
byte eepromerase = 1;
byte settimedate = 1;
byte backtobegin = 1;

long loopspeed;

String ClearLine = "                    ";  // 20 spaces


// rotary encoder push button KY-040 https://www.google.com/search?q=KY-040
// Robust Rotary encoder reading
// Copyright John Main - best-microcontroller-projects.com
// https://www.best-microcontroller-projects.com/rotary-encoder.html
#define rotarybutton_SW 2           // input D2 rotary encoder SW
#define CLK 3                       // input D3 rotary encoder CLK
#define DATA 4                      // input D4 rotary encoder DT
// and connect rotary encoder to +5vdc and GND to 0vdc, -, min, GND, ground, or whatever they call it
static uint8_t prevNextCode = 0;
static uint16_t store = 0;
//

byte menu_nr = 0;


long TempLong;                                   // Temporary re-used over and over again
float TempFloat;
int TempInt;
byte TempByte;

int backlightofftimeout = 5;                  // time to switch backlight off in minutes
long backlightstart;
byte backlightflag;

byte blinknodelay_flag;                         // a blink flag that is 1 second==1, and 1 second==0

int counter;



byte exitflag = 0;

//**********************************************************************************************
void setup () {

  pinMode(13, OUTPUT);                 // pin 13 for valve open / close is also the onboard LED

  //                                            i have 3 pullup resistors on my KY-040 so INPUT_PULLUP should not be needed
  //                                            BUT
  //                                            Jo says NO pullup resistor on SW on his rotary decoder => so made it input_pullup
  pinMode(rotarybutton_SW, INPUT_PULLUP);     // rotary encoder SW = pulled up by resistor on KY-040 to +
  pinMode(CLK, INPUT_PULLUP);                 // rotary encoder CLK = pulled up by resistor on KY-040 to +
  pinMode(DATA, INPUT_PULLUP);                // rotary encoder DATA = pulled up by resistor on KY-040 to +


  Serial.begin(115200);               // serial monitor


  lcd.begin();                        // Initialize I2C LCD module (SDA = GPIO21, SCL = GPIO22)
  // lcd.begin(21, 22);               // Jo de Martelaer says lcd.begin(); does not work on win10, use  lcd.init(); he says
  // lcd.init();                      // strange i have no problem with it on linux arduino ide 1.8.13

  lcd.backlight();                    // Turn backlight ON
  lcd.clear();                        // clear lcd


  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // want to see the contents of EEPROM there is a read example Arduino IDE =>File=>Examples=>EEPROM=>eeprom_read
  //               Six Hundred and Sixty-Six on adres Six Hundred and Sixty-Six is written in EEPROM like this
  // 665 255
  // 666 154       2x256+154=666
  // 667 2
  // 668 255





  // DO NEXT ONLY ONCE
  // first run ??? write some val to eeprom if value at eepromadres 90 not is 666
  // if this is first run then val will not be 666 at eeprom adres 90
  // so next will be run
  EEPROM.get(90, TempInt);                                  // read eeprom adress 90 into TempInt
  if (TempInt != 666) {                                     // IF this is the first run THEN val at eeprom adres 90 is not 666 ???
    EEPROM.put(0, moistureforstartwatering);
    EEPROM.put(5, dry_sensor_one);
    EEPROM.put(10, wet_sensor_one);
    EEPROM.put(15, dry_sensor_two);
    EEPROM.put(20, wet_sensor_two);
    EEPROM.put(25, watering_duration);
    EEPROM.put(30, pauze_after_watering);
    EEPROM.put(35, maximumaantalbeurtenperdag);
    EEPROM.put(40, start_hour);
    EEPROM.put(45, end_hour);
    EEPROM.put(50, backlightofftimeout);
    //  EEPROM.put(55, Variable-Here);
    //  EEPROM.put(60, Variable-Here);
    EEPROM.put(90, 666);                 // ONLY ONCE, Make His Mark, set eepromadres 90 to val 666 no need to call / run this anymore in future

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Hi There! First run"));
    lcd.setCursor(0, 1);
    lcd.print(F("Have written value's"));
    lcd.setCursor(0, 2);
    lcd.print(F("to EEPROM "));
    lcd.setCursor(0, 3);
    lcd.print(F("Thanks for trying"));
    for (int i = 30 ; i >= 0 ; i--) {   // on LCD countdown 30 to 0 half a second tick
      lcd.setCursor(18, 3);
      if (i <= 9) lcd.print(" ");
      lcd.print(i);
      delay(500);
    }
    delay(1000); // want to see the 0
    lcd.clear();
  }// END Do it ONLY ONCE, MADE HIS MARK



  // Read stored valeus from EEPROM
  EEPROM.get(0, moistureforstartwatering);
  EEPROM.get(5, dry_sensor_one);
  EEPROM.get(10, wet_sensor_one);
  EEPROM.get(15, dry_sensor_two);
  EEPROM.get(20, wet_sensor_two);
  EEPROM.get(25, watering_duration);
  EEPROM.get(30, pauze_after_watering);
  EEPROM.get(35, maximumaantalbeurtenperdag);
  EEPROM.get(40, start_hour);
  EEPROM.get(45, end_hour);
  EEPROM.get(50, backlightofftimeout);
  //  EEPROM.get(55, Variable-Here);
  //  EEPROM.get(60, Variable-Here);

  backlightstart = millis();          // load millis() in backlightstart

  lcd.clear();

  // they say eeprom life 100 thousand writes
  // say i write/erase a byte 4 times a day = 25 thousand days = 68 Years
  // could also use i2c eeprom that is on the ds3231 rtc board wich i think has 1 milion writes, RTC DS3231 with AT24C32 eeprom
  // https://www.google.com/search?q=AT24C32+pdf
  for (int i = 100 ; i < 1010 /*EEPROM.length()*/ ; i++) {   // erase eprom water start times
    lcd.setCursor(0, 0);
    lcd.print("Erase WaterTime LOG");
    lcd.setCursor(8, 2);
    lcd.print(i);
    EEPROM.write(i, 0);                             // erase eprom water start times
    Serial.println(i);
  }
  lcd.clear();


  Serial.print("Initializing SD card...");
#if (defined(__AVR_ATmega2560__))
  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");

#endif

  // rtc.adjust(DateTime(2021, 4, 9,7, 59, 00));
}






//**********************************************************************************************************
void loop () {

  if (backlightflag == 1 && millis() - backlightstart > (backlightofftimeout * 60 * 1000L)) {             // if backlight timed out turn it off
    lcd.noBacklight();                    // Turn backlight OFF
    backlightflag = 0;
  }
  if (backlightflag == 0 && millis() - backlightstart < (backlightofftimeout * 60 * 1000L)) {
    lcd.backlight();                    // Turn backlight ON
    backlightflag = 1;
  }


  // 5-ways-to-blink-an-led-with-arduino
  // https://urish.medium.com/5-ways-to-blink-an-led-with-arduino-8f8a41b4fc7d
  // like this oneliner
  // used for blinking text when time is not in range
  blinknodelay_flag = (millis() / 1000) % 2;  // continues 1second high, 1second low



  // read the sensors 10 times and divide by 10

  readsensors();

  // end read the sensors 10 times and divide by 10



  if (SetButton() == LOW) {                     //  SW = pulled up by resistor on KY-040 to +,  so LOW is button pressed

    backlightstart = millis();                                     // load current millis() into backlightstart
    lcd.backlight();                                               // turn backlight on
    TempLong = millis();  //  load current millis() into TempLong
    while (SetButton() == LOW) {                                   // while setbutton==LOW, pulled up by resistor, LOW is pressed

      if ((millis() - TempLong)  < 3000) {                         // only show this mssage the first 5 seconds
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("Backlight ON"));
        lcd.setCursor(5, 3);
        lcd.print(F("Menu Enter in"));
        lcd.setCursor(19, 3);
        lcd.print(3 - (millis() - TempLong) / 1000);              // on LCD countdown until menu system enter
        delay(250);
      }

      if ((millis() - TempLong)  > 3000) {                        // after 5 seconds pressed we get into menu system
        lcd.clear();
        lcd.setCursor(7, 1);
        lcd.print(F("in Menu"));
        lcd.setCursor(3, 3);
        lcd.print(F("Release Button"));
        delay(500);
        menu_nr = 1;
      }
    }
    lcd.clear();
  }




  if (menu_nr != 0) {        // only check next menus if menu_nr not = 0

    // 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
    // setpoint setpoint setpoint setpoint setpoint setpoint setpoint setpoint setpoint setpoint setpoint setpoint setpoint setpoint
    TempLong = millis();  //  load current millis() into TempLong
    if (menu_nr == 1) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(F("1 Set SwitchPoint %"));
      lcd.setCursor(8, 2);
      lcd.print(moistureforstartwatering);
      lcd.print(F(" % "));
    }
    while (menu_nr == 1) {
      lcd.setCursor(18, 3);
      if ((10 - (millis() - TempLong) / 1000) <= 9)lcd.print(" ");      // move 1 char when smaller a 10 wich is 2 chars
      lcd.print(10 - (millis() - TempLong) / 1000);                     // on lcd timeout countdown
      if ((millis() - TempLong)  > 10000) {
        delay(1000);  // want to see the zero 0
        TimeOut();
        break;
      }

      float rval;
      if ( rval = read_rotary() ) {
        moistureforstartwatering = moistureforstartwatering + (rval);
        if (moistureforstartwatering >= 70) moistureforstartwatering = 70;
        if (moistureforstartwatering <= 10) moistureforstartwatering = 10;
        TempLong = millis();  //  load current millis() into TempLong
        lcd.setCursor(8, 2);
        lcd.print(moistureforstartwatering);
        lcd.print(F(" % "));

      }

      if (SetButton() == LOW) {                                    // if setbutton==LOW, pulled up by resistor, LOW is pressed
        while (SetButton() == LOW) {
          /*wait for button released*/
        }
        menu_nr = 2;
        lcd.clear();

        EEPROM.get(0, TempInt);                                   // limmited write to eeprom = read is unlimmited
        if (moistureforstartwatering != TempInt) {            // only write to eeprom if value is different
          EEPROM.put(0, moistureforstartwatering);            // put already checks if val is needed to write
          lcd.setCursor(0, 0);
          lcd.print(F("Saving to EEPROM"));
          lcd.setCursor(0, 2);
          lcd.print("old= ");
          lcd.print(TempInt);
          lcd.print(F(" new= "));
          lcd.print(moistureforstartwatering);
          TempLong = millis();                                    // load millis() into Templong for next countdown delay
          while ((millis() - TempLong)  <= 5000) {
            lcd.setCursor(19, 3);
            lcd.print(5 - (millis() - TempLong) / 1000);          // on lcd timeout countdown
          }
          delay(1000);  // want to see the zero 0
          // for (int i = 0; i < 10; i++)Serial.println(F("moistureforstartwatering DATA WRITTEN / SAVED TO EEPROM "));
          lcd.clear();
        }


      }
    }// end menu_nr 1




    // 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2
    // Calibrate sensors Calibrate sensors Calibrate sensors Calibrate sensors Calibrate sensors Calibrate sensors
    TempLong = millis();  //  load current millis() into TempLong
    if (menu_nr == 2) {
      lcd.setCursor(0, 0);
      lcd.print(F("2 Calibrate Sensors"));
      lcd.setCursor(7, 2);
      if (Calibrate_Sensors == 1)lcd.print(F(" No   "));
      if (Calibrate_Sensors == 2)lcd.print(F(" Yes  "));
      if (Calibrate_Sensors == 3)lcd.print(F("Adjust"));
    }
    while (menu_nr == 2) {

      lcd.setCursor(18, 3);
      if ((10 - (millis() - TempLong) / 1000) <= 9)lcd.print(" ");      // move 1 char when smaller a 10 wich is 2 chars
      lcd.print(10 - (millis() - TempLong) / 1000);                     // on lcd timeout countdown
      if ((millis() - TempLong)  > 10000) {
        delay(1000);  // want to see the zero 0
        TimeOut();
        break;
      }

      float rval;
      if ( rval = read_rotary() ) {
        Calibrate_Sensors  = Calibrate_Sensors  + rval;
        TempLong = millis();  //reset innactive time counter
        if (Calibrate_Sensors < 1)Calibrate_Sensors = 3;
        if (Calibrate_Sensors > 3)Calibrate_Sensors = 1;
        lcd.setCursor(7, 2);
        if (Calibrate_Sensors == 1)lcd.print(F(" No   "));
        if (Calibrate_Sensors == 2)lcd.print(F(" Yes  "));
        if (Calibrate_Sensors == 3)lcd.print(F("Adjust"));
      }

      if (SetButton() == LOW) {        // LOW setbutton is pressed
        while (SetButton() == LOW) {
          /*wait for button released*/
        }
        menu_nr = 3;
        lcd.clear();
        delay(250);
      }
    }// end menu_nr 2



    if (Calibrate_Sensors == 2) {   // you chose Yes so whe go to calibrate
      Calibrate_Sensors = 1;

      // WARNING!!!
      // there is no timeout on this calibrate screens => for safety turn water off => its up to you to do a succesfull calibration
      digitalWrite(13, LOW);          // 13 is onboard led  en waterklep en/of waterpomp stop
      // there is no timeout on this calibrate screens => for safety turn water off => its up to you to do a succesfull calibration
      // WARNING!!!

      lcd.clear();
      while (0 == 0) {
        readsensors();
        lcd.setCursor(0, 0);
        lcd.print("Sensor1 analog A0");
        lcd.setCursor(0, 1);
        lcd.print("Clean Dry in Air?");
        lcd.setCursor(9, 3);
        lcd.print(sense1);
        delay(250);
        if (SetButton() == LOW) {        // LOW setbutton is pressed
          while (SetButton() == LOW) {
            /*wait for button released*/
          }
          dry_sensor_one = sense1;
          EEPROM.put(5, dry_sensor_one);
          lcd.clear();
          delay(500); // user gets a better experience switch to next screen?
          break;
        }
      }


      while (0 == 0) {
        readsensors();
        lcd.setCursor(0, 0);
        lcd.print("Sensor1 analog A0");
        lcd.setCursor(0, 1);
        lcd.print("Wet in Watter?");
        lcd.setCursor(9, 3);
        lcd.print(sense1);
        delay(250);
        if (SetButton() == LOW) {        // LOW setbutton is pressed
          while (SetButton() == LOW) {
            /*wait for button released*/
          }
          wet_sensor_one = sense1;
          EEPROM.put(10, wet_sensor_one);
          lcd.clear();
          delay(500); // user gets a better experience switch to next screen?
          break;
        }
      }


      while (0 == 0) {
        readsensors();
        lcd.setCursor(0, 0);
        lcd.print("Sensor2 analog A3");
        lcd.setCursor(0, 1);
        lcd.print("Clean Dry in Air?");
        lcd.setCursor(9, 3);
        lcd.print(sense2);
        delay(250);
        if (SetButton() == LOW) {        // LOW setbutton is pressed
          while (SetButton() == LOW) {
            /*wait for button released*/
          }
          dry_sensor_two = sense2;
          EEPROM.put(15, dry_sensor_two);
          lcd.clear();
          delay(500); // user gets a better experience switch to next screen?
          break;
        }
      }


      while (0 == 0) {
        readsensors();
        lcd.setCursor(0, 0);
        lcd.print("Sensor2 analog A3");
        lcd.setCursor(0, 1);
        lcd.print("Wet in Watter?");
        lcd.setCursor(9, 3);
        lcd.print(sense2);
        delay(250);
        if (SetButton() == LOW) {        // LOW setbutton is pressed
          while (SetButton() == LOW) {
            /*wait for button released*/
          }
          wet_sensor_two = sense2;
          EEPROM.put(20, wet_sensor_two);
          lcd.clear();
          delay(500); // user gets a better experience switch to next screen?
          Calibrate_Sensors = 3;
          break;
        }
      }
    }// end calibrate sensors==2




    // Calibrate_Sensors == 3 Calibrate_Sensors == 3 Calibrate_Sensors == 3 Calibrate_Sensors == 3 Calibrate_Sensors == 3 Calibrate_Sensors == 3
    if (Calibrate_Sensors == 3) {   // you chose Adjust
      Calibrate_Sensors = 1;
      //   lcd.clear();
      //   lcd.print("Adjust not yet!");
      //   delay(2000);
      lcd.clear();
      while (exitflag == 0) {
        readsensors();
        lcd.setCursor(0, 0);
        lcd.print("actual   dry   wet ");
        lcd.setCursor(0, 1);
        lcd.print("s1 "); lcd.print(sense1); lcd.setCursor(9, 1); lcd.print(dry_sensor_one); lcd.setCursor(15, 1); lcd.print(wet_sensor_one);
        lcd.setCursor(0, 2);
        lcd.print("s2 "); lcd.print(sense2); lcd.setCursor(9, 2); lcd.print(dry_sensor_two); lcd.setCursor(15, 2); lcd.print(wet_sensor_two);
        lcd.setCursor(0, 3);
        lcd.print("s1 "); lcd.print(map(sense1, dry_sensor_one, wet_sensor_one, 0, 100)); lcd.print("% ");
        lcd.setCursor(9, 3);
        lcd.print("s2 "); lcd.print(map(sense2, dry_sensor_two, wet_sensor_two, 0, 100)); lcd.print("% ");


        // dry_sensor_one dry_sensor_one dry_sensor_one dry_sensor_one dry_sensor_one dry_sensor_one dry_sensor_one dry_sensor_one dry_sensor_one
        if (exitflag == 0) {
          while (1 == 1) {
            lcd.setCursor(8, 1); lcd.print("["); lcd.setCursor(12, 1); lcd.print("]");
            float rval;
            if ( rval = read_rotary() ) {
              dry_sensor_one = dry_sensor_one + (rval);
              lcd.setCursor(8, 1); lcd.print("["); lcd.setCursor(12, 1); lcd.print("]");
              readsensors();
              lcd.setCursor(0, 0);
              lcd.print("actual   dry   wet ");
              lcd.setCursor(0, 1);
              lcd.print("s1 "); lcd.print(sense1); lcd.setCursor(9, 1); lcd.print(dry_sensor_one); lcd.setCursor(15, 1); lcd.print(wet_sensor_one);
              lcd.setCursor(0, 2);
              lcd.print("s2 "); lcd.print(sense2); lcd.setCursor(9, 2); lcd.print(dry_sensor_two); lcd.setCursor(15, 2); lcd.print(wet_sensor_two);
              lcd.setCursor(0, 3);
              lcd.print("s1 "); lcd.print(map(sense1, dry_sensor_one, wet_sensor_one, 0, 100)); lcd.print("% ");
              lcd.setCursor(9, 3);
              lcd.print("s2 "); lcd.print(map(sense2, dry_sensor_two, wet_sensor_two, 0, 100)); lcd.print("% ");
            }

            if (SetButton() == LOW) {                                    // if setbutton==LOW, pulled up by resistor, LOW is pressed
              TempLong = millis();
              while (SetButton() == LOW) {
                lcd.setCursor(0, 0);
                lcd.print("long press exit    ");
                if ((millis() - TempLong)  > 2500) {                       // after 5 seconds pressed we get into menu system
                  lcd.clear();
                  lcd.setCursor(0, 0);
                  lcd.print(F("Ok, EXIT  "));
                  lcd.setCursor(0, 3);
                  lcd.print(F("   Release Button   "));
                  delay(500);
                  exitflag = 1;
                }
              }
              lcd.setCursor(0, 0);
              lcd.print("actual   dry   wet ");
              EEPROM.put(5, dry_sensor_one);
              lcd.setCursor(8, 1); lcd.print(" "); lcd.setCursor(12, 1); lcd.print(" ");
              break;
            }
          }
        }



        // wet_sensor_one wet_sensor_one wet_sensor_one wet_sensor_one wet_sensor_one wet_sensor_one wet_sensor_one wet_sensor_one wet_sensor_one wet_sensor_one
        if (exitflag == 0) {
          while (1 == 1) {
            lcd.setCursor(14, 1); lcd.print("["); lcd.setCursor(18, 1); lcd.print("]");
            float rval;
            if ( rval = read_rotary() ) {
              wet_sensor_one = wet_sensor_one + (rval);
              lcd.setCursor(14, 1); lcd.print("["); lcd.setCursor(18, 1); lcd.print("]");
              readsensors();
              lcd.setCursor(0, 0);
              lcd.print("actual   dry   wet ");
              lcd.setCursor(0, 1);
              lcd.print("s1 "); lcd.print(sense1); lcd.setCursor(9, 1); lcd.print(dry_sensor_one); lcd.setCursor(15, 1); lcd.print(wet_sensor_one);
              lcd.setCursor(0, 2);
              lcd.print("s2 "); lcd.print(sense2); lcd.setCursor(9, 2); lcd.print(dry_sensor_two); lcd.setCursor(15, 2); lcd.print(wet_sensor_two);
              lcd.setCursor(0, 3);
              lcd.print("s1 "); lcd.print(map(sense1, dry_sensor_one, wet_sensor_one, 0, 100)); lcd.print("% ");
              lcd.setCursor(9, 3);
              lcd.print("s2 "); lcd.print(map(sense2, dry_sensor_two, wet_sensor_two, 0, 100)); lcd.print("% ");
            }

            if (SetButton() == LOW) {                                    // if setbutton==LOW, pulled up by resistor, LOW is pressed
              TempLong = millis();
              while (SetButton() == LOW) {
                lcd.setCursor(0, 0);
                lcd.print("long press exit    ");
                if ((millis() - TempLong)  > 2500) {                       // after 5 seconds pressed we get into menu system
                  lcd.clear();
                  lcd.setCursor(0, 0);
                  lcd.print(F("Ok, EXIT  "));
                  lcd.setCursor(0, 3);
                  lcd.print(F("   Release Button   "));
                  delay(500);
                  exitflag = 1;
                }
              }
              lcd.setCursor(0, 0);
              lcd.print("actual   dry   wet ");
              EEPROM.put(10, wet_sensor_one);
              lcd.setCursor(14, 1); lcd.print(" "); lcd.setCursor(18, 1); lcd.print(" ");
              break;
            }
          }
        }



        // dry_sensor_two dry_sensor_two dry_sensor_two dry_sensor_two dry_sensor_two dry_sensor_two dry_sensor_two dry_sensor_two dry_sensor_two
        if (exitflag == 0) {
          while (1 == 1) {
            lcd.setCursor(8, 2); lcd.print("["); lcd.setCursor(12, 2); lcd.print("]");
            float rval;
            if ( rval = read_rotary() ) {
              dry_sensor_two = dry_sensor_two + (rval);
              lcd.setCursor(8, 2); lcd.print("["); lcd.setCursor(12, 2); lcd.print("]");
              readsensors();
              lcd.setCursor(0, 0);
              lcd.print("actual   dry   wet ");
              lcd.setCursor(0, 1);
              lcd.print("s1 "); lcd.print(sense1); lcd.setCursor(9, 1); lcd.print(dry_sensor_one); lcd.setCursor(15, 1); lcd.print(wet_sensor_one);
              lcd.setCursor(0, 2);
              lcd.print("s2 "); lcd.print(sense2); lcd.setCursor(9, 2); lcd.print(dry_sensor_two); lcd.setCursor(15, 2); lcd.print(wet_sensor_two);
              lcd.setCursor(0, 3);
              lcd.print("s1 "); lcd.print(map(sense1, dry_sensor_one, wet_sensor_one, 0, 100)); lcd.print("% ");
              lcd.setCursor(9, 3);
              lcd.print("s2 "); lcd.print(map(sense2, dry_sensor_two, wet_sensor_two, 0, 100)); lcd.print("% ");
            }

            if (SetButton() == LOW) {                                    // if setbutton==LOW, pulled up by resistor, LOW is pressed
              TempLong = millis();
              while (SetButton() == LOW) {
                lcd.setCursor(0, 0);
                lcd.print("long press exit    ");
                if ((millis() - TempLong)  > 2500) {                       // after 5 seconds pressed we get into menu system
                  lcd.clear();
                  lcd.setCursor(0, 0);
                  lcd.print(F("Ok, EXIT  "));
                  lcd.setCursor(0, 3);
                  lcd.print(F("   Release Button   "));
                  delay(500);
                  exitflag = 1;
                }
              }
              lcd.setCursor(0, 0);
              lcd.print("actual   dry   wet ");
              EEPROM.put(15, dry_sensor_two);
              lcd.setCursor(8, 2); lcd.print(" "); lcd.setCursor(12, 2); lcd.print(" ");
              break;
            }
          }
        }


        // wet_sensor_two wet_sensor_two wet_sensor_two wet_sensor_two wet_sensor_two wet_sensor_two wet_sensor_two wet_sensor_two wet_sensor_two
        if (exitflag == 0) {
          while (1 == 1) {
            lcd.setCursor(14, 2); lcd.print("["); lcd.setCursor(18, 2); lcd.print("]");
            float rval;
            if ( rval = read_rotary() ) {
              wet_sensor_two = wet_sensor_two + (rval);
              lcd.setCursor(14, 2); lcd.print("["); lcd.setCursor(18, 2); lcd.print("]");
              readsensors();
              lcd.setCursor(0, 0);
              lcd.print("actual   dry   wet  ");
              lcd.setCursor(0, 1);
              lcd.print("s1 "); lcd.print(sense1); lcd.setCursor(9, 1); lcd.print(dry_sensor_one); lcd.setCursor(15, 1); lcd.print(wet_sensor_one);
              lcd.setCursor(0, 2);
              lcd.print("s2 "); lcd.print(sense2); lcd.setCursor(9, 2); lcd.print(dry_sensor_two); lcd.setCursor(15, 2); lcd.print(wet_sensor_two);
              lcd.setCursor(0, 3);
              lcd.print("s1 "); lcd.print(map(sense1, dry_sensor_one, wet_sensor_one, 0, 100)); lcd.print("% ");
              lcd.setCursor(9, 3);
              lcd.print("s2 "); lcd.print(map(sense2, dry_sensor_two, wet_sensor_two, 0, 100)); lcd.print("% ");
            }

            if (SetButton() == LOW) {                                    // if setbutton==LOW, pulled up by resistor, LOW is pressed
              TempLong = millis();
              while (SetButton() == LOW) {
                lcd.setCursor(0, 0);
                lcd.print("long press exit    ");
                if ((millis() - TempLong)  > 2500) {                       // after 5 seconds pressed we get into menu system
                  lcd.clear();
                  lcd.setCursor(0, 0);
                  lcd.print(F("Ok, EXIT  "));
                  lcd.setCursor(0, 3);
                  lcd.print(F("   Release Button   "));
                  delay(500);
                  exitflag = 1;
                }
              }
              lcd.setCursor(0, 0);
              lcd.print("actual   dry   wet  ");
              EEPROM.put(20, wet_sensor_two);
              lcd.setCursor(14, 2); lcd.print(" "); lcd.setCursor(18, 2); lcd.print(" ");
              break;
            }

          }
        }
      }
    }   // end    if (Calibrate_Sensors == 3) {   // you chose Adjust
    lcd.clear();
    exitflag = 0;




    // 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3
    // watering_duration watering_duration watering_duration watering_duration watering_duration
    TempLong = millis();  //  load current millis() into TempLong
    if (menu_nr == 3) {
      lcd.setCursor(0, 0);
      lcd.print(F("3 Watering T in Sec."));
      lcd.setCursor(6, 2);
      lcd.print(watering_duration);
      lcd.print(F(" Sec. "));
    }
    while (menu_nr == 3) {
      lcd.setCursor(18, 3);
      if ((10 - (millis() - TempLong) / 1000) <= 9)lcd.print(" ");      // move 1 char when smaller a 10 wich is 2 chars
      lcd.print(10 - (millis() - TempLong) / 1000);                     // on lcd timeout countdown
      if ((millis() - TempLong)  > 10000) {
        delay(1000);  // want to see the zero 0
        TimeOut();
        break;
      }

      float rval;
      if ( rval = read_rotary() ) {
        if (watering_duration >= 600)watering_duration = watering_duration + (rval * 30);        // 30+24+6=60 second steps above 600 seconds
        if (watering_duration >= 300)watering_duration = watering_duration + (rval * 24);        // 24+6=30 second steps above 300 seconds
        if (watering_duration >= 60)watering_duration = watering_duration + (rval * 6);          // 6 second steps above 60 seconds
        if (watering_duration <= 60)watering_duration = watering_duration + (rval);              // 1 second steps
        if (watering_duration <= 2) watering_duration = 2;                                        // minimal watering time 2 seconds
        TempLong = millis();  //  load current millis() into TempLong
        lcd.setCursor(6, 2);
        lcd.print(watering_duration);
        lcd.print(F(" Sec. "));
        lcd.setCursor(6, 3);
        float wateringtime = watering_duration;
        float minutetime = (wateringtime / 60);
        lcd.print(minutetime, 1);
        lcd.print(F(" Min. "));

      }

      if (SetButton() == LOW) {                                    // if setbutton==LOW, pulled up by resistor, LOW is pressed
        while (SetButton() == LOW) {
          /*wait for button released*/
        }
        menu_nr = 4;
        lcd.clear();

        EEPROM.get(25, TempInt);                                   // limmited write to eeprom = read is unlimmited
        if (watering_duration != TempInt) {            // only write to eeprom if value is different
          EEPROM.put(25, watering_duration);            // put already checks if val is needed to write
          lcd.setCursor(0, 0);
          lcd.print(F("Saving to EEPROM"));
          lcd.setCursor(0, 2);
          lcd.print("old= ");
          lcd.print(TempInt);
          lcd.print(F(" new= "));
          lcd.print(watering_duration);
          TempLong = millis();                                    // load millis() into Templong for next countdown delay
          while ((millis() - TempLong)  <= 5000) {
            lcd.setCursor(19, 3);
            lcd.print(5 - (millis() - TempLong) / 1000);          // on lcd timeout countdown
          }
          delay(1000);  // want to see the zero 0
          // for (int i = 0; i < 10; i++)Serial.println(F("watering_duration DATA WRITTEN / SAVED TO EEPROM "));
          lcd.clear();
        }


      }
    }// end menu_nr 3



    // 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4
    // pauze_after_watering pauze_after_watering pauze_after_watering pauze_after_watering pauze_after_watering
    TempLong = millis();  //  load current millis() into TempLong
    if (menu_nr == 4) {
      lcd.setCursor(0, 0);
      lcd.print(F("4 PauzeTime T in Min"));
      lcd.setCursor(6, 2);
      lcd.print(pauze_after_watering);
      lcd.print(F(" Min. "));
    }
    while (menu_nr == 4) {
      lcd.setCursor(18, 3);
      if ((10 - (millis() - TempLong) / 1000) <= 9)lcd.print(" ");      // move 1 char when smaller a 10 wich is 2 chars
      lcd.print(10 - (millis() - TempLong) / 1000);                     // on lcd timeout countdown
      if ((millis() - TempLong)  > 10000) {
        delay(1000);  // want to see the zero 0
        TimeOut();
        break;
      }

      float rval;
      if ( rval = read_rotary() ) {
        pauze_after_watering = pauze_after_watering + (rval);          // 1 minute steps
        if (pauze_after_watering <= 1) pauze_after_watering = 1;       // minimal pauzetime 1 minutes
        TempLong = millis();  //  load current millis() into TempLong
        lcd.setCursor(6, 2);
        lcd.print(pauze_after_watering);
        lcd.print(F(" Min. "));

      }

      if (SetButton() == LOW) {                                    // if setbutton==LOW, pulled up by resistor, LOW is pressed
        while (SetButton() == LOW) {
          /*wait for button released*/
        }
        menu_nr = 5;
        lcd.clear();

        EEPROM.get(30, TempInt);                                   // limmited write to eeprom = read is unlimmited
        if (pauze_after_watering != TempInt) {            // only write to eeprom if value is different
          EEPROM.put(30, pauze_after_watering);            // put already checks if val is needed to write
          lcd.setCursor(0, 0);
          lcd.print(F("Saving to EEPROM"));
          lcd.setCursor(0, 2);
          lcd.print("old= ");
          lcd.print(TempInt);
          lcd.print(F(" new= "));
          lcd.print(pauze_after_watering);
          TempLong = millis();                                    // load millis() into Templong for next countdown delay
          while ((millis() - TempLong)  <= 5000) {
            lcd.setCursor(19, 3);
            lcd.print(5 - (millis() - TempLong) / 1000);          // on lcd timeout countdown
          }
          delay(1000);  // want to see the zero 0
          // for (int i = 0; i < 10; i++)Serial.println(F("pauze_after_watering DATA WRITTEN / SAVED TO EEPROM "));
          lcd.clear();
        }


      }
    }// end menu_nr 4




    // 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5
    // maximumaantalbeurtenperdag maximumaantalbeurtenperdag maximumaantalbeurtenperdag maximumaantalbeurtenperdag
    TempLong = millis();  //  load current millis() into TempLong
    if (menu_nr == 5) {
      lcd.setCursor(0, 0);
      lcd.print(F("5 Max water count"));
      lcd.setCursor(9, 2);
      lcd.print(maximumaantalbeurtenperdag);
      lcd.print(F(" "));
    }
    while (menu_nr == 5) {
      lcd.setCursor(18, 3);
      if ((10 - (millis() - TempLong) / 1000) <= 9)lcd.print(" ");      // move 1 char when smaller a 10 wich is 2 chars
      lcd.print(10 - (millis() - TempLong) / 1000);                     // on lcd timeout countdown
      if ((millis() - TempLong)  > 10000) {
        delay(1000);  // want to see the zero 0
        TimeOut();
        break;
      }

      float rval;
      if ( rval = read_rotary() ) {
        maximumaantalbeurtenperdag = maximumaantalbeurtenperdag + (rval);          // 1  step
        if (maximumaantalbeurtenperdag <= 2) maximumaantalbeurtenperdag = 2;       // minimal maximumaantalbeurtenperdag
        if (maximumaantalbeurtenperdag >= 90) maximumaantalbeurtenperdag = 90;       // eeprom 110 to 1010 not beyond  1020 otherwise overwrites 0to100
        TempLong = millis();  //  load current millis() into TempLong
        lcd.setCursor(9, 2);
        lcd.print(maximumaantalbeurtenperdag);
        lcd.print(F(" "));

      }

      if (SetButton() == LOW) {                                    // if setbutton==LOW, pulled up by resistor, LOW is pressed
        while (SetButton() == LOW) {
          /*wait for button released*/
        }
        menu_nr = 6;
        lcd.clear();

        EEPROM.get(35, TempByte);                                   // limmited write to eeprom = read is unlimmited
        if (maximumaantalbeurtenperdag != TempByte) {            // only write to eeprom if value is different
          EEPROM.put(35, maximumaantalbeurtenperdag);            // put already checks if val is needed to write
          lcd.setCursor(0, 0);
          lcd.print(F("Saving to EEPROM"));
          lcd.setCursor(0, 2);
          lcd.print("old= ");
          lcd.print(TempByte);
          lcd.print(F(" new= "));
          lcd.print(maximumaantalbeurtenperdag);
          TempLong = millis();                                    // load millis() into Templong for next countdown delay
          while ((millis() - TempLong)  <= 5000) {
            lcd.setCursor(19, 3);
            lcd.print(5 - (millis() - TempLong) / 1000);          // on lcd timeout countdown
          }
          delay(1000);  // want to see the zero 0
          // for (int i = 0; i < 10; i++)Serial.println(F("maximumaantalbeurtenperdag DATA WRITTEN / SAVED TO EEPROM "));
          lcd.clear();
        }


      }
    }// end menu_nr 5



    // 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6
    // start_hour start_hour start_hour start_hour start_hour start_hour
    TempLong = millis();  //  load current millis() into TempLong
    if (menu_nr == 6) {
      lcd.setCursor(0, 0);
      lcd.print(F("6 Start Hour water"));
      lcd.setCursor(9, 2);
      lcd.print(start_hour);
      lcd.print(F(" "));
    }
    while (menu_nr == 6) {
      lcd.setCursor(18, 3);
      if ((10 - (millis() - TempLong) / 1000) <= 9)lcd.print(" ");      // move 1 char when smaller a 10 wich is 2 chars
      lcd.print(10 - (millis() - TempLong) / 1000);                     // on lcd timeout countdown
      if ((millis() - TempLong)  > 10000) {
        delay(1000);  // want to see the zero 0
        TimeOut();
        break;
      }

      float rval;
      if ( rval = read_rotary() ) {
        start_hour = start_hour + (rval);          // 1  step
        if (start_hour <= 0) start_hour = 0;       // start_hour
        if (start_hour >= 23) start_hour = 23;     // start_hour max 12 o clock
        //should check that start_hour < end_hour
        TempLong = millis();  //  load current millis() into TempLong
        lcd.setCursor(9, 2);
        lcd.print(start_hour);
        lcd.print(F(" "));

      }

      if (SetButton() == LOW) {                                    // if setbutton==LOW, pulled up by resistor, LOW is pressed
        while (SetButton() == LOW) {
          /*wait for button released*/
        }
        menu_nr = 7;
        lcd.clear();

        EEPROM.get(40, TempInt);                                   // limmited write to eeprom = read is unlimmited
        if (start_hour != TempInt) {                       // only write to eeprom if value is different
          EEPROM.put(40, start_hour);                       // put already checks if val is needed to write
          lcd.setCursor(0, 0);
          lcd.print(F("Saving to EEPROM"));
          lcd.setCursor(0, 2);
          lcd.print("old= ");
          lcd.print(TempInt);
          lcd.print(F(" new= "));
          lcd.print(start_hour);
          TempLong = millis();                                    // load millis() into Templong for next countdown delay
          while ((millis() - TempLong)  <= 5000) {
            lcd.setCursor(19, 3);
            lcd.print(5 - (millis() - TempLong) / 1000);          // on lcd timeout countdown
          }
          delay(1000);  // want to see the zero 0
          // for (int i = 0; i < 10; i++)Serial.println(F("start_hour DATA WRITTEN / SAVED TO EEPROM "));
          lcd.clear();
        }


      }
    }// end menu_nr 6



    // 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7
    // end_hour end_hour end_hour end_hour end_hour end_hour end_hour
    TempLong = millis();  //  load current millis() into TempLong
    if (menu_nr == 7) {
      lcd.setCursor(0, 0);
      lcd.print(F("7 End Hour water"));
      lcd.setCursor(9, 2);
      lcd.print(end_hour);
      lcd.print(F(" "));
    }
    while (menu_nr == 7) {
      lcd.setCursor(18, 3);
      if ((10 - (millis() - TempLong) / 1000) <= 9)lcd.print(" ");      // move 1 char when smaller a 10 wich is 2 chars
      lcd.print(10 - (millis() - TempLong) / 1000);                     // on lcd timeout countdown
      if ((millis() - TempLong)  > 10000) {
        delay(1000);  // want to see the zero 0
        TimeOut();
        break;
      }

      float rval;
      if ( rval = read_rotary() ) {
        end_hour = end_hour + (rval);          // 1  step
        if (end_hour <= 0) end_hour = 0;       // end_hour
        if (end_hour >= 23) end_hour = 23;     // end_hour max 12 o clock
        //should check that start_hour < end_hour
        TempLong = millis();                                //  load current millis() into TempLong
        lcd.setCursor(9, 2);
        lcd.print(end_hour);
        lcd.print(F(" "));

      }

      if (SetButton() == LOW) {                         // if setbutton==LOW, pulled up by resistor, LOW is pressed
        while (SetButton() == LOW) {
          /*wait for button released*/
        }
        menu_nr = 8;
        lcd.clear();

        EEPROM.get(45, TempInt);                          // limmited write to eeprom = read is unlimmited
        if (end_hour != TempInt) {                       // only write to eeprom if value is different
          EEPROM.put(45, end_hour);                       // put already checks if val is needed to write
          lcd.setCursor(0, 0);
          lcd.print(F("Saving to EEPROM"));
          lcd.setCursor(0, 2);
          lcd.print("old= ");
          lcd.print(TempInt);
          lcd.print(F(" new= "));
          lcd.print(end_hour);
          TempLong = millis();                                    // load millis() into Templong for next countdown delay
          while ((millis() - TempLong)  <= 5000) {
            lcd.setCursor(19, 3);
            lcd.print(5 - (millis() - TempLong) / 1000);          // on lcd timeout countdown
          }
          delay(1000);  // want to see the zero 0
          // for (int i = 0; i < 10; i++)Serial.println(F("end_hour DATA WRITTEN / SAVED TO EEPROM "));
          lcd.clear();
        }


      }
    }// end menu_nr 7



    // 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8
    // backlightofftimeout backlightofftimeout backlightofftimeout backlightofftimeout backlightofftimeout
    TempLong = millis();  //  load current millis() into TempLong
    if (menu_nr == 8) {
      lcd.setCursor(0, 0);
      lcd.print(F("8 Backlight T in Min"));
      lcd.setCursor(6, 2);
      lcd.print(backlightofftimeout);
      lcd.print(F(" Min. "));
    }
    while (menu_nr == 8) {
      lcd.setCursor(18, 3);
      if ((10 - (millis() - TempLong) / 1000) <= 9)lcd.print(" ");      // move 1 char when smaller a 10 wich is 2 chars
      lcd.print(10 - (millis() - TempLong) / 1000);                     // on lcd timeout countdown
      if ((millis() - TempLong)  > 10000) {
        delay(1000);  // want to see the zero 0
        TimeOut();
        break;
      }

      float rval;
      if ( rval = read_rotary() ) {
        backlightofftimeout = backlightofftimeout + (rval);          // 1 minute steps
        if (backlightofftimeout <= 1) backlightofftimeout = 1;       // minimal backlightofftimeout 1 minutes
        TempLong = millis();  //  load current millis() into TempLong
        lcd.setCursor(6, 2);
        lcd.print(backlightofftimeout);
        lcd.print(F(" Min. "));

      }

      if (SetButton() == LOW) {                                    // if setbutton==LOW, pulled up by resistor, LOW is pressed
        while (SetButton() == LOW) {
          /*wait for button released*/
        }
        menu_nr = 9;
        lcd.clear();

        EEPROM.get(50, TempInt);                                   // limmited write to eeprom = read is unlimmited
        if (backlightofftimeout != TempInt) {            // only write to eeprom if value is different
          EEPROM.put(50, backlightofftimeout);            // put already checks if val is needed to write
          lcd.setCursor(0, 0);
          lcd.print(F("Saving to EEPROM"));
          lcd.setCursor(0, 2);
          lcd.print("old= ");
          lcd.print(TempInt);
          lcd.print(F(" new= "));
          lcd.print(backlightofftimeout);
          TempLong = millis();                                    // load millis() into Templong for next countdown delay
          while ((millis() - TempLong)  <= 5000) {
            lcd.setCursor(19, 3);
            lcd.print(5 - (millis() - TempLong) / 1000);          // on lcd timeout countdown
          }
          delay(1000);  // want to see the zero 0
          // for (int i = 0; i < 10; i++)Serial.println(F("pauze_after_watering DATA WRITTEN / SAVED TO EEPROM "));
          lcd.clear();
        }


      }
    }// end menu_nr 8



    // 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9 9
    // wateringtimeslog wateringtimeslog wateringtimeslog wateringtimeslog wateringtimeslog wateringtimeslog wateringtimeslog wateringtimeslog
    TempLong = millis();  //  load current millis() into TempLong
    if (menu_nr == 9) {

      lcd.setCursor(0, 0);
      lcd.print(F("9 wateringtime log V"));
      char date[10] = "hh:mm:ss";  // maybe to eprom
      lcd.setCursor(0, 1); lcd.print("1  "); EEPROM.get(110, date); lcd.print(date); lcd.print("    ");
      lcd.setCursor(0, 2); lcd.print("2  "); EEPROM.get(120, date); lcd.print(date); lcd.print("    ");
      lcd.setCursor(0, 3); lcd.print("3  "); EEPROM.get(130, date); lcd.print(date); lcd.print("    ");
      TempInt = 0;
    }
    while (menu_nr == 9) {
      lcd.setCursor(18, 3);
      if ((60 - (millis() - TempLong) / 1000) <= 9)lcd.print(" ");      // move 1 char when smaller a 10 wich is 2 chars
      lcd.print(60 - (millis() - TempLong) / 1000);                     // on lcd timeout countdown
      if ((millis() - TempLong)  > 60000) {
        delay(1000);  // want to see the zero 0
        TimeOut();
        break;
      }

      float rval;
      if ( rval = read_rotary() ) {
        TempInt = TempInt + (rval * 10);        // 10  steps
        if (TempInt <= 0) TempInt = 0;
        if (TempInt >= (maximumaantalbeurtenperdag * 10) - 30) TempInt = (maximumaantalbeurtenperdag * 10) - 30;
        // Serial.println(TempInt);

        TempLong = millis();  //  load current millis() into TempLong
        char date[10] = "hh:mm:ss";  // maybe to eprom
        int adress = 110 + TempInt;
        lcd.setCursor(0, 1); lcd.print(ClearLine);
        if (TempInt / 10 + 1 <= maximumaantalbeurtenperdag) {
          lcd.setCursor(0, 1); lcd.print(TempInt / 10 + 1); lcd.print("  "); /*lcd.print(adress); lcd.print(" ");*/ EEPROM.get(adress, date); lcd.print(date);

        }
        lcd.setCursor(0, 2); lcd.print(ClearLine);
        if (TempInt / 10 + 2 <= maximumaantalbeurtenperdag) {
          lcd.setCursor(0, 2); lcd.print(TempInt / 10 + 2); lcd.print("  "); /*lcd.print(adress + 10); lcd.print(" ");*/ EEPROM.get(adress + 10, date); lcd.print(date);
        }
        lcd.setCursor(0, 3); lcd.print(ClearLine);
        if (TempInt / 10 + 3 <= maximumaantalbeurtenperdag) {
          lcd.setCursor(0, 3); lcd.print(TempInt / 10 + 3); lcd.print("  ");  EEPROM.get(adress + 20, date); lcd.print(date); lcd.print(" "); lcd.print(adress + 20); lcd.print(" "); /**/

        }
      }


      if (SetButton() == LOW) {                                    // if setbutton==LOW, pulled up by resistor, LOW is pressed
        while (SetButton() == LOW) {
          /*wait for button released*/
        }
        menu_nr = 10;
        lcd.clear();
      }



    }// end menu_nr 9



    // 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10
    // set time date set time date set time date set time date set time date set time date set time date set time date
    TempLong = millis();  //  load current millis() into TempLong
    if (menu_nr == 10) {
      lcd.setCursor(0, 0);
      lcd.print(F("10 Set Time / Date"));
      lcd.setCursor(7, 2);
      if (settimedate == 1)lcd.print(F(" No   "));
      if (settimedate == 2)lcd.print(F(" Yes  "));

    }
    while (menu_nr == 10) {

      lcd.setCursor(18, 3);
      if ((10 - (millis() - TempLong) / 1000) <= 9)lcd.print(" ");      // move 1 char when smaller a 10 wich is 2 chars
      lcd.print(10 - (millis() - TempLong) / 1000);                     // on lcd timeout countdown
      if ((millis() - TempLong)  > 10000) {
        delay(1000);  // want to see the zero 0
        TimeOut();
        break;
      }

      float rval;
      if ( rval = read_rotary() ) {
        settimedate  = settimedate  + rval;
        TempLong = millis();  //reset innactive time counter
        if (settimedate < 1)settimedate = 2;
        if (settimedate > 2)settimedate = 1;
        lcd.setCursor(7, 2);
        if (settimedate == 1)lcd.print(F(" No   "));
        if (settimedate == 2)lcd.print(F(" Yes  "));
      }

      if (SetButton() == LOW) {        // LOW setbutton is pressed
        while (SetButton() == LOW) {
          /*wait for button released*/
        }
        menu_nr = 11;
        lcd.clear();
        delay(250);
      }
    }// end menu_nr 10






    if (settimedate == 2) {
      settimedate = 1;
      lcd.clear();
      TempLong = millis();  //  load current millis() into TempLong

      DateTime now = rtc.now();
      int uur = now.hour();
      int minuut = now.minute();
      int seconde = now.second();

      int dag = now.day();
      int maand = now.month();
      int jaar = now.year();

      while (1 == 1) {

        DateTime now = rtc.now();

        lcd.setCursor(3, 1);
        if (uur <= 9)lcd.print(" ");
        lcd.print(uur);
        lcd.print(" : ");
        if (minuut <= 9)lcd.print("0");
        lcd.print(minuut);
        lcd.print(" : ");
        if (seconde <= 9)lcd.print("0");
        lcd.print(seconde);

        lcd.setCursor(3, 2);
        if (dag <= 9)lcd.print(" ");
        lcd.print(dag);
        lcd.print(" - ");
        if (maand <= 9)lcd.print(" ");
        lcd.print(maand);
        lcd.print(" - ");
        lcd.print(jaar);

        lcd.setCursor(4, 3);
        lcd.print("Set & Exit");

        lcd.setCursor(0, 0);
        lcd.print(now.hour());
        lcd.print(':');
        if (now.minute() <= 9)lcd.print('0');
        lcd.print(now.minute());
        lcd.print(':');
        if (now.second() <= 9)lcd.print('0');
        lcd.print(now.second());
        lcd.print(" ");
        lcd.print(now.day());
        lcd.print("-");
        lcd.print(now.month());
        lcd.print("-");
        lcd.print(now.year());


        // uur uur uur uur uur uur uur uur uur uur uur uur uur uur uur uur uur uur uur uur uur uur
        while (1 == 1) {

          // uurselect on
          lcd.setCursor(2, 1); lcd.print("["); lcd.setCursor(5, 1); lcd.print("]");
          float rval;
          if ( rval = read_rotary() ) {
            uur = uur + (rval);
            if (uur < 0 )uur = 23;
            if (uur > 23 )uur = 0;
            lcd.setCursor(3, 1);
            if (uur <= 9)lcd.print(" ");
            lcd.print(uur);
          }

          if (SetButton() == LOW) {                                    // if setbutton==LOW, pulled up by resistor, LOW is pressed
            while (SetButton() == LOW) {
              /*wait for button released*/
            }
            // uurselect off
            lcd.setCursor(2, 1); lcd.print(" "); lcd.setCursor(5, 1); lcd.print(" ");
            break;
          }
        }


        // minuut minuut minuut minuut minuut minuut minuut minuut minuut minuut minuut minuut minuut
        while (1 == 1) {

          // minuutselect
          lcd.setCursor(7, 1); lcd.print("["); lcd.setCursor(10, 1); lcd.print("]");
          float rval;
          if ( rval = read_rotary() ) {
            minuut = minuut + (rval);
            if (minuut < 0 )minuut = 59;
            if (minuut > 59 )minuut = 0;
            lcd.setCursor(8, 1);
            if (minuut <= 9)lcd.print("0");
            lcd.print(minuut);
          }

          if (SetButton() == LOW) {                                    // if setbutton==LOW, pulled up by resistor, LOW is pressed
            while (SetButton() == LOW) {
              /*wait for button released*/
            }
            // minuutselect
            lcd.setCursor(7, 1); lcd.print(" "); lcd.setCursor(10, 1); lcd.print(" ");
            break;
          }
        }


        // seconde seconde seconde seconde seconde seconde seconde seconde seconde seconde seconde seconde
        while (1 == 1) {

          // secondeselect
          lcd.setCursor(12, 1); lcd.print("["); lcd.setCursor(15, 1); lcd.print("]");
          float rval;
          if ( rval = read_rotary() ) {
            seconde = seconde + (rval);
            if (seconde < 0 )seconde = 59;
            if (seconde > 59 )seconde = 0;
            lcd.setCursor(13, 1);
            if (seconde <= 9)lcd.print("0");
            lcd.print(seconde);
          }

          if (SetButton() == LOW) {                                    // if setbutton==LOW, pulled up by resistor, LOW is pressed
            while (SetButton() == LOW) {
              /*wait for button released*/
            }
            // secondeselect
            lcd.setCursor(12, 1); lcd.print(" "); lcd.setCursor(15, 1); lcd.print(" ");
            break;
          }
        }


        // dag dag dag dag dag dag dag dag dag dag dag dag dag dag dag dag dag dag dag dag dag dag dag
        while (1 == 1) {

          // dagselect
          lcd.setCursor(2, 2); lcd.print("["); lcd.setCursor(5, 2); lcd.print("]");
          float rval;
          if ( rval = read_rotary() ) {
            dag = dag + (rval);
            if (dag < 1 )dag = 31;
            if (dag > 31 )dag = 1;
            lcd.setCursor(3, 2);
            if (dag <= 9)lcd.print(" ");
            lcd.print(dag);
          }

          if (SetButton() == LOW) {                                    // if setbutton==LOW, pulled up by resistor, LOW is pressed
            while (SetButton() == LOW) {
              /*wait for button released*/
            }
            // dagselect
            lcd.setCursor(2, 2); lcd.print(" "); lcd.setCursor(5, 2); lcd.print(" ");
            break;
          }
        }


        // maand maand maand maand maand maand maand maand maand maand maand maand maand maand
        while (1 == 1) {

          // maandselect
          lcd.setCursor(7, 2); lcd.print("["); lcd.setCursor(10, 2); lcd.print("]");
          float rval;
          if ( rval = read_rotary() ) {
            maand = maand + (rval);
            if (maand < 1 )maand = 12;
            if (maand > 12 )maand = 1;
            lcd.setCursor(8, 2);
            if (maand <= 9)lcd.print(" ");
            lcd.print(maand);
          }

          if (SetButton() == LOW) {                                    // if setbutton==LOW, pulled up by resistor, LOW is pressed
            while (SetButton() == LOW) {
              /*wait for button released*/
            }
            // maandselect
            lcd.setCursor(7, 2); lcd.print(" "); lcd.setCursor(10, 2); lcd.print(" ");
            break;
          }
        }


        // jaar jaar jaar jaar jaar jaar jaar jaar jaar jaar jaar jaar jaar jaar jaar jaar jaar jaar jaar
        while (1 == 1) {

          // jaarselect
          lcd.setCursor(12, 2); lcd.print("["); lcd.setCursor(17, 2); lcd.print("]");
          float rval;
          if ( rval = read_rotary() ) {
            jaar = jaar + (rval);
            lcd.setCursor(13, 2);
            lcd.print(jaar);
          }

          if (SetButton() == LOW) {                                    // if setbutton==LOW, pulled up by resistor, LOW is pressed
            while (SetButton() == LOW) {
              /*wait for button released*/
            }
            // jaarselect
            lcd.setCursor(12, 2); lcd.print(" "); lcd.setCursor(17, 2); lcd.print(" ");
            break;
          }
        }


        // saveandexitselect saveandexitselect saveandexitselect saveandexitselect saveandexitselect saveandexitselect saveandexitselect
        while (1 == 1) {

          // saveandexitselect
          lcd.setCursor(3, 3); lcd.print("["); lcd.setCursor(14, 3); lcd.print("]");


          if (SetButton() == LOW) {                                    // if setbutton==LOW, pulled up by resistor, LOW is pressed
            while (SetButton() == LOW) {
              /*wait for button released*/
            }
            lcd.setCursor(12, 2); lcd.print(" "); lcd.setCursor(17, 2); lcd.print(" ");
            rtc.adjust(DateTime(jaar, maand, dag, uur, minuut, seconde));
            exitflag = 1;
            break;
          }


        }

        if (exitflag == 1) {
          exitflag = 0;
          lcd.clear();
          break;
        }

      }
    }


    // 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11
    // EEPROM Erase Reboot
    TempLong = millis();  //  load current millis() into TempLong
    if (menu_nr == 11) {
      lcd.setCursor(0, 0);
      lcd.print(F("EEPROM Erase Reboot"));
      lcd.setCursor(0, 1);
      lcd.print(F("Factory Settings?"));
      lcd.setCursor(9, 3);
      if (eepromerase == 1)lcd.print(F("No "));
      if (eepromerase == 2)lcd.print(F("No"));
      if (eepromerase == 3)lcd.print(F("Yes "));
      if (eepromerase == 4)lcd.print(F("No "));
      if (eepromerase == 5)lcd.print(F("No "));
    }
    while (menu_nr == 11) {

      lcd.setCursor(18, 3);
      if ((10 - (millis() - TempLong) / 1000) <= 9)lcd.print(" ");      // move 1 char when smaller a 10 wich is 2 chars
      lcd.print(10 - (millis() - TempLong) / 1000);                     // on lcd timeout countdown
      if ((millis() - TempLong)  > 10000) {
        delay(1000);  // want to see the zero 0
        TimeOut();
        break;
      }

      float rval;
      if ( rval = read_rotary() ) {
        eepromerase  = eepromerase  + rval;
        TempLong = millis();  //reset innactive time counte
        lcd.setCursor(9, 3);
        lcd.print(eepromerase);
        lcd.print(F(" "));
        if (eepromerase < 1)eepromerase = 5;
        if (eepromerase > 5)eepromerase = 1;
        lcd.setCursor(9, 3);
        if (eepromerase == 1)lcd.print(F("No "));
        if (eepromerase == 2)lcd.print(F("No ")); // prevent acidental yes press
        if (eepromerase == 3)lcd.print(F("Yes"));
        if (eepromerase == 4)lcd.print(F("No "));
        if (eepromerase == 5)lcd.print(F("No "));
      }

      if (SetButton() == LOW) {        // LOW setbutton is pressed
        while (SetButton() == LOW) {
          /*wait for button released*/
        }
        menu_nr = 12;
        lcd.clear();
        delay(250);
      }
    }// end menu_nr 11



    if (eepromerase == 3) {                              // you chose Yes so whe go to erase eeprom
      for (int i = 0 ; i < EEPROM.length() ; i++) {
        lcd.setCursor(3, 2);
        lcd.print("Erase "); lcd.print(i);
        EEPROM.write(i, 0);                             // erase eeprom
      }
      delay(1000);
      lcd.setCursor(3, 2);
      lcd.print("Reboot Now!    ");
      delay(2000);
      asm volatile("jmp 0");                          // reboot
    }



    // 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12
    // backtobegin backtobegin backtobegin backtobegin backtobegin backtobegin backtobegin backtobegin backtobegin backtobegin backtobegin backtobegin
    TempLong = millis();  //  load current millis() into TempLong
    if (menu_nr == 12) {
      lcd.setCursor(0, 0);
      lcd.print(F("12 Menu again?"));
      lcd.setCursor(3, 2);
      if (backtobegin == 1)lcd.print(F("Yes Menu Again"));
      if (backtobegin == 2)lcd.print(F("No Exit Menu  "));
    }
    while (menu_nr == 12) {

      lcd.setCursor(18, 3);
      if ((10 - (millis() - TempLong) / 1000) <= 9)lcd.print(" ");      // move 1 char when smaller a 10 wich is 2 chars
      lcd.print(10 - (millis() - TempLong) / 1000);                     // on lcd timeout countdown
      if ((millis() - TempLong)  > 10000) {
        delay(1000);  // want to see the zero 0
        TimeOut();
        break;
      }

      float rval;
      if ( rval = read_rotary() ) {
        backtobegin  = backtobegin  + rval;
        TempLong = millis();  //reset innactive time counte
        if (backtobegin < 1)backtobegin = 2;
        if (backtobegin > 2)backtobegin = 1;
        lcd.setCursor(3, 2);
        if (backtobegin == 1)lcd.print(F("Yes Menu Again"));
        if (backtobegin == 2)lcd.print(F("No Exit Menu  "));
      }

      if (SetButton() == LOW) {        // LOW setbutton is pressed
        while (SetButton() == LOW) {
          /*wait for button released*/
        }
        if (backtobegin == 1)menu_nr = 1;
        if (backtobegin == 2)menu_nr = 15;
        backtobegin = 1;
        lcd.clear();
        delay(250);
      }
    }// end menu_nr 12




    // 15 15 15 15 15 15 15 15 15 15 15 15 15 15 15 15 15 15 15 15 15 15 15 15 15 15 15 15 15 15 15 15 15 15 15 15 15 15 15 15 15 15 15 15
    // Credits Credits Credits Credits Credits Credits Credits Credits Credits Credits Credits Credits Credits Credits Credits Credits
    TempLong = millis();  //  load current millis() into TempLong
    if (menu_nr == 15) {
      lcd.clear();
      DateTime now = rtc.now();
      lcd.setCursor(0, 0);
      lcd.print(F("Copyright 2020->")); lcd.print(now.year());
      lcd.setCursor(0, 1);
      lcd.print(F("Dirk Luberth Dijkman"));
      lcd.setCursor(0, 2);
      lcd.print(F("Bangert 30 1619GJ"));
      lcd.setCursor(0, 3);
      lcd.print(F("Andijk Holland"));
    }
    while (menu_nr == 15) {
      lcd.setCursor(18, 3);
      if ((3 - (millis() - TempLong) / 1000) <= 9)lcd.print(" ");      // move 1 char when smaller a 10 wich is 2 chars
      lcd.print(3 - (millis() - TempLong) / 1000);                     // on lcd timeout countdown
      if ((millis() - TempLong)  > 3000) {
        delay(1000);  // want to see the zero 0
        lcd.clear();
        menu_nr = 0;
        break;
      }

    }// end menu_nr 15



  } // end    if (menu_nr != 0) {




  DateTime now = rtc.now();



  second_now = now.second();
  if (last_second != second_now) {       // only do this once each second

    last_second = second_now;


    // test for mega 2560
    // future for Mega2560 SD Card log  // not enough space nano uno == 105%    for mega 2560 or mega pro mini
#if (defined(__AVR_ATmega2560__))

    String dataString = "";
    /*
        dataString += now.day();          // leave the year is allready in filename
        dataString += "-";
        dataString += now.month();
        dataString += "-";
        dataString += now.year();
        dataString += ",";
    */
    dataString += now.hour();
    dataString += ":";
    if (now.minute() <= 9)dataString += "0";
    dataString += now.minute();
    dataString += ":";
    if (now.second() <= 9)dataString += "0";
    dataString += now.second();
    dataString += ",";
    dataString += String(map(sense1, dry_sensor_one, wet_sensor_one, 0, 100));
    dataString += ",";
    dataString += String(map(sense2, dry_sensor_two, wet_sensor_two, 0, 100));
    dataString += ",";
    dataString += String(averageinprocent);
    dataString += ",";
    dataString += String(moistureforstartwatering);
    dataString += ",";
    dataString += String(start_hour);
    dataString += ",";
    dataString += String(end_hour);
    dataString += ",";
    dataString += rtc.getTemperature();
    dataString += ",";
    dataString += String(watergiftcounter);
    dataString += ",";
    dataString += String(maximumaantalbeurtenperdag);
    dataString += ",";
    dataString += String(watering_duration);
    dataString += ",";
    dataString += String(pauze_after_watering);
    dataString += ",";
    dataString += String(lastwatering);
    dataString += ",";
    dataString += String(ValveStatus);
    dataString += ",";
    dataString += String(watergifttimer);
    dataString += ",";
    dataString += String(pauzetimer / 1000);
    dataString += ",";

    lcd.setCursor(9, 0);
    lcd.print("SD  ");

    File myFile;

    // filename must be 8.3 size
    String DateStampFile = String(now.day()) + "_" + String(now.month()) + "_" + String(now.year() - 2000) + ".TXT";
    String LogFileHeader = "time, sensor1, sensor2, averageinprocent, moisturestartprocent, starthour, endhour, temperature, jobcounter, maxjobs, wateringduration, pauzeduration, lastwateringtime, ValveStatus, watergifttimer, pauzetimer,";
    // must be a units header here?, but cannot find info about that

    if (SD.exists(DateStampFile)) {                                 // does the file exist on sdcard?
      Serial.print("File exists. "); Serial.println(DateStampFile);

      myFile = SD.open(DateStampFile, FILE_WRITE);                  // if yes open it

      if (myFile) {                                                 // looks like println allready seeks end of file, where to append
        myFile.println(dataString);                                 // print string to sdcard log file
        myFile.close();
        Serial.println(dataString);                                 // print to the serial port too:
        lcd.setCursor(9, 0);
        lcd.print("SDok");
      } else {
        Serial.print("error opening ");  Serial.println(myFile);    // if the file isn't open, pop up an error:
        lcd.setCursor(9, 0);
        lcd.print("SD=X");
      }

    }
    else
    {
      Serial.print(DateStampFile); Serial.println(" Does not exist.");
      Serial.print(DateStampFile); Serial.println(" Creating File.");
      myFile = SD.open(DateStampFile, FILE_WRITE);                  // create file with datestamp.txt MUST BE 8.3 SIZE
      myFile.println(LogFileHeader);                                // print header to file for spreadsheet or chartmaker
      myFile.close();
      lcd.setCursor(9, 0);
      lcd.print("SD=X");
    }



#endif
    // future for Mega2560 SD Card log  // not enough space nano uno == 105%    for mega 2560 or mega pro mini
    // end test for mega 2560




    // Serial.println("///////////////////////////////////////");
    // Serial.println("");
    // Serial.println("watering start times today");
    // for (int i = 0; i < watergiftcounter; i++) {
    //   int adress = 100 + ((i + 1) * 10);
    //   char date[10] = "hh:mm:ss";  // maybe to eprom
    //   if (i + 1 <= maximumaantalbeurtenperdag) {
    //     Serial.print(i + 1); Serial.print(" "); Serial.print(adress); Serial.print(" "); EEPROM.get(adress, date); Serial.println(date);
    //   }
    //  }
    // Serial.println("end");
    // Serial.println("");
    // Serial.println("////////////////////////////////////////");

    // Serial.print("millis() "); Serial.println(millis());
    // Serial.println("");

    // Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    // Serial.print(" ");
    // Serial.print(now.day(), DEC);
    // Serial.print("-");
    // Serial.print(now.month(), DEC);
    // Serial.print("-");
    // Serial.print(now.year(), DEC);


    lcd.setCursor(0, 0);
    lcd.print(now.hour());
    lcd.print(':');
    if (now.minute() <= 9)lcd.print('0');
    lcd.print(now.minute());
    lcd.print(':');
    if (now.second() <= 9)lcd.print('0');
    lcd.print(now.second());
    lcd.print(" ");
    //lcd.print(now.day());
    //lcd.print("-");
    //lcd.print(now.month());
    //lcd.print("-");
    //lcd.print(now.year());
    //lcd.print(" ");
    lcd.setCursor(14, 0);
    lcd.print("sp=");                           // setpoint switchpoint ??%
    lcd.print(moistureforstartwatering);
    lcd.print(" ");

    // Serial.println(' ');

    // Serial.print(now.hour(), DEC);
    // Serial.print(':');
    // Serial.print(now.minute(), DEC);
    // Serial.print(':');
    // Serial.print(now.second(), DEC);
    // Serial.println();

    // Serial.print("Temperature: ");
    // Serial.print(rtc.getTemperature());
    // Serial.println(" C");

    // Serial.println();
    // Serial.print("1 read analogread A0 = "); Serial.println(analogRead(A0));
    // Serial.print("1 read analogread A3 = "); Serial.println(analogRead(A3));
    // Serial.print("100 read analogread A0 = "); Serial.println(sense1);
    // Serial.print("100 read analogread A3 = "); Serial.println(sense2);
    // Serial.print("analogread average = "); Serial.println((sense1 + sense2) / 2);


    // Serial.print("sensor1 "); Serial.print(map(sense1, dry_sensor_one, wet_sensor_one, 0, 100)); Serial.println(" % wet");
    //  Serial.print("sensor2 "); Serial.print(map(sense2, dry_sensor_two, wet_sensor_two, 0, 100)); Serial.println(" % wet");

    averageinprocent = (map(sense1, dry_sensor_one, wet_sensor_one, 0, 100) + map(sense2, dry_sensor_two, wet_sensor_two, 0, 100)) / 2;
    // Serial.print("Average "); Serial.print(averageinprocent); Serial.println(" % wet");

    // Serial.println(' ');

    //Serial.print("averageinprocent = "); Serial.print(averageinprocent); Serial.print(" switchpoint = "); Serial.println(moistureforstartwatering);
    lcd.setCursor(0, 1);
    lcd.print("S1=");
    lcd.print(map(sense1, dry_sensor_one, wet_sensor_one, 0, 100)); lcd.print("   ");
    lcd.setCursor(7, 1);
    lcd.print("S2=");
    lcd.print(map(sense2, dry_sensor_two, wet_sensor_two, 0, 100)); lcd.print("   ");
    lcd.setCursor(14, 1);
    lcd.print("av=");
    lcd.print(averageinprocent); lcd.print("   ");

  }  // end do this only once each second



  if (now.hour() >= start_hour && now.hour() < end_hour) {
    //Serial.println("zit binnnen watergift mogelijk tijden");

    if (averageinprocent <= moistureforstartwatering) {      // if soil is dryer as setpoint
      if (watergiftcounter == 0 || pauzetimer == 0 ) {          // if firstwaterpoor of day or pauzetimer==0
        if (watergiftcounter <= maximumaantalbeurtenperdag) {
          if (ValveStatus == 0) {
            starttime = millis();                              // save starttime millis only once
            ValveStatus = 1;                                   // next time whe do no get here because valvestatus is now 1
            watergiftcounter = watergiftcounter + 1;
            int adress = 100 + (watergiftcounter * 10);
            DateTime now = rtc.now();
            char date[10] = "hh:mm:ss";  // maybe to eprom

            rtc.now().toString(date);
            lastwatering = date;
            EEPROM.put(adress, date);           // write starttime to eeprom 810 820 830 840 ....
            //   Serial.println("date");
          }
        }
      }
    }
  }









  if (ValveStatus == 1) {
    backlightstart = millis();                                 // keep backlight on when valvestatus is open
    //Serial.println("watergift start kraan open pomp aan");
    digitalWrite(13, HIGH);                  // 13 is onboard led en waterklep en/of waterpomp start
    startpauzetimer = millis();              // the latest time  we get into "if (ValveStatus == 1) {" will be used to set "startpauzetimer = millis();"
    pauzetimer =  (pauze_after_watering * 60 * 1000L); // show pauzetime, wich countdown after valvestaus=0
    if (millis() - starttime <= (watering_duration * 1000L)) {
      lcd.setCursor(0, 3);
      lcd.print("Open");
      lcd.print(" ");
      watergifttimer = (starttime + (watering_duration * 1000L) - millis()) / 1000;
      if (watergifttimer <= 0)watergifttimer = 0;
      lcd.print(watergifttimer);
      lcd.print("      ");
      // Serial.print("watergifttimer ");
      // Serial.println(watergifttimer);
    }
  }



  if (millis() - starttime >= (watering_duration * 1000L)) {
    //Serial.println("watergift stop / kraan dicht pomp uit");
    digitalWrite(13, LOW);          // 13 is onboard led  en waterklep en/of waterpomp stop
    ValveStatus = 0;

  }

  if (ValveStatus == 0) {
    pauzetimer =  (pauze_after_watering * 60 * 1000L) - (millis() - startpauzetimer) ;
    if (pauzetimer <= 0) pauzetimer = 0;
    if (pauzetimer > 0)backlightstart = millis();            // keep backlight on when pauzetimer is running
  }
  if (watergiftcounter <= 0) pauzetimer = 0;                 // anders gaat pauzetimer onnodig lopen bij start of reboot




  if (blinknodelay_flag == 0) {                          // part off blink when time not ok for watering
    lcd.setCursor(0, 2);
    if (now.hour() < start_hour) {
      lcd.print(F("OFF, Time < ")); lcd.print(start_hour); lcd.print(F(" Hour"));  // time not in range for watering, let the plants sleep
    }
    if (now.hour() >= end_hour) {
      lcd.print(F("OFF, Time >= ")); lcd.print(end_hour); lcd.print(F(" Hour"));  // time not in range for watering, let the plants sleep
    }
  }

  if (blinknodelay_flag == 1) {                        // part off blink when time not ok for watering
    lcd.setCursor(16, 2);
    lcd.print("    "); // erase the last part of line of text  = erase =>   Hour
    lcd.setCursor(0, 2);
    lcd.print(F("count="));
    lcd.print(watergiftcounter);
    lcd.print(F(" pauze="));
    lcd.print(pauzetimer / 1000);
    lcd.print(" ");
  }
  lcd.setCursor(0, 3);
  if (ValveStatus == 0) {
    lcd.print("Closed      ");     // dont know sometimes a long value at 0/close = erase it with extra spaces
    // looked like a overflow from long 0 countdown to max long = 2^32-1 value, now in log i see sometimes 4294967
    // should count signed long to -1 and i say if -1 count is 0, now in log i see sometimes 4294967
  }
  if (ValveStatus == 1) {
    lcd.print("Open");
  }
  lcd.setCursor(14, 3);
  lcd.print(rtc.getTemperature());
  lcd.print("C");
  // Serial.print("pauzetimer ");
  // Serial.println(pauzetimer / 1000);






  if (watergiftcounter > maximumaantalbeurtenperdag) {
    lcd.setCursor(0, 3);
    lcd.print(maximumaantalbeurtenperdag);
    lcd.print(" MaximumCount");
    //Serial.print("maximumaantalbeurtenperdag ");
    //Serial.println(maximumaantalbeurtenperdag);
    //Serial.println("watergift stop / kraan dicht pomp uit");
    digitalWrite(13, LOW);          // 13 is onboard led  en waterklep en/of waterpomp stop

    // if (blinknodelay_flag == 0)lcd.noBacklight();              // max count reached error blink backlight
    // if (blinknodelay_flag == 1)lcd.backlight();               // max count reached error blink backlight
  }


  if (now.hour() == 23 && now.minute() == 59 && now.second() >= 58) {
    lcd.clear();
    lcd.backlight();                    // Turn backlight ON
    // they say eeprom life 100 thousand writes
    // say i write/erase a byte 4 times a day = 25 thousand days = 68 Years
    // could also use i2c eeprom that is on the ds3231 rtc board wich i think has 1 milion writes, RTC DS3231 with AT24C32 eeprom
    // https://www.google.com/search?q=AT24C32+pdf

    // erase is done at reboot in setup()
    //    for (int i = 100 ; i < EEPROM.length() ; i++) {   // erase eprom water start times
    //      lcd.setCursor(0, 0);
    //      lcd.print("Erase WaterTime LOG");
    //      lcd.setCursor(8, 2);
    //      lcd.print(i);
    //      EEPROM.write(i, 0);                             // erase eprom water start times
    //    }
    // erase is done at reboot in setup()

    watergiftcounter = 0; // if you trust the millis(); 49-50 days overflow       comment out the line // asm volatile("jmp 0");
    lcd.setCursor(3, 2);
    lcd.print("Reboot Now!    ");
    delay(2500);
    asm volatile("jmp 0");                                              // end of day reset/reboot arduino //start the day with a fresh millis() counter
    // no worry's about millis overflow every 50 days
    // and resets watergiftcounter
  }



  /////////////////////////////////////////////////////////////////////////////////////////////
  /*
    counter = counter + 1;                // just a counter to see how many times i get here
    if (counter >= 480)counter = 0;
    lcd.setCursor(9, 0);
    if (counter <= 99)lcd.print(" ");
    if (counter <= 9)lcd.print(" ");
    lcd.print(counter);                  // just a counter to see how many times i get here
  */


  // there should be an  arduino command  outsidedelay(); or max_ex_per_sec(); like next
  while (millis() - loopspeed <= 125) {
    /**/
  }       // limmit loop to max 8 times per second, 480 times a minute
  loopspeed = millis();
  // there should be an  arduino command  outsidedelay();  or max_ex_per_sec(); like above

  //////////////////////////////////////////////////////////////////////////////////////////////
}













//*******************************************************************
boolean SetButton() {
  boolean sval;
  sval = digitalRead(rotarybutton_SW);
  return sval;
}









//*******************************************************************
int8_t read_rotary() {
  static int8_t rot_enc_table[] = {0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0};

  // Robust Rotary encoder reading
  // Copyright John Main - best-microcontroller-projects.com
  // https://www.best-microcontroller-projects.com/rotary-encoder.html

  prevNextCode <<= 2;
  if (digitalRead(DATA)) prevNextCode |= 0x02;
  if (digitalRead(CLK)) prevNextCode |= 0x01;
  prevNextCode &= 0x0f;

  // If valid then store as 16 bit data.
  if  (rot_enc_table[prevNextCode] ) {
    store <<= 4;
    store |= prevNextCode;
    //if (store==0xd42b) return 1;
    //if (store==0xe817) return -1;
    if ((store & 0xff) == 0x2b) return -1;
    if ((store & 0xff) == 0x17) return 1;
  }
  return 0;

  // Robust Rotary encoder reading
  // Copyright John Main - best-microcontroller-projects.com
  // https://www.best-microcontroller-projects.com/rotary-encoder.html
}






//******************************************************************
void TimeOut() {
  lcd.clear();  //exit menu_nr if 20 seconds innactive
  lcd.setCursor(6, 0);
  lcd.print(F("TimeOut"));
  lcd.setCursor(0, 1);
  lcd.print(F("Return to Mainscreen"));
  lcd.setCursor(0, 3);
  lcd.print(F("NOT saved to EEPROM!"));
  delay(2500);
  lcd.clear();
  Calibrate_Sensors = 1; // NO when timedout in YES
  eepromerase = 1;       // NO when timedout in YES
  menu_nr = 0;
}




//****************************************************************
void readsensors() {
  Read_A0 = 0;
  Read_A3 = 0;
  for (int cc = 0; cc < 10; cc++) {     // do 10 readings

    dummy_read = analogRead(A0);          // reduce analog pins influence eachother?
    delay(1);
    Read_A0 = Read_A0 + analogRead(A0);
    delay(1);
    dummy_read = analogRead(A3);            // reduce analog pins influence eachother?
    delay(1);
    Read_A3 = Read_A3 + analogRead(A3);
    delay(1);
  }
  sense1 = (Read_A0 / 10);             // divide by 10
  sense2 = (Read_A3 / 10);

}

/*
*/
// Een Heitje voor een karweitje
// A Penny for Sharing My Thoughts?
// http://www.paypal.me/LDijkman

// Arduino Advanced Automated Plant Watering System, StandAlone, Low Cost, Low Power Consumption
// Copyright 2021 Dirk Luberth Dijkman Bangert 30 1619GJ Andijk The Netherlands
