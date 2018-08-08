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
  int WORK_DURATION = 60 * 3 / 2;
  int WAIT_DURATION = 60 * 4;
  
  char secBuffer [5];

  // Включаем розетку
  digitalWrite(R1, HIGH);
  digitalWrite(R2, HIGH);

  lcd.begin(16, 2);

  lcd.setCursor(0, 0);
  lcd.print("MODE: WORK");   

  // Ждем 60 сек
  for (int sec = 0; sec < WORK_DURATION; sec ++) {
    lcd.setCursor(0, 1);
    delay(100);  
    int n = sprintf(secBuffer, "%d", sec);
    secBuffer[n] = '\0';
    lcd.print("TIME: ");
    lcd.print(secBuffer); 
    lcd.print("     ");
    delay(900);  
  }

  // Выключаем розетку
  digitalWrite(R1, LOW);
  digitalWrite(R2, LOW);

  lcd.begin(16, 2);

  lcd.setCursor(0, 0);
  lcd.print("MODE: WAIT");   

  // Ждем 60 сек
  for (int sec = 0; sec < WAIT_DURATION; sec ++) {
    lcd.setCursor(0, 1);
    delay(100);  
    int n = sprintf(secBuffer, "%d", sec);
    secBuffer[n] = '\0';
    lcd.print("TIME: ");
    lcd.print(secBuffer); 
    lcd.print("     ");
    delay(900);  
  }
}
