#include <inttypes.h>
#include <LiquidCrystal.h> //библиотека для работы с дисплеем
#include <sstream>
#include <string>
#include <stdio.h>
#include <cstdlib>


//пины подключенные к реле//
#define R1 5
#define R2 16
///////////////////////////

//настраиваем дисплей, указывая пины к которым подключен дисплей
LiquidCrystal lcd { 4, 0, 2, 14, 12, 13 }; 

void setup() {
  // put your setup code here, to run once:
  pinMode(R1, OUTPUT);
  pinMode(R2, OUTPUT);
  
  digitalWrite(R1, LOW);
  digitalWrite(R2, LOW);

  lcd.begin(16, 2);
}

void loop() {
  int WORK_DURATION = 30;
  int WAIT_DURATION = 180;
  char lcdBuffer [30];

  // Включаем розетку
  digitalWrite(R1, HIGH);
  digitalWrite(R2, HIGH);

  // Рабочий цикл
  lcd.begin(16, 2);
  for (int sec = WORK_DURATION; sec > 0; sec--) {
    int n = sprintf(lcdBuffer, "ON :%ds         ", sec);
    lcdBuffer[16] = '\0';
    lcd.setCursor(0, 0);
    lcd.print(lcdBuffer);
    delay(1000);
  }

  // Выключаем розетку
  digitalWrite(R1, LOW);
  digitalWrite(R2, LOW);

  // Цикл ожидания
  lcd.begin(16, 2);
  for (int sec = WAIT_DURATION; sec > 0; sec--) {
    int n = sprintf(lcdBuffer, "OFF:%ds         ", sec);
    lcdBuffer[16] = '\0';
    lcd.setCursor(0, 0);
    lcd.print(lcdBuffer);
    delay(1000);
  }
}
