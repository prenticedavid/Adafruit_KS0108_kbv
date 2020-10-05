
// Demo of KS0108B graphics LCD screen connected to MCP23017 16-port I/O expander

// Author: Nick Gammon
// Date: 14 February 2011
// translated to GFX calls by David Prentice 05 Oct 2020
//
// Gammon's library expects MCP23017 BANK0 i.e. POR state
// Adafruit_KS0108_kbv uses BANK1.  start from BANK0 or BANK1
// defaults to DIYMORE Adapter wiring. i.e. PaulRB's Pull Request
// #define USE_GAMMON_WIRING in diymore_min.cpp

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_KS0108_kbv.h>
Adafruit_KS0108_kbv lcd;

// example bitmap
const byte picture [] PROGMEM = {
    0x1C, 0x22, 0x49, 0xA1, 0xA1, 0x49, 0x22, 0x1C,  // face
    0x10, 0x08, 0x04, 0x62, 0x62, 0x04, 0x08, 0x10,  // star destroyer
    0x4C, 0x52, 0x4C, 0x40, 0x5F, 0x44, 0x4A, 0x51,  // OK logo
};


void setup ()
{
    lcd.begin ();

    // draw all available letters
    for (int i = ' '; i <= 0x7f; i++)
        lcd.print((char)i); // 7x5 system font

    // black box
    lcd.fillRect(6, 40, 25, 24, WHITE); //x, y, w, h, color

    // draw text in inverse
    lcd.setTextColor(BLACK, WHITE); //print background
    lcd.setCursor(40, 40);
    lcd.print("David Prentice");
    lcd.setTextColor(WHITE); //restore default transparent

    // bit blit in a picture
    //lcd.gotoxy (40, 56);
    //lcd.blit (picture, sizeof picture);
    memcpy_P(lcd.getBuffer() + (56 / 8) * 128 + 40, picture, sizeof(picture));
    // draw a framed rectangle
    lcd.drawRect (40, 49, 21, 5, WHITE); //x, y, w, h, color

    // draw a white diagonal line
    lcd.drawLine (6, 40, 30, 63, BLACK); //x1, y1, x2, y2, c
    lcd.display();                       // ### go show it ###
}  // end of setup

void loop ()
{}  // nothing to see here, move along
