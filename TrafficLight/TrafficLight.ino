#define BUZZER_PIN   8
#define RED_A_PIN   10
#define RED_B_PIN    5
#define GREEN_A_PIN 11
#define GREEN_B_PIN  6
#define YELLOW_PIN   9

#define YELLOW  128
#define RED_A    64
#define GREEN_A  32
#define RED_B    16
#define GREEN_B   8
#define BUZZER    4

char turn = 'A';
int turnLengthMillis = 10000;
int turnStartedAt = 0;
bool enabled = true;

void setup() {
  pinMode(BUZZER_PIN,  OUTPUT);
  pinMode(RED_A_PIN,   OUTPUT);
  pinMode(RED_B_PIN,   OUTPUT);
  pinMode(GREEN_A_PIN, OUTPUT);
  pinMode(GREEN_B_PIN, OUTPUT);
  pinMode(YELLOW_PIN,  OUTPUT);
  // Попищим и посветим для проверки
  tone(BUZZER_PIN, 3000, 1000);
  digitalWrite(RED_A_PIN,   HIGH);
  digitalWrite(RED_B_PIN,   HIGH);
  digitalWrite(GREEN_A_PIN, HIGH);
  digitalWrite(GREEN_B_PIN, HIGH);
  digitalWrite(YELLOW_PIN,  HIGH);
  delay(1000);
  noTone(BUZZER_PIN);
  // Мы начинаем КВН!
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
    if (left < 1000) {
      stg += YELLOW;
      stg += (turn == 'A') ? RED_B : RED_A;
    } else {
      stg += (turn == 'A') ? RED_B   : RED_A;
      if (left > 4000 || left%1000 < 500)
        stg += (turn == 'A') ? GREEN_A : GREEN_B;
      if ((left > 4000 && left%1000 < 100) || (left <= 4000 && left%500 < 100))
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
  if (millisLeft() <= 0) {
    turn = (turn == 'A') ? 'B' : 'A';
    turnStartedAt = millis();
  }
}
