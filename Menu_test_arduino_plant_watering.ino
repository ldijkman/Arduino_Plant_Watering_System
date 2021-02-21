
/*
   removed the date for showing sp= setpoint               (add another LCD display with different i2c adress? ;-)

   added rotary encoder push button KY-040 https://www.google.com/search?q=KY-040

   Rotary encoder menu mostly working
   few more menu items i would like to add
      menu backlight_time
      menu time / date set
      menu eeprom erase      No / Yes
      menu software reboot   No / Yes
      menu's parameter info => at wich times a wateringjob started today

           a bit of copy, paste, modify from http://www.sticker.tk/forum/index.php?action=view&id=296
                                             http://www.sticker.tk/forum/index.php?action=view&id=299

*/

/*
  &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
  Arduino Automated Plant Watering System, automatic irrigation system
  Arduino Advanced Automated Plant Watering System, StandAlone, Low Cost, Low Power Consumption

  Copyright 2021 Dirk Luberth Dijkman Bangert 30 1619GJ Andijk The Netherlands
  https://m.facebook.com/luberth.dijkman
  https://github.com/ldijkman/Arduino_Plant_Watering_System
  https://youtu.be/1jKAxLyOY_s

  GNU General Public License,
  which basically means that you may freely copy, change, and distribute it,
  but you may not impose any restrictions on further distribution,
  and you must make the source code available.
  https://www.gnu.org/licenses

  http://Paypal.Me/LDijkman
  All above must be included in any redistribution
  &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
*/

//
// kan makkelijker met gewone delays instead of millis();
// maar dan staat alles stil als je bijvoorbeeld een delay van 10 minuten hebt
// gebeurt er niks gedurende die 10 minuten
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
// maybe use i2c analog 4 channel 16bit https://www.google.com/search?q=ads1115
// is the ads1115 multiplexing like arduino?
// or use 2 single channel i2c analog to digital converters with a different i2c adress, No noise?
// https://www.google.com/search?q=1+channel+i2c+analog+to+digital+converter
// future maybe 2x 1 channel 12bit? MCP4725 I2C, both with different i2c adres
//
//
// with irrigation drippers / drip emmiters you have more control of amount of water per time!?
// different types liters/per hour
// white tyleen hose less heat / polution, better not use black, do not use transparent
//
//
// DS3231 RTC realtimeclock connected to +5vdc GND SCL SDA
// 4x20 i2c LCD connected to +5vdc GND SCL SDA
//
// Cheap dollar capacitive soil moisture sensors are in the Khz range
// more expensive capacitive soil moisture sensors are  in the +/-75Mhz range (mineral/contemination transperency?)
// but it isnt the 555 timer replacement that justfys the 30x and up price
//
// 2 capacitive soil moisture sensors connected to analog A0 and A3
//
// Pump and or valve output D13 (wich is also onboard LED)
//
// rotarybutton_SW 2           // input D2 rotary encoder SW
// CLK 3                       // input D3 rotary encoder CLK
// DATA 4                      // input D4 rotary encoder DT
// and connect rotary encoder to +5vdc and GND to 0vdc, -, min, GND, ground, or whatever they call it






#include "RTClib.h"                   // https://github.com/adafruit/RTClib

RTC_DS3231 rtc;

#include <EEPROM.h>

#include <LiquidCrystal_I2C.h>       // https://github.com/marcoschwartz/LiquidCrystal_I2C/archive/master.zip

LiquidCrystal_I2C lcd(0x27, 20, 4);  // Configure LiquidCrystal_I2C library with 0x27 address, 20 columns and 4 rows

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

int starttijdwatergift = 9;                                 // starttime 9 o clock in the morning
int eindtijdwatergift = 21;                                 // endtime  21 o clock in te evening
signed long duurwatergiftbeurt = 60;                        // in seconds = 60 seconds
signed long pauzenawatergiftbeurt = 1 ;                     // in minutes = 1 minute
signed long watergifttimer;
signed long pauzetimer;
signed long startpauzetimer;
signed long starttime;

int wetnesforstartwatergiftbeurt = 30;                      // if smaller als 30% start watering

int dry_sensor_one = 795;                                   // my sensor value Dry in air   795
int wet_sensor_one = 435;                                   // my sensor value wet in water 435

int dry_sensor_two = 795;                                   // my sensor value Dry in air   795
int wet_sensor_two = 435;                                   // my sensor value wet in water 435

int sense1;
int sense2;


int averageinprocent;                                      // average of 2 sensors in %

byte watergiftcounter = 0;

byte ValveStatus = 0;

byte second_now;
byte last_second;

byte maximumaantalbeurtenperdag = 8;

byte Calibrate_Sensors = 1;

// rotary encoder push button KY-040 https://www.google.com/search?q=KY-040
// Robust Rotary encoder reading
// Copyright John Main - best-microcontroller-projects.com
#define rotarybutton_SW 2           // input D2 rotary encoder SW
#define CLK 3                       // input D3 rotary encoder CLK
#define DATA 4                      // input D4 rotary encoder DT
// and connect rotary encoder to +5vdc and GND to 0vdc, -, min, GND, ground, or whatever they call it
static uint8_t prevNextCode = 0;
static uint16_t store = 0;
//

byte menu = 0;


long TempLong;                                   // Temporary re-used over and over again
float TempFloat;
int TempInt;
byte TempByte;

long backlightofftimeout = 1 * 60 * 1000L;      // time to switch backlight off in milliseconds (the L is needed, otherwise wrong calculation Arduino IDE)
long backlightstart;
byte backlightflag;



//**********************************************************************************************
void setup () {

  pinMode(13, OUTPUT);                 // pin 13 for valve open / close is also the onboard LED

  pinMode(rotarybutton_SW, INPUT);     // rotary encoder SW = pulled up by resistor on KY-040 to +
  pinMode(CLK, INPUT);                 // rotary encoder
  pinMode(CLK, INPUT_PULLUP);          // rotary encoder
  pinMode(DATA, INPUT);                // rotary encoder
  pinMode(DATA, INPUT_PULLUP);         // rotary encoder

 
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
  //               666 on adres 666 is written in EEPROM like this
  // 665 255
  // 666 154       2x256+154=666
  // 667 2
  // 668 255





  // DO NEXT ONLY ONCE
  int override_doitonce = 0;  // 1 set variables as above to eeprom,  program set it back to 0 after boot and reprogram with 0
  // first run ??? write some val to eeprom if value at eepromadres 666 not is 666
  // if this is first run then val will not be 666 at eeprom adres 666
  // so next will be run
  EEPROM.get(666, TempInt);        // read eeprom adress 666 into TempInt
  if (override_doitonce == 1 || TempInt != 666) {       // IF not is 666, this is the first run THEN val at eeprom adres 666 is -1???
    EEPROM.put(0, wetnesforstartwatergiftbeurt);
    EEPROM.put(5, dry_sensor_one);
    EEPROM.put(10, wet_sensor_one);
    EEPROM.put(15, dry_sensor_two);
    EEPROM.put(20, wet_sensor_two);
    EEPROM.put(25, duurwatergiftbeurt);
    EEPROM.put(30, pauzenawatergiftbeurt);
    EEPROM.put(35, maximumaantalbeurtenperdag);
    EEPROM.put(40, starttijdwatergift);
    EEPROM.put(45, eindtijdwatergift);
    EEPROM.put(50, backlightofftimeout);
    //  EEPROM.put(55, Variable-Here);
    //  EEPROM.put(60, Variable-Here);
    EEPROM.put(666, 666);        // ONLY ONCE, Make His Mark, set eepromadres 666 to val 666 no need to call / run this anymore in future

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Hi There! First run"));
    lcd.setCursor(0, 1);
    lcd.print(F("Have written value's"));
    lcd.setCursor(0, 2);
    lcd.print(F("to EEPROM "));
    lcd.setCursor(0, 3);
    lcd.print(F("Thanks for trying"));
    for (int i = 30 ; i > 0 ; i--) {
      lcd.setCursor(17, 2);
      lcd.print(i);
      lcd.print(" ");
      delay(500);
    }
    lcd.clear();
  }// END ONLY ONCE, MADE HIS MARK

  // Read stored valeus from EEPROM
  EEPROM.get(0, wetnesforstartwatergiftbeurt);
  EEPROM.get(5, dry_sensor_one);
  EEPROM.get(10, wet_sensor_one);
  EEPROM.get(15, dry_sensor_two);
  EEPROM.get(20, wet_sensor_two);
  EEPROM.get(25, duurwatergiftbeurt);
  EEPROM.get(30, pauzenawatergiftbeurt);
  EEPROM.get(35, maximumaantalbeurtenperdag);
  EEPROM.get(40, starttijdwatergift);
  EEPROM.get(45, eindtijdwatergift);
  EEPROM.get(50, backlightofftimeout);
  //  EEPROM.get(55, Variable-Here);
  //  EEPROM.get(60, Variable-Here);
  
  backlightstart = millis();          // load millis() in backlightstart

}






//**********************************************************************************************************
void loop () {

  if (backlightflag == 1 && millis() - backlightstart > backlightofftimeout) {              // if backlight timed out turn it off
    lcd.noBacklight();                    // Turn backlight OFF
    backlightflag = 0;
  }
  if (backlightflag == 0 && millis() - backlightstart < backlightofftimeout) {
    lcd.backlight();                    // Turn backlight ON
    backlightflag = 1;
  }



  long Read_A0 = 0;
  long Read_A3 = 0;
  int dummy_read;
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



  if (SetButton() == LOW) {                     // if !=not SetButton, SW = pulled up by resistor on KY-040 to +,  so LOW is button pressed

    backlightstart = millis();
    lcd.backlight();
    TempLong = millis();  // reset innactive time counter
    while (SetButton() == LOW) {                                    // while setbutton==LOW, pulled up by resistor, LOW is pressed
      // loop until button released
      // maybe a timer here
      // alarm if button never released
      lcd.setCursor(0, 0);
      lcd.print(F("Ok Backlight ON     "));
      lcd.setCursor(0, 1);
      lcd.print(F("                    "));
      lcd.setCursor(0, 2);
      lcd.print(F("   Keep pressed?    "));
      lcd.setCursor(0, 3);
      lcd.print(F(" For Menu Enter in  "));

      lcd.setCursor(19, 3);
      lcd.print(5 - (millis() - TempLong) / 1000);                     // on lcd timeout countdown
      if ((millis() - TempLong)  > 5000) {
        lcd.setCursor(0, 0);
        lcd.print(F("Ok we are in        "));
        lcd.setCursor(0, 1);
        lcd.print(F("                    "));
        lcd.setCursor(0, 2);
        lcd.print(F("   Entering Menu    "));
        lcd.setCursor(0, 3);
        lcd.print(F("   Release Button   "));
        delay(500);
        menu = 1;
      }
    }
    lcd.clear();
  }



  // 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
  // setpoint setpoint setpoint setpoint setpoint setpoint setpoint setpoint setpoint setpoint setpoint setpoint setpoint setpoint
  TempLong = millis();  // reset innactive time counter
  if (menu == 1) {
    lcd.setCursor(0, 0);
    lcd.print(F("1 Set SwitchPoint %"));
    lcd.setCursor(8, 2);
    lcd.print(wetnesforstartwatergiftbeurt);
    lcd.print(F(" % "));
  }
  while (menu == 1) {
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
      wetnesforstartwatergiftbeurt = wetnesforstartwatergiftbeurt + (rval);
      if (wetnesforstartwatergiftbeurt >= 70) wetnesforstartwatergiftbeurt = 70;
      if (wetnesforstartwatergiftbeurt <= 10) wetnesforstartwatergiftbeurt = 10;
      TempLong = millis();  //reset innactive time counter
      lcd.setCursor(8, 2);
      lcd.print(wetnesforstartwatergiftbeurt);
      lcd.print(F(" % "));

    }

    if (SetButton() == LOW) {                                    // if setbutton==LOW, pulled up by resistor, LOW is pressed
      while (SetButton() == LOW) {
        /*wait for button released*/
      }
      menu = 2;
      lcd.clear();

      EEPROM.get(0, TempInt);                                   // limmited write to eeprom = read is unlimmited
      if (wetnesforstartwatergiftbeurt != TempInt) {            // only write to eeprom if value is different
        EEPROM.put(0, wetnesforstartwatergiftbeurt);            // put already checks if val is needed to write
        lcd.setCursor(0, 0);
        lcd.print(F("Saving to EEPROM"));
        lcd.setCursor(0, 2);
        lcd.print("old= ");
        lcd.print(TempInt);
        lcd.print(F(" new= "));
        lcd.print(wetnesforstartwatergiftbeurt);
        TempLong = millis();                                    // load millis() into Templong for next countdown delay
        while ((millis() - TempLong)  <= 5000) {
          lcd.setCursor(19, 3);
          lcd.print(5 - (millis() - TempLong) / 1000);          // on lcd timeout countdown
        }
        delay(1000);  // want to see the zero 0
        for (int i = 0; i < 10; i++)Serial.println(F("wetnesforstartwatergiftbeurt DATA WRITTEN / SAVED TO EEPROM "));
        lcd.clear();
      }


    }
  }// end menu 1






  // 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2
  // Calibrate sensors Calibrate sensors Calibrate sensors Calibrate sensors Calibrate sensors Calibrate sensors
  TempLong = millis();  //reset innactive time counter
  if (menu == 2) {
    lcd.setCursor(0, 0);
    lcd.print(F("2 Calibrate Sensors"));
    lcd.setCursor(9, 2);
    if (Calibrate_Sensors == 1)lcd.print(F("No "));
    if (Calibrate_Sensors == 2)lcd.print(F("Yes"));
  }
  while (menu == 2) {

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
      TempLong = millis();  //reset innactive time counte
      lcd.setCursor(9, 2);
      lcd.print(Calibrate_Sensors);
      lcd.print(F(" "));
      if (Calibrate_Sensors < 1)Calibrate_Sensors = 2;
      if (Calibrate_Sensors > 2)Calibrate_Sensors = 1;
      lcd.setCursor(9, 2);
      if (Calibrate_Sensors == 1)lcd.print(F("No "));
      if (Calibrate_Sensors == 2)lcd.print(F("Yes"));
    }

    if (SetButton() == LOW) {        // LOW setbutton is pressed
      while (SetButton() == LOW) {
        /*wait for button released*/
      }
      menu = 3;
      lcd.clear();
      delay(250);
    }
  }// end menu 2



  if (Calibrate_Sensors == 2) {   // you chose Yes so whe go to calibrate
    Calibrate_Sensors = 1;
    lcd.clear();
    while (0 == 0) {
      lcd.setCursor(0, 0);
      lcd.print("Sensor1 analog A0");
      lcd.setCursor(0, 1);
      lcd.print("Clean Dry in Air?");
      lcd.setCursor(9, 3);
      lcd.print(analogRead(A0));
      delay(250);
      if (SetButton() == LOW) {        // LOW setbutton is pressed
        while (SetButton() == LOW) {
          /*wait for button released*/
        }
        dry_sensor_one = analogRead(A0);
        EEPROM.put(5, dry_sensor_one);
        lcd.clear();
        delay(500); // user gets a better experience switch to next screen?
        break;
      }
    }


    while (0 == 0) {
      lcd.setCursor(0, 0);
      lcd.print("Sensor1 analog A0");
      lcd.setCursor(0, 1);
      lcd.print("Wet in Watter?");
      lcd.setCursor(9, 3);
      lcd.print(analogRead(A0));
      delay(250);
      if (SetButton() == LOW) {        // LOW setbutton is pressed
        while (SetButton() == LOW) {
          /*wait for button released*/
        }
        wet_sensor_one = analogRead(A0);
        EEPROM.put(10, wet_sensor_one);
        lcd.clear();
        delay(500); // user gets a better experience switch to next screen?
        break;
      }
    }


    while (0 == 0) {
      lcd.setCursor(0, 0);
      lcd.print("Sensor2 analog A3");
      lcd.setCursor(0, 1);
      lcd.print("Clean Dry in Air?");
      lcd.setCursor(9, 3);
      lcd.print(analogRead(A3));
      delay(250);
      if (SetButton() == LOW) {        // LOW setbutton is pressed
        while (SetButton() == LOW) {
          /*wait for button released*/
        }
        dry_sensor_two = analogRead(A3);
        EEPROM.put(15, dry_sensor_two);
        lcd.clear();
        delay(500); // user gets a better experience switch to next screen?
        break;
      }
    }


    while (0 == 0) {
      lcd.setCursor(0, 0);
      lcd.print("Sensor2 analog A3");
      lcd.setCursor(0, 1);
      lcd.print("Wet in Watter?");
      lcd.setCursor(9, 3);
      lcd.print(analogRead(A3));
      delay(250);
      if (SetButton() == LOW) {        // LOW setbutton is pressed
        while (SetButton() == LOW) {
          /*wait for button released*/
        }
        wet_sensor_two = analogRead(A3);
        EEPROM.put(20, wet_sensor_two);
        lcd.clear();
        delay(500); // user gets a better experience switch to next screen?
        break;
      }
    }
  }// calibrate sensors==2





  // 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3
  // duurwatergiftbeurt duurwatergiftbeurt duurwatergiftbeurt duurwatergiftbeurt duurwatergiftbeurt
  TempLong = millis();  // reset innactive time counter
  if (menu == 3) {
    lcd.setCursor(0, 0);
    lcd.print(F("3 Watering T in Sec."));
    lcd.setCursor(6, 2);
    lcd.print(duurwatergiftbeurt);
    lcd.print(F(" Sec. "));
  }
  while (menu == 3) {
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
      if (duurwatergiftbeurt >= 600)duurwatergiftbeurt = duurwatergiftbeurt + (rval * 30);        // 30+24+6=60 second steps above 600 seconds
      if (duurwatergiftbeurt >= 300)duurwatergiftbeurt = duurwatergiftbeurt + (rval * 24);        // 24+6=30 second steps above 300 seconds
      if (duurwatergiftbeurt >= 60)duurwatergiftbeurt = duurwatergiftbeurt + (rval * 6);          // 6 second steps above 60 seconds
      if (duurwatergiftbeurt <= 60)duurwatergiftbeurt = duurwatergiftbeurt + (rval);              // 1 second steps
      if (duurwatergiftbeurt <= 2) duurwatergiftbeurt = 2;                                        // minimal watering time 2 seconds
      TempLong = millis();  //reset innactive time counter
      lcd.setCursor(6, 2);
      lcd.print(duurwatergiftbeurt);
      lcd.print(F(" Sec. "));
      lcd.setCursor(6, 3);
      float wateringtime = duurwatergiftbeurt;
      float minutetime = (wateringtime / 60);
      lcd.print(minutetime, 1);
      lcd.print(F(" Min. "));

    }

    if (SetButton() == LOW) {                                    // if setbutton==LOW, pulled up by resistor, LOW is pressed
      while (SetButton() == LOW) {
        /*wait for button released*/
      }
      menu = 4;
      lcd.clear();

      EEPROM.get(25, TempInt);                                   // limmited write to eeprom = read is unlimmited
      if (duurwatergiftbeurt != TempInt) {            // only write to eeprom if value is different
        EEPROM.put(25, duurwatergiftbeurt);            // put already checks if val is needed to write
        lcd.setCursor(0, 0);
        lcd.print(F("Saving to EEPROM"));
        lcd.setCursor(0, 2);
        lcd.print("old= ");
        lcd.print(TempInt);
        lcd.print(F(" new= "));
        lcd.print(duurwatergiftbeurt);
        TempLong = millis();                                    // load millis() into Templong for next countdown delay
        while ((millis() - TempLong)  <= 5000) {
          lcd.setCursor(19, 3);
          lcd.print(5 - (millis() - TempLong) / 1000);          // on lcd timeout countdown
        }
        delay(1000);  // want to see the zero 0
        for (int i = 0; i < 10; i++)Serial.println(F("duurwatergiftbeurt DATA WRITTEN / SAVED TO EEPROM "));
        lcd.clear();
      }


    }
  }// end menu 3




  // 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4
  // pauzenawatergiftbeurt pauzenawatergiftbeurt pauzenawatergiftbeurt pauzenawatergiftbeurt pauzenawatergiftbeurt
  TempLong = millis();  // reset innactive time counter
  if (menu == 4) {
    lcd.setCursor(0, 0);
    lcd.print(F("4 PauzeTime T in Min"));
    lcd.setCursor(6, 2);
    lcd.print(pauzenawatergiftbeurt);
    lcd.print(F(" Min. "));
  }
  while (menu == 4) {
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
      pauzenawatergiftbeurt = pauzenawatergiftbeurt + (rval);          // 1 minute steps
      if (pauzenawatergiftbeurt <= 1) pauzenawatergiftbeurt = 1;       // minimal pauzetime 1 minutes
      TempLong = millis();  //reset innactive time counter
      lcd.setCursor(6, 2);
      lcd.print(pauzenawatergiftbeurt);
      lcd.print(F(" Min. "));

    }

    if (SetButton() == LOW) {                                    // if setbutton==LOW, pulled up by resistor, LOW is pressed
      while (SetButton() == LOW) {
        /*wait for button released*/
      }
      menu = 5;
      lcd.clear();

      EEPROM.get(30, TempInt);                                   // limmited write to eeprom = read is unlimmited
      if (pauzenawatergiftbeurt != TempInt) {            // only write to eeprom if value is different
        EEPROM.put(30, pauzenawatergiftbeurt);            // put already checks if val is needed to write
        lcd.setCursor(0, 0);
        lcd.print(F("Saving to EEPROM"));
        lcd.setCursor(0, 2);
        lcd.print("old= ");
        lcd.print(TempInt);
        lcd.print(F(" new= "));
        lcd.print(pauzenawatergiftbeurt);
        TempLong = millis();                                    // load millis() into Templong for next countdown delay
        while ((millis() - TempLong)  <= 5000) {
          lcd.setCursor(19, 3);
          lcd.print(5 - (millis() - TempLong) / 1000);          // on lcd timeout countdown
        }
        delay(1000);  // want to see the zero 0
        for (int i = 0; i < 10; i++)Serial.println(F("pauzenawatergiftbeurt DATA WRITTEN / SAVED TO EEPROM "));
        lcd.clear();
      }


    }
  }// end menu 4







  // 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5
  // maximumaantalbeurtenperdag maximumaantalbeurtenperdag maximumaantalbeurtenperdag maximumaantalbeurtenperdag
  TempLong = millis();  // reset innactive time counter
  if (menu == 5) {
    lcd.setCursor(0, 0);
    lcd.print(F("5 Max water count"));
    lcd.setCursor(9, 2);
    lcd.print(maximumaantalbeurtenperdag);
    lcd.print(F(" "));
  }
  while (menu == 5) {
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
      TempLong = millis();  //reset innactive time counter
      lcd.setCursor(9, 2);
      lcd.print(maximumaantalbeurtenperdag);
      lcd.print(F(" "));

    }

    if (SetButton() == LOW) {                                    // if setbutton==LOW, pulled up by resistor, LOW is pressed
      while (SetButton() == LOW) {
        /*wait for button released*/
      }
      menu = 6;
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
        for (int i = 0; i < 10; i++)Serial.println(F("maximumaantalbeurtenperdag DATA WRITTEN / SAVED TO EEPROM "));
        lcd.clear();
      }


    }
  }// end menu 5


  // 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6
  // starttijdwatergift starttijdwatergift starttijdwatergift starttijdwatergift starttijdwatergift starttijdwatergift
  TempLong = millis();  // reset innactive time counter
  if (menu == 6) {
    lcd.setCursor(0, 0);
    lcd.print(F("6 Start Hour water"));
    lcd.setCursor(9, 2);
    lcd.print(starttijdwatergift);
    lcd.print(F(" "));
  }
  while (menu == 6) {
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
      starttijdwatergift = starttijdwatergift + (rval);          // 1  step
      if (starttijdwatergift <= 0) starttijdwatergift = 0;       // starttijdwatergift
      if (starttijdwatergift >= 23) starttijdwatergift = 23;     // starttijdwatergift max 12 o clock
      //should check that starttijdwatergift < eindtijdwatergift
      TempLong = millis();  //reset innactive time counter
      lcd.setCursor(9, 2);
      lcd.print(starttijdwatergift);
      lcd.print(F(" "));

    }

    if (SetButton() == LOW) {                                    // if setbutton==LOW, pulled up by resistor, LOW is pressed
      while (SetButton() == LOW) {
        /*wait for button released*/
      }
      menu = 7;
      lcd.clear();

      EEPROM.get(40, TempInt);                                   // limmited write to eeprom = read is unlimmited
      if (starttijdwatergift != TempInt) {                       // only write to eeprom if value is different
        EEPROM.put(40, starttijdwatergift);                       // put already checks if val is needed to write
        lcd.setCursor(0, 0);
        lcd.print(F("Saving to EEPROM"));
        lcd.setCursor(0, 2);
        lcd.print("old= ");
        lcd.print(TempInt);
        lcd.print(F(" new= "));
        lcd.print(starttijdwatergift);
        TempLong = millis();                                    // load millis() into Templong for next countdown delay
        while ((millis() - TempLong)  <= 5000) {
          lcd.setCursor(19, 3);
          lcd.print(5 - (millis() - TempLong) / 1000);          // on lcd timeout countdown
        }
        delay(1000);  // want to see the zero 0
        for (int i = 0; i < 10; i++)Serial.println(F("starttijdwatergift DATA WRITTEN / SAVED TO EEPROM "));
        lcd.clear();
      }


    }
  }// end menu 6





  // 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7
  // eindtijdwatergift eindtijdwatergift eindtijdwatergift eindtijdwatergift eindtijdwatergift eindtijdwatergift eindtijdwatergift
  TempLong = millis();  // reset innactive time counter
  if (menu == 7) {
    lcd.setCursor(0, 0);
    lcd.print(F("7 End Hour water"));
    lcd.setCursor(9, 2);
    lcd.print(eindtijdwatergift);
    lcd.print(F(" "));
  }
  while (menu == 7) {
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
      eindtijdwatergift = eindtijdwatergift + (rval);          // 1  step
      if (eindtijdwatergift <= 0) eindtijdwatergift = 0;       // eindtijdwatergift
      if (eindtijdwatergift >= 23) eindtijdwatergift = 23;     // eindtijdwatergift max 12 o clock
      //should check that starttijdwatergift < eindtijdwatergift
      TempLong = millis();  //reset innactive time counter
      lcd.setCursor(9, 2);
      lcd.print(eindtijdwatergift);
      lcd.print(F(" "));

    }

    if (SetButton() == LOW) {                                    // if setbutton==LOW, pulled up by resistor, LOW is pressed
      while (SetButton() == LOW) {
        /*wait for button released*/
      }
      menu = 10;
      lcd.clear();

      EEPROM.get(45, TempInt);                                   // limmited write to eeprom = read is unlimmited
      if (eindtijdwatergift != TempInt) {                       // only write to eeprom if value is different
        EEPROM.put(45, eindtijdwatergift);                       // put already checks if val is needed to write
        lcd.setCursor(0, 0);
        lcd.print(F("Saving to EEPROM"));
        lcd.setCursor(0, 2);
        lcd.print("old= ");
        lcd.print(TempInt);
        lcd.print(F(" new= "));
        lcd.print(eindtijdwatergift);
        TempLong = millis();                                    // load millis() into Templong for next countdown delay
        while ((millis() - TempLong)  <= 5000) {
          lcd.setCursor(19, 3);
          lcd.print(5 - (millis() - TempLong) / 1000);          // on lcd timeout countdown
        }
        delay(1000);  // want to see the zero 0
        for (int i = 0; i < 10; i++)Serial.println(F("eindtijdwatergift DATA WRITTEN / SAVED TO EEPROM "));
        lcd.clear();
      }


    }
  }// end menu 7






  // 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10
  // Credits Credits Credits Credits Credits Credits Credits Credits Credits Credits Credits Credits Credits Credits Credits Credits
  TempLong = millis();  // reset innactive time counter
  if (menu == 10) {
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
  while (menu == 10) {
    lcd.setCursor(18, 3);
    if ((10 - (millis() - TempLong) / 1000) <= 9)lcd.print(" ");      // move 1 char when smaller a 10 wich is 2 chars
    lcd.print(10 - (millis() - TempLong) / 1000);                     // on lcd timeout countdown
    if ((millis() - TempLong)  > 10000) {
      delay(1000);  // want to see the zero 0
      lcd.clear();
      menu = 0;
      break;
    }

  }// end menu 10






  DateTime now = rtc.now();

  second_now = now.second();
  if (last_second != second_now) {       // only do this once each second

    last_second = second_now;



    Serial.print("millis() "); Serial.println(millis());
    Serial.println("");

    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(" ");
    Serial.print(now.day(), DEC);
    Serial.print("-");
    Serial.print(now.month(), DEC);
    Serial.print("-");
    Serial.print(now.year(), DEC);


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
    lcd.print(wetnesforstartwatergiftbeurt);
    lcd.print(" ");

    Serial.println(' ');

    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();

    Serial.print("Temperature: ");
    Serial.print(rtc.getTemperature());
    Serial.println(" C");

    Serial.println();
    Serial.print("1 read analogread A0 = "); Serial.println(analogRead(A0));
    Serial.print("1 read analogread A3 = "); Serial.println(analogRead(A3));
    Serial.print("100 read analogread A0 = "); Serial.println(sense1);
    Serial.print("100 read analogread A3 = "); Serial.println(sense2);
    Serial.print("analogread average = "); Serial.println((sense1 + sense2) / 2);


    Serial.print("sensor1 "); Serial.print(map(sense1, dry_sensor_one, wet_sensor_one, 0, 100)); Serial.println(" % wet");
    Serial.print("sensor2 "); Serial.print(map(sense2, dry_sensor_two, wet_sensor_two, 0, 100)); Serial.println(" % wet");

    averageinprocent = (map(sense1, dry_sensor_one, wet_sensor_one, 0, 100) + map(sense2, dry_sensor_two, wet_sensor_two, 0, 100)) / 2;
    Serial.print("Average "); Serial.print(averageinprocent); Serial.println(" % wet");

    Serial.println(' ');

    Serial.print("averageinprocent = "); Serial.print(averageinprocent); Serial.print(" switchpoint = "); Serial.println(wetnesforstartwatergiftbeurt);
    lcd.setCursor(0, 1);
    lcd.print("S1=");
    lcd.print(map(sense1, dry_sensor_one, wet_sensor_one, 0, 100)); lcd.print("   ");
    lcd.setCursor(7, 1);
    lcd.print("S2=");
    lcd.print(map(sense2, dry_sensor_two, wet_sensor_two, 0, 100)); lcd.print("   ");
    lcd.setCursor(14, 1);
    lcd.print("av=");
    lcd.print(averageinprocent); lcd.print("   ");

    if (now.hour() >= starttijdwatergift && now.hour() < eindtijdwatergift) {
      Serial.println("zit binnnen watergift mogelijk tijden");

      if (averageinprocent <= wetnesforstartwatergiftbeurt) {      // if soil is dryer as setpoint
        if (watergiftcounter == 0 || pauzetimer == 0 ) {          // if firstwaterpoor of day or pauzetimer==0
          if (watergiftcounter <= maximumaantalbeurtenperdag) {
            if (ValveStatus == 0) {
              starttime = millis();                              // save starttime millis only once
              ValveStatus = 1;                                   // next time whe do no get here because valvestatus is now 1
              watergiftcounter = watergiftcounter + 1;
            }
          }
        }
      }
    } else {
      lcd.setCursor(0, 2);
      if (now.hour() < starttijdwatergift) {
        lcd.print("OFF, Time < "); lcd.print(starttijdwatergift); lcd.print(" Hour");  // time not in range for watering, let the plants sleep
      }
      if (now.hour() >= eindtijdwatergift) {
        lcd.print("OFF, Time >= "); lcd.print(eindtijdwatergift); lcd.print(" Hour");  // time not in range for watering, let the plants sleep
      }
      lcd.setCursor(0, 3);
      lcd.print("Closed");
      delay(500);
      lcd.setCursor(0, 2);
      lcd.print("                    "); // erase the line of text
    }









    if (ValveStatus == 1) {
      backlightstart = millis();                                 // keep backlight on when valvestatus is open
      Serial.println("watergift start kraan open pomp aan");
      digitalWrite(13, HIGH);                  // 13 is onboard led en waterklep en/of waterpomp start
      startpauzetimer = millis();              // the latest time  we get into "if (ValveStatus == 1) {" will be used to set "startpauzetimer = millis();"
      pauzetimer =  (pauzenawatergiftbeurt * 60 * 1000L); // show pauzetime, wich countdown after valvestaus=0
      if (millis() - starttime <= (duurwatergiftbeurt * 1000L)) {
        lcd.setCursor(0, 3);
        lcd.print("Open");
        lcd.print(" ");
        watergifttimer = (starttime + (duurwatergiftbeurt * 1000L) - millis()) / 1000;
        if (watergifttimer <= 0)watergifttimer = 0;
        lcd.print(watergifttimer);
        lcd.print("      ");
        Serial.print("watergifttimer ");
        Serial.println(watergifttimer);
      }
    }



    if (millis() - starttime >= (duurwatergiftbeurt * 1000L)) {
      Serial.println("watergift stop / kraan dicht pomp uit");
      digitalWrite(13, LOW);          // 13 is onboard led  en waterklep en/of waterpomp stop
      ValveStatus = 0;

    }

    if (ValveStatus == 0) {
      pauzetimer =  (pauzenawatergiftbeurt * 60 * 1000L) - (millis() - startpauzetimer) ;
      if (pauzetimer <= 0) pauzetimer = 0;
      if (pauzetimer > 0)backlightstart = millis();            // keep backlight on when pauzetimer is running
    }
    if (watergiftcounter <= 0) pauzetimer = 0;                 // anders gaat pauzetimer onnodig lopen bij start of reboot


    lcd.setCursor(0, 2);
    lcd.print("count=");
    lcd.print(watergiftcounter);
    lcd.print(" pauze=");
    lcd.print(pauzetimer / 1000);
    lcd.print(" ");
    lcd.setCursor(0, 3);
    if (ValveStatus == 0) {
      lcd.print("Closed      ");     // dont know sometimes a long value at 0/close = erase it with extra spaces
      // looked like a overflow from long 0 countdown to max long = 2^32-1 value
      // should count signed long to -1 and i say if -1 count is 0
    }
    if (ValveStatus == 1) {
      lcd.print("Open");
    }
    lcd.setCursor(14, 3);
    lcd.print(rtc.getTemperature());
    lcd.print("C");
    Serial.print("pauzetimer ");
    Serial.println(pauzetimer / 1000);



  }  // end do this only once each second


  if (watergiftcounter > maximumaantalbeurtenperdag) {
    lcd.setCursor(0, 3);
    lcd.print(maximumaantalbeurtenperdag);
    lcd.print(" maximumaantalbeurt");
    Serial.print("maximumaantalbeurtenperdag ");
    Serial.println(maximumaantalbeurtenperdag);
    Serial.println("watergift stop / kraan dicht pomp uit");
    digitalWrite(13, LOW);          // 13 is onboard led  en waterklep en/of waterpomp stop

  }


  if (now.hour() == 23 && now.minute() == 59 && now.second() >= 59) {
    asm volatile("jmp 0");                                              // end of day reset/reboot arduino //start the day with a fresh millis() counter
    // no worry's about millis overflow every 50 days
    // and resets watergiftcounter
    watergiftcounter = 0; // if you trust the millis(); 49-50 days overflow       comment out the line // asm volatile("jmp 0");
  }

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
  lcd.clear();  //exit menu if 20 seconds innactive
  lcd.setCursor(6, 0);
  lcd.print(F("TimeOut"));
  lcd.setCursor(0, 1);
  lcd.print(F("Return to Mainscreen"));
  lcd.setCursor(0, 3);
  lcd.print(F("NOT saved to EEPROM!"));
  delay(2500);
  lcd.clear();
  menu = 0;
}




// Een Heitje voor een karweitje
// If I had a nickel ...
// A Penny for Sharing My Thoughts?
// http://www.paypal.me/LDijkman

// Arduino Advanced Automated Plant Watering System, StandAlone, Low Cost, Low Power Consumption
// Copyright 2021 Dirk Luberth Dijkman Bangert 30 1619GJ Andijk The Netherlands
