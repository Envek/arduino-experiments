#define BUZZER_PIN 12
#define PLAYER_COUNT 2

int buttonPins[PLAYER_COUNT]  = {2, 3};  // Пины с кнопками игроков. Это должны быть пины с прерываниями!
int failLedPins[PLAYER_COUNT] = {11, 9}; // Пины с диодами, означающие победу (зелёные)
int succLedPins[PLAYER_COUNT] = {6,  5}; // Пины с диодами, означающие фальстарт (красные)

volatile bool started, finished;
volatile bool playerWon[PLAYER_COUNT] = {false, false};
volatile bool playerFailed[PLAYER_COUNT] = {false, false};

void handlePlayer1Press();
void handlePlayer2Press();
void handlePlayerPress(int player);

void setup() {
  for (int player = 0; player < PLAYER_COUNT; ++player) {
    pinMode(succLedPins[player], OUTPUT);
    pinMode(failLedPins[player], OUTPUT);
    pinMode(buttonPins[player],   INPUT_PULLUP);
    attachInterrupt(INT0, handlePlayer1Press, FALLING);
    attachInterrupt(INT1, handlePlayer2Press, FALLING);
  }
}

void handlePlayer1Press() { handlePlayerPress(0); }
void handlePlayer2Press() { handlePlayerPress(1); }

// Регистрируем и обрабатываем нажатия клавиш игроков
// Строгие условия нужны для того, чтобы избежать проблем с дребезгом кнопок
// так как триггеров Шмитта в схеме нет.
void handlePlayerPress(int player) {
  if (started && !finished && !playerFailed[player]) {
    finished = true;
    playerWon[player] = true;
    digitalWrite(succLedPins[player], HIGH);
  } else if (!started && !finished && !playerFailed[player]) {
    playerFailed[player] = true;
    digitalWrite(failLedPins[player], HIGH);
  }
}

void loop() {
  resetGameData();
  delay(random(2000, 7000));
  // Сигнал! Можно жать кнопки!
  started = true;
  tone(BUZZER_PIN, 3000, 250);
  finished = isAllPlayersFailed(); // Если оба игрока сделали фальстарт — игру нужно завершить.
  while (!finished); // Ждём конца игры
  // Всё ясно. Пищим и расходимся.
  tone(BUZZER_PIN, 4000, 250);
  delay(5000);
}

// Сбрасываем все данные и тушим все светодиоды
void resetGameData() {
  started  = false;
  finished = false;
  for (int player = 0; player < PLAYER_COUNT; ++player) {
    playerWon[player]    = false;
    playerFailed[player] = false;
    digitalWrite(succLedPins[player], LOW);
    digitalWrite(failLedPins[player], LOW);
  }
}

// Проверяем, вдруг оба игрока сделали фальстарт.
int isAllPlayersFailed() {
  bool anyoneNotFailed = false;
  for (int player = 0; player < PLAYER_COUNT; ++player) {
    if (!playerFailed[player]) anyoneNotFailed = true;
  }
  return !anyoneNotFailed;
}
