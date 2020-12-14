
/** ***************************************************************************
 * @file    lcd.h
 * @brief   LCD HAL for EFM32GG STK3200
 * @version 1.0
******************************************************************************/
#ifndef LCD_H
#define LCD_H
#include <stdint.h>


/**
 * Symbols used in the WriteSpecial function
 */

typedef enum {
    LCD_GECKO             =     0,        // COM6 S00
    LCD_MINUS             =     1,        // COM4 S00
    LCD_PAD0              =     2,        // COM4 S19
    LCD_PAD1              =     3,        // COM5 S00
    LCD_ANTENNA           =     4,        // COM7 S12
    LCD_EMF32             =     5,        // COM7 S09
    LCD_BATTERY           =     6,        // COM7 S13
    LCD_BAT0              =     7,        // COM7 S17
    LCD_BAT1              =     8,        // COM7 S16
    LCD_BAT2              =     9,        // COM7 S18
    LCD_ARC0              =    10,        // COM7 S07
    LCD_ARC1              =    11,        // COM7 S06
    LCD_ARC2              =    12,        // COM7 S05
    LCD_ARC3              =    13,        // COM7 S04
    LCD_ARC4              =    14,        // COM7 S03
    LCD_ARC5              =    15,        // COM7 S02
    LCD_ARC6              =    16,        // COM7 S01
    LCD_ARC7              =    17,        // COM7 S00
    LCD_TARGET0           =    18,        // COM7 S19
    LCD_TARGET1           =    19,        // COM6 S19
    LCD_TARGET2           =    20,        // COM0 S19
    LCD_TARGET3           =    21,        // COM5 S19
    LCD_TARGET4           =    22,        // COM1 S19
    LCD_C                 =    23,        // COM7 S14
    LCD_F                 =    24,        // COM7 S15
    LCD_COLLON3           =    25,        // COM3 S00
    LCD_COLLON5           =    26,        // COM7 S10
    LCD_COLLON10          =    27,        // COM2 S19
    LCD_DP2               =    28,        // COM0 S00
    LCD_DP3               =    29,        // COM2 S00
    LCD_DP4               =    30,        // COM1 S00
    LCD_DP5               =    31,        // COM0 S09
    LCD_DP6               =    32,        // COM0 S11
    LCD_DP10              =    33,        // COM3 S19
    LCD_GROUP             =   100,
    LCD_ARC               =   101,
    LCD_BAT               =   102,
    LCD_LOCK              =   103,
    LCD_TARGET            =   104,

    LCD_OFF               =   104,
    LCD_ON                =   105,
} LCD_Code_t;


#define LCD_CLOCK_DEFAULT           0
#define LCD_CLOCK_LFRCO             1
#define LCD_CLOCK_LFXO              2
#define LCD_CLOCK_HFCORECLK_2       3
#define LCD_CLOCK_HFCORECLK_4       4
#define LCD_CLOCK_ULFRCO            5

#define LCD_PRESC_DEFAULT           0
#define LCD_PRESC_DIV16             0
#define LCD_PRESC_DIV32             1
#define LCD_PRESC_DIV64             2
#define LCD_PRESC_DIV128            3

#define LCD_DIV_DEFAULT             0

#define LCD_REF_VLCD                0
#define LCD_REF_GND                 1

#define LCD_USE_VDD                 0
#define LCD_USE_VBOOST              1

uint32_t LCD_Init(void);
uint32_t LCD_Config(uint32_t presc, uint32_t div);

void LCD_ClearAll(void);
void LCD_SetAll(void);
void LCD_WriteChar(uint8_t c, uint8_t pos);
void LCD_WriteString(char *s);
void LCD_WriteAlphanumericDisplay(char *s);
void LCD_WriteNumericDisplay(char *s);
void LCD_WriteSpecial(LCD_Code_t code, uint8_t v);

uint32_t SetLFAClock(uint32_t clock);
uint32_t SetLCDClock(uint32_t presc, uint32_t div);

void LCD_SetVoltage(uint32_t source, uint32_t div, uint32_t level );
void LCD_SetContrast(uint32_t level, uint32_t ref);

void LCD_Freeze(void);
void LCD_Unfreeze(void);



#endif // LCD_H
