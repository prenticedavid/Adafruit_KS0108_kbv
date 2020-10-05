#include <Wire.h>                     // requried to run I2C SH1106
#include <SPI.h>                      // requried to run I2C SH1106
#include <Adafruit_GFX.h>             // https://github.com/adafruit/Adafruit-GFX-Library

#define USE_LCD 0x0108

#if 0
#elif USE_LCD == 0x0108
#include <Adafruit_KS0108_kbv.h>
Adafruit_KS0108_kbv display;
#define LCD_BEGIN()    display.begin()
#define LCD_DISPLAY()  display.display()
#elif USE_LCD == 0x1306
#include <Adafruit_SSD1306.h> 
Adafruit_SSD1306 display(128, 64, &Wire);
#define LCD_BEGIN()    display.begin(SSD1306_SWITCHCAPVCC, 0x3C)
#define LCD_DISPLAY()  display.display()
#elif USE_LCD == 0x8544
#include <Adafruit_PCD8544.h>
Adafruit_PCD8544 display(9, 10, 8); //HW SPI
//Adafruit_PCD8544 display(13, 11, 9, 10, 8); //SW SPI
#define LCD_BEGIN()    display.begin(50, 4)
#define LCD_DISPLAY()  display.display() 
#endif 

// Select any u8g2_font for one-time print
// Select u8g2_font_xxxx_mn Font for Number fields
// you can afford a special Number font
// there is no need to use my clearprint() function
// simply use dtostrf() with setFontMode(0)

// since a _mn font does not cost much,  we might
// just have a drawFloat(val, x, y, digits, dec) function

#include <U8g2_for_Adafruit_GFX.h>
U8G2_FOR_ADAFRUIT_GFX u8g2_for_adafruit_gfx;
#include <Fonts/FreeSans9pt7b.h>  //"u8g2_font_crox3hb_tr"
#include <Fonts/FreeSans12pt7b.h>  //"u8g2_font_crox5h_tr"

void clearprint(int x, int y, char *buf)
{
    uint16_t color = u8g2_for_adafruit_gfx.u8g2.font_decode.bg_color;
    int8_t a = u8g2_for_adafruit_gfx.getFontAscent();
    int8_t d = u8g2_for_adafruit_gfx.getFontDescent();
    int8_t w = u8g2_for_adafruit_gfx.getUTF8Width(buf);
    int ofs = 0;
    int8_t digwid = u8g2_for_adafruit_gfx.getUTF8Width("9");
    int8_t spcwid = u8g2_for_adafruit_gfx.getUTF8Width(" ");
    int8_t sgnwid = u8g2_for_adafruit_gfx.getUTF8Width("-");
    char c;
    for (char *p = buf; c = *p++; ) {
        if (c == ' ') ofs += digwid - spcwid + 1;
        if (c == '-') ofs += digwid - sgnwid - 0;
        if (c != ' ') break;
    }
    display.fillRect(x, y - a, w + ofs + 2, a - d, color);
    u8g2_for_adafruit_gfx.drawUTF8(x + ofs, y, buf);
}

void testprint(int x, int y, char *buf)
{
    clearprint(x, y, buf);
    LCD_DISPLAY();                 // make everything visible
    delay(1000);
}

void testfont(const uint8_t *f, const char *msg)
{
    display.fillScreen(WHITE);
    u8g2_for_adafruit_gfx.setFont(f);
    u8g2_for_adafruit_gfx.drawUTF8(2, 13, msg);
    display.setFont(&FreeSans9pt7b);
    //display.setFont(&FreeSans12pt7b);
    display.setTextWrap(false);
    display.setCursor(2, 29);
    display.setTextColor(BLACK);
    display.print(msg);
    LCD_DISPLAY();                 // make everything visible
    delay(1000);
}

void chequer(int x, int y, int w, int h)
{
    display.fillRect(x, y, w, h, WHITE);
    for (int row = 0; row < h; row++) {
        for (int col = 0; col < w; col++) {
            if ((col + row) & 1) display.drawPixel(x + col, y + row, BLACK);
        }
    }
}

void setup()
{
    LCD_BEGIN();
    u8g2_for_adafruit_gfx.begin(display);            // connect u8g2 procedures to Adafruit GFX
    u8g2_for_adafruit_gfx.setFontDirection(0);       // left to right
    if (display.width() < display.height()) display.setRotation(1);
    u8g2_for_adafruit_gfx.setForegroundColor(BLACK);  // apply Adafruit GFX color
    u8g2_for_adafruit_gfx.setBackgroundColor(WHITE);  // apply Adafruit GFX color
    u8g2_for_adafruit_gfx.setFontMode(0);                 // use u8g2 transparent mode (this is default)
    delay(1000);
    //testfont(u8g2_font_profont22_tr, "u8g2_font_profont22_tr");
    //testfont(u8g2_font_fur14_tr, "u8g2_font_fur14_tr");
    //testfont(u8g2_font_helvR14_tr, "u8g2_font_helvR14_tr");
    testfont(u8g2_font_crox4h_tr, "u8g2_font_crox4h_tr");
    testfont(u8g2_font_crox3hb_tr, "u8g2_font_crox3hb_tr");
    //testfont(u8g2_font_helvR18_tr, "u8g2_font_helvR18_tr");
    //testfont(u8g2_font_logisoso18_tr, "u8g2_font_logisoso18_tr");
    //testfont(u8g2_font_crox5h_tr, "u8g2_font_crox5h_tr");
    chequer(0, 36, display.width(), 12);
    /*
        display.fillScreen(WHITE);
        for (int row = 0; row < display.height(); row++) {
            for (int col = 0; col < display.width(); col++) {
                if ((col + row) & 1) display.drawPixel(col, row, BLACK);
            }
        }
        char msg[] = "u8g2_font_crox5h_tr";
        u8g2_for_adafruit_gfx.setFont(u8g2_font_crox5h_tr);
        u8g2_for_adafruit_gfx.drawUTF8(2, 16, msg);
        display.setFont(&FreeSans9pt7b);
        display.setCursor(2, 32);
        display.setTextColor(BLACK);
        display.print(msg);
        //clearprint(2, 16, "GFX format");
        OLED_DISPLAY();                                       // make everything visible
        delay(1000);
    */
    //u8g2_for_adafruit_gfx.setFont(u8g2_font_logisoso16_tn);
    //u8g2_for_adafruit_gfx.setFont(u8g2_font_7x14_mn);
    //u8g2_for_adafruit_gfx.setFont(u8g2_font_9x18_mn);
    u8g2_for_adafruit_gfx.setFont(u8g2_font_9x18B_mn);
}

void loop()
{
    int x = 10;
    char buf[10];
    dtostrf(0.01 * random(-19999, 19999), 7, 2, buf);
    //clearprint(x, 40, buf);
    u8g2_for_adafruit_gfx.drawUTF8(x, 47, buf);
    LCD_DISPLAY();                                       // make everything visible
    delay(1000);
    /*
        testprint(x, 40, "   1.11");
        testprint(x, 40, "  11.11");
        testprint(x, 40, " -11.11");
        testprint(x, 40, " 111.11");
        testprint(x, 40, "1111.11");
        testprint(x, 40, "   9.99");
        testprint(x, 40, "  99.99");
        testprint(x, 40, " 999.99");
        testprint(x, 40, "9999.99");
        testprint(x, 40, "9999.11");
    */
}

