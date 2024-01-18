#ifndef _DispU8G2_
#define _DispU8G2_

typedef struct {
  char text[30];
  uint8_t row;
  uint8_t font;
  bool select;
} dispRow;

void DispInit(void);
void DispPrint(uint8_t line, char *text);
void DispBuffer(void);
void DispClear(void);

//void DispRow(char *text, uint8_t row, uint8_t font, uint8_t selected);
void DispPage(dispRow row[]);
void DispClock(uint8_t hour, uint8_t min, uint8_t sec);

#endif // _DispU8G2_
