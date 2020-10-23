#ifndef ADAFRUIT_KS0108_kbv_H_
#define ADAFRUIT_KS0108_kbv_H_   100
#include "Adafruit_GFX.h"

#define KS0108_BLACK   0
#define KS0108_WHITE   1
#define KS0108_INVERSE 2
#define BLACK                     KS0108_BLACK    ///< Draw 'off' pixels
#define WHITE                     KS0108_WHITE    ///< Draw 'on' pixels
#define INVERSE                   KS0108_INVERSE  ///< Invert pixels

class Adafruit_KS0108_kbv : public Adafruit_GFX {

    public:
        Adafruit_KS0108_kbv(void);  //dummy arguments
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
        void clearDisplay(uint8_t color = KS0108_BLACK);
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
        void     ks0108_command(uint8_t cmd);
        uint8_t _left, _rt, _top, _bot;

    private:
        uint8_t buffer[1024];
};


#endif
