#include "Arduino_LED_Matrix.h"

ArduinoLEDMatrix matrix;

enum GameState { PLAYING, GAME_OVER };
GameState gameState = PLAYING;

const int PIN_POT = A0;
const int PADDLE_H = 3;
const int MATRIX_W = 12;
const int MATRIX_H = 8;

float ballX = 10.0, ballY = 3.5;
float ballVX = -0.22, ballVY = 0.15;
int score = 0;
unsigned long lastUpdate = 0;
const int FRAME_DELAY = 20;

const uint8_t GLYPHS[10][5] = {
  {0x7, 0x5, 0x5, 0x5, 0x7}, {0x2, 0x6, 0x2, 0x2, 0x7},
  {0x7, 0x1, 0x7, 0x4, 0x7}, {0x7, 0x1, 0x7, 0x1, 0x7},
  {0x5, 0x5, 0x7, 0x1, 0x1}, {0x7, 0x4, 0x7, 0x1, 0x7},
  {0x7, 0x4, 0x7, 0x5, 0x7}, {0x7, 0x1, 0x1, 0x1, 0x1},
  {0x7, 0x5, 0x7, 0x5, 0x7}, {0x7, 0x5, 0x7, 0x1, 0x7}
};

void setup() {
  matrix.begin();
  randomSeed(analogRead(A1));
}

void loop() {
  if (gameState == PLAYING) {
    if (millis() - lastUpdate > FRAME_DELAY) {
      lastUpdate = millis();
      updatePhysics();
      renderFrame();
    }
  } else {
    processGameOver();
    resetSession();
  }
}

void updatePhysics() {
  int raw = analogRead(PIN_POT);
  int paddleY = map(constrain(raw, 20, 1000), 20, 1000, 0, MATRIX_H - PADDLE_H);

  ballX += ballVX;
  ballY += ballVY;

  if (ballY <= 0 || ballY >= (MATRIX_H - 1)) ballVY *= -1.0f;
  if (ballX >= (MATRIX_W - 1)) {
    ballX = MATRIX_W - 1;
    ballVX *= -1.0f;
  }

  if (ballX <= 0.8f && ballX >= -0.2f) {
    if (ballY >= (float)paddleY - 0.4f && ballY <= (float)paddleY + PADDLE_H - 0.6f) {
      ballVX = abs(ballVX) * 1.03f;
      ballVY *= 1.03f;
      ballX = 1.0f;
      score++;
    }
  }

  if (ballX < -1.0f) gameState = GAME_OVER;
}

void renderFrame() {
  uint8_t canvas[8][12] = {0};
  int raw = analogRead(PIN_POT);
  int paddleY = map(constrain(raw, 20, 1000), 20, 1000, 0, MATRIX_H - PADDLE_H);

  for (int i = 0; i < PADDLE_H; i++) canvas[paddleY + i][0] = 1;

  int bx = (int)round(ballX);
  int by = (int)round(ballY);
  if (bx >= 0 && bx < MATRIX_W && by >= 0 && by < MATRIX_H) canvas[by][bx] = 1;

  matrix.renderBitmap(canvas, MATRIX_H, MATRIX_W);
}

void processGameOver() {
  uint8_t xGraphic[8][12] = {
    {0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,1,0,0,0,0,1,0,0,0,0},
    {0,0,0,1,0,0,1,0,0,0,0,0}, {0,0,0,0,1,1,0,0,0,0,0,0},
    {0,0,0,0,1,1,0,0,0,0,0,0}, {0,0,0,1,0,0,1,0,0,0,0,0},
    {0,0,1,0,0,0,0,1,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0}
  };
  matrix.renderBitmap(xGraphic, 8, 12);
  delay(1200);

  int tens = score / 10;
  int ones = score % 10;

  for (int x = MATRIX_W; x > -10; x--) {
    uint8_t sF[8][12] = {0};
    if (tens > 0) blitDigit(sF, tens, x, 1);
    blitDigit(sF, ones, x + 5, 1);
    matrix.renderBitmap(sF, 8, 12);
    delay(75);
  }
}

void blitDigit(uint8_t f[8][12], int num, int xOff, int yOff) {
  for (int r = 0; r < 5; r++) {
    for (int c = 0; c < 3; c++) {
      if ((GLYPHS[num][r] >> (2 - c)) & 1) {
        int px = xOff + c;
        int py = yOff + r;
        if (px >= 0 && px < MATRIX_W && py >= 0 && py < MATRIX_H) f[py][px] = 1;
      }
    }
  }
}

void resetSession() {
  score = 0;
  ballX = 10.0;
  ballY = 3.5;
  ballVX = -0.22;
  ballVY = (random(0, 2) == 0) ? 0.15 : -0.15;
  gameState = PLAYING;
}