#include <Arduino.h>
#include <U8g2lib.h>
#include "Pin.h"

#define WIDTH 128
#define HEIGHT 64
#define PADDLE_WIDTH 3
#define PADDLE_HEIGHT 15

//U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ 21, /* clock=*/ 18, /* data=*/ 17);     // Hectec WiFi_Kit_32 V3
  U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);                                      // For 1.30" display
//U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);                                     // For 0.96" display

typedef struct {
  uint8_t score;   /* data */
  uint8_t padX, padY;
} gamePlayer;

typedef struct {
  uint8_t radius;
  uint8_t x, y;
  int8_t  velX, velY;
} gameBall;

gamePlayer player[2];
gameBall ball;

void drawCenterLine();
void drawPlayers();
void drawBall();
void trigBall(uint8_t key);
void moveBall();
uint8_t readPaddlePos(uint8_t pot);
bool hitPaddle(uint8_t paddle, int8_t *angle);
void initGame();
void blink();

void setup() {
  Serial.begin(115200);
  PinInit();

  u8g2.begin();                                 // start display
  u8g2.setFont(u8g2_font_mystery_quest_32_tr);

  u8g2.clearBuffer();
  u8g2.drawStr(30, 40, (char *)"Pong");
  u8g2.sendBuffer();

  u8g2.setFont(u8g2_font_helvR10_tf);           // choose a suitable font
  delay(2000);

  initGame();
}

void loop() {
  static uint32_t task1, task1cnt, task1Interval=50;
  static uint32_t task2, task2cnt, task2Interval=100;
  static uint32_t task3, task3cnt, task3Interval=0;           // Dont run this task

  //------------------------//
  //      T A S K   1       //
  //------------------------//
  if (millis() > task1) {
    if (task1Interval > 0) {
      task1 = millis() + task1Interval;

      player[0].padY = readPaddlePos(0);
      player[1].padY = readPaddlePos(1);
      trigBall(PinReadButFlank());
      moveBall();
      
      u8g2.clearBuffer();
      drawCenterLine();
      drawPlayers();
      drawBall();
      u8g2.sendBuffer();
    }
  }

  //------------------------//
  //      T A S K   2       //
  //------------------------//
  if (millis() > task2) {
    if (task2Interval > 0) {
      task2 = millis() + task2Interval;

      uint8_t but = PinReadBut();
      PinStatusLed(but);
      blink();
    }
  }

  //------------------------//
  //      T A S K   3       //
  //------------------------//
  if (millis() > task3) {
    if (task3Interval > 0) {
      task3 = millis() + task3Interval;

      static uint8_t hour=0, minute=0, second=0;    
      if (++second > 59) {        // Every second
        second = 0;
        if (++minute > 59) {
          minute = 0;
          if (++hour > 23) {
            hour = 0;
          }
        }
      }
      if (task3cnt < 300)         // Every 5 minute
        task3cnt++;
      else {
        task3cnt = 0;
  //      WifiTimeRead();         // Update time from Internet
      }
    }
  }
}

void drawCenterLine() {
  const uint8_t x=WIDTH/2, yDelta=HEIGHT/8;
  uint8_t y;

  for (y=yDelta/4; y<HEIGHT; y+=yDelta)
    u8g2.drawVLine(x, y , yDelta/2);
}

void drawPlayers() {
  char text[10];

  u8g2.drawVLine(player[0].padX, player[0].padY , PADDLE_HEIGHT);
  u8g2.drawVLine(player[1].padX, player[1].padY , PADDLE_HEIGHT);

  sprintf(text, "%d", player[0].score);
  u8g2.drawStr(25, 15, text);
  sprintf(text, "%d", player[1].score);
  u8g2.drawStr(96, 15, text);
}

void drawBall() {
  u8g2.drawCircle(ball.x, ball.y, ball.radius);
}

void trigBall(uint8_t key) {
  if (key & 1) ball.velX += 1;
  if (key & 2) ball.velY += 1;
  if (key & 4) ball.velX -= 1;
}

void moveBall() {
  int8_t angle;

  ball.x += ball.velX;
  ball.y += ball.velY;

  if ((ball.y <= ball.radius) ||                              // Hit top
      (ball.y >= HEIGHT - ball.radius)) {                     // Hit bottom

    ball.velY *= -1;
  }

  if (hitPaddle(0, &angle) || hitPaddle(1, &angle)) {         // Hit paddle
    ball.velX *= -1;
    ball.velY = angle;
  } else if ((ball.x <= ball.radius) ||                       // Hit left 
             (ball.x >= WIDTH - ball.radius)) {               // Hit right
    
    if (ball.x <= ball.radius)
      player[1].score++;
    else
      player[0].score++;

    ball.x = WIDTH/2;                                       // Reset ball postion
    ball.y = HEIGHT/2;
    ball.velX = ball.velY = 0;                             // Stop the ball
  }
}

bool hitPaddle(uint8_t paddle, int8_t *angle) {
  bool retValue = false;
  uint8_t playerPadY;

  *angle = 0;
  switch (paddle) {
  case 0:
    retValue = ((player[0].padX >= ball.x - ball.radius) && 
                (player[0].padY <= ball.y) &&
                (player[0].padY+PADDLE_HEIGHT >= ball.y));
    playerPadY = player[0].padY;
    break;
  case 1:
    retValue = ((player[1].padX <= ball.x + ball.radius) && 
                (player[1].padY <= ball.y) &&
                (player[1].padY+PADDLE_HEIGHT >= ball.y));
    playerPadY = player[1].padY;
    break;
  }

  if (retValue) {
    if (ball.y == playerPadY)                      *angle = -2;
    else if (ball.y == playerPadY+PADDLE_HEIGHT)   *angle =  2;
    else if (ball.y <  playerPadY+PADDLE_HEIGHT/2) *angle = -1;
    else if (ball.y > playerPadY+PADDLE_HEIGHT/2)  *angle =  1;
  }

  return retValue;
}

uint8_t readPaddlePos(uint8_t pot) {
  const uint8_t paddleTop = HEIGHT - PADDLE_HEIGHT;
  int16_t potValue;

  potValue = map(PinReadPot(pot), 64, 192, 0, paddleTop);
  if (potValue < 0)
    potValue = 0;
  if (potValue > paddleTop)
    potValue = paddleTop;

  return potValue;
  }

void initGame() {
  player[0].padX = 3;
  player[0].padY = HEIGHT/2 - PADDLE_HEIGHT/2;
  player[0].score = 0;

  player[1].padX = WIDTH - 3;
  player[1].padY = HEIGHT/2 - PADDLE_HEIGHT/2;
  player[1].score = 0;

  ball.radius = 2;
  ball.x = WIDTH/2;
  ball.y = HEIGHT/2;
  ball.velX = ball.velY = 0;
}

void blink() {
  static uint8_t count;

  if (count++ == 32) {
    count = 0;
    digitalWrite(LED_BUILTIN, HIGH);
  } else
    digitalWrite(LED_BUILTIN, LOW);
}

