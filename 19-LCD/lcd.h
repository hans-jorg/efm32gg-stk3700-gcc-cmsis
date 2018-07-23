
/** ***************************************************************************
 * @file    lcd.h
 * @brief   LCD HAL for EFM32GG STK3200
 * @version 1.0
******************************************************************************/
#ifndef LCD_H
#define LCD_H
#include <stdint.h>

uint32_t LCD_Init(void);
uint32_t LCD_Config(uint32_t presc, uint32_t div);

void LCD_Clear(void);
void LCD_SetAll(void);
void LCD_WriteChar(uint8_t c, uint8_t pos);
void LCD_WriteString(char *s);
void LCD_WriteAlphanumericField(char *s);
void LCD_WriteNumericField(char *s);
void LCD_SetSpecial(uint8_t c, uint8_t v);

uint32_t SetLFAClock(uint32_t clock);
uint32_t SetLCDClock(uint32_t presc, uint32_t div);

#define LCD_CLOCK_DEFAULT         0
#define LCD_CLOCK_LFRCO           1
#define LCD_CLOCK_LFXO            2
#define LCD_CLOCK_HFCORECLK_2     3
#define LCD_CLOCK_HFCORECLK_4     4
#define LCD_CLOCK_ULFRCO          5

#define LCD_PRESC_DEFAULT         0
#define LCD_PRESC_DIV16           0
#define LCD_PRESC_DIV32           1
#define LCD_PRESC_DIV64           2
#define LCD_PRESC_DIV128          3

#define LCD_DIV_DEFAULT           0

#define LCD_REF_VDD     0
#define LCD_REF_GND     1

/**
 * Symbols used in the WriteSpecial function
 */

static const uint8_t LCD_OFF               =     0;
static const uint8_t LCD_ON                =     1;

static const uint8_t LCD_GECKO             =     3;        // COM6 S00     SEGE    P0
static const uint8_t LCD_MINUS             =     4;        // COM4 S00
static const uint8_t LCD_PAD0              =     5;        // COM4 S19     SEGK    P0
static const uint8_t LCD_PAD1              =     6;        // COM5 S00     SEGM    P0
static const uint8_t LCD_ANTENNA           =     7;        // COM7 S12     SEGA    P0
static const uint8_t LCD_EMF32             =     8;        // COM7 S09     SEGA    P5
static const uint8_t LCD_BATTERY           =     9;        // COM7 S13     SEGB    P0
static const uint8_t LCD_BAT0              =    10;        // COM7 S17     SEGF    P0
static const uint8_t LCD_BAT1              =    11;        // COM7 S16     SEGG    P0
static const uint8_t LCD_BAT2              =    12;        // COM7 S18     SEGH    P0
static const uint8_t LCD_ARC0              =    13;        // COM7 S07     SEGH    P11
static const uint8_t LCD_ARC1              =    14;        // COM7 S06     SEGJ    P11
static const uint8_t LCD_ARC2              =    15;        // COM7 S05     SEGK    P11
static const uint8_t LCD_ARC3              =    16;        // COM7 S04     SEGM    P11
static const uint8_t LCD_ARC4              =    17;        // COM7 S03     SEGN    P11
static const uint8_t LCD_ARC5              =    18;        // COM7 S02     SEGP    P11
static const uint8_t LCD_ARC6              =    19;        // COM7 S01     SEGQ    P11
static const uint8_t LCD_ARC7              =    20;        // COM7 S00     SEGDP   P11
static const uint8_t LCD_TARGET0           =    21;        // COM7 S19     SEGH    P8
static const uint8_t LCD_TARGET1           =    22;        // COM6 S19     SEGJ    P8
static const uint8_t LCD_TARGET2           =    23;        // COM0 S19     SEGK    P8
static const uint8_t LCD_TARGET3           =    24;        // COM5 S19     SEGM    P8
static const uint8_t LCD_TARGET4           =    25;        // COM1 S19     SEGN    P8
static const uint8_t LCD_C                 =    26;        // COM7 S14     SEGC    P0
static const uint8_t LCD_F                 =    27;        // COM7 S15     SEGD    P0
static const uint8_t LCD_COLLON3           =    28;        // COM3 S00     SEGH    P9
static const uint8_t LCD_COLLON5           =    29;        // COM7 S10     SEGK    P9
static const uint8_t LCD_COLLON10          =    30;        // COM2 S19     SEGN    P9
static const uint8_t LCD_DP2               =    31;        // COM0 S00     SEGN    P0
static const uint8_t LCD_DP3               =    32;        // COM2 S00     SEGDP   P3
static const uint8_t LCD_DP4               =    33;        // COM1 S00     SEGD    P6
static const uint8_t LCD_DP5               =    34;        // COM0 S09     SEGDP   P5
static const uint8_t LCD_DP6               =    35;        // COM0 S11     SEGDP   P6
static const uint8_t LCD_DP10              =    36;        // COM3 S19     SEGDP   P10

static const uint8_t LCD_ARC               =   100;
static const uint8_t LCD_BAT               =   101;
static const uint8_t LCD_LOCK              =   102;
static const uint8_t LCD_TARGET            =   103;

#endif // LCD_H
