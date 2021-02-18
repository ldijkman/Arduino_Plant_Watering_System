/*
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
  
  GNU General Public License,
  which basically means that you may freely copy, change, and distribute it,
  but you may not impose any restrictions on further distribution,
  and you must make the source code available.
  
  https://www.gnu.org/licenses
  &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
*/
// https://github.com/ldijkman/Arduino_Plant_Watering_System
// https://youtu.be/1jKAxLyOY_s
//
// http://Paypal.Me/LDijkman
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


RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

#include <LiquidCrystal_I2C.h>       // Include LiquidCrystal_I2C library
// https://github.com/marcoschwartz/LiquidCrystal_I2C/archive/master.zip

LiquidCrystal_I2C lcd(0x27, 20, 4);  // Configure LiquidCrystal_I2C library with 0x27 address, 20 columns and 4 rows

int starttijdwatergift = 13;                             // 9 uur smorgens
int eindtijdwatergift = 21;                             // 21 uur savonds
long duurwatergiftbeurt = 1 * 60 * 1000L;     // 10 minuten in miliseconds  L is needed anders spoort arduino niet 30582 maakt hij van deze berekening
long pauzenawatergiftbeurt = 1 * 60 * 1000L;  // 30 minuten pauze in milliseconds
long watergifttimer;
long pauzetimer;
long startpauzetimer;
long starttime;
int wetnesforstartwatergiftbeurt = 30;                  // if smaller als 30% start watering

int dry_sensor_one = 800;                                  // my sensor value Dry in air   653
int wet_sensor_one = 437;                                  // my sensor value wet in water 285

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



void setup () {

  pinMode(13, OUTPUT);                 // pin 13 for valve open / close is also the onboard LED

  Serial.begin(115200);               // serial monitor


  lcd.begin();                        // lcd.begin(21, 22);  // Initialize I2C LCD module (SDA = GPIO21, SCL = GPIO22)
  // lcd.init();                      // Jo de Martelaer says lcd.begin(); does not work on win10, strange i have no problem with it on linux arduino ide 1.8.13

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
}




void loop () {

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
    lcd.print(now.day());
    lcd.print("-");
    lcd.print(now.month());
    lcd.print("-");
    lcd.print(now.year());
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
      lcd.print("Time NO Watering");   // time not in range for watering, let the plants sleep
      lcd.setCursor(0, 3);
      lcd.print("Closed");
      delay(500);
    }









    if (ValveStatus == 1) {
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

    pauzetimer =  pauzenawatergiftbeurt - (millis() - startpauzetimer) ;
    if (pauzetimer <= 0) pauzetimer = 0;
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
