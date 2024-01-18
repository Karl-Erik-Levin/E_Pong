#include <Arduino.h>
#include <U8g2lib.h>
#include "Pin.h"

#define WIDTH 128
#define HEIGHT 64
#define PADDLE_WIDTH 3
#define PADDLE_HEIGHT 12

//U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ 21, /* clock=*/ 18, /* data=*/ 17);     // Hectec WiFi_Kit_32 V3
  U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);                                      // For 1.30" display
//U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);                                     // For 0.96" display

typedef struct {
  uint8_t score;   /* data */
  uint8_t potValue;
  uint8_t padX, padY;
} play;

play player[2];


void drawCenterLine();
void drawPlayers();
void initGame();
void blink();

void setup() {
  Serial.begin(115200);

  PinInit();
  u8g2.begin();                             // start display
  u8g2.setFont(u8g2_font_helvR10_tf);       // choose a suitable font

  u8g2.clearBuffer();
  u8g2.drawStr(0, 15, (char *)"Hello World!");
  u8g2.sendBuffer();

  delay(2000);

  initGame();
}

void loop() {
  static uint32_t task1, task1cnt, task1Interval=400;
  static uint32_t task2, task2cnt, task2Interval=100;
  static uint32_t task3, task3cnt, task3Interval=1000;

  //------------------------//
  //      T A S K   1       //
  //------------------------//
  if (millis() > task1) {
    task1 = millis() + task1Interval;

  player[0].potValue = map(PinReadPot(0), 0, 255, 0, HEIGHT-PADDLE_HEIGHT);
  player[1].potValue = map(PinReadPot(1), 0, 255, 0, HEIGHT-PADDLE_HEIGHT);
  
  u8g2.clearBuffer();
  drawCenterLine();
  drawPlayers();
  u8g2.sendBuffer();
}

  //------------------------//
  //      T A S K   2       //
  //------------------------//
  if (millis() > task2) {
    task2 = millis() + task2Interval;

    uint8_t but = PinReadBut();
    PinStatusLed(but);
    blink();
}

  //------------------------//
  //      T A S K   3       //
  //------------------------//
  if (millis() > task3) {
    task3 = millis() + task3Interval;
    static uint8_t hour=8, minute=48, second=12; 

    // Every second
    if (++second > 59) {
      second = 0;
      if (++minute > 59) {
        minute = 0;
        if (++hour > 23) {
          hour = 0;
        }
      }
    }

    // Every 5 minute
    if (task3cnt < 300)
      task3cnt++;
    else {
      task3cnt = 0;
//      WifiTimeRead();         // Update time from Internet
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

  u8g2.drawVLine(player[0].padX, player[0].potValue, PADDLE_HEIGHT);
  u8g2.drawVLine(player[1].padX, player[1].potValue, PADDLE_HEIGHT);

  sprintf(text, "%d", player[0].score);
  u8g2.drawStr(32, 15, text);
  sprintf(text, "%d", player[1].score);
  u8g2.drawStr(96, 15, text);
}

void initGame() {
  player[0].padX = 5;
  player[0].padY = HEIGHT/2 - PADDLE_HEIGHT/2;
  player[0].score = 1;

  player[1].padX = WIDTH - 5;
  player[1].padY = HEIGHT/2 - PADDLE_HEIGHT/2;
  player[1].score = 5;
}

void blink() {
  static uint8_t count;

  if (count++ == 32) {
    count = 0;
    digitalWrite(LED_BUILTIN, HIGH);
  } else
    digitalWrite(LED_BUILTIN, LOW);
}

