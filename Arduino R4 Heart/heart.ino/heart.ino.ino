#include <Arduino_LED_Matrix.h>

ArduinoLEDMatrix matrix;

uint8_t heart[8][12] = {
  {0,1,1,0,0,0,0,1,1,0,0,0},
  {1,1,1,1,0,0,1,1,1,1,0,0},
  {1,1,1,1,1,1,1,1,1,1,1,0},
  {1,1,1,1,1,1,1,1,1,1,1,0},
  {0,1,1,1,1,1,1,1,1,1,0,0},
  {0,0,1,1,1,1,1,1,1,0,0,0},
  {0,0,0,1,1,1,1,1,0,0,0,0},
  {0,0,0,0,1,1,1,0,0,0,0,0}
};

void setup() {
  matrix.begin();
}

void loop() {
  matrix.renderBitmap(heart, 8, 12);
  delay(400);

  matrix.clear();
  delay(200);
}
