
// https://github.com/ldijkman/Arduino_Plant_Watering_System
// http://Paypal.Me/LDijkman

// komt wat logica in
// watergift timer werkt, pauzetimer werkt ook
// maar is alleen zichtbaar met LCD
// kan makkelijker met gewone delays
// maar dan staat alles stil als je bijvoorbeeld een delay van 10 minuten hebt
// gebeurt er niks gedurende die 10 minuten
//
//
// Copyright 2021 Dirk Luberth Dijkman Bangert 30 1619GJ Andijk The Netherlands
/*
&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

GNU General Public License,

which basically means that you may freely copy, change, and distribute it,

but you may not impose any restrictions on further distribution,

and you must make the source code available.

&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
*/

// Jo de Martelaer arduino uno watergift
// Boomkwekerij Hortus Conclusus
// https://www.hortusconclusus.be/
//
//
// DS3231 connected to +5vdc GND SCL SDA
// 4x20 i2c LCD connected to +5vdc GND SCL SDA
// capacitive soil moisture sensors A0 A1
// Pump and or valve output D13 (is also onboard LED)

#include "RTClib.h" // https://github.com/adafruit/RTClib


RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

#include <LiquidCrystal_I2C.h>       // Include LiquidCrystal_I2C library
// https://github.com/marcoschwartz/LiquidCrystal_I2C/archive/master.zip

LiquidCrystal_I2C lcd(0x27, 20, 4);  // Configure LiquidCrystal_I2C library with 0x27 address, 20 columns and 4 rows

int starttijdwatergift = 9;                             // 9 uur smorgens
int eindtijdwatergift = 21;                             // 21 uur savonds
long duurwatergiftbeurt = 1 * 60 * 1000L;     // 10 minuten in miliseconds  L is needed anders spoort arduino niet 30582 maakt hij van deze berekening
long pauzenawatergiftbeurt = 1 * 60 * 1000L;  // 30 minuten pauze in milliseconds
long watergifttimer;
long pauzetimer;
long startpauzetimer;
long starttime;
int wetnesforstartwatergiftbeurt = 30;                  // if kleiner als 30 start watergift

int drogesensor = 626;                                  // een droge sensor geeft bij mij 653
int nattesensor = 256;                                  // een natte sensor geeft bij mij 285

int sense1;
int sense2;
int sense3;

int averageinprocent;                                  // gemiddelde van meerdere sensoren

byte watergiftcounter = 0;

byte ValveStatus = 0;

byte second_;
byte last_second;

byte maximumaantalbeurtenperdag = 8;



void setup () {

  pinMode(13, OUTPUT);                 // pin 13 for valve open / close is also the onboard LED

  Serial.begin(115200);               // serial monitor

  lcd.begin();                        // 21, 22);  // Initialize I2C LCD module (SDA = GPIO21, SCL = GPIO22)

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



  sense1 = analogRead(A0);
  sense2 = analogRead(A1);
  sense3 = analogRead(A2);

  DateTime now = rtc.now();

  second_ = now.second();
  if (last_second != second_) {       // only do this once each second

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
    Serial.print("analogread A0 = "); Serial.println(sense1);
    Serial.print("analogread A1 = "); Serial.println(sense2);
    Serial.print("analogread A2 = "); Serial.println(sense3);
    Serial.print("analogread average = "); Serial.println((sense1 + sense2 + sense3) / 3);

    // een droge sensor geeft bij mij 653
    // een natte sensor geeft bij mij 285

    Serial.print("sensor1 "); Serial.print(map(sense1, drogesensor, nattesensor, 0, 100)); Serial.println(" % wet");
    Serial.print("sensor2 "); Serial.print(map(sense2, drogesensor, nattesensor, 0, 100)); Serial.println(" % wet");
    Serial.print("sensor3 "); Serial.print(map(sense3, drogesensor, nattesensor, 0, 100)); Serial.println(" % wet");
    Serial.print("Average "); Serial.print(map(((sense1 + sense2 + sense3) / 3), drogesensor, nattesensor, 0, 100)); Serial.println(" % wet");


    Serial.println(' ');




    if (now.hour() >= starttijdwatergift && now.hour() <= eindtijdwatergift) {
      Serial.println("zit binnnen watergift mogelijk tijden");
      averageinprocent = map(((sense1 + sense2 ) / 2), drogesensor, nattesensor, 0, 100);
      Serial.print("averageinprocent = "); Serial.print(averageinprocent); Serial.print(" switchpoint = "); Serial.println(wetnesforstartwatergiftbeurt);
      lcd.setCursor(0, 1);
      lcd.print("S1=");
      lcd.print(map(sense1, drogesensor, nattesensor, 0, 100));
      lcd.print(" S2=");
      lcd.print(map(sense2, drogesensor, nattesensor, 0, 100));
      lcd.print(" av=");
      lcd.print(averageinprocent);
      lcd.print("   ");


      if (averageinprocent <= wetnesforstartwatergiftbeurt) {      // grond is droger als setpoint
        if (watergiftcounter == 0 || pauzetimer == 0 ) {          // eerstewatergift van de dag of pauzetimer==0
          if (watergiftcounter <= maximumaantalbeurtenperdag) {
            if (ValveStatus == 0) {
              starttime = millis();                              // save starttime millis only once
              ValveStatus = 1;                                   // next time whe do no get here because valvestatus is now 1
              watergiftcounter = watergiftcounter + 1;
            }
          }
        }
      }
    }









    if (ValveStatus == 1) {
      Serial.println("watergift start kraan open pomp aan");
      digitalWrite(13, HIGH);             // 13 is onboard led en waterklep en/of waterpomp start
      startpauzetimer = millis();
      if (millis() - starttime <= duurwatergiftbeurt) {
        lcd.setCursor(0, 3);
        if (ValveStatus == 0) {
          lcd.print("Closed");
        }
        if (ValveStatus == 1) {
          lcd.print("Open");
        }
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

    last_second = second_;

  }  // end do this only once each second


  if (watergiftcounter > maximumaantalbeurtenperdag) {
    // blijft in deze loop tot midnight reset/reboot
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
  }

}
