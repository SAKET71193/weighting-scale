
#include <Arduino.h>
#include "ST7036.h"
#include "LCD_C0220BiZ.h"
#include <Wire1.h>
#include <PinChangeInt.h>
#include <TimeLib.h>
#include <TinyGPS.h>

extern int get_loadcell_reading(void);

extern unsigned int __bss_end;
extern unsigned int __heap_start;
extern void *__brkval;


int Batt_Value;
double result_weight,total_weight;
bool tare_flag,M_add_flag,M_total_flag,send_flag=0;
float flat, flon;

#define HC05 Serial1  
#define  Tare_interruptPin 7
#define  M_add_interruptPin 9
#define  M_clr_interruptPin 6
#define  send_interruptPin 14
#define total_interruptPin 8
#define backlight_interruptPin A7
#define adc_Pin A4
#define batt_led A6
/*!
    @defined    CHAR_WIDTH
    @abstract   Character witdth of the display, expressed in pixeles per character.
*/
#define CHAR_WIDTH  5


//LCD_C0220BIZ lcd = LCD_C0220BIZ ( );
ST7036 lcd = ST7036 ( 2, 20, 0x78 );

/*!
    @const      charBitmap 
    @abstract   Define Character bitmap for the bargraph.
    @discussion Defines a character bitmap to represent a bargraph on a text
    display. The bitmap goes from a blank character to full black.
*/
const uint8_t charBitmap[][8] = {
   { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 },
   { 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x0 },
   { 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x0 },
   { 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x0 },
   { 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x0 },
   { 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x0 }
};



uint8_t smiley[8] = {
  B00000,
  B10001,
  B00000,
  B00000,
  B10001,
  B01110,
  B00000,
};

byte low_bat[] = {
  B11111,
  B10001,
  B10001,
  B10001,
  B10001,
  B11111,
  B11111,
  B00000
};

byte med_bat[] = {
  B11111,
  B10001,
  B10001,
  B11111,
  B11111,
  B11111,
  B11111,
  B00000
};

byte high_bat[] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B00000
};

byte gps_symbol[] = {
 B00000,
  B01110,
  B10000,
  B10110,
  B10010,
  B01110,
  B00000,
  B00000
};

byte bluetooth_symbol[] = {
0b00100,
  0b00110,
  0b10101,
  0b01110,
  0b10101,
  0b00110,
  0b00100,
  0b00000
};

byte blank_symbol[] = {
   B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};



#define bluetooth_symb 2
#define gps_symb 1
#define lowbat_symb 3
#define medbat_symb 4
#define highbat_symb 5
#define smiley_symb 6
#define blank_symb 7


#define device_id "201709139256"
#define make "Genius"
#define version_  "1"

