#if USE_U8X8 == 0
#include <Wire.h>                     // requried to run I2C SSD106
#include <SPI.h>                      // requried to run I2C SSD106
#include <Adafruit_GFX.h>             // https://github.com/adafruit/Adafruit-GFX-Library

#define USE_LCD 0x0108

#define OLED_CLK 13
#define OLED_MOSI 11
#define OLED_RESET 8
#define OLED_I2C_RESET 4
#define OLED_DC 9
#define OLED_CS 10

#if 0
#elif USE_LCD == 0x0108
#include <Adafruit_KS0108_kbv.h>
Adafruit_KS0108_kbv display;
#define LCD_BEGIN()  display.begin()
#elif USE_LCD == 0x1106
#include <Adafruit_SSD1106.h>          // https://github.com/wonho-maker/Adafruit_SH1106
Adafruit_SSD1106 display(OLED_I2C_RESET);
#define LCD_BEGIN()  display.begin(SSD1106_SWITCHCAPVCC, 0x3C)
#elif USE_LCD == 0x1306
#include <Adafruit_SSD1306.h> 
Adafruit_SSD1306 display(128, 64, &Wire);
//Adafruit_SSD1306 display(OLED_DC, OLED_RESET, OLED_CS); // I use SPI
//Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS); // I use SPI
#define LCD_BEGIN()  display.begin(SSD1306_SWITCHCAPVCC, 0x3C)
#endif 

#include "bitmap_mono.h"
#include "xbm_mono.h"

void setup()
{

    Serial.begin(9600);
    if (!Serial) delay(1000);
    Serial.println(F("GFX drawBitmap() plots one mono pixel at a time"));
    Serial.println(F("it defaults to transparent plotting"));
    Serial.println(F("unless you specify foreground and background colours"));
    Serial.println(F("it can plot a monochrome bitmap from Flash or SRAM"));

    Serial.println(F(""));
    Serial.println(F("GFX drawGrayscaleBitmap() is not relevant for OLED, TFT"));
    Serial.println(F("GFX drawRGBBitmap() plots one colour pixel at a time"));
    Serial.println(F("from Flash or SRAM and with a monochrome transparent bitmask"));
    Serial.println(F(""));

    LCD_BEGIN();
    display.clearDisplay();
}

void msgtime(String msg, uint32_t start)
{
    display.display();
    uint32_t elapsed = millis() - start;
    delay(1000);
    display.setCursor(0, 56);
    display.setTextColor(WHITE, BLACK);
    display.print(msg);
    display.print(" ");
    display.print(elapsed);
    display.print("ms ");
    display.display();
    delay(5000);    
    display.clearDisplay();
}

void loop(void)
{
    int x = 0, y = 0, w = 128, h = 64;
    uint32_t t;
    //draws background and foreground
    t = millis();
    display.setRotation(1);
    display.drawBitmap(0, 0, beetle_64x128, 64, 128, WHITE);
    msgtime(F("transparent"), t);
    t = millis();
    display.setRotation(1);
    display.drawBitmap(0, 14, beetle_64x99, 64, 99, WHITE);
    msgtime(F("transparent"), t);
    display.setRotation(0);
    t = millis();
    display.drawBitmap(x, y, tractor_128x64, 128, 64, WHITE);
    msgtime(F("transparent"), t);
#if defined(_Adafruit_SSD1306_H_)
    for (int i = 63; i >= 0; i--) {
        display.ssd1306_command(0xD3);
        display.ssd1306_command(i);
        delay(100);
    }
#endif
    t = millis();
    display.drawBitmap(x, y, portrait_64x64, 64, 64, WHITE);
    msgtime(F("transparent"), t);
    //draws background and foreground
    t = millis();
    display.drawBitmap(x, y, tractor_128x64, 128, 64, BLACK, WHITE);
    msgtime(F("background"), t);
    //draws foreground i.e. transparent
    t = millis();
    display.drawXBitmap(x, y, truck_128x64_xbm, 128, 64, WHITE);
    msgtime(F("XBM"), t);
    //draws foreground i.e. transparent
    t = millis();
    display.invertDisplay(1);
    display.drawXBitmap(x, y, tractor2_128x64_xbm, 128, 64, WHITE);
    msgtime(F("RVS"), t);
    display.invertDisplay(0);

    uint8_t sram[48*48/8];
    memcpy_P(sram, portrait_48x48, 48*48/8);
    t = millis();
    display.drawBitmap(64, y, sram, 48, 48, WHITE, BLACK);
    msgtime(F("SRAM"), t);

}

#endif
