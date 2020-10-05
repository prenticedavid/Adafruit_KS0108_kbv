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
    u8g2_for_adafruit_gfx.begin(display);                 // connect u8g2 procedures to Adafruit GFX
    u8g2_for_adafruit_gfx.setFontDirection(0);       // left to right
    if (display.width() < display.height()) display.setRotation(1);
    u8g2_for_adafruit_gfx.setForegroundColor(BLACK);  // apply Adafruit GFX color
    u8g2_for_adafruit_gfx.setBackgroundColor(WHITE);  // apply Adafruit GFX color
    u8g2_for_adafruit_gfx.setFontMode(0);                 // use u8g2 transparent mode (this is default)
    delay(1000);
    int y = 0;
    chequer(0, y, display.width(), display.height() - y);
    display.drawFastVLine(87, 80, display.height() - 80, BLACK);
    //u8g2_for_adafruit_gfx.setFont(u8g2_font_9x18B_mn);
    //u8g2_for_adafruit_gfx.setFont(u8g2_font_t0_18b_mn);
    //u8g2_for_adafruit_gfx.setFont(u8g2_font_t0_22b_mn);
    //u8g2_for_adafruit_gfx.setFont(u8g2_font_profont29_mn);
    u8g2_for_adafruit_gfx.setFont(u8g2_font_crox3cb_mn);
    //u8g2_for_adafruit_gfx.setFont(u8g2_font_inb16_mn);
    //u8g2_for_adafruit_gfx.setFont(u8g2_font_inb19_mn);
    //u8g2_for_adafruit_gfx.setFont(u8g2_font_inb21_mn);
    //u8g2_for_adafruit_gfx.setFont(u8g2_font_inb24_mn);
#if WHITE == 0xFFFF    //color display
    display.setTextColor(BLACK, 0xFFE0); //YELLOW
#else
    display.setTextColor(BLACK, WHITE); //
#endif
    display.setTextSize(2);
    display.setTextWrap(false);
}

void drawFloat_u8g(float val, int x, int y, int digits, int dec)
{
    char buf[10];
    int ofs = 0;
    dtostrf(val, digits, dec, buf);
    int digwid = u8g2_for_adafruit_gfx.getUTF8Width("99");
    digwid -= u8g2_for_adafruit_gfx.getUTF8Width("9");
    uint16_t color = u8g2_for_adafruit_gfx.u8g2.font_decode.bg_color;
    int8_t a = u8g2_for_adafruit_gfx.getFontAscent();
    int8_t d = u8g2_for_adafruit_gfx.getFontDescent();
    int16_t w = u8g2_for_adafruit_gfx.getUTF8Width(buf);
    ofs = digwid * digits - w;
    display.fillRect(x, y - a, w + ofs, a, 0x07FF); //CYAN
    u8g2_for_adafruit_gfx.drawUTF8(x + ofs, y, buf);
    LCD_DISPLAY();                                       // make everything visible
}

void drawFloat_u8g_f(float val, int x, int y, int digits, int dec, const uint8_t *f)
{
    u8g2_for_adafruit_gfx.setFont(f);
    drawFloat_u8g(val, x, y, digits, dec);
}

void drawFloat_gfx(float val, int x, int y, int digits, int dec)
{
    char buf[10];
    dtostrf(val, digits, dec, buf);
    display.setCursor(x, y);
    display.print(buf);
    LCD_DISPLAY();                                       // make everything visible
}

void loop()
{
    int x = 10;
    float val = 0.01 * random(-19999, 19999);
    display.setTextSize(2);
    drawFloat_gfx(val,  5,  5, 7, 2);
    drawFloat_u8g_f(val,  5, 40, 7, 2, u8g2_font_crox3cb_mn); //x2
    drawFloat_u8g_f(val,  5, 60, 7, 2, u8g2_font_t0_22b_mn);
    drawFloat_u8g_f(val,  5, 80, 7, 2, u8g2_font_t0_22b_tn);
    //drawFloat_u8g_f(val,  5, 80, 7, 2, u8g2_font_inb16_mn); //ok
    drawFloat_u8g_f(val,  5, 120, 7, 2, u8g2_font_logisoso28_tn); //BIG
    //drawFloat_u8g_f(val,  5, 120, 7, 2, u8g2_font_fur25_tn); //BIG
    yield();
    display.setTextSize(1);
    drawFloat_gfx(val,  105,  5, 7, 2);
    drawFloat_u8g_f(val,  105, 40, 7, 2, u8g2_font_6x12_mn); //x1
    drawFloat_u8g_f(val,  105, 60, 7, 2, u8g2_font_t0_11b_mn);
    drawFloat_u8g_f(val,  105, 80, 7, 2, u8g2_font_7x14B_mn); //ok
    delay(1000);
}

