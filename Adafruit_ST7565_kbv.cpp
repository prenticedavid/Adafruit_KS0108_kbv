//#define USE_SWSPI
//#define USE_SHIFTOUT
#if defined(USE_SHIFTOUT)    //SLOW digitalWrite etc
#define MOSI_PIN 11 //A1
#define SCK_PIN  13 //A2
#else                        //FAST AVR hardware macros
#define MOSI_PORT PORTB      //PB3
#define MOSI_PIN  3
#define SCK_PORT  PORTB      //PB5
#define SCK_PIN   5
#define PIN_LOW(p, b)        (p) &= ~(1<<(b))
#define PIN_HIGH(p, b)       (p) |= (1<<(b))
#define PIN_OUTPUT(p, b)     *(&p-1) |= (1<<(b))
#define MOSI_HI  PIN_HIGH(MOSI_PORT, MOSI_PIN)
#define MOSI_LO  PIN_LOW(MOSI_PORT, MOSI_PIN)
#define MOSI_OUT PIN_OUTPUT(MOSI_PORT, MOSI_PIN)
#define SCK_HI   PIN_HIGH(SCK_PORT, SCK_PIN)
#define SCK_LO   PIN_LOW(SCK_PORT, SCK_PIN)
#define SCK_OUT  PIN_OUTPUT(SCK_PORT, SCK_PIN)
#endif

// much of this was stolen from https://github.com/BornaBiro/ST7920_GFX_Library
// and SPI code from https://github.com/cbm80amiga/ST7920_SPI
//
// it shows how simple to add hardware to GFX
// uses a 1024 byte buffer.  call lcd.display() to show the buffer
// speed and efficiency is not important.  LCDs are SLOW.

#include <Arduino.h>

#include "Adafruit_ST7565_kbv.h"

#define ST7565_swap(a, b) (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b)))
#define ST7565Setting     SPISettings(1000000, MSBFIRST, SPI_MODE0)

#if defined(USE_SWSPI)

void SPI_begin(void)             
{
}
#define SPI_beginTransaction(x) //SPCR = 0
void SPI_transfer(uint8_t c)  //MODE0
{
#if defined(USE_SHIFTOUT)
    pinMode(MOSI_PIN, OUTPUT);
    pinMode(SCK_PIN, OUTPUT);
    shiftOut(MOSI_PIN, SCK_PIN, MSBFIRST, c);
#else
    MOSI_OUT; 
    SCK_OUT; 
    uint8_t mask = 0x80;
    while (mask) {
        if (c & mask) MOSI_HI;
        else MOSI_LO;
        SCK_HI;     //125ns
        asm("nop"); //62ns
        asm("nop"); //62ns
        SCK_LO;     //125ns
        mask >>= 1;
    }
#endif
}
#define SPI_endTransaction()
#else
#include <SPI.h>
#define SPI_begin()             SPI.begin()
#define SPI_beginTransaction(x) SPI.beginTransaction(x)
#define SPI_transfer(x)         SPI.transfer(x)
#define SPI_endTransaction()    SPI.endTransaction()
#endif

void Adafruit_ST7565_kbv::begin(void)
{
    ST7565Init();   //pinMode, reset, SPI, ...
    sendCmd(0xAE);  //display OFF
    sendCmd(0xA5);  //all pixels on
    sendCmd(0xA2);  //bias = 1:9
    sendCmd(0xA0);  //MX = 0
    sendCmd(0xC8);  //MY = 1
    sendCmd(0x26);  //Regulation ratio
    sendCmd(0x81);sendCmd(0x20); //contrast
    sendCmd(0xF8);sendCmd(0x00); //booster
    sendCmd(0xAF);  //display ON
    sendCmd(0xA4);  //Normal
    sendCmd(0x2F);  //power
    clearDisplay();
    backlight(true);
    setTextColor(WHITE); //because GFX defaults to 0xFFFF
    _xor = 0x00;
    _left = 0, _rt = 127; //ensure that whole buffer is displayed at start
    _top = 0, _bot = 63;
}

void Adafruit_ST7565_kbv::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    if ((x < 0) || (x >= width()) || (y < 0) || (y >= height())) return;
    // Pixel is in-bounds. Rotate coordinates if needed.
    switch (getRotation()) {
        case 1:
            ST7565_swap(x, y);
            x = WIDTH - x - 1;
            break;
        case 2:
            x = WIDTH  - x - 1;
            y = HEIGHT - y - 1;
            break;
        case 3:
            ST7565_swap(x, y);
            y = HEIGHT - y - 1;
            break;
    }
        uint8_t *ads = &buffer[x + (y >> 3) * WIDTH];
        uint8_t mask = (1 << (y & 7));
        uint8_t d = *ads;
        uint8_t old = d;
    switch (color) {
        case ST7565_WHITE:   d |= mask; break;
        case ST7565_BLACK:   d &= ~mask; break;
        case ST7565_INVERSE: d ^= mask; break;
    }
    if (d != old) { //is not too expensive
        *ads = d;
        if (x < _left) _left = x;
        if (x > _rt) _rt = x;
        if (y < _top) _top = y;
        if (y > _bot) _bot = y;
    }
}

Adafruit_ST7565_kbv::Adafruit_ST7565_kbv(int8_t CS, int8_t DC, int8_t RST, int8_t LED)
                    : Adafruit_GFX(ST7565_WIDTH, ST7565_HEIGHT)
{
    cs = CS;
    dc = DC;
    rst = RST;
    led = LED;
}

void Adafruit_ST7565_kbv::fillRect(int16_t x0, int16_t y0, int16_t w, int16_t h, uint16_t color)
{
#if 1
    // constrain the logical arguments
    if (x0 >= width() || y0 >= height()) return;
    if (x0 < 0) w += x0, x0 = 0; //adding -ve values
    if (y0 < 0) h += y0, y0 = 0;
    if (x0 + w > width()) w = width() - x0;
    if (y0 + h > height()) h = height() - y0;
    //transform to the physical memory layout
    switch (getRotation()) {
        case 1:
            ST7565_swap(x0, y0);
            ST7565_swap(w, h);
            x0 = WIDTH - x0 - w;
            break;
        case 2:
            x0 = WIDTH  - x0 - w;
            y0 = HEIGHT - y0 - h;
            break;
        case 3:
            ST7565_swap(x0, y0);
            ST7565_swap(w, h);
            y0 = HEIGHT - y0 - h;
            break;
    }
    //update the physical buffer memory
    uint8_t lshift = y0 & 7;
    for (uint8_t y = y0 & ~7; y < 64 && h > 0; y += 8) {
        uint8_t mask = 0xFF;
        if (h < 8) mask >>= 8 - h;
        uint8_t *p = getBuffer() + ((y / 8) * 128) + x0;
        if (lshift) {
            mask <<= lshift;
            h -= 8 - lshift;
            lshift = 0;  //one-off
        }
        else h -= 8;
        for (int x = x0; x < x0 + w; x++, p++) {
            uint8_t oldb = *p;
            uint8_t newb = (oldb & ~mask);
            if (color == ST7565_WHITE) newb |= mask;
            if (color == ST7565_INVERSE) newb |= oldb ^ mask;
            if (newb != oldb) {
                *p = newb;

                if (x < _left) _left = x;
                if (x > _rt) _rt = x;
                if (y < _top) _top = y;
                if (y > _bot) _bot = y;

            }
        }
    }
#else
    for (int row =  0; row < h; row++) {
        for (int col = 0; col < w; col++) {
            drawPixel(x0 + col, y0 + row, color);
        }
    }
#endif
}

void Adafruit_ST7565_kbv::clearDisplay(void)
{
    fillRect(0, 0, width(), height(), ST7565_BLACK);
}

void Adafruit_ST7565_kbv::invertDisplay(bool i)
{
    _xor = (i) ? 0xFF : 0x00;
    display();
}

void Adafruit_ST7565_kbv::display(void)
{
    //ST7565Blit((const uint8_t*)buffer, 0);  //use SRAM
    //is not too expensive to minimise I2C traffic
    //extra housekeeping in ks0108BlitRect() is one off
    if (_rt < _left) _left = 0, _rt = 127;
    if (_bot < _top) _top = 0, _bot = 63;
    ST7565BlitRect((const uint8_t*)buffer, _left, _top, _rt - _left + 1, _bot - _top + 1, 0);  //use SRAM
    _left = 127;
    _rt = 0;
    _top = 63;
    _bot = 0;
}

void Adafruit_ST7565_kbv::backlight(bool on)
{
    if (led > 0) {
        pinMode(led, OUTPUT);
        digitalWrite(led, on);
    }
    display();   //compatibility with Adafruit_KS0108_kbv class
}

void Adafruit_ST7565_kbv::sendCmd(byte b)
{
    SPI_beginTransaction(ST7565Setting);
    digitalWrite(cs, LOW);
    digitalWrite(dc, LOW); //command
    SPI_transfer(b);
    digitalWrite(cs, HIGH);
    SPI_endTransaction();
}
// ----------------------------------------------------------------

void Adafruit_ST7565_kbv::sendData(byte b)
{
    SPI_beginTransaction(ST7565Setting);
    digitalWrite(cs, LOW);
    digitalWrite(dc, HIGH); //data
    SPI_transfer(b);
    digitalWrite(cs, HIGH);
    SPI_endTransaction();
}

void Adafruit_ST7565_kbv::ST7565Init(void)
{
    pinMode(cs, OUTPUT);
    digitalWrite(cs, HIGH);
    pinMode(dc, OUTPUT);
    pinMode(rst, OUTPUT);
    digitalWrite(rst, HIGH); //RST
    SPI_begin();  //sets MOSI, SCK pins
    delay(50);
    digitalWrite(rst, LOW); //RST
    delay(1);
    digitalWrite(rst, HIGH); //RST
}

void Adafruit_ST7565_kbv::ST7565BlitRect(const uint8_t *buf, uint8_t x0, uint8_t y0, uint8_t w, uint8_t h, uint8_t isflash)
{
    uint8_t *p, col, page, y = y0, pagwid = 128;
    for (page = y >> 3; page < (y + h + 7) >> 3; page++) {
        sendCmd(0xB0 | page);       
        sendCmd(0x10 | (x0 >> 4));
        sendCmd(0x00 | (x0 & 15));
        SPI_beginTransaction(ST7565Setting);
        digitalWrite(cs, LOW);
        digitalWrite(dc, HIGH); //data
        p = buf + pagwid * page + x0;
        for (col = 0; col < w; col++) {
            uint8_t b = *p++ ^ _xor;
            SPI_transfer(b);
        }
        digitalWrite(cs, HIGH);
        SPI_endTransaction();
    }
}

