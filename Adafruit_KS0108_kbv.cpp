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

void Adafruit_KS0108_kbv::drawPixel(int16_t x, int16_t y, uint16_t color)
{
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
        uint8_t *ads = &buffer[x + (y / 8) * WIDTH];
        uint8_t mask = (1 << (y & 7));
        uint8_t old = *ads;
        switch (color) {
            case KS0108_WHITE:   *ads |= mask; break;
            case KS0108_BLACK:   *ads &= ~mask; break;
            case KS0108_INVERSE: *ads ^= mask; break;
        }
        if (*ads != old) { //is not too expensive
            if (x < _left) _left = x;
            if (x > _rt) _rt = x;
            if (y < _top) _top = y;
            if (y > _bot) _bot = y;
        }
    }
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

void Adafruit_KS0108_kbv::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    for (int row = y; row < y + h; row++) {
        for (int col = x; col < x + w; col++) {
            drawPixel(col, row, color);
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
