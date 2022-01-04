// Two different pins for yellow are used to avoid overloading Arduino Mini digital output pins (they have limit of 40 mA each).

#define RED_A_PIN    2
#define YELLOW_A_PIN 3
#define GREEN_A_PIN  4
#define RED_B_PIN    5
#define YELLOW_B_PIN 6
#define GREEN_B_PIN  7
#define SWITCH_PIN   8

#define RED_A    32
#define YELLOW_A 16
#define GREEN_A   8
#define RED_B     4
#define YELLOW_B  2
#define GREEN_B   1

char turn = 'A'; // A or B
int yellowLengthMillis = 1500;
int turnLengthMillis = 13500 + yellowLengthMillis;
int blinkingGreenLengthMillis = 3500;
int turnStartedAt = 0;
bool operating = true; // true: red/green turns, false: blinking yellow
boolean lastButton = LOW;
boolean currentButton = LOW;

void setup() {
  pinMode(SWITCH_PIN,   INPUT_PULLUP);
  pinMode(RED_A_PIN,    OUTPUT);
  pinMode(RED_B_PIN,    OUTPUT);
  pinMode(YELLOW_A_PIN, OUTPUT);
  pinMode(YELLOW_B_PIN, OUTPUT);
  pinMode(GREEN_A_PIN,  OUTPUT);
  pinMode(GREEN_B_PIN,  OUTPUT);
  // Ligths test
  digitalWrite(RED_A_PIN,    HIGH);
  digitalWrite(RED_B_PIN,    HIGH);
  digitalWrite(YELLOW_A_PIN, HIGH);
  digitalWrite(YELLOW_B_PIN, HIGH);
  digitalWrite(GREEN_A_PIN,  HIGH);
  digitalWrite(GREEN_B_PIN,  HIGH);
  delay(1000);
  // Let's start!
  turnStartedAt = millis();
}

inline int millisLeft() {
  return turnLengthMillis - (millis() - turnStartedAt);
}

int stage() {
  int stg = 0;
  if (operating) {
    int left = millisLeft();
    if (left < yellowLengthMillis) {
      stg += YELLOW_A + YELLOW_B;
      stg += (turn == 'A') ? RED_B : RED_A;
    } else {
      stg += (turn == 'A') ? RED_B : RED_A;
      if (left > (yellowLengthMillis+blinkingGreenLengthMillis) || left%1000 < 500)
        stg += (turn == 'A') ? GREEN_A : GREEN_B;
    }
  } else {
    if(millis()%2000 < 1000) stg += YELLOW_A + YELLOW_B;
  }
  return stg;
}

// Dumb button debounce (don't want to place Schmitt trigger to the schema)
boolean debounce(boolean last)
{
  boolean current = digitalRead(SWITCH_PIN);
  if (last != current)
  {
    delay(5);
    current = digitalRead(SWITCH_PIN);
  }
  return current;
}

void loop() {
  int stg = stage();
  digitalWrite(RED_A_PIN,    (stg & RED_A)    ? HIGH : LOW);
  digitalWrite(RED_B_PIN,    (stg & RED_B)    ? HIGH : LOW);
  digitalWrite(GREEN_A_PIN,  (stg & GREEN_A)  ? HIGH : LOW);
  digitalWrite(GREEN_B_PIN,  (stg & GREEN_B)  ? HIGH : LOW);
  digitalWrite(YELLOW_A_PIN, (stg & YELLOW_A) ? HIGH : LOW);
  digitalWrite(YELLOW_B_PIN, (stg & YELLOW_B) ? HIGH : LOW);

  if (millisLeft() <= 0) {
    turn = (turn == 'A') ? 'B' : 'A';
    turnStartedAt = millis();
  }

  currentButton = debounce(lastButton);
  if (lastButton == LOW && currentButton == HIGH) {
    operating = !operating;
    turnStartedAt = millis();
  }
  lastButton = currentButton;
}
