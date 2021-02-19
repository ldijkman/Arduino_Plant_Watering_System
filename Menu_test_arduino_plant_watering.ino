/*
   removed the date for showing sp= setpoint               (add another LCD display with different i2c adress? ;-)
   added rotary encoder push button KY-040 https://www.google.com/search?q=KY-040
   trying to make a start with menu for changing parameters
   shows only text now => inside menu when you push the rotary encoder button
           a bit of copy paste modify from http://www.sticker.tk/forum/index.php?action=view&id=296
                                           http://www.sticker.tk/forum/index.php?action=view&id=299

  maybe a way to discharge analog read, affecting other pin????
  switch from low output to analogread
  i do not have 2 sensors to test it, only a floating second analog pin
  i dont know, im no programmer just a prutser / thinker
    pinMode(A0,OUTPUT);
    digitalWrite(A0,LOW);
    pinMode(A0,INPUT);
    delay(75);
    test1 = test1 + analogRead(A0);
    pinMode(A0,OUTPUT);
    digitalWrite(A0,LOW);
    pinMode(A3,OUTPUT);
    digitalWrite(A3,LOW);
    pinMode(A3,INPUT);
    delay(75);
    test2 = test2 + analogRead(A3);
    pinMode(A3,OUTPUT);
    digitalWrite(A3,LOW);
*/


/*
  &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

  Arduino Automated Plant Watering System

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
// komt wat logica in
// watergift timer werkt, pauzetimer werkt ook
// maar is alleen zichtbaar met LCD
// kan makkelijker met gewone delays
// maar dan staat alles stil als je bijvoorbeeld een delay van 10 minuten hebt
// gebeurt er niks gedurende die 10 minuten
//
//
// Jo de Martelaer arduino uno watergift
// Boomkwekerij Hortus Conclusus
// https://www.hortusconclusus.be/
// Catalogus https://www.hortusconclusus.be/collections
//
//
// DS3231 connected to +5vdc GND SCL SDA
// 4x20 i2c LCD connected to +5vdc GND SCL SDA
// capacitive soil moisture sensors A0 A3
// Pump and or valve output D13 (is also onboard LED)

#include "RTClib.h" // https://github.com/adafruit/RTClib

#include <EEPROM.h>

RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

#include <LiquidCrystal_I2C.h>       // Include LiquidCrystal_I2C library
// https://github.com/marcoschwartz/LiquidCrystal_I2C/archive/master.zip

LiquidCrystal_I2C lcd(0x27, 20, 4);  // Configure LiquidCrystal_I2C library with 0x27 address, 20 columns and 4 rows

int starttijdwatergift = 10;                             // 9 uur smorgens
int eindtijdwatergift = 21;                             // 21 uur savonds
long duurwatergiftbeurt = 1 * 60 * 1000L;     // 10 minuten in miliseconds  L is needed anders spoort arduino niet 30582 maakt hij van deze berekening
long pauzenawatergiftbeurt = 1 * 60 * 1000L;  // 30 minuten pauze in milliseconds
long watergifttimer;
long pauzetimer;
long startpauzetimer;
long starttime;

long TempLong;

int wetnesforstartwatergiftbeurt = 30;                  // if smaller als 30% start watering

int dry_sensor_one = 730;                                  // my sensor value Dry in air   653
int wet_sensor_one = 428;                                  // my sensor value wet in water 285

int dry_sensor_two = 800;                                  // my sensor value Dry in air   653
int wet_sensor_two = 437;                                  // my sensor value wet in water 285

int sense1;
int sense2;


int averageinprocent;                                  // gemiddelde van meerdere average multiple sensoren

byte watergiftcounter = 0;

byte ValveStatus = 0;

byte second_now;
byte last_second;

byte maximumaantalbeurtenperdag = 8;

// rotary encoder push button KY-040 https://www.google.com/search?q=KY-040
// Robust Rotary encoder reading
// Copyright John Main - best-microcontroller-projects.com
#define rotarybutton_SW 2           // input D2 rotary encoder SW
#define CLK 3                       // input D3 rotary encoder CLK
#define DATA 4                      // input D4 rotary encoder DT
// and connect rotary encoder to +5vdc and GND to 0vdc, -, min, GND, ground, or whatever they call it
static uint8_t prevNextCode = 0;
static uint16_t store = 0;

#define backlightstartbutton 5        // button on input D5, if pulled down to ground backlight goes on
//

byte menu = 0;

float SwitchOnTemp;
float TempFloat;
int TempInt;

long backlightofftimeout = 1 * 60 * 1000L;      // time to switch backlight off
long backlightstart;


void setup () {

  pinMode(13, OUTPUT);                  // pin 13 for valve open / close is also the onboard LED

  pinMode(rotarybutton_SW, INPUT);     // rotary encoder SW = pulled up by resistor on KY-040 to +
  pinMode(CLK, INPUT);                 // rotary encoder
  pinMode(CLK, INPUT_PULLUP);          // rotary encoder
  pinMode(DATA, INPUT);                // rotary encoder
  pinMode(DATA, INPUT_PULLUP);         // rotary encoder
 
  pinMode(backlightstartbutton, INPUT_PULLUP);            // button on input "backlightstartbutton",  if pulled down to ground backlight goes on

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



  // read stored valeus from EEPROM
  EEPROM.get(0, wetnesforstartwatergiftbeurt);
  //  EEPROM.get(5, CalibrationOffset);
  //  EEPROM.get(10, relayonpointbelowsetpoint);
  //  EEPROM.get(15, relayoffabovesetpoint);
  //  EEPROM.get(20, MaxTimeRelayMayBeonInSeconds);
  //  EEPROM.get(25, CoolorHeat);
  //  EEPROM.get(30, HighTempAlarmVal);
  //  EEPROM.get(35, LowTempAlarmVal);
  //  EEPROM.get(40, MaxTime2SetPoint);


  backlightstart = millis();          // load millis() in backlightstart

}




void loop () {

  if (millis() - backlightstart > backlightofftimeout) {                // if backlight timed out turn it off
    lcd.noBacklight();                    // Turn backlight OFF
  }
  if (millis() - backlightstart < backlightofftimeout) {
    lcd.backlight();                    // Turn backlight ON
  }

  if(digitalRead(backlightstartbutton)==LOW){backlightstart = millis();}   // button on input "backlightstartbutton",  if pulled down to ground backlight goes on

  long test1 = 0;
  long test2 = 0;
  for (int cc = 0; cc < 5; cc++) {     // do 100 readings
    //(void) analogRead(A0);            // reduce analog pins influence eachother?

    pinMode(A0, OUTPUT);
    digitalWrite(A0, LOW);
    pinMode(A0, INPUT);
    delay(75);
    test1 = test1 + analogRead(A0);
    pinMode(A0, OUTPUT);
    digitalWrite(A0, LOW);

    pinMode(A3, OUTPUT);
    digitalWrite(A3, LOW);
    pinMode(A3, INPUT);
    delay(75);
    test2 = test2 + analogRead(A3);
    pinMode(A3, OUTPUT);
    digitalWrite(A3, LOW);

  }
  sense1 = (test1 / 5);             // divide by 100
  sense2 = (test2 / 5);

  pinMode(A0, INPUT);
  pinMode(A3, INPUT);
  delay(75);


  if (!SetButton()) {                       // if !=not SetButton, SW = pulled up by resistor on KY-040 to +,  so LOW is button pressed
    menu = 1;
    backlightstart = millis();
    lcd.backlight(); 
    while (SetButton() == LOW) {                                    // while setbutton==LOW, pulled up by resistor, LOW is pressed
      // loop until button released
      // maybe a timer here
      // alarm if button never released
      lcd.setCursor(0, 0);
      lcd.print(F("                    "));
      lcd.setCursor(0, 1);
      lcd.print(F(" In Menu System Now "));
      lcd.setCursor(0, 2);
      lcd.print(F("   Release Button   "));
      lcd.setCursor(0, 3);
      lcd.print(F("                    "));
    }
    lcd.clear();
  }

  //1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
  //setpoint setpoint setpoint setpoint setpoint setpoint setpoint setpoint setpoint setpoint setpoint setpoint setpoint setpoint
  TempLong = millis();  // reset innactive time counter
  if (menu == 1) {
    lcd.setCursor(0, 0);
    lcd.print(F("1 Set SwitchPoint %"));
    lcd.setCursor(2, 2);
    lcd.print(wetnesforstartwatergiftbeurt);
    lcd.print(F(" % "));
  }
  while (menu == 1) {
    lcd.setCursor(16, 3);
    lcd.print(10 - (millis() - TempLong) / 1000); lcd.print(" ");
    if ((millis() - TempLong)  > 10000) {
      TimeOut();
      break;
    }

    float rval;
    if ( rval = read_rotary() ) {
      wetnesforstartwatergiftbeurt = wetnesforstartwatergiftbeurt + (rval);      
      if (wetnesforstartwatergiftbeurt >= 70) wetnesforstartwatergiftbeurt = 70;
      if (wetnesforstartwatergiftbeurt <= 10) wetnesforstartwatergiftbeurt = 10;
      TempLong = millis();  //reset innactive time counter
      lcd.setCursor(2, 2);
      lcd.print(wetnesforstartwatergiftbeurt);
      lcd.print(F(" % "));

    }

    if (SetButton()==LOW) {                                      // if setbutton==LOW, pulled up by resistor, LOW is pressed
      menu = 2;
      lcd.clear();
      delay(250);
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
          lcd.setCursor(16, 3);
          lcd.print(5 - (millis() - TempLong) / 1000); lcd.print(" ");
        }
        for (int i = 0; i < 10; i++)Serial.println(F("wetnesforstartwatergiftbeurt DATA WRITTEN / SAVED TO EEPROM "));
        lcd.clear();
      }


    }
  }






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
    lcd.print("sp=");                           // switchpoint
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
      digitalWrite(13, HIGH);             // 13 is onboard led en waterklep en/of waterpomp start
      startpauzetimer = millis();         // the latest time  we get into "if (ValveStatus == 1) {" will be used to set "startpauzetimer = millis();"
      if (millis() - starttime <= duurwatergiftbeurt) {
        lcd.setCursor(0, 3);
        lcd.print("Open");
        lcd.print(" ");
        watergifttimer = (starttime + duurwatergiftbeurt - millis()) / 1000;
        if (watergifttimer <= 0)watergifttimer = 0;
        lcd.print(watergifttimer);
        lcd.print("      ");
        Serial.print("watergifttimer ");
        Serial.println(watergifttimer);
      }
    }



    if (millis() - starttime >= duurwatergiftbeurt) {
      Serial.println("watergift stop / kraan dicht pomp uit");
      digitalWrite(13, LOW);          // 13 is onboard led  en waterklep en/of waterpomp stop
      ValveStatus = 0;

    }

    if (ValveStatus == 0) {
      pauzetimer =  pauzenawatergiftbeurt - (millis() - startpauzetimer) ;
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
      lcd.print("Closed");
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

