#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_KS0108_kbv.h"

#define ks0108_swap(a, b) (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b)))
extern void ks0108Init(void);
extern void ks0108Blit(const uint8_t *p, int len);
extern void ks0108BlitRect(const uint8_t *p, uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t isflash);
extern void ks0108Command(uint8_t cmd);
extern uint8_t ks0108Xor, ks0108Led;

Adafruit_KS0108_kbv::Adafruit_KS0108_kbv(void): Adafruit_GFX(128, 64)
{

}

void Adafruit_KS0108_kbv::begin(void)
{
    ks0108Init();
    clearDisplay();
    backlight(true);
    setTextColor(WHITE); //because GFX defaults to 0xFFFF
    _left = 0, _rt = 127; //ensure that whole buffer is displayed at start
    _top = 0, _bot = 63;
}

#define USE_FILLRECT 1
void Adafruit_KS0108_kbv::drawPixel(int16_t x, int16_t y, uint16_t color)
{
#if USE_FILLRECT == 2
    fillRect(x, y, 1, 1, color);
#else
    if ((x >= 0) && (x < width()) && (y >= 0) && (y < height())) {
        // Pixel is in-bounds. Rotate coordinates if needed.
        switch (getRotation()) {
            case 1:
                ks0108_swap(x, y);
                x = WIDTH - x - 1;
                break;
            case 2:
                x = WIDTH  - x - 1;
                y = HEIGHT - y - 1;
                break;
            case 3:
                ks0108_swap(x, y);
                y = HEIGHT - y - 1;
                break;
        }
        uint8_t *ads = &buffer[x + (y >> 3) * WIDTH];
        uint8_t mask = (1 << (y & 7));
        uint8_t d = *ads;
        uint8_t old = d;
        switch (color) {
            case KS0108_WHITE:   d |= mask; break;
            case KS0108_BLACK:   d &= ~mask; break;
            case KS0108_INVERSE: d ^= mask; break;
        }
        if (d != old) { //is not too expensive
            *ads = d;
            if (x < _left) _left = x;
            if (x > _rt) _rt = x;
            if (y < _top) _top = y;
            if (y > _bot) _bot = y;
        }
    }
#endif
}

bool Adafruit_KS0108_kbv::getPixel(int16_t x, int16_t y) {
    if ((x >= 0) && (x < width()) && (y >= 0) && (y < height())) {
        // Pixel is in-bounds. Rotate coordinates if needed.
        switch (getRotation()) {
            case 1:
                ks0108_swap(x, y);
                x = WIDTH - x - 1;
                break;
            case 2:
                x = WIDTH  - x - 1;
                y = HEIGHT - y - 1;
                break;
            case 3:
                ks0108_swap(x, y);
                y = HEIGHT - y - 1;
                break;
        }
        return (buffer[x + (y / 8) * WIDTH] & (1 << (y & 7)));
    }
    return false; // Pixel out of bounds
}

void Adafruit_KS0108_kbv::fillRect(int16_t x0, int16_t y0, int16_t w, int16_t h, uint16_t color)
{
#if USE_FILLRECT > 0
    // constrain the logical arguments
    if (x0 >= width() || y0 >= height()) return;
    if (x0 < 0) w += x0, x0 = 0; //adding -ve vales
    if (y0 < 0) h += y0, y0 = 0;
    if (x0 + w > width()) w = width() - x0;
    if (y0 + h > height()) h = height() - y0;
    //transform to the physical memory layout
    switch (getRotation()) {
        case 1:
            ks0108_swap(x0, y0);
            ks0108_swap(w, h);
            x0 = WIDTH - x0 - w;
            break;
        case 2:
            x0 = WIDTH  - x0 - w;
            y0 = HEIGHT - y0 - h;
            break;
        case 3:
            ks0108_swap(x0, y0);
            ks0108_swap(w, h);
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
            if (color == KS0108_WHITE) newb |= mask;
            if (color == KS0108_INVERSE) newb |= oldb ^ mask;
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
    for (int row = y0; row < y0 + h; row++) {
        for (int col = x0; col < x0 + w; col++) {
            drawPixel(col, row, color);
        }
    }
#endif
}

void Adafruit_KS0108_kbv::clearDisplay(uint8_t color)
{
    uint8_t c = (color == KS0108_WHITE) ? 0xFF : 0x00;
    for (uint8_t y = 0; y < 64; y += 8) {
        uint8_t *p = getBuffer() + (y / 8) * 128;
        for (uint8_t x = 0; x < 128; x++, p++) {
            uint8_t oldb = *p;
            if (color == KS0108_INVERSE) c = oldb ^ 0xFF;
            if (oldb != c) {
                *p = c;
                if (x < _left) _left = x;
                if (x > _rt) _rt = x;
                if (y < _top) _top = y;
                if (y > _bot) _bot = y;
            }
        }
    }
}

void Adafruit_KS0108_kbv::invertDisplay(bool i)
{
    ks0108Xor = i ? 0xFF : 0x00;
    display();   //and show the result
}

void Adafruit_KS0108_kbv::backlight(bool i)
{
    ks0108Led = i ? 0x01 : 0x00;
    display();  //and show the result
}

void Adafruit_KS0108_kbv::display(void)
{
    //ks0108Blit((const uint8_t*)buffer, 0);  //use SRAM
    //is not too expensive to minimise I2C traffic
    //extra housekeeping in ks0108BlitRect() is one off
    if (_rt < _left) _left = 0, _rt = 127;
    if (_bot < _top) _top = 0, _bot = 63;
    ks0108BlitRect((const uint8_t*)buffer, _left, _top, _rt - _left + 1, _bot - _top + 1, 0);  //use SRAM
    _left = 127;
    _rt = 0;
    _top = 63;
    _bot = 0;
}

void Adafruit_KS0108_kbv::ks0108_command(uint8_t cmd)
{
    ks0108Command(cmd);
}
