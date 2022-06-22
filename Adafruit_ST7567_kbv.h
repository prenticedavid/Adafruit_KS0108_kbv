#ifndef ADAFRUIT_ST7567_kbv_H_
#define ADAFRUIT_ST7567_kbv_H_   100

#define USE_HW_I2C
// defaults to HW SPI
// edit Adafruit_ST7565_kbv.cpp to enable SW SPI pins

#include "Adafruit_GFX.h"

#define ST7567_HEIGHT  64      //64 pixels tall display
#define ST7567_WIDTH   128     //128 pixels wide display
#define ST7567_BLACK   0
#define ST7567_WHITE   1
#define ST7567_INVERSE 2
#define BLACK                     ST7567_BLACK    ///< Draw 'off' pixels
#define WHITE                     ST7567_WHITE    ///< Draw 'on' pixels
#define INVERSE                   ST7567_INVERSE  ///< Invert pixels

class Adafruit_ST7567_kbv : public Adafruit_GFX {

    public:
#ifdef USE_HW_I2C
        Adafruit_ST7567_kbv(int8_t LED = -1);  //
#else
        Adafruit_ST7567_kbv(int8_t CS, int8_t DC=8, int8_t RST=9, int8_t LED = -1);  //
        //Adafruit_ST7567_kbv(int8_t CS, int8_t DC, int8_t RST, int8_t MOSI, int8_t SCK, int8_t LED = -1);  //
#endif
        void     begin(void);                       // you only need the constructor
        virtual void     drawPixel(int16_t x, int16_t y, uint16_t color);  // and these three
        virtual void     fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
        virtual void     drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
            fillRect(x, y, 1, h, color);
        }
        virtual void     drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
            fillRect(x, y, w, 1, color);
        }
        virtual void     fillScreen(uint16_t color)                                     {
            fillRect(0, 0, _width, _height, color);
        }
        void clearDisplay(void);
        //virtual void     setRotation(uint8_t r);
        virtual void     invertDisplay(bool i);
        bool     getPixel(int16_t x, int16_t y);
        uint8_t *getBuffer(void) {
            return buffer;
        }
        void     display(void);
        void     backlight(bool on);
        void     dim(bool off) { backlight(!off); } //like Adafruit_SSD1306

    protected:
        uint8_t _left, _rt, _top, _bot;
        void sendCmd(uint8_t b);
        void sendCmd2(uint8_t b1, uint8_t b2);
        void sendData(uint8_t b);

    private:
        uint8_t buffer[1024];
        uint8_t _xor;
#ifdef USE_HW_I2C
        uint8_t i2caddr;
        int8_t led;
#else
        int8_t cs, dc, rst, mosi, sck, led;
#endif
        void ST7567Init(void);                       // interface specific
        void ST7567CmdPort(uint8_t cmd, uint8_t port);
        void ST7567BlitRect(const uint8_t *buf, uint8_t x, uint8_t y, 
                            uint8_t w, uint8_t h, uint8_t isflash);
};
#endif