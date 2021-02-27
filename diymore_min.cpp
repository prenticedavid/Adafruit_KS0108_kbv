#define USE_DIYMORE  1
#define INVERT_PIC   0x00
#define USE_BYTEMODE 1

#if USE_DIYMORE == 1
#if defined(__CODEVISIONAVR__)
#include <stdio.h> //for NULL
#include <stdint.h>
#include <delay.h>
#include <twi.h>

#define pgm_read_byte(x) *((__flash uint8_t*)(x))

#elif defined(ARDUINO)
#include <Arduino.h>
#include <Wire.h>
#define twi_master_init(kHz) { Wire.begin(); Wire.setClock(1000L * kHz); }

void twi_master_trans(uint8_t slave, uint8_t *txbuf, int txn, uint8_t *rxbuf, int rxn)
{
    Wire.beginTransmission(slave);
    while (txn--) Wire.write(*txbuf++);
    Wire.endTransmission();
    if (rxn) {
        Wire.requestFrom(slave, rxn);    // request 6 bytes from slave device #8

        while (Wire.available()) { // slave may send less than requested
            *rxbuf++ = Wire.read(); // receive a byte as character
        }
    }
}

#else  //Fleury
#include <stdio.h> //for NULL
#include <avr/pgmspace.h>
#include "i2cmaster.h"

#define twi_master_init(kHz) { i2c_init(); TWBR = 10; }

void twi_master_trans(uint8_t slave, uint8_t *txbuf, int txn, uint8_t *rxbuf, int rxn)
{
    i2c_start(slave << 1);
    while (txn--) i2c_write(*txbuf++);
    if (rxn) {
        i2c_start((slave << 1) | 1);
        while (rxn--) {
            *rxbuf++ = i2c_read(rxn != 0);
        }
    }
    i2c_stop();
}

#endif

#if defined(USE_GAMMON_WIRING)
#define LCD_CS1    0b00000100   // chip select 1  (pin 23)                            0x04
#define LCD_CS2    0b00001000   // chip select 2  (pin 24)                            0x08
#define LCD_RESET  0b00010000   // reset (pin 25)                                     0x10
#define LCD_DATA   0b00100000   // 1xxxxxxx = data; 0xxxxxxx = instruction  (pin 26)  0x20
#define LCD_READ   0b01000000   // x1xxxxxx = read; x0xxxxxx = write  (pin 27)        0x40
#define LCD_ENABLE 0b10000000   // enable by toggling high/low  (pin 28)              0x80
#define LCD_BLK    (1<<1)   //PA1
#else
#define LCD_CS1    (1<<4)   //PA4 
#define LCD_CS2    (1<<3)
#define LCD_RESET  (1<<2)
#define LCD_DATA   (1<<7)   //PA7
#define LCD_READ   (1<<6)
#define LCD_ENABLE (1<<5)
#define LCD_BLK    (1<<1)   //PA1
#endif

#define LCD_CMD_MASK   (LCD_CS1|LCD_CS2|LCD_DATA|LCD_READ|LCD_ENABLE)

#define LCD_SETADSX 0x40
#define LCD_SETPAGE 0xB8
#define LCD_SETSCRL 0xC0
#define LCD_SETDISP 0x00
#define LCD_ON      0x3F
#define LCD_OFF     0x3E

//AUTOINC: BANK=?, SEQOP=0. BYTE_TOGGLE: BANK=0, SEQOP=1.  BYTE_MODE: BANK=1, SEQOP=1
//AUTOINC BANK?: OLATB,data; OLATA,E=1; OLATA,E=0.  9 I2C bytes (old data 6 bytes)
//AUTOINC BANK0: GPIOB,data,E=1; OLATA,E=0.   7 I2C bytes. (old data 6 bytes)
//BYTE_TOGGLE: OLATB,data,E=1; OLATA,E=0.   7 I2C bytes. (old data 6 bytes)
//BYTE_MODE:   OLATB,data; OLATA,E=1,E=0. 6 I2C bytes.  (old data: 2 bytes)
#define MCP23017   0x20
#define IODIRA     0x00   //BANK1  BYTE_MODE is most efficient 
#define IODIRB     0x10
#define IOCON      0x05
#define OLATA      0x0A
#define OLATB      0x1A

static uint8_t __x = 255, __y = 255, LCD_CMD_PORT;
static uint8_t LCD_DATA_OUT;
uint8_t ks0108Xor = 0, ks0108Led = 1;

static void i2cRegData(uint8_t slave, uint8_t reg, uint8_t data)
{
    uint8_t txbuf[2];
    txbuf[0] = reg;
    txbuf[1] = data;
    twi_master_trans(slave, txbuf, 2, NULL, 0);
}

static void i2cRegData2(uint8_t slave, uint8_t reg, uint8_t data1, uint8_t data2)
{
    uint8_t txbuf[3];
    txbuf[0] = reg;
    txbuf[1] = data1;
    txbuf[2] = data2;
    twi_master_trans(slave, txbuf, 3, NULL, 0);
}

static void ks0108CmdPort(uint8_t cmd, uint8_t port)
{
    port = (LCD_CMD_PORT & ~LCD_CMD_MASK) | (port & LCD_CMD_MASK);
    if (ks0108Led) port |= LCD_BLK;
    else port &= ~LCD_BLK;    //data and port can only change when E=0
    if (LCD_DATA_OUT != cmd) i2cRegData(MCP23017, OLATB, cmd);
    i2cRegData2(MCP23017, OLATA, port | LCD_ENABLE, port); //tWH>450ns. tWH=2.25us @400kHz
    LCD_DATA_OUT = cmd;
    LCD_CMD_PORT = port;
}

extern void ks0108Init(void)
{
    twi_master_init(400);
    i2cRegData(MCP23017, 0x0A, 0xA0); //BANK=1, SEQOP=1 if BANK0 (POR)
    i2cRegData(MCP23017, 0x05, 0xA0); //BANK=1, SEQOP=1 i.e. BYTE_MODE
    i2cRegData(MCP23017, IODIRA, (LCD_CMD_MASK | LCD_RESET | LCD_BLK) ^ 0xFF); //Output
    i2cRegData(MCP23017, IODIRB, 0xFF ^ 0xFF);                              //Output
    i2cRegData(MCP23017, OLATA, LCD_CMD_PORT = LCD_RESET);
    i2cRegData(MCP23017, OLATA, LCD_CMD_PORT = 0);
    i2cRegData(MCP23017, OLATA, LCD_CMD_PORT = LCD_RESET);
    ks0108CmdPort(LCD_ON, LCD_CS1 | LCD_CS2);
    ks0108CmdPort(LCD_SETSCRL | 0, LCD_CS1 | LCD_CS2);
}

extern void ks0108BlitRect(const uint8_t *buf, uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t isflash)
{
    uint8_t c, col, page, endcol, controller, *p;
    uint8_t txbuf[32], cnt, port, c_xor = ks0108Xor ^ INVERT_PIC, pagwid = isflash ? isflash : 128;
    for (page = y >> 3; page < (y + h + 7) >> 3; page++) {
        for (controller = 0; controller < 128; controller += 64) {
            col = x;
            endcol = x + w;
            if (controller > col) col = controller;
            if (endcol > controller + 64) endcol = controller + 64;
            if (endcol < controller) continue;
            p = (uint8_t*)buf + pagwid * page + col;
            ks0108CmdPort(LCD_SETPAGE | (page), LCD_CS1 | LCD_CS2);
            ks0108CmdPort(LCD_SETADSX | (col & 63), (col < 64) ? LCD_CS1 : LCD_CS2);
            while (col < endcol) {
                c = ((isflash) ? pgm_read_byte(p) : (*p)) ^ c_xor;
                if (LCD_DATA_OUT != c) i2cRegData(MCP23017, OLATB, LCD_DATA_OUT = c);
                port = LCD_DATA | LCD_RESET | ((col < 64) ? LCD_CS1 : LCD_CS2);
                if (ks0108Led) port |= LCD_BLK;
                cnt = 0;
                txbuf[cnt++] = OLATA;
                do {
                    txbuf[cnt++] = port | LCD_ENABLE;
                    txbuf[cnt++] = port;
                    p++;
                    col++;
                    c = ((isflash) ? pgm_read_byte(p) : (*p)) ^ c_xor;
                } while ((col < endcol - 1) && (cnt < 30) && c == LCD_DATA_OUT);
                twi_master_trans(MCP23017, txbuf, cnt, NULL, 0); //N strobes
            }
        }
    }
}

extern void ks0108Blit(const uint8_t *p, int len) // len == 0 means SRAM
{
#if defined(__CODEVISIONAVR__)
    ks0108BlitRect(p, 0, 0, 128, 64, 128); //CV displays from PROGMEM
#else
    ks0108BlitRect(p, 0, 0, 128, 64, 0); //default to SRAM
#endif
}

extern void ks0108Command(uint8_t cmd)
{
    ks0108CmdPort(cmd, LCD_CS1 | LCD_CS2);
}

#else   //regular Parallel interface
#if defined(__CODEVISIONAVR__)
#include <io.h>
#include <stdint.h>
#include <delay.h>
#define pgm_read_byte(x) *((__flash uint8_t*)(x))

#else
#include <Arduino.h>
#define delay_us(x) _delay_us(x)
#define delay_ms(x) _delay_ms(x)
#endif

#define LCD_CS1    (1<<3)   // 
#define LCD_CS2    (1<<4)
#define LCD_RESET  (1<<5)
#define LCD_DATA   (1<<0)   //
#define LCD_READ   (1<<1)
#define LCD_ENABLE (1<<2)
#define LCD_BLK    (0<<6)   //unused.  LED is hard-wired
#define LCD_CMD_MASK   (LCD_CS1|LCD_CS2|LCD_DATA|LCD_READ|LCD_ENABLE)

#define LCD_SETADSX 0x40
#define LCD_SETPAGE 0xB8
#define LCD_SETSCRL 0xC0
#define LCD_SETDISP 0x00
#define LCD_ON      0x3F
#define LCD_OFF     0x3E

#if defined(__AVR_ATmega328P__) || defined(_CHIP_ATMEGA328P_)
#define LCD_CMD_PORT PORTC
#define LCD_CMD_DIR  DDRC
#define BMASK 0x03
#define write_data(x) {\
        LCD_DATA_OUT = x;\
        PORTB=(PORTB&~BMASK)|((LCD_DATA_OUT)&BMASK);\
        PORTD=(PORTD&BMASK)|((LCD_DATA_OUT)&~BMASK);}
#define data_output() { DDRB |= BMASK; DDRD |= ~BMASK; }
#else
#define LCD_CMD_PORT PORTB
#define LCD_CMD_DIR  DDRB
#define write_data(x) { LCD_DATA_OUT = x; PORTA = LCD_DATA_OUT; }
#define data_output() { DDRA = 0xFF; }
#endif


static uint8_t __x = 255, __y = 255, LCD_DATA_OUT;
uint8_t ks0108Xor = INVERT_PIC, ks0108Led = 1;

static void ks0108CmdPort(uint8_t cmd, uint8_t port)
{
    write_data(cmd);
    delay_us(1);
    LCD_CMD_PORT = port | LCD_RESET | LCD_ENABLE;
    delay_us(1);
    LCD_CMD_PORT &= ~LCD_ENABLE;
    delay_us(3);
}

extern void ks0108Init(void)
{
    data_output();
    LCD_CMD_DIR = (LCD_CMD_MASK | LCD_RESET);
    LCD_CMD_PORT = LCD_RESET;
    LCD_CMD_PORT = 0;
    delay_us(1);
    LCD_CMD_PORT = LCD_RESET;
    ks0108CmdPort(LCD_ON, LCD_CS1 | LCD_CS2);
    ks0108CmdPort(LCD_SETSCRL, LCD_CS1 | LCD_CS2);
}

static void ks0108GotoXY(uint8_t x, uint8_t y)
{
    ks0108CmdPort(LCD_SETPAGE | (y >> 3), LCD_CS1 | LCD_CS2);
    ks0108CmdPort(LCD_SETADSX | (x & 63), LCD_CS1 | LCD_CS2);
    __x = x;
    __y = y;
}

static void ks0108WriteXY(uint8_t c, uint8_t x, uint8_t y)
{
    if (x != __x || x == 64 || y != __y) ks0108GotoXY(x, y);   //optimise on I2C
    ks0108CmdPort(c, LCD_DATA | ((x < 64) ? LCD_CS1 : LCD_CS2));
    __x++;
}

extern void ks0108BlitRect(const uint8_t *buf, uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t isflash)
{
    uint8_t c, col, page, endcol, controller, *p;
    uint8_t port, c_xor = ks0108Xor ^ INVERT_PIC, pagwid = isflash ? isflash : 128;
    for (page = y >> 3; page < (y + h + 7) >> 3; page++) {
        for (controller = 0; controller < 128; controller += 64) {
            col = x;
            endcol = x + w;
            if (controller > col) col = controller;
            if (endcol > controller + 64) endcol = controller + 64;
            if (endcol < controller) continue;
            p = buf + pagwid * page + col;
            ks0108CmdPort(LCD_SETPAGE | (page), LCD_CS1 | LCD_CS2);
            ks0108CmdPort(LCD_SETADSX | (col & 63), (col < 64) ? LCD_CS1 : LCD_CS2);
            port = LCD_DATA | ((col < 64) ? LCD_CS1 : LCD_CS2);
            while (col < endcol) {
                c = ((isflash) ? pgm_read_byte(p) : (*p)) ^ c_xor;
                ks0108CmdPort(c, port);
                p++;
                col++;
            }
        }
    }
    delay_ms(20);  //20ms is nearer SSD1306 I2C performance
}

extern void ks0108Blit(const uint8_t *p, int len) // len == 0 means SRAM
{
#if defined(__CODEVISIONAVR__)
    ks0108BlitRect(p, 0, 0, 128, 64, 128); //CV displays from PROGMEM
#else
    ks0108BlitRect(p, 0, 0, 128, 64, 0); //default to SRAM
#endif
}

extern void ks0108Command(uint8_t cmd)
{
    ks0108CmdPort(cmd, LCD_CS1 | LCD_CS2);
}

#endif //USE_DIYMORE
