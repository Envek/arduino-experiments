#include <Servo.h>

#define RED_A_PIN      5 // Must be PWM-enabled
#define RED_B_PIN      6 // Must be PWM-enabled
#define WHITE_PIN      3 // Must be PWM-enabled
#define SWITCH_PIN  INT0 // INT0 is pin # 2
#define SERVO_1_PIN   10
#define SERVO_2_PIN   11

volatile bool crossingIsClosed = false;
volatile unsigned long turnStartedAt = 0;

int fadeDurationMillis = 500;
int blinkDurationMillis = 1000 + 2 * fadeDurationMillis;

int maxBrightness = 255;
int minBrightness = 0;

Servo servo1, servo2;
int servoMinAngle =  0;
int servoMaxAngle = 90;
int barrierChangeDurationMillis = 2000;

void setup() {
  pinMode(RED_A_PIN, OUTPUT);
  pinMode(RED_B_PIN, OUTPUT);
  pinMode(WHITE_PIN, OUTPUT);

  // Ligths test
  analogWrite(RED_A_PIN, maxBrightness);
  analogWrite(RED_B_PIN, maxBrightness);
  analogWrite(WHITE_PIN, maxBrightness);
  delay(1000);
  analogWrite(RED_A_PIN, minBrightness);
  analogWrite(RED_B_PIN, minBrightness);
  analogWrite(WHITE_PIN, minBrightness);

  attachInterrupt(SWITCH_PIN, changeTurn, FALLING);
  servo1.attach(SERVO_1_PIN);
  servo2.attach(SERVO_2_PIN);

  // Let's start!
  crossingIsClosed = false;
  turnStartedAt = millis();
}

void changeTurn() {
  crossingIsClosed = !crossingIsClosed;
  turnStartedAt = millis();
}

void loop() {
  unsigned long now = millis();

  unsigned long blinkCount = (now - turnStartedAt) / blinkDurationMillis;
  unsigned long currentBlinkDuration = (now - turnStartedAt) % blinkDurationMillis;
  int lightBrightness = minBrightness;

  int brightnessOfRedA  = minBrightness;
  int brightnessOfRedB  = minBrightness;
  int brightnessOfWhite = minBrightness;

  unsigned long barrierCount = (now - turnStartedAt) / barrierChangeDurationMillis;
  unsigned long currentBarrierDuration = (now - turnStartedAt) % blinkDurationMillis;

  if (currentBlinkDuration < fadeDurationMillis) { // fading in
    lightBrightness = constrain(maxBrightness * currentBlinkDuration / fadeDurationMillis, minBrightness, maxBrightness);
  } else if (currentBlinkDuration > (blinkDurationMillis - fadeDurationMillis)) { // fading out
    lightBrightness = constrain(maxBrightness * (blinkDurationMillis - currentBlinkDuration) / fadeDurationMillis, minBrightness, maxBrightness);
  } else {
    lightBrightness = maxBrightness;
  }

  if (crossingIsClosed) {
    brightnessOfRedA = (blinkCount % 2 == 0) ? lightBrightness : minBrightness;
    brightnessOfRedB = (blinkCount % 2 == 1) ? lightBrightness : minBrightness;
    brightnessOfWhite = minBrightness;
  } else {
    brightnessOfWhite = (blinkCount % 2 == 0) ? lightBrightness : minBrightness;
    brightnessOfRedA = minBrightness;
    brightnessOfRedB = minBrightness;
  }

  int angleOfBarrier1 = servoMinAngle;
  int angleOfBarrier2 = servoMinAngle;
  unsigned int angle = constrain(servoMaxAngle * currentBarrierDuration / barrierChangeDurationMillis, servoMinAngle, servoMaxAngle);

  if (crossingIsClosed) {
    angleOfBarrier1 = barrierCount > 1 ? servoMinAngle : barrierCount < 1 ? servoMaxAngle : servoMaxAngle - angle;
    angleOfBarrier2 = barrierCount > 2 ? servoMinAngle : barrierCount < 2 ? servoMaxAngle : servoMaxAngle - angle;
  } else {
    angleOfBarrier1 = barrierCount > 0 ? servoMaxAngle : barrierCount < 0 ? servoMinAngle : angle;
    angleOfBarrier2 = barrierCount > 1 ? servoMaxAngle : barrierCount < 1 ? servoMinAngle : angle;
  }

  analogWrite(RED_A_PIN, brightnessOfRedA);
  analogWrite(RED_B_PIN, brightnessOfRedB);
  analogWrite(WHITE_PIN, brightnessOfWhite);

  servo1.write(angleOfBarrier1);
  servo2.write(angleOfBarrier2);
}
