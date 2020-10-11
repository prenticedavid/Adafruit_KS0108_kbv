/*
 * ST7920_SPIGLUE.h   @@@@@@@@@@@@@@@@@@@@@@@@ LIBRARY @@@@@@@@@@@@@@@@@@@@@@@@@
 *
 * Created: 06/10/2020 14:25:06
 *  Author: David Prentice
 *
 * Original ST7920 library : https://github.com/cbm80amiga/ST7920_SPI
 *
 *  1. replace "ST7920_SPI.h" with "ST7920_SPIGLUE.h"
 *  2. replace ST7920_SPI constructor with ST7920_SPIGLUE(lcd_cs)
 *  3. 
 *  4. ST7920_SPIGLUE uses FreeFonts or NULL (System 5x7)
 *  5. I have made no attempt to support cbm80amiga's font or text methods.
 *
 */

#warning   @@@@@@@@@@@@@@@@@@@@@@@@@ <ST7920_SPIGLUE.h> @@@@@@@@@@@@@@@@@@@@@@@@@@

#ifndef ST7920_SPIGLUE_H_
#define ST7920_SPIGLUE_H_

#define IMPL_KBV 1

#include <Adafruit_KS0108_kbv.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Adafruit_GFX.h>
#if defined(__arm__) && !defined(TEENSYDUINO)
#include <avr/dtostrf.h>
#endif

#define SCR_WD  128
#define SCR_HT  64
#define ALIGN_LEFT    0
#define ALIGN_RIGHT  -1
#define ALIGN_CENTER -2

#if IMPL_KBV > 2
#include <PropFont.h>

struct _propFont
{
    const uint8_t* font;
    int8_t xSize;
    uint8_t ySize;
    uint8_t firstCh;
    uint8_t lastCh;
    uint8_t minCharWd;
    uint8_t minDigitWd;
};
#endif

class ST7920_SPIGLUE : public Adafruit_KS0108_kbv
{
    public:
        ST7920_SPIGLUE(int CS) : Adafruit_KS0108_kbv() { }
        void init(void) {
            Adafruit_KS0108_kbv::begin();
            setDither(8);
            setTextColor(WHITE);
        }
#if IMPL_KBV > 2
        void sendCmd(byte b);
        void sendData(byte b);
        void switchBuf(byte b);
#endif
        void display(int buf = 0) {
            Adafruit_KS0108_kbv::display();
        }
#if IMPL_KBV > 2
        void copy(uint8_t x16, uint8_t y, uint8_t w16, uint8_t h, uint8_t buf=0);
        void gotoXY(byte x, byte y);
        void sleep(bool mode=true);
        void setGfxMode(bool mode=true);
        void printTxt(uint8_t pos, char *str);
        void printTxt(uint8_t pos, uint16_t *signs);
#endif
        void cls() {
            Adafruit_KS0108_kbv::clearDisplay();
        }
        void drawLineH(uint8_t x0, uint8_t x1, uint8_t y, uint8_t col) {
            Adafruit_KS0108_kbv::fillRect(x0, y, x1 - x0 + 1, 1, col);
        }
        void drawLineV(uint8_t x,  uint8_t y0, uint8_t y1, uint8_t col) {
            Adafruit_KS0108_kbv::fillRect(x, y0, 1, y1 - y0 + 1, col);
        }
        void drawLineVfast(uint8_t x, uint8_t y0, uint8_t y1, uint8_t col) {
            Adafruit_KS0108_kbv::fillRect(x, y0, 1, y1 - y0 + 1, col);
        }
        void drawLineVfastD(uint8_t x, uint8_t y0, uint8_t y1, uint8_t col) {
            fillRectD(x, y0, 1, y1 - y0 + 1, col);
        }
        void drawLineHfast(uint8_t x0, uint8_t x1, uint8_t y, uint8_t col) {
            Adafruit_KS0108_kbv::fillRect(x0, y, x1 - x0 + 1, 1, col);
        }
        void drawLineHfastD(uint8_t x0, uint8_t x1, uint8_t y, uint8_t col) {
            fillRectD(x0, y, x1 - x0 + 1, 1, col);
        }
        //void drawRect(uint8_t x0, uint8_t y0, uint8_t w, uint8_t h, uint8_t col);
        void drawRectD(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t col) {
            drawLineHfastD(x, x + w - 1, y + h - 1, col);
            drawLineHfastD(x, x + w - 1, y, col);
            drawLineVfastD(x, y + 1, y + h - 2, col);
            drawLineVfastD(x + w - 1, y + 1, y + h - 2, col);
        }
        //void fillRect(uint8_t x0, uint8_t y0, uint8_t w, uint8_t h, uint8_t col);
        void fillRectD(uint8_t x0, uint8_t y0, uint8_t w, uint8_t h, uint8_t col) {
            for (int y = y0; y < y0 + h; y++) { //y reads pat in MSB order
                uint8_t mask = 0x80 >> (y & 7); //(0,0) : 0xAA & 0x80. (0,1) : 0xAA & 0x40
                for (int x = x0; x < x0 + w; x++) {
                    uint8_t pat = pattern[x & 3];
                    if (pat & mask) drawPixel(x, y, col);
                }
            }
        }
        //void drawCircle(uint8_t x0, uint8_t y0, uint8_t radius, uint8_t col);
        //void fillCircle(uint8_t x0, uint8_t y0, uint8_t r, uint8_t col);
        void fillCircleD(uint8_t x0, uint8_t y0, uint8_t r, uint8_t col) {
            drawLineHfastD(x0 - r, x0 - r + 2 * r + 1, y0, col);
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
                drawLineHfastD(x0 - x, x0 - x + 2 * x + 1, y0 + y, col);
                drawLineHfastD(x0 - y, x0 - y + 2 * y + 1, y0 + x, col);
                drawLineHfastD(x0 - x, x0 - x + 2 * x + 1, y0 - y, col);
                drawLineHfastD(x0 - y, x0 - y + 2 * y + 1, y0 - x, col);
            }
        }
        void setDither(uint8_t s) {
            if (s > 16) return;
            pattern[0] = pgm_read_byte(ditherTab + s * 4 + 0);
            pattern[1] = pgm_read_byte(ditherTab + s * 4 + 1);
            pattern[2] = pgm_read_byte(ditherTab + s * 4 + 2);
            pattern[3] = pgm_read_byte(ditherTab + s * 4 + 3);
        }
        int drawBitmap(const uint8_t *bmp, int x, uint8_t y, uint8_t w, uint8_t h) {
            uint8_t wdb = w;
            //ALIGNMENT;
            if (x == -1) x = width() - w;
            else if (x < 0) x = (width() - w) / 2;
            if (x < 0) x = 0;
            if (x + w > width()) w = width() - x;
            if (y + h > height()) h = height() - y;
            byte i, j8, d, b, ht8 = (h + 7) / 8;
            for (j8 = 0; j8 < ht8; j8++) {
                for (i = 0; i < w; i++) {
                    byte mask = 0x80 >> ((x + i) & 7);
                    d = pgm_read_byte(bmp + wdb * j8 + i);
                    int lastbit = h - j8 * 8;
                    if (lastbit > 8) lastbit = 8;
                    for (b = 0; b < lastbit; b++) {
                        //if (d & 1) scr[(y + j8 * 8 + b)*scrWd + (x + i) / 8] |= mask;
                        if (d & 1) drawPixel(x + i, y + j8 * 8 + b, WHITE);
                        d >>= 1;
                    }
                }
            }
            return x + w;
        }
        int drawBitmap(const uint8_t *bmp, int x, uint8_t y) {
            int w = pgm_read_byte(bmp);
            int h = pgm_read_byte(bmp + 1);
            drawBitmap(bmp + 2, x, y, w, h);
        }

        void setFont(const uint8_t* f) {
            //Adafruit_KS0108_kbv::setFont(&FreeSans9pt7b);
            Adafruit_KS0108_kbv::setFont(NULL);
        }
#if IMPL_KBV > 2
        void setCR(uint8_t _cr) {
            cr = _cr;
        }
        void setInvert(uint8_t _inv) {
            invertCh = _inv;
        }
        void setFontMinWd(uint8_t wd) {
            cfont.minCharWd = wd;
        }
        void setCharMinWd(uint8_t wd) {
            cfont.minCharWd = wd;
        }
        void setDigitMinWd(uint8_t wd) {
            cfont.minDigitWd = wd;
        }
#endif
        int printChar(int xpos, int ypos, unsigned char c) {
            setCursor(xpos, ypos);
            print((char)c);
            return getCursorX() - xpos;
        }
        int printStr(int xpos, int ypos, char *str) {
            if (xpos == ALIGN_CENTER) xpos = (width() - strWidth(str)) / 2;
            setCursor(xpos, ypos);
            print(str);
            return getCursorX() - xpos;
        }
        int charWidth(uint8_t _ch, bool last = true) {
            char buf[2] = { _ch, 0};
            return strWidth(buf);
        }
        int strWidth(char *txt) {
            int16_t pos, x1, y1;
            uint16_t len, w, h;
            bool is_gfx = (Adafruit_KS0108_kbv::gfxFont != NULL);
            getTextBounds(txt, 0, 0, &x1, &y1, &w, &h);   //always
            len = x1 + w + 0;    // assumes the final right padding = 1.
            return len;
        }
#if IMPL_KBV > 2
        unsigned char convertPolish(unsigned char _c);
        static bool isNumber(uint8_t ch);
        static bool isNumberExt(uint8_t ch);
        void setIsNumberFun(bool (*fun)(uint8_t)) {
            isNumberFun = fun;
        }
#endif
    public:
#if IMPL_KBV > 2
        byte scr[SCR_WD * SCR_HT / 8];
        byte scrWd = 128 / 8;
        byte scrHt = 64;
        uint8_t csPin  = 10;

        static byte xstab[8];
        static byte xetab[8];
#endif
        byte pattern[4];
        static const byte ditherTab[4 * 17];

        //private:
#if IMPL_KBV > 2
        bool (*isNumberFun)(uint8_t ch);
        _propFont  cfont;
        uint8_t cr;  // carriage return mode for printStr
        uint8_t dualChar;
        uint8_t invertCh;
        uint8_t spacing = 1;
#endif
    protected:

    private:
};

const byte ST7920_SPIGLUE::ditherTab[4 * 17] PROGMEM = {
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

#endif /* ST7920_SPIGLUE_H_ */
