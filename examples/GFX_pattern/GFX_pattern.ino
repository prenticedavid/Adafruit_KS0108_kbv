// (C)2019 Pawel A. Hernik

/*
  128x64 ST7920 connections in SPI mode (only 6 wires between LCD and MCU):

  #01 GND  -> GND
  #02 VCC  -> VCC (5V)
  #04 RS   -> D10/CS or any pin
  #05 R/W  -> D11/MOSI
  #06 E    -> D13/SCK
  #15 PSB  -> GND (for SPI mode)
  #19 BLA  -> D9, VCC or any pin via 300ohm resistor
  #20 BLK  -> GND
*/

#define USE_LCD 0x0108   //select header and constructor
//#define USE_LCD 0x1306   //select header and constructor
//#define USE_LCD 0x7565   //select header and constructor
//#define USE_LCD 0x7567   //select header and constructor
//#define USE_LCD 0x7735   //select header and constructor

#define LCD_BACKLIGHT  8
#define LCD_CS         10 //10

#include <SPI.h>
#include <Wire.h>

#if 0
#elif USE_LCD == 0x0000
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv display;
#define LCD_BEGIN()    { display.begin(display.readID()); }
#define WHITE          TFT_WHITE
#define BLACK          TFT_BLACK
#define INVERSE        TFT_BLUE
#define display()      pushCommand(0x00, 0, 0)
#elif USE_LCD == 0x0108
#include <Adafruit_KS0108_kbv.h>
Adafruit_KS0108_kbv display;
#define LCD_BEGIN()    display.begin()
#elif USE_LCD == 0x1306
#include <Adafruit_SSD1306.h>
Adafruit_SSD1306 display(128, 64, &Wire);
#define LCD_BEGIN()    display.begin(SSD1306_SWITCHCAPVCC, 0x3C)
#elif USE_LCD == 0x7565
#include <Adafruit_ST7565_kbv.h> //global local
Adafruit_ST7565_kbv display(LCD_CS, 8, 9, 6);
#define LCD_BEGIN()    display.begin()
#elif USE_LCD == 0x7567
#include "Adafruit_ST7567_kbv.h" //local
Adafruit_ST7567_kbv display(-1);
#define LCD_BEGIN()    display.begin()
#elif USE_LCD == 0x7735
#include <Adafruit_ST7735.h>
Adafruit_ST7735 display(10, 9, 8);
#define LCD_BEGIN()    {display.initR(INITR_BLACKTAB); display.setRotation(1); }
#define WHITE          ST7735_WHITE
#define BLACK          ST7735_BLACK
#define INVERSE        ST7735_BLUE
#define display()      writeCommand(0)
#elif USE_LCD == 0x7920
#include "Adafruit_ST7920_kbv.h" //local
Adafruit_ST7920_kbv display(LCD_CS);
#define LCD_BEGIN()    display.begin()
#elif USE_LCD == 0xE7920
#include <ST7920_GFX_Library.h>
ST7920 display(10);
#define LCD_BEGIN()    BLACK
#define INVERSE        2
#endif

#if WHITE == 0xFFFF
#define color_t        uint16_t
#else
#define color_t        uint8_t
#endif
// from PropFonts library
//#include "c64enh_font.h"

char buf[20];
bool gflag;

void setup()
{
    Serial.begin(9600);
    pinMode(LCD_BACKLIGHT, OUTPUT);
    //analogWrite(LCD_BACKLIGHT,30);
    digitalWrite(LCD_BACKLIGHT, HIGH);
    LCD_BEGIN();
    display.setTextColor(WHITE);
    display.fillScreen(BLACK);
    display.drawRect(0, 0, 128, 64, WHITE);
    display.setCursor(5, 32);
    display.print("GFX 0x");
    display.print(USE_LCD, HEX);
    display.display();
    delay(5000);
    show_timings();
}

float t = 0;

void animHello()
{
    static int _x = 5;
    if (gflag) {
      gflag = false;
      _x = 5;
      cls();
      display.drawRect(0, 0, 128, 64, WHITE);
    }
    display.setCursor(_x, 32);
    display.setTextColor(BLACK);
    display.print("GFX 0x");
    display.print(USE_LCD, HEX);
    if (++_x > 64) _x = 5;
    display.setCursor(_x, 32);
    display.setTextColor(WHITE);
    display.print("GFX 0x");
    display.print(USE_LCD, HEX);
    display.display();
}

void animSinLines()
{
#if 1
    static uint8_t x1[64], x2[64];
    uint8_t xD, xW;
    int len;
    if (gflag) {
        gflag = false;
        for (int i = 0; i < 64; i++) x1[i] = x2[i] = 0;
    }
    for (int i = 0; i < 64; i++) {
        xD = 20.0 * (2.0 + sin(t / 10.0 + i / 18.0) * sin(i / 9.0)); //20 ... 60
        xW = 10.0 * (2.0 + sin(t / 8.0 + i / 15.0) * sin(i / 5.0));  //10 ... 30
        if (xD > xW) {  // BLACK, DITHER, WHITE
            len = xD - x1[i];
            if (len > 0) { //increasing
                drawLineHfastD(64 - xD, i, len, WHITE);
                drawLineHfastD(64 + x1[i], i, len, WHITE);
            }
            if (len < 0) { //decreasing DITHER
                display.drawFastHLine(64 - x1[i], i, -len, BLACK);
                display.drawFastHLine(64 + xD, i, -len, BLACK);
            }
        }
        len = xW - x2[i];
        if (len < 0) {  //decreaing WHITE
            display.drawFastHLine(64 - x2[i], i, -len, BLACK);
            display.drawFastHLine(64 + xW, i, -len, BLACK);
            if (xD > xW) { // BLACK, DITHER, WHITE
                drawLineHfastD(64 - x2[i], i, -len, WHITE);
                drawLineHfastD(64 + xW, i, -len, WHITE);
            }
        }
        display.drawFastHLine(64 - xW, i, xW * 2 + 0, WHITE);
        x1[i] = xD;
        x2[i] = xW;
    }
#else
    float x;
    //cls();  //96ms vs 115ms w FastHLine()
    for (int i = 0; i < 64; i++) {
        x = 20.0 * (2.0 + sin(t / 10.0 + i / 18.0) * sin(i / 9.0));
        display.drawFastHLine(0, i, 128, BLACK);
        drawLineHfastD(64 - x, i, x * 2, WHITE);
        x = 10.0 * (2.0 + sin(t / 8.0 + i / 15.0) * sin(i / 5.0));
        display.drawFastHLine(64 - x, i, x * 2 + 1, WHITE);
    }
#endif
    t += 0.7;
    display.display();
}

char *banner = "GLCD Library";

void animTextSin()
{
    //cls();
    //display.setFont(c64enh);
    char *txt = banner;
    int x = 4, i = 0;
    float y;
    while (*txt) {
        y = 20 + 10.0 * (1.0 + sin(t / 10.0 + i / 6.0) * sin(t / 18.0 + i / 9.0));
        //display.fillRect(x, y - 4, 6, 16, BLACK);
        //display.fillRect(x, y - 2, 6, 12, BLACK);
        display.fillRect(x, y - 1, 6, 10, BLACK);
        display.setCursor(x, (int)y);
        display.print(*txt++);
        x += display.getCursorX() - x;
        i++;
    }
    t += 0.7;
    display.display();
}

void animTextCircle()
{
    //cls();
    //display.setFont(c64enh);
    static int oldr, oldxo;
    char *txt = banner;
    int x = 4;
    float y, y2, xr;
    float r = 10.0 + 6.0 * (1.0 + sin(t / 28.0)); // 4..16
    float xo = 38.0 * (0.0 + sin(t / 20.0)); //-38..+38
    float yt = 24;
    while (*txt) {
        xr = 2 + x - 64 - xo;
        y2 = (r + 3) * (r + 3) - (xr) * (xr);
        y = yt;
        if (y2 > 0) y = 32 - sqrt(y2) - 5;
        if (y < 0) y = 0;
        if (y > yt) y = yt;
        display.fillRect(x, y - 3, 6, 14, BLACK);
        display.setCursor(x, (int)y);
        display.print(*txt++);
        x += display.getCursorX() - x;
    }

    display.drawCircle(64 + oldxo, 32, oldr, BLACK); //rub out previous
    display.drawCircle(64 + oldxo, 32, oldr + 1, BLACK); //rub out previous
    //display.fillCircle(64 + oldxo, 32, oldr, BLACK); //rub out previous
    fillCircleD(64 + (int)xo, 32, r, WHITE); //draw fresh dithered circle
    oldxo = xo;
    oldr = r;
    //display.drawCircle(64+xo,32,r+1,WHITE);
    t += 0.4;
    display.display();
}

void rects()
{
    int x, y;
    x = random(128 - 30);
    y = random(64 - 30);
    setDither(random(12));
    display.fillRect(x, y, 30, 30, BLACK);
    fillRectD(x, y, 30, 30, WHITE);
    display.drawRect(x, y, 30, 30, WHITE);
    display.display();
    //delay(100);
}

void circles()
{
    int x, y;
    x = 15 + random(127 - 30);
    y = 15 + random(63 - 30);
    setDither(random(12));
    display.fillCircle(x, y, 15, BLACK);
    fillCircleD(x, y, 15, WHITE);
    display.drawCircle(x, y, 16, WHITE);
    display.display();
    //delay(100);
}

int x = 40, y = 0;
int dx = 6, dy = 5;
int x2 = 80, y2 = 40;
int dx2 = 9, dy2 = 8;

void animRect()
{
    x += dx;
    y += dy;
    if (x > 64 || x < 0) {
        dx = -dx;
        x += dx;
    }
    if (y > 32 || y < 0) {
        dy = -dy;
        y += dy;
    }
    display.fillRect(x, y, 64, 32, INVERSE);  //INVERSE
    display.display();
    display.fillRect(x, y, 64, 32, INVERSE);  //INVERSE
    delay(40);
}

#define MAX_LINES 10
byte lx0[MAX_LINES];
byte lx1[MAX_LINES];
byte ly0[MAX_LINES];
byte ly1[MAX_LINES];
byte curLine = 0;

void animLines()
{
    x += dx;
    y += dy;
    x2 += dx2;
    y2 += dy2;
    if (x > 127) {
        dx = -dx;
        x = 127;
    }
    if (x < 1) {
        dx = -dx;
        x = 0;
    }
    if (y > 63) {
        dy = -dy;
        y = 63;
    }
    if (y < 1) {
        dy = -dy;
        y = 0;
    }
    if (x2 > 127) {
        dx2 = -dx2;
        x2 = 127;
    }
    if (x2 < 1) {
        dx2 = -dx2;
        x2 = 0;
    }
    if (y2 > 63) {
        dy2 = -dy2;
        y2 = 63;
    }
    if (y2 < 1) {
        dy2 = -dy2;
        y2 = 0;
    }
    lx0[curLine] = x;
    lx1[curLine] = x2;
    ly0[curLine] = y;
    ly1[curLine] = y2;
    if (++curLine >= MAX_LINES) curLine = 0;
    cls();
    for (int i = 0; i < MAX_LINES; i++) {
        display.drawLine(lx0[i], ly0[i], lx1[i], ly1[i], WHITE);
        display.drawLine(127 - lx0[i], ly0[i], 127 - lx1[i], ly1[i], WHITE);
        display.drawLine(lx0[i], 63 - ly0[i], lx1[i], 63 - ly1[i], WHITE);
        display.drawLine(127 - lx0[i], 63 - ly0[i], 127 - lx1[i], 63 - ly1[i], WHITE);
    }
    display.display();
    delay(50);
}

void patterns()
{
    cls();
    for (int i = 0; i < 16; i++) {
        setDither(i);
        fillRectD(0, 0, 128, 64, WHITE);
        display.display();
    }
}

void clss()
{
    for (int i = 0; i < 8; i++) {
        display.fillRect(0, 0, 128, 64, WHITE);
        display.display();
        display.fillRect(0, 0, 128, 64, BLACK);
        display.display();
    }
}

unsigned long tm, demoTime = 14000;
void run_animation( void (*f)(void), bool clear)
{
    if (gflag = clear) cls();
    tm = millis();
    while (millis() - tm < demoTime) (*f)();
}

void time_animation( void (*f)(void), String msg)
{
    cls();
    gflag = true;
    uint32_t us = micros();
    for (int i = 0; i < 10; i++) (*f)();
    us = micros() - us;
    Serial.print(msg);
    Serial.print(" ");
    Serial.print(us / 10000);
    Serial.println("ms");
    delay(1000);   //so we see the screen
}

void show_timings()
{
    time_animation(animHello, F("animHello"));
    //time_animation(patterns, F("patterns"));
    time_animation(rects, F("rects"));
    time_animation(circles, F("circles"));
    setDither(8);
    time_animation(animSinLines, F("animSinLines"));
    time_animation(animTextSin, F("animTextSin"));
    time_animation(animTextCircle, F("animTextCircle"));
    time_animation(animLines, F("animLines"));
}

void loop()
{
    //run_animation(animHello, true);
    run_animation(rects, true);
    run_animation(circles, true);
    setDither(8);
    run_animation(animSinLines, true);
    run_animation(animTextSin, true);
    run_animation(animTextCircle, true);
    run_animation(animLines, true);
    //display.setFont(c64enh);
    display.fillRect(0, 27, 128, 9, BLACK);
    display.setCursor((display.width() - strWidth(banner)) / 2, 28);
    display.print(banner);
    x = 40; y = 0;
    dx = 2; dy = 1;
    run_animation(animRect, false);
}


//############################### Dither functions ###########################
static const byte ditherTab[4 * 17] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, // 0

    0x88, 0x00, 0x00, 0x00, // 1
    0x88, 0x00, 0x22, 0x00, // 2
    0x88, 0x00, 0xaa, 0x00, // 3
    0xaa, 0x00, 0xaa, 0x00, // 4
    0xaa, 0x44, 0xaa, 0x00, // 5
    0xaa, 0x44, 0xaa, 0x11, // 6
    0xaa, 0x44, 0xaa, 0x55, // 7

    0xaa, 0x55, 0xaa, 0x55, // 8

    0xaa, 0xdd, 0xaa, 0x55, // 9
    0xaa, 0xdd, 0xaa, 0x77, // 10
    0xaa, 0xdd, 0xaa, 0xff, // 11
    0xaa, 0xff, 0xaa, 0xff, // 12
    0xbb, 0xff, 0xaa, 0xff, // 13
    0xbb, 0xff, 0xee, 0xff, // 14
    0xbb, 0xff, 0xff, 0xff, // 15

    0xff, 0xff, 0xff, 0xff // 16
};

byte pattern[4];

void cls(void)
{
    display.fillRect(0, 0, 128, 64, BLACK);
}

int strWidth(char *txt) {
    int16_t pos, x1, y1;
    uint16_t len, w, h;
    display.getTextBounds(txt, 0, 0, &x1, &y1, &w, &h);   //always
    len = x1 + w + 0;    // assumes the final right padding = 1.
    return len;
}

void fillRectD(uint8_t x0, uint8_t y0, uint8_t w, uint8_t h, color_t col)
{
    for (int y = y0; y < y0 + h; y++) { //y reads pat in MSB order
        uint8_t mask = 0x80 >> (y & 7); //(0,0) : 0xAA & 0x80. (0,1) : 0xAA & 0x40
        for (int x = x0; x < x0 + w; x++) {
            uint8_t pat = pattern[x & 3];
            if (pat & mask) display.drawPixel(x, y, col);
        }
    }
}

void drawLineHfastD(uint8_t x, uint8_t y, uint8_t w, color_t col)
{
    fillRectD(x, y, w, 1, col);
}

void fillCircleD(uint8_t x0, uint8_t y0, uint8_t r, color_t col)
{
    drawLineHfastD(x0 - r, y0, 2 * r + 1, col);
    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x     = 0;
    int16_t y     = r;

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f     += ddF_y;
        }
        x++;
        ddF_x += 2;
        f     += ddF_x;
        drawLineHfastD(x0 - x, y0 + y, 2 * x + 1, col);
        drawLineHfastD(x0 - y, y0 + x, 2 * y + 1, col);
        drawLineHfastD(x0 - x, y0 - y, 2 * x + 1, col);
        drawLineHfastD(x0 - y, y0 - x, 2 * y + 1, col);
    }
}

void setDither(uint8_t s)
{
    if (s > 16) return;
    pattern[0] = pgm_read_byte(ditherTab + s * 4 + 0);
    pattern[1] = pgm_read_byte(ditherTab + s * 4 + 1);
    pattern[2] = pgm_read_byte(ditherTab + s * 4 + 2);
    pattern[3] = pgm_read_byte(ditherTab + s * 4 + 3);
}

