#include <Arduino.h>
#include <LiquidCrystal.h>

enum State { MENU, SET_TIME, SET_DATE };
enum AMPM { AM, PM };

State state = MENU;
AMPM ampm = PM;

int ticks = 0;
int timeCursor = 0;
int dateCursor = 0;

int hour = 12;
int minute = 0;
int second = 0;
int year = 2000;
int month = 1;
int day = 1;

int buttonPin = A0;
int potentiometerPin = A1;

int rs = 7;
int e = 8;
int d4 = 9;
int d5 = 10;
int d6 = 11;
int d7 = 12;
LiquidCrystal lcd(rs,e,d4,d5,d6,d7);

void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(potentiometerPin, INPUT);

  lcd.begin(16, 2);

  Serial.begin(9600);
}

int inputToIntervals (int input, int numberOfIntervals) {
  return (input  / 1023.0) * numberOfIntervals;
}

void updateTime () {
  second++;
  if (second >= 60) {
    second = 0;
    minute++;
  }
  if (minute >= 60) {
    minute = 0;
    hour++;
    if (hour == 12) {
      switch (ampm)
      {
        case AM:
          ampm = PM;
          break;
        case PM:
          ampm = AM;
          day++;
          break;
      }
    }
  }
  if (hour >= 13) {
    hour = 1;
  }
  if (day >= 32) {
    month++;
    day = 1;
  }
  if (month >= 13) {
    year++;
    month = 1;
  }
}

String ampmToString(AMPM ampm) {
  switch (ampm) {
    case AM:
      return "AM";
      break;
    case PM:
      return "PM";
      break;
  }
}

void loop() {
  int potentiometerInput = analogRead(potentiometerPin);
  int input = inputToIntervals(potentiometerInput, 10);
  int buttonRead = digitalRead(buttonPin);
  Serial.print("potentiometer: ");
  Serial.println(potentiometerInput);
  Serial.print("buttonRead outside: ");
  Serial.println(buttonRead);

  int menuCursor = input % 2 == 0 ? 0 : 1;
  String line1Cursor = menuCursor == 0 ? ">" : "";
  String line2Cursor = menuCursor == 1 ? ">" : "";
  String hourString = hour >= 10 ? String(hour) : "0" + String(hour);
  String minuteString = minute >= 10 ? String(minute) : "0" + String(minute);
  String secondString = second >= 10 ? String(second) : "0" + String(second);
  String monthString = month >= 10 ? String(month) : "0" + String(month);
  String dayString = day >= 10 ? String(day) : "0" + String(day);

  String hourCursor = timeCursor == 0 ? ">" : "";
  String minuteCursor = timeCursor == 1 ? ">" : "";
  String secondCursor = timeCursor == 2 ? ">" : "";
  String ampmCursor = timeCursor == 3 ? ">" : "";

  String monthCursor = dateCursor == 0 ? ">" : "";
  String dayCursor = dateCursor == 1 ? ">" : "";
  String yearCursor = dateCursor == 2 ? ">" : "";

  Serial.print("state: ");
  Serial.println(state);
  Serial.print("timeCursor: ");
  Serial.println(timeCursor);
  Serial.print("dateCursor: ");
  Serial.println(dateCursor);

  switch (state) {
    case MENU:
        Serial.println("menu switch case");
        lcd.setCursor(0, 0);
        lcd.print(line1Cursor + "Time:" + hourString + ":" + minuteString + ":" + secondString + ampmToString(ampm));
        lcd.setCursor(0, 1);
        lcd.print(line2Cursor + "Date:" + monthString + "/" + dayString + "/" + String(year));
        if (buttonRead) {
          if (menuCursor == 0) {
            state = SET_TIME;
          }
          else if (menuCursor == 1) {
            state = SET_DATE;
          }
        }
      break;
    case SET_TIME:
        Serial.println("set time switch case");
        lcd.setCursor(0, 0);
        lcd.print("Please set time:");
        lcd.setCursor(0, 1);
        lcd.print("Time:" +
          hourCursor + hourString + ":" +
          minuteCursor + minuteString + ":" +
          secondCursor + secondString +
          ampmCursor + ampmToString(ampm));
        Serial.print("buttonRead inside set time: ");
        Serial.println(buttonRead);

        if (timeCursor == 0) {
          input = inputToIntervals(potentiometerInput, 12);
          hour = input;
        } else if (timeCursor == 1) {
          input = inputToIntervals(potentiometerInput, 60);
          minute = input;
        } else if (timeCursor == 2) {
          input = inputToIntervals(potentiometerInput, 60);
          second = input;
        } else if (timeCursor == 3) {
          input = inputToIntervals(potentiometerInput, 10);
          ampm = input % 2 == 0 ? AM : PM;
        }

        if (buttonRead) {
          timeCursor++;
          if (timeCursor >= 4) {
            timeCursor = 0;
            state = MENU;
          }
        }
      break;
    case SET_DATE:
        Serial.println("set date switch case");
        lcd.setCursor(0, 0);
        lcd.print("Please set date:");
        lcd.setCursor(0, 1);
        lcd.print("Date:" +
          monthCursor + monthString + "/" +
          dayCursor + dayString + "/" +
          yearCursor + String(year));
        Serial.print("buttonRead inside set date: ");
        Serial.println(buttonRead);

        if (dateCursor == 0) {
          input = inputToIntervals(potentiometerInput, 12);
          month = input;
        } else if (dateCursor == 1) {
          input = inputToIntervals(potentiometerInput, 31);
          day = input;
        } else if (dateCursor == 2) {
          input = inputToIntervals(potentiometerInput, 50);
          year = 2000 + input;
        }

        if (buttonRead) {
          dateCursor++;
          if (dateCursor >= 3) {
            dateCursor = 0;
            state = MENU;
          }
        }
      break;
  }

  Serial.println("");
  Serial.println("");
  Serial.println("");

  delay(100);
  ticks++;
  if (ticks >= 10 && state != SET_TIME) { //update time only once per second
    ticks = 0;
    updateTime();
  }
  lcd.clear();
}
