#define BUZZER_PIN   8
#define RED_A_PIN   10
#define RED_B_PIN    5
#define GREEN_A_PIN 11
#define GREEN_B_PIN  6
#define YELLOW_PIN   9
#define S_DATA_PIN   2
#define S_LATCH_PIN  3
#define S_CLOCK_PIN  4
#define BAR_09_PIN  12
#define BAR_10_PIN  13
#define POT_PIN     A0

#define YELLOW  128
#define RED_A    64
#define GREEN_A  32
#define RED_B    16
#define GREEN_B   8
#define BUZZER    4

char turn = 'A';
int turnLengthMillis = 11000;
int turnLengthMinMillis = 10000;
int turnLengthMaxMillis = 30000;
int yellowLengthMillis = 1500;
int blinkingGreenLengthMillis = 3000;
int turnStartedAt = 0;
bool enabled = true;

void barDisplay(int count);
void calculateTurnLength();

void setup() {
  pinMode(BUZZER_PIN,  OUTPUT);
  pinMode(RED_A_PIN,   OUTPUT);
  pinMode(RED_B_PIN,   OUTPUT);
  pinMode(GREEN_A_PIN, OUTPUT);
  pinMode(GREEN_B_PIN, OUTPUT);
  pinMode(YELLOW_PIN,  OUTPUT);
  pinMode(S_DATA_PIN,  OUTPUT);
  pinMode(S_LATCH_PIN, OUTPUT);
  pinMode(S_CLOCK_PIN, OUTPUT);
  pinMode(BAR_09_PIN,  OUTPUT);
  pinMode(BAR_10_PIN,  OUTPUT);
  pinMode(POT_PIN,      INPUT);
  // Попищим и посветим для проверки
  tone(BUZZER_PIN, 3000, 1000);
  digitalWrite(RED_A_PIN,   HIGH);
  digitalWrite(RED_B_PIN,   HIGH);
  digitalWrite(GREEN_A_PIN, HIGH);
  digitalWrite(GREEN_B_PIN, HIGH);
  digitalWrite(YELLOW_PIN,  HIGH);
  digitalWrite(BAR_09_PIN,  HIGH);
  digitalWrite(BAR_10_PIN,  HIGH);
  barDisplay(10);
  delay(1000);
  noTone(BUZZER_PIN);
  // Мы начинаем КВН!
  calculateTurnLength();
  turnStartedAt = millis();
}

inline int millisElapsed() {
  return millis() - turnStartedAt;
}

inline int millisLeft() {
  return turnLengthMillis - (millisElapsed());
}


int stage() {
  int stg = 0;
  if (enabled) {
    int left = millisLeft();
    if (left < yellowLengthMillis) {
      stg += YELLOW;
      stg += (turn == 'A') ? RED_B : RED_A;
    } else {
      stg += (turn == 'A') ? RED_B   : RED_A;
      if (left > (yellowLengthMillis+blinkingGreenLengthMillis) || left%1000 < 500)
        stg += (turn == 'A') ? GREEN_A : GREEN_B;
      if ((left > (yellowLengthMillis+blinkingGreenLengthMillis) && left%1000 > 900) || (left <= (yellowLengthMillis+blinkingGreenLengthMillis) && left%500 > 400))
        stg += BUZZER;
    }
  } else {
    if(millis()%1000 < 500) stg += YELLOW;
  }
  return stg;
}

void loop() {
  int stg = stage();
  digitalWrite(RED_A_PIN,   (stg & RED_A)   ? HIGH : LOW);
  digitalWrite(RED_B_PIN,   (stg & RED_B)   ? HIGH : LOW);
  digitalWrite(GREEN_A_PIN, (stg & GREEN_A) ? HIGH : LOW);
  digitalWrite(GREEN_B_PIN, (stg & GREEN_B) ? HIGH : LOW);
  digitalWrite(YELLOW_PIN,  (stg & YELLOW)  ? HIGH : LOW);
  if (stg & BUZZER) {
    tone(BUZZER_PIN, 3000);
  } else {
    noTone(BUZZER_PIN);
  }
  int left = millisLeft();
  if (left > 1000)
    barDisplay((millisLeft() - yellowLengthMillis) / ((turnLengthMillis - yellowLengthMillis) / 10) + 1);
  else barDisplay(0);
  if (millisLeft() <= 0) {
    turn = (turn == 'A') ? 'B' : 'A';
    calculateTurnLength();
    turnStartedAt = millis();
  }
}

void barDisplay(int count) {
  byte mask = 0, b09 = LOW, b10 = LOW;
  if (count) {
    byte cnt = count > 8 ? 8 : count;
    for (byte i = 0; i < cnt; i++) mask += (1 << i);
  }
  if (count > 8) b09 = HIGH;
  if (count > 9) b10 = HIGH;
  digitalWrite(S_LATCH_PIN, LOW);
  shiftOut(S_DATA_PIN, S_CLOCK_PIN, MSBFIRST, mask);
  digitalWrite(S_LATCH_PIN, HIGH);
  digitalWrite(BAR_09_PIN, b09);
  digitalWrite(BAR_10_PIN, b10);
}

void calculateTurnLength() {
  int pot_rotation = analogRead(POT_PIN);
  float partLength = float(turnLengthMaxMillis - turnLengthMinMillis) / 1023;
  turnLengthMillis = turnLengthMinMillis + (partLength * pot_rotation);
}
