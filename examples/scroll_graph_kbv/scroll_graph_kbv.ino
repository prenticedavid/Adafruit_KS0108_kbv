#include <SPI.h>          // used by SSD1306, ST7735, ...
#include <Wire.h>         // used by SSD1306. VERY SLOW
#include <Adafruit_GFX.h> // use GFX methods

#define USE_TFT 0x0108

#if USE_TFT == 0x0000
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
#elif USE_TFT == 0x0108
#include <Adafruit_KS0108_kbv.h>
Adafruit_KS0108_kbv tft;
#elif USE_TFT == 0x1306
#include <Adafruit_SSD1306.h>
//Adafruit_SSD1306 tft(128, 64, &SPI, 9, 8, 10);
Adafruit_SSD1306 tft(128, 64, &Wire);
#elif USE_TFT == 0x7735
#include <ST7789_kbv.h>
ST7789_kbv tft(0x7735, 128, 160);
//ST7789_kbv tft(0x9341, 240, 320);
//ST7789_kbv tft(0x7796, 320, 480);
#elif USE_TFT == 0x7920
#include <SPI.h>
#include <ST7920_GFX_Library.h>
ST7920 tft(10);
#endif

#if defined(USB_PID) && USB_PID == 0x804E // Arduino M0 Native
#define Serial SerialUSB
#endif

#include "sinetables.h"
#define MAXBAR 4                  //how many bars and bar_dots
#define MAXSEG 16                 //how many segments

uint16_t wid, ht, lines, current, dx, points;
const uint8_t *sinetbl;
float t, scale;  //12 + 256 + 8. static 6*MAXBAR + 2*MAXSEG + 14. [=346]
// Serial and Wire buffers ~96.  GFX and TFT/LCD class variables...
// Which leaves little headroom on the stack

void SerialHelp(void)
{
    Serial.begin(9600);
    while (!Serial);
    Serial.println(F("Scroll Graph Demonstration"));
    Serial.println();
    Serial.println(F("Left windows show hardware scrolling"));
    Serial.println(F("Top Right window shows software bar graphs"));
    Serial.println(F("Bottomn Right shows graph with random points"));
    Serial.println(F("1-9ms means 3 window update time in 1ms"));
    Serial.println(F("draw one graph segment as well in 9ms"));
    Serial.println(F("Random Line drawing is expensive !"));
    Serial.println(F("In real life you only update for point change"));
    Serial.println(F(""));
    Serial.println(F("LCD like KS0108, ST7920, ... look smeary"));
    Serial.println(F("they use software scroll but it is still too fast"));
    Serial.println(F(""));
    Serial.println(F("TFT and OLED can show fast moving graphs"));
    Serial.println(F(""));
    delay(1000);   //allow Serial to complete
/*
    generate_tbl(64);
    generate_tbl(128);
    generate_tbl(176);
    generate_tbl(240);
    generate_tbl(320);
    generate_tbl(480);
*/
    Serial.end();  //might give us some SRAM back
}

void generate_tbl(int ht)
{
    Serial.print("const uint8_t sinetbl_");
    Serial.print(ht);
    Serial.println("[256] PROGMEM = {");
    for (int i = 0; i < 256; i++) {
        Serial.print((int)((1.2 + sin((M_PI / 128) * i)) * 0.2 * ht));
        Serial.print(",");
        if ((i & 15) == 15) Serial.println("");
    }
    Serial.println("};");
}

int divide(int dividend, int divisor) //integer div with rounding
{
    if (divisor == 0) return 0;
    return (dividend + divisor / 2) / divisor;
}

void setup()
{
#if USE_TFT == 0x1306
#define CYAN   WHITE  //SSD1306 knows WHITE & BLACK
#define YELLOW WHITE
#define GREEN  WHITE
    tft.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    tft.display();
    delay(2000);      //show Adafruit logo
#elif USE_TFT == 0x0108 || USE_TFT == 0x7920
#define CYAN   WHITE  //KS0108 knows WHITE & BLACK
#define YELLOW WHITE
#define GREEN  WHITE
    tft.begin();
    tft.display();
    delay(2000);      //show Adafruit logo
#else
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
    uint16_t ID = tft.readID();
    if (ID == 0xD3D3) ID = 0x9486;
    tft.begin(ID);
    tft.setRotation(1);     //Landscape
#endif

    wid = tft.width();
    ht = tft.height();
    if (ht > 320) ht = 320;    //SSD1963 is 480x800
    lines = wid / 2 - 2;
    scale = 0.2 * ht;
    dx = divide(lines, MAXSEG);     //e.g. 158 / 16 = 9.8 = 10
    if (dx < 8) dx = 8;             //e.g. for SSD1306
    points = divide(lines, dx);     //e.g. 158 / 10 = 15.8 = 16
    // build a positive sine table in SRAM
    //for (int i = 0; i < 256; i++)
    //    sinetbl[i] = (1.2 + sin((M_PI / 128) * i)) * scale;
    if (ht == 64) sinetbl = sinetbl_64; //use table from PROGMEM
    if (ht == 128) sinetbl = sinetbl_128;
    if (ht == 176) sinetbl = sinetbl_176;
    if (ht == 240) sinetbl = sinetbl_240;
    if (ht == 320) sinetbl = sinetbl_320;
    if (ht == 480) sinetbl = sinetbl_480;
    tft.fillScreen(BLACK);
    tft.setTextColor(WHITE, BLACK);
    tft.drawRect(0, 0, wid, ht, WHITE);
    tft.drawFastHLine(0, ht / 2, wid, WHITE);
    tft.drawFastVLine(wid / 2, 0, ht, WHITE);
    int d7 = wid / 2 / 7;
    for (int x = wid / 2 + d7; x < wid; x += d7) {
        tft.drawFastVLine(x, ht - 5, 5, WHITE);
    }
    SerialHelp();           //everyone else
}

// shows bargraph with or without dot padding.
void bar(int x, int y, int len, int h, uint16_t color, bool flag = false )
{
    static int lens[MAXBAR], dots[MAXBAR], ys[MAXBAR];
    int dotwid = wid / 32, dot, old_dot, i, old_len;
    for (dot = 0; dot < len; dot += dotwid) ;
    for (i = 0; i < MAXBAR; i++) {
        if (ys[i] == y) break;    //found existing bar
        if (ys[i] == 0) {
            ys[i] = y;            //new bar
            lens[i] = 0;
            dots[i] = wid / 2;
            break;
        }
    }
    if (i == MAXBAR) return;      //shit out
    old_len = lens[i];
    lens[i] = len;
    old_dot = dots[i];
    dots[i] = dot;
    if (len > old_len)
        tft.fillRect(x + old_len, y, len - old_len, h, color);
    else tft.fillRect(x + len, y, (flag ? dot : old_len) - len, h, BLACK);
    if (flag) {
        for (i = dot;  i < old_dot; i += dotwid) {
            tft.fillRect(x + i, y, dotwid / 2, h, BLACK);
            tft.fillRect(x + i + dotwid / 2, y, dotwid / 2, h, WHITE);
        }
    }
}

void addpoint(uint16_t x, uint16_t y, int val, int dx, int idx)
{
    // we are just changing one point.
    // we rub out line(idx-1, idx) and line(idx, idx+1)
    // replace the old point.  join it up to its neighbours
    static uint16_t ys[MAXSEG];
    int old = ys[idx];
    ys[idx] = val;
    x += idx * dx;
    if (idx > 0) {
        tft.drawLine(x - dx, y - ys[idx - 1], x, y - old, BLACK);
        tft.drawLine(x - dx, y - ys[idx - 1], x, y - val, YELLOW);
    }
    if (idx < points - 1) {
        tft.drawLine(x, y - old, x + dx, y - ys[idx + 1], BLACK);
        tft.drawLine(x, y - val, x + dx, y - ys[idx + 1], YELLOW);
    }
}

void loop()
{
    static uint32_t tot3, tot4;
    static uint8_t t1, t2, t3, t4, t5, t6;
    uint32_t start = millis();
    uint16_t x, y = ht / 4;
    int16_t len, idx, h = ht / 64, plot;
    if (++current > lines) {
        tft.setCursor(wid - 60, 2);
        tft.print(divide(tot3, lines - points));
        tft.print("-");
        tft.print(divide(tot4, points));
        tft.print("ms ");
        current = 1;
        tot3 = 0;
        tot4 = 0;
    }
#if 0
#elif USE_TFT == 0x0108 || USE_TFT == 0x1306
    uint8_t *p = tft.getBuffer(); //cheap to soft scroll
    for (uint8_t i = 0; i < 8; i++) {
        memmove(p + 1, p + 2, lines);
        p += wid;
    }
    plot = lines + 1;             //add data at RHS of window
    tft.drawPixel(1, 0, BLACK); //force KS0108 update
    tft.drawPixel(1, 0, WHITE); //force KS0108 update
    tft.drawPixel(plot, ht - 1, BLACK); //force KS0108 update
    tft.drawPixel(plot, ht - 1, WHITE); //force KS0108 update
#else
    tft.vertScroll(1, lines, current); //use hardware on TFT
    plot = current;               //account for window scroll
#endif
    tft.drawFastVLine(plot, 1, ht - 2, BLACK); //rubout prev
    tft.drawPixel(plot, ht / 2, WHITE);        //central line
    tft.drawPixel(plot, pgm_read_byte(&sinetbl[t1 += 3]), YELLOW); //draw plot
    tft.drawPixel(plot, ht / 2 + pgm_read_byte(&sinetbl[t2 += 5]), GREEN);
    y = ht / 12;
    bar(wid / 2 + 1, y * 2, pgm_read_byte(&sinetbl[t3 += 1]), h, CYAN);
    bar(wid / 2 + 1, y * 3, pgm_read_byte(&sinetbl[t4 += 2]) / 2, h, CYAN);
    bar(wid / 2 + 1, y * 4, pgm_read_byte(&sinetbl[t5 += 3]), h, CYAN, true); //with dots
    bar(wid / 2 + 1, y * 5, pgm_read_byte(&sinetbl[t6 += 4]), h, CYAN, true);
    if ((current % dx) == 0 || current == lines) { //new graph point
        y = ht - 2;
        x = wid / 2 + 1;
        addpoint(x, y, random(10, ht / 3), dx, current / dx);
    }
#if USE_TFT == 0x0108 || USE_TFT == 0x1306 || USE_TFT == 0x7920
    tft.display();                //send buffer to screen
#endif
    uint32_t t = millis() - start;
    if ((current % dx) == 0) tot4 += t; //all 4 windows drawn
    else tot3 += t;                     //just 3
    delay(5);
}
