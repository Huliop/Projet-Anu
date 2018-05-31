//////////////////////
// Library Includes //
//////////////////////
#include <stdlib.h>
#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library

// R0, G0, B0, R1, G1, B1 should be connected to pins
// 2, 3, 4, 5, 6, and 7 respectively. Their pins aren't defined,
// because they're controlled directly in the library. These pins
// can be moved (somewhat):

#define CLK 11
#define LAT 10
#define OE  9

#define A   A0
#define B   A1
#define C   A2
#define D   A4

#define GREEN 1920
#define RED 61440
#define YELLOW 63360

// Enum UP, DOWN, LEFT, RIGHT to make programming easier:
enum directions {
  UP,    // 0
  DOWN,  // 1
  LEFT,  // 2
  RIGHT  // 3
};

int cursorX = 16;
int cursorY = 14;
directions directio = UP;
int randX = 0;
int randY = 0;
int start = 0;
int spd = 12000;
int timer = 0;
int jeu = 4;
int scr = 0;

RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false); // 32x32

int snakeX[11];
int snakeY[11];
int lgth = 3;



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup()
{
  matrix.begin();       // Initialize the matrix.
  Serial.begin(9600);   // Start serial
  randomSeed(analogRead(8));
  startscreen();
}


void loop()
{
  if (jeu == 0) { //initialisation de la matrice
    matrix.fillScreen(0);
    snakeX[0] = 16;
    snakeX[1] = 16;
    snakeX[2] = 16;
    snakeY[0] = 16;
    snakeY[1] = 15;
    snakeY[2] = 14;
    cursorX = 16;
    cursorY = 14;
    matrix.drawPixel(16, 16, GREEN);
    matrix.drawPixel(16, 15, GREEN);
    matrix.drawPixel(16, 14, GREEN);
    drawApple();
    directio = UP;
    scr = 0;
    jeu = 1;
  }
  else if (jeu == 4) { ///////////////////////////////////////// écran d'accueil
    if (Serial.available()) {
      char c = Serial.read();
      if (c == 'p') {
        jeu = 0;
      }
    }
  }
  else if (jeu == 1) { ///////////////////////////////////////// jeu en soit
    if (timer > spd) {
      timer=0;
      moveSnake(directio);
    }
    if (Serial.available())
    {
      char c = Serial.read();     // Read data into c
      switch (c)
      {
        case 'z':                 // w - Move up
          moveSnake(UP);
          directio = UP;
          break;
        case 's':                 // s - Move down
          moveSnake(DOWN);
          directio = DOWN;
          break;
        case 'q':                 // a - Move left
          moveSnake(LEFT);
          directio = LEFT;
          break;
        case 'd':                 // d - Move Right
          moveSnake(RIGHT);
          directio = RIGHT;
          break;
      }
    }
    timer++;
  }
  else if (jeu == 2) { ///////////////////////////////////////// écran game over
    matrix.fillScreen(0);
    endscreen();
    jeu = 3;
  }
  else if (jeu == 3) { ///////////////////////////////////////// display du score
    if (Serial.available()) {
      char c = Serial.read();
      if (c == 'r') {
        matrix.fillScreen(0);
        score();
        jeu = 5;
      }
    }
  }
  else if (jeu == 5) { ///////////////////////////////////////// attente de restart
    if (Serial.available()) {
      char c = Serial.read();
      if (c == 'r') {
        jeu = 0;
      }
    }
  }
}

void decale(int list[]) {
  for (int i=0; i<(lgth - 1) ; i++) {
    list[i] = list[i+1];
  }
}

bool isSnake(int x, int y) {
  return getPixelColor(x, y) == GREEN;
}

bool isWall(int x, int y) {
  return getPixelColor(x, y) == YELLOW;
}

bool isColored(int x, int y) {
  return (isSnake(x, y) or isWall(x, y));
}

void startscreen() {
  matrix.drawChar(1, 9, 'P', GREEN, 1, 1);
  matrix.drawChar(7, 9, 'R', GREEN, 1, 1);
  matrix.drawChar(13, 9, 'E', GREEN, 1, 1);
  matrix.drawChar(19, 9, 'S', GREEN, 1, 1);
  matrix.drawChar(25, 9, 'S', GREEN, 1, 1);
  matrix.drawChar(13, 20, 'P', GREEN, 1, 1);
}

void endscreen() {
  matrix.drawChar(4, 7, 'G', RED, 1, 1);
  matrix.drawChar(11, 7, 'A', RED, 1, 1);
  matrix.drawChar(17, 7, 'M', RED, 1, 1);
  matrix.drawChar(23, 7, 'E', RED, 1, 1);
  matrix.drawChar(4, 19, 'O', RED, 1, 1);
  matrix.drawChar(10, 19, 'V', RED, 1, 1);
  matrix.drawChar(16, 19, 'E', RED, 1, 1);
  matrix.drawChar(22, 19, 'R', RED, 1, 1);
}

void score() {
  matrix.drawChar(1, 7, 'S', YELLOW, 1, 1);
  matrix.drawChar(7, 7, 'C', YELLOW, 1, 1);
  matrix.drawChar(13, 7, 'O', YELLOW, 1, 1);
  matrix.drawChar(19, 7, 'R', YELLOW, 1, 1);
  matrix.drawChar(25, 7, 'E', YELLOW, 1, 1);
  if (scr < 10) {
    matrix.drawChar(13, 19, transform(scr), YELLOW, 1, 1);
  }
  else if (scr < 100) {
    int i = scr % 10;
    matrix.drawChar(16, 19, transform(i), YELLOW, 1, 1);
    int a = (scr - i)/10;
    matrix.drawChar(10, 19, transform(a), YELLOW, 1, 1);
  }
  else {
    int i = scr % 100;
    matrix.drawChar(19, 19, transform(i), YELLOW, 1, 1);
    int a = ((scr - i)/100) % 10;
    matrix.drawChar(13, 19, transform(i), YELLOW, 1, 1);
    int b = (scr - i - 10*a)/100;
    matrix.drawChar(7, 19, transform(i), YELLOW, 1, 1);
  }
}

char transform(int i) {
  switch (i) {
    case 0:
      return '0';
    case 1:
      return '1';
    case 2:
      return '2';
    case 3:
      return '3';
    case 4:
      return '4';
    case 5:
      return '5';
    case 6:
      return '6';
    case 7:
      return '7';
    case 8:
      return '8';
    case 9:
      return '9';
  }
}

void drawApple() {
  do {
  randX = random(0, 32);
  randY = random(0, 32);
  } while (isSnake(randX, randY) or isWall(randX, randY));
  matrix.drawPixel(randX, randY, RED);
}

void mue() {
  snakeX[0] = snakeX[8];
  snakeX[1] = snakeX[9];
  snakeX[2] = snakeX[10];
  snakeY[0] = snakeX[8];
  snakeY[1] = snakeX[9];
  snakeY[2] = snakeX[10];
}

void moveSnake(directions direc) {
  moveCursor(direc);
  int oldX = snakeX[0];
  int oldY = snakeY[0];
  if ((randX == cursorX) && (randY == cursorY)) {
    drawApple();
    lgth++;
    scr++;
    if (lgth==11) {
      for (int i=0; i<(lgth -1); i++) {
        int x = snakeX[i];
        int y = snakeY[i];
        matrix.drawPixel(x, y, YELLOW);
      }
      lgth = 3;
      mue();
      spd -= 2000;
    }
  }
  else {
    decale(snakeX);
    decale(snakeY);
    matrix.drawPixel(oldX, oldY, 0);
  }
  snakeX[lgth - 1] = cursorX;
  snakeY[lgth - 1] = cursorY;
}

void moveCursor(directions dir)
{
  switch (dir)
  {
    case UP:
      if (cursorY == 0) { cursorY = 31; }
      else { cursorY --; }
      break;
    case DOWN:
      if (cursorY == 31) { cursorY = 0; }
      else { cursorY ++; }
      break;
    case LEFT:
      if (cursorX == 0) { cursorX = 31; }
      else { cursorX --;}
      break;
    case RIGHT:
      if (cursorX == 31) { cursorX = 0; }
      else { cursorX ++; }
      break;
  }
  if (!isColored(cursorX, cursorY)) {
    matrix.drawPixel(cursorX, cursorY, GREEN);
  }
  else {
    jeu = 2;
    spd = 12000;
    lgth = 3;
  }
}

uint16_t getPixelColor(uint8_t x, uint8_t y)
{
  uint8_t * ptr; // A movable pointer to read data from
  uint16_t r = 0, g = 0, b = 0; // We'll store rgb values in these
  uint8_t nPlanes = 4; // 4 planes. This is static in the library
  uint8_t nRows = matrix.height() / 2; // 16 for 32 row, 8 for 16

  // Get a pointer to the matrix backBuffer, where the pixel/
  // color data is stored:
  uint8_t * buf = matrix.backBuffer();

  // Data for upper half is stored in lower bits of each byte
  if (y < nRows)
  {
    ptr = &buf[y * matrix.width() * (nPlanes - 1) + x];
    // get data from plane 0:
    if (ptr[64] & 0x01)
      r = 1;
    if (ptr[64] & 0x02)
      g = 1;
    if (ptr[32] & 0x01)
      b = 1;
    // get data from other planes:
    for (uint8_t i = 2; i < (1 << nPlanes);  i <<= 1)
    {
      if (*ptr & 0x04)
        r |= i;
      if (*ptr & 0x08)
        g |= i;
      if (*ptr & 0x10)
        b |= i;
      ptr += matrix.width();
    }
  }
  else
  {
    ptr = &buf[(y - nRows) * matrix.width() * (nPlanes - 1) + x];
    // get data from plane 0
    if (ptr[32] & 0x02)
      r = 1;
    if (*ptr & 0x01)
      g = 1;
    if (*ptr & 0x02)
      b = 1;
    for (uint8_t i = 2; i < (1 << nPlanes); i <<= 1)
    {
      if (*ptr & 0x20)
        r |= i;
      if (*ptr & 0x40)
        g |= i;
      if (*ptr & 0x80)
        b |= i;
      ptr += matrix.width();
    }
  }
  return (r << 12) | (g << 7) | (b << 1);
}
