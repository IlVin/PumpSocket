#include <inttypes.h>
#include <LiquidCrystal.h>  // библиотека для работы с дисплеем
#include <sstream>
#include <string>
#include <stdio.h>
#include <cstdlib>


// пины подключенные к реле //
#define R1 5
#define R2 16

// Тайминги
#define ON_DURATION 30
#define OFF_DURATION 200
#define OL_OFF_DURATION 900
#define TRAN_DURATION 3

// Пробники тока и напряжения //
long I1 = 0;
long I2 = 0;
long V1 = 0;

// Данные для отображения на дисплее //
std::string statusString;
long sec;

long last_I1 = 0;
long last_I2 = 0;
bool isOverLoad = false;

// настраиваем дисплей, указывая пины к которым подключен дисплей
LiquidCrystal lcd { 4, 0, 2, 14, 12, 13 }; 
char lcdBuffer [30];

struct {
  long startMs;
  long diffMs;
} ctx_wait;


enum STATE { POWER_ON, WAIT_LOAD, TRAN, WORK, POWER_OFF, PAUSE };
STATE state;

// /////////////////////////////////////

// Парсит показания пробников
void parseString(std::string S){
  if ((S[0] == 'R') && (S[1] == 'M') && (S[2] == 'S')){
    switch(S[4]){
      case '5': I2 = atol(std::string(S,7).c_str()); break;
      case '6': I1 = atol(std::string(S,7).c_str()); break;
      case '7': V1 = atol(std::string(S,7).c_str()); break;
    }
  }
}

// Cчитывает показания пробников из UART
void readUART(){
  static std::string S;
  while (Serial.available()) {  // пока есть данные, читаем
    char ch = (char)(Serial.read());
    Serial.write(ch);
    if (ch == '\n') {
      parseString(S);
      S = "";
    } else {
      S = S + ch;
    }
  }
}

// Выводит информацию на дисплей
void showInfo() {
    lcd.setCursor(0, 0);
    long secs = sec % 60;
    long mins = (sec / 60) % 60;
    long hours = sec / 3600;
    int n = sprintf(lcdBuffer, "%4s: %02d:%02d:%02d        ", statusString.c_str(), hours, mins, secs);
    lcdBuffer[16] = '\0';
    lcd.write(lcdBuffer);

    lcd.setCursor(0, 1);
    n = sprintf(lcdBuffer, "%d : %d           ", I1, I2);
    if (isOverLoad) {
      lcdBuffer[14] = 'O';
      lcdBuffer[15] = 'L';
    }
    lcdBuffer[16] = '\0';
    lcd.write(lcdBuffer);
}

void state_power_on() {
  digitalWrite(R1, LOW);
  digitalWrite(R2, HIGH);
  lcd.begin(16, 2);
  ctx_wait.startMs = millis();
  state = WAIT_LOAD;
}

void state_wait_load() {
  statusString = "WAIT";
  ctx_wait.diffMs = millis() - ctx_wait.startMs;
  sec = ctx_wait.diffMs / 1000;
  if (I1 > 300 || I2 > 300) {
    lcd.begin(16, 2);
    ctx_wait.startMs = millis();
    state = TRAN;
  }
}

void state_wait_transition_process() {
  statusString = "TRAN";
  ctx_wait.diffMs = ctx_wait.startMs + 1000 * TRAN_DURATION - millis();
  sec = ctx_wait.diffMs / 1000;
  if (ctx_wait.diffMs <= 0) {
    last_I1 = I1;
    last_I2 = I2;
    ctx_wait.startMs = millis();
    state = WORK;
  }
}

void state_work() {
  statusString = " ON ";
  ctx_wait.diffMs = ctx_wait.startMs + 1000 * ON_DURATION - millis();
  sec = ctx_wait.diffMs / 1000;
  // Выключение по перегрузке
  isOverLoad = false;
  if (I1 > 4000 || I2 > 4000) {
    isOverLoad = true;
    state = POWER_OFF;
  }
  // Выключение по таймеру
  if (ctx_wait.diffMs <= 0) {
    state = POWER_OFF;
  }
  // При засасывании воздуха насос начинает потреблять меньше тока
  // При срабатывании реле давления насос перестает работать
  if (last_I1 > I1 && last_I1 - I1 > 150) {
    state = POWER_OFF;
  }
  if (last_I2 > I2 && last_I2 - I2 > 150) {
    state = POWER_OFF;
  }
  // При повышении давления в системе насос потребляет больше тока
  if (last_I1 < I1) last_I1 = I1;
  if (last_I2 < I2) last_I2 = I2;
}

void state_power_off() {
  digitalWrite(R1, HIGH);
  digitalWrite(R2, LOW);
  lcd.begin(16, 2);
  ctx_wait.startMs = millis();
  state = PAUSE;
}

void state_pause() {
  statusString = "OFF ";
  ctx_wait.diffMs = ctx_wait.startMs + 1000 * (isOverLoad ? OL_OFF_DURATION : OFF_DURATION) - millis();
  sec = ctx_wait.diffMs / 1000;
  if (ctx_wait.diffMs <= 0) {
    state = POWER_ON;
  }
}

void setup() {
  pinMode(R1, OUTPUT);
  pinMode(R2, OUTPUT);

  Serial.begin(19200, SERIAL_8N1);

  lcd.begin(16, 2);

  state = POWER_ON;
}

void loop() {
  switch (state) {
    case POWER_ON: state_power_on(); break;
    case WAIT_LOAD: state_wait_load(); break;
    case TRAN: state_wait_transition_process(); break;
    case WORK: state_work(); break;
    case POWER_OFF: state_power_off(); break;
    case PAUSE: state_pause(); break;
  }
  readUART();
  showInfo();
}
