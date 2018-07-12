/**
 * @file    lcd.c
 * @brief   LCD HAL for EFM32GG STK
 * @version 1.0
 *
 * @note    To debug there is a LCD_EMULATION compiler flag. By defining it, all writes of
 *          segments are directed to a 8x2 array in memory
 *
 * @note    There are at least two segment numbering methods. One of them is the LCD device.
 *          Other of the LCD controller in the microcontroller.
 * @note    The same for common numbering. The numbering of the LCD device is the reverse of the
 *          numbering used in the LCD controller.
 */

//#define LCD_EMULATION


#include  <stdint.h>
#include "lcd.h"

#ifndef LCD_EMULATION
#include "em_device.h"/////////
//#include "efm32gg_lcd.h"
//#include "efm32gg_cmu.h"
#endif

/** BIT macro generates a 1 bit in the N position */
#define BIT(N) (1U<<(N))


/**
 * Flag. When set, clock is initialized.
 */
static uint8_t lcdclock_set = 0;

/**
 *  @brief  Table for 14 segments displays
 *
 *  @note   For each character, it gives the segments to be lit
 *  @note   It uses a 16 bit word and each bit corresponds to a segment
 *  @note   0 position corresponds to space (0x20=32)
 *  @note   To get the segments for character c use segments14forchar[c-' ']
 *
 */

// bit-segment association
#define _DP 0x4000
#define _L  0x2000
#define _M  0x1000
#define _N  0x800
#define _K  0x400
#define _J  0x200
#define _H  0x100
#define _G2 0x80
#define _G1 0x40
#define _F  0x20
#define _E  0x10
#define _D  0x8
#define _C  0x4
#define _B  0x2
#define _A  0x1

const uint16_t segments14forchar[96] = {
            0                                                        , /* (space) */
          _DP                                            |_C |_B     , /* ! */
                              _J                             |_B     , /* " */
                  _M         |_J     |_G2|_G1        |_D |_C |_B     , /* # */
                  _M         |_J     |_G2|_G1|_F     |_D |_C     |_A , /* $ */
              _L |_M |_N |_K |_J |_H |_G2|_G1|_F         |_C         , /* % */
              _L             |_J |_H     |_G1    |_E |_D         |_A , /* & */
                              _J                                     , /* ' */
              _L         |_K                                         , /* ( */
                      _N         |_H                                 , /* ) */
              _L |_M |_N |_K |_J |_H |_G2|_G1                        , /* * */
                  _M         |_J     |_G2|_G1                        , /* + */
                      _N                                             , /* , */
                                      _G2|_G1                        , /* - */
          _DP                                                        , /* . */
                      _N |_K                                         , /* / */
                      _N |_K                 |_F |_E |_D |_C |_B |_A , /* 0 */
                          _K                             |_C |_B     , /* 1 */
                                      _G2|_G1    |_E |_D     |_B |_A , /* 2 */
                                      _G2            |_D |_C |_B |_A , /* 3 */
                                      _G2|_G1|_F         |_C |_B     , /* 4 */
              _L                         |_G1|_F     |_D         |_A , /* 5 */
                                      _G2|_G1|_F |_E |_D |_C     |_A , /* 6 */
                                                          _C |_B |_A , /* 7 */
                                      _G2|_G1|_F |_E |_D |_C |_B |_A , /* 8 */
                                      _G2|_G1|_F     |_D |_C |_B |_A , /* 9 */
                  _M         |_J                                     , /* : */
                      _N     |_J                                     , /* ; */
              _L         |_K             |_G1                        , /* < */
                                      _G2|_G1        |_D             , /* = */
                      _N         |_H |_G2                            , /* > */
          _DP    |_M                 |_G2                    |_B |_A , /* ? */
                              _J     |_G2    |_F |_E |_D     |_B |_A , /* @ */
                                      _G2|_G1|_F |_E     |_C |_B |_A , /* A */
                  _M         |_J     |_G2            |_D |_C |_B |_A , /* B */
                                              _F |_E |_D         |_A , /* C */
                  _M         |_J                     |_D |_C |_B |_A , /* D */
                                          _G1|_F |_E |_D         |_A , /* E */
                                          _G1|_F |_E             |_A , /* F */
                                      _G2    |_F |_E |_D |_C     |_A , /* G */
                                      _G2|_G1|_F |_E     |_C |_B     , /* H */
                  _M         |_J                     |_D         |_A , /* I */
                                                  _E |_D |_C |_B     , /* J */
              _L         |_K             |_G1|_F |_E                 , /* K */
                                              _F |_E |_D             , /* L */
                          _K     |_H         |_F |_E     |_C |_B     , /* M */
              _L                 |_H         |_F |_E     |_C |_B     , /* N */
                                              _F |_E |_D |_C |_B |_A , /* O */
                                      _G2|_G1|_F |_E         |_B |_A , /* P */
              _L                             |_F |_E |_D |_C |_B |_A , /* Q */
              _L                     |_G2|_G1|_F |_E         |_B |_A , /* R */
                                      _G2|_G1|_F     |_D |_C     |_A , /* S */
                  _M         |_J                                 |_A , /* T */
                                              _F |_E |_D |_C |_B     , /* U */
                      _N |_K                 |_F |_E                 , /* V */
              _L     |_N                     |_F |_E     |_C |_B     , /* W */
              _L     |_N |_K     |_H                                 , /* X */
                                      _G2|_G1|_F     |_D |_C |_B     , /* Y */
                      _N |_K                         |_D         |_A , /* Z */
                                              _F |_E |_D         |_A , /* [ */
              _L                 |_H                                 , /* \ */
                                                      _D |_C |_B |_A , /* ] */
              _L     |_N                                             , /* ^ */
                                                      _D             , /* _ */
                                  _H                                 , /* ` */
                  _M                     |_G1    |_E |_D             , /* a */
              _L                         |_G1|_F |_E |_D             , /* b */
                                      _G2|_G1    |_E |_D             , /* c */
                      _N             |_G2            |_D |_C |_B     , /* d */
                      _N                 |_G1    |_E |_D             , /* e */
                  _M     |_K         |_G2|_G1                        , /* f */
                          _K         |_G2            |_D |_C |_B     , /* g */
                  _M                     |_G1|_F |_E                 , /* h */
                  _M                                                 , /* i */
                      _N     |_J                 |_E                 , /* j */
              _L |_M     |_K |_J                                     , /* k */
                                              _F |_E                 , /* l */
                  _M                 |_G2|_G1    |_E     |_C         , /* m */
                  _M                     |_G1    |_E                 , /* n */
                                      _G2|_G1    |_E |_D |_C         , /* o */
                                  _H     |_G1|_F |_E                 , /* p */
                          _K         |_G2                |_C |_B     , /* q */
                                          _G1    |_E                 , /* r */
              _L                     |_G2            |_D             , /* s */
                                          _G1|_F |_E |_D             , /* t */
                                                  _E |_D |_C         , /* u */
                      _N                         |_E                 , /* v */
              _L     |_N                         |_E     |_C         , /* w */
              _L     |_N |_K     |_H                                 , /* x */
                              _J     |_G2            |_D |_C |_B     , /* y */
                      _N                 |_G1        |_D             , /* z */
                      _N         |_H     |_G1        |_D         |_A , /* { */
                  _M         |_J                                     , /* | */
              _L         |_K         |_G2            |_D         |_A , /* } */
                      _N |_K         |_G2|_G1                        , /* ~ */
          0                                                          , /* (del) */
};


/**
 *  @brief  Table for 7 segments displays
 *
 *  @note   Only used when trying to display alphabetic characters in the numeric display
 *  @note   For each character, it gives the segments to be lit
 *  @note   It uses a 16 bit word and each bit corresponds to a segment
 *  @note   0 position corresponds to space (0x20=32)
 *  @note   To get the segments for character c use segments14forchar[c-' ']

 *
 */

// bit-segment association. Symbols are reused!!!!
#if USE_7SEGMENTS_FOR_CHARS
#undef _DP
#undef _DP
#undef _G
#undef _F
#undef _E
#undef _D
#undef _C
#undef _B
#undef _A

#define _DP 0x80
#define _G  0x40
#define _F  0x20
#define _E  0x10
#define _D  0x8
#define _C  0x4
#define _B  0x2
#define _A  0x1

const uint8_t segment7forchar[96] = {
         0                              , /* (space) */
         _DP                |_C |_B     , /* ! */
                 _F             |_B     , /* " */
             _G |_F |_E |_D |_C |_B     , /* # */
             _G |_F     |_D |_C     |_A , /* $ */
         _DP|_G     |_E         |_B     , /* % */
             _G             |_C |_B     , /* & */
                 _F                     , /* ' */
                 _F     |_D         |_A , /* ( */
                         _D     |_B |_A , /* ) */
                 _F                 |_A , /* * */
             _G |_F |_E                 , /* + */
                     _E                 , /* , */
             _G                         , /* - */
         _DP                            , /* . */
             _G     |_E         |_B     , /* / */
                 _F |_E |_D |_C |_B |_A , /* 0 */
                             _C |_B     , /* 1 */
             _G     |_E |_D     |_B |_A , /* 2 */
             _G         |_D |_C |_B |_A , /* 3 */
             _G |_F         |_C |_B     , /* 4 */
             _G |_F     |_D |_C     |_A , /* 5 */
             _G |_F |_E |_D |_C     |_A , /* 6 */
                             _C |_B |_A , /* 7 */
             _G |_F |_E |_D |_C |_B |_A , /* 8 */
             _G |_F     |_D |_C |_B |_A , /* 9 */
                         _D         |_A , /* : */
                         _D |_C     |_A , /* ; */
             _G |_F                 |_A , /* < */
             _G         |_D             , /* = */
             _G                 |_B |_A , /* > */
         _DP|_G     |_E         |_B |_A , /* ? */
             _G     |_E |_D |_C |_B |_A , /* @ */
             _G |_F |_E     |_C |_B |_A , /* A */
             _G |_F |_E |_D |_C         , /* B */
                 _F |_E |_D         |_A , /* C */
             _G     |_E |_D |_C |_B     , /* D */
             _G |_F |_E |_D         |_A , /* E */
             _G |_F |_E             |_A , /* F */
                 _F |_E |_D |_C     |_A , /* G */
             _G |_F |_E     |_C |_B     , /* H */
                 _F |_E                 , /* I */
                     _E |_D |_C |_B     , /* J */
             _G |_F |_E     |_C     |_A , /* K */
                 _F |_E |_D             , /* L */
                     _E     |_C     |_A , /* M */
                 _F |_E     |_C |_B |_A , /* N */
                 _F |_E |_D |_C |_B |_A , /* O */
             _G |_F |_E         |_B |_A , /* P */
             _G |_F     |_D     |_B |_A , /* Q */
                 _F |_E         |_B |_A , /* R */
             _G |_F     |_D |_C     |_A , /* S */
             _G |_F |_E |_D             , /* T */
                 _F |_E |_D |_C |_B     , /* U */
                 _F |_E |_D |_C |_B     , /* V */
                 _F     |_D     |_B     , /* W */
             _G |_F |_E     |_C |_B     , /* X */
             _G |_F     |_D |_C |_B     , /* Y */
             _G     |_E |_D     |_B |_A , /* Z */
                 _F |_E |_D         |_A , /* [ */
             _G |_F         |_C         , /* \ */
                         _D |_C |_B |_A , /* ] */
                 _F             |_B |_A , /* ^ */
                         _D             , /* _ */
                                 _B     , /* ` */
             _G     |_E |_D |_C |_B |_A , /* a */
             _G |_F |_E |_D |_C         , /* b */
             _G     |_E |_D             , /* c */
             _G     |_E |_D |_C |_B     , /* d */
             _G |_F |_E |_D     |_B |_A , /* e */
             _G |_F |_E             |_A , /* f */
             _G |_F     |_D |_C |_B |_A , /* g */
             _G |_F |_E     |_C         , /* h */
                     _E                 , /* i */
                         _D |_C         , /* j */
             _G |_F |_E     |_C     |_A , /* k */
                 _F |_E                 , /* l */
                     _E     |_C         , /* m */
             _G     |_E     |_C         , /* n */
             _G     |_E |_D |_C         , /* o */
             _G |_F |_E         |_B |_A , /* p */
             _G |_F         |_C |_B |_A , /* q */
             _G     |_E                 , /* r */
             _G |_F     |_D |_C     |_A , /* s */
             _G |_F |_E |_D             , /* t */
                     _E |_D |_C         , /* u */
                     _E |_D |_C         , /* v */
                     _E     |_C         , /* w */
             _G |_F |_E     |_C |_B     , /* x */
             _G |_F     |_D |_C |_B     , /* y */
             _G     |_E |_D     |_B |_A , /* z */
             _G             |_C |_B     , /* { */
                 _F |_E                 , /* | */
             _G |_F |_E                 , /* } */
                                     _A , /* ~ */
         0                              , /* (del) */
};
#endif


/**
  *
  *  @brief  Configuration for LCD Segments
  *
  *  @note   There are two segment information. One in the display and the other in the controller
  *
  */

//
// LCD Matriz 20x8
//
//
//
//        LCD Seg   LCD Pin   LCD Controller        MCU Port      MCU Pin
//        S0          1         Seg 12      <--->    PA15           B1
//        S1          2         Seg 13      <--->    PA0            C2
//        S2          3         Seg 14      <--->    PA1            C1
//        S3          4         Seg 15      <--->    PA2            D2
//        S4          5         Seg 16      <--->    PA3            D1
//        S5          6         Seg 17      <--->    PA4            E3
//        S6          7         Seg 18      <--->    PA5            E2
//        S7          8         Seg 19      <--->    PA6            E1
//        S8          9         Seg 28      <--->    PD9            D6
//        S9         10         Seg 29      <--->    PD10           A5
//        S10        11         Seg 30      <--->    PD11           B5
//        S11        12         Seg 31      <--->    PD12           C5
//        S12        13         Seg 32      <--->    PB0            E4
//        S13        14         Seg 33      <--->    PB1            F1
//        S14        15         Seg 34      <--->    PB2            F2
//        S15        16         Seg 35      <--->    PA7            H4
//        S16        17         Seg 36      <--->    PA8            H5
//        S17        18         Seg 37      <--->    PA9            J5
//        S18        19         Seg 38      <--->    PA10           J6
//        S19        20         Seg 39      <--->    PA11           K5
//        COM7       21         Com 0       <--->    PB6            G2
//        COM6       22         Com 1       <--->    PB5            G1
//        COM5       23         Com 2       <--->    PB4            F4
//        COM4       24         Com 3       <--->    PB3            F3
//        COM3       25         Com 4       <--->    PE7            D9
//        COM2       26         Com 5       <--->    PE6            C9
//        COM1       27         Com 6       <--->    PE5            B9
//        COM0       28         Com 7       <--->    PD4            J11
//
//                              CAP+                 PA12           J4
//                              CAP-                 PA13           K3
//                              EXT                  PA14           L3
//


/**
 *  Segment information is encoded in a 16 bit word
 *  Each segment of display is addressed by a segment signal AND a common signal
 *  The high order byte contains the segment
 *  The lower order byte containts the common
 */
#define SEGS    20
#define COMMS   8

#define SEGMASK(X) ((X)<<8)
#define GET_SEG(M) ((M)>>8)
#define GET_COMMON(M) ((M)&0xFF)

#ifdef TWO_STEPS_ENCODING
/**
 *  TWO STEPS ENCODING
 *  In Two Steps Encoding, the LCD Segment Pin and Common Pin is found
 *  in lookup tables seg_encoding and com_encoding
 */

#define S00 SEGMASK(0)
#define S01 SEGMASK(1)
#define S02 SEGMASK(2)
#define S03 SEGMASK(3)
#define S04 SEGMASK(4)
#define S05 SEGMASK(5)
#define S06 SEGMASK(6)
#define S07 SEGMASK(7)
#define S08 SEGMASK(8)
#define S09 SEGMASK(9)
#define S10 SEGMASK(10)
#define S11 SEGMASK(11)
#define S12 SEGMASK(12)
#define S13 SEGMASK(13)
#define S14 SEGMASK(14)
#define S15 SEGMASK(15)
#define S16 SEGMASK(16)
#define S17 SEGMASK(17)
#define S18 SEGMASK(18)
#define S19 SEGMASK(19)

#define C0 0
#define C1 1
#define C2 2
#define C3 3
#define C4 4
#define C5 5
#define C6 6
#define C7 7
/////////////////////////// END OF TWO STEPS ENCODING //////////////////////////////////////////////

#else
/////////////////////////// BEGIN OF ONE STEP ENCODING /////////////////////////////////////////////
//
// In one step encoding, the Pins used are specified by preprocessor symbols
//

#define S00 SEGMASK(12)
#define S01 SEGMASK(13)
#define S02 SEGMASK(14)
#define S03 SEGMASK(15)
#define S04 SEGMASK(16)
#define S05 SEGMASK(17)
#define S06 SEGMASK(18)
#define S07 SEGMASK(19)
#define S08 SEGMASK(28)
#define S09 SEGMASK(29)
#define S10 SEGMASK(30)
#define S11 SEGMASK(31)
#define S12 SEGMASK(32)
#define S13 SEGMASK(33)
#define S14 SEGMASK(34)
#define S15 SEGMASK(35)
#define S16 SEGMASK(36)
#define S17 SEGMASK(37)
#define S18 SEGMASK(38)
#define S19 SEGMASK(39)

#define C0 7
#define C1 6
#define C2 5
#define C3 4
#define C4 3
#define C5 2
#define C6 1
#define C7 0


#endif

/*
 * @brief table for LCD 14 segments adjusted por position
 *
 * @note  basically, how to turn on a LCD segment knowing its position
 */
uint16_t tablcd[15][12] = {
//          P00     P01     P02     P03     P04     P05     P06     P07     P08     P09     P10     P11
/* A  */ { C7|S12, C6|S01, C6|S03, C6|S05, C6|S07, C7|S09, C7|S11, C6|S13, C0|S18, C0|S17, C0|S16, C0|S15 },
/* B  */ { C7|S13, C6|S02, C6|S04, C6|S06, C6|S08, C6|S10, C6|S12, C6|S14, C2|S18, C2|S17, C2|S16, C2|S15 },
/* C  */ { C7|S14, C2|S02, C2|S04, C2|S06, C2|S08, C2|S10, C2|S12, C2|S14, C5|S18, C5|S17, C5|S16, C5|S15 },
/* D  */ { C7|S15, C0|S02, C0|S04, C0|S06, C0|S08, C0|S10, C0|S12, C0|S14, C6|S18, C6|S17, C6|S16, C6|S15 },
/* E  */ { C6|S00, C0|S01, C0|S03, C0|S05, C0|S07, C1|S09, C1|S11, C0|S13, C4|S18, C4|S17, C4|S16, C4|S15 },
/* F  */ { C7|S17, C4|S01, C4|S03, C4|S05, C4|S07, C5|S09, C5|S11, C4|S13, C1|S18, C1|S17, C1|S16, C1|S15 },
/* G  */ { C7|S16, C3|S01, C3|S03, C3|S05, C3|S07, C4|S09, C4|S11, C3|S13, C3|S18, C3|S17, C3|S16, C3|S15 },
/* H  */ { C7|S18, C5|S01, C5|S03, C5|S05, C5|S07, C6|S09, C6|S11, C5|S13, C7|S19, C3|S00, C0|S00, C7|S07 },
/* J  */ { C0|S00, C4|S02, C4|S04, C4|S06, C4|S08, C4|S10, C4|S12, C4|S14, C6|S19, C0|S00, C0|S00, C7|S06 },
/* K  */ { C4|S19, C5|S02, C5|S04, C5|S06, C5|S08, C5|S10, C5|S12, C5|S14, C0|S19, C7|S10, C0|S00, C7|S05 },
/* M  */ { C5|S00, C3|S02, C3|S04, C3|S06, C3|S08, C3|S10, C3|S12, C3|S14, C5|S19, C2|S19, C0|S00, C7|S04 },
/* N  */ { C0|S00, C1|S02, C1|S04, C1|S06, C1|S08, C1|S10, C1|S12, C1|S14, C1|S19, C0|S00, C0|S00, C7|S03 },
/* P  */ { C0|S00, C2|S01, C2|S03, C2|S05, C2|S07, C3|S09, C3|S11, C2|S13, C0|S00, C0|S00, C0|S00, C7|S02 },
/* Q  */ { C0|S00, C1|S01, C1|S03, C1|S05, C1|S07, C2|S09, C2|S11, C1|S13, C4|S00, C0|S00, C0|S00, C7|S01 },
/* DP */ { C0|S00, C0|S00, C0|S00, C2|S00, C1|S00, C0|S09, C0|S11, C0|S00, C7|S08, C0|S00, C3|S19, C7|S00 }
};


/**
 *  @brief  struct to hold information about segment
 */
typedef struct {
    uint32_t    hi;
    uint32_t    lo;
} SegEncoding_t;


#ifdef TWO_STEPS_ENCODING
//////////////////////////////// BEGIN OF TWO STEPS ENCODING ///////////////////////////////////////

const SegEncoding_t seg_encoding[] = {
    //   SEG19-16     SEG15-0
    {   0x00000000,  BIT(12)    },  //    S0       ->      Seg 12    SEGEN3
    {   0x00000000,  BIT(13)    },  //    S1       ->      Seg 13    SEGEN3
    {   0x00000000,  BIT(14)    },  //    S2       ->      Seg 14    SEGEN3
    {   0x00000000,  BIT(15)    },  //    S3       ->      Seg 15    SEGEN3
    {   0x00000000,  BIT(16)    },  //    S4       ->      Seg 16    SEGEN4
    {   0x00000000,  BIT(17)    },  //    S5       ->      Seg 17    SEGEN4
    {   0x00000000,  BIT(18)    },  //    S6       ->      Seg 18    SEGEN4
    {   0x00000000,  BIT(19)    },  //    S7       ->      Seg 19    SEGEN4
    {   0x00000000,  BIT(28)    },  //    S8       ->      Seg 28    SEGEN7
    {   0x00000000,  BIT(29)    },  //    S9       ->      Seg 29    SEGEN7
    {   0x00000000,  BIT(30)    },  //    S10      ->      Seg 30    SEGEN7
    {   0x00000000,  BIT(31)    },  //    S11      ->      Seg 31    SEGEN7
    {   BIT(32-32),  0x00000000 },  //    S12      ->      Seg 32    SEGEN8
    {   BIT(33-32),  0x00000000 },  //    S13      ->      Seg 33    SEGEN8
    {   BIT(34-32),  0x00000000 },  //    S14      ->      Seg 34    SEGEN8
    {   BIT(35-32),  0x00000000 },  //    S15      ->      Seg 35    SEGEN8
    {   BIT(36-32),  0x00000000 },  //    S16      ->      Seg 36    SEGEN0
    {   BIT(37-32),  0x00000000 },  //    S17      ->      Seg 37    SEGEN9
    {   BIT(38-32),  0x00000000 },  //    S18      ->      Seg 38    SEGEN9
    {   BIT(39-32),  0x00000000 },  //    S19      ->      Seg 39    SEGEN9
};

const uint8_t com_encoding[8] = { 7, 6, 5, 4, 3, 2, 1, 0 };

//////////////////////////////// END OF TWO STEPS ENCODING /////////////////////////////////////////
#else

const uint32_t seg_encoding[] = {
  /* 0  */     BIT(0),
  /* 1  */     BIT(1),
  /* 2  */     BIT(2),
  /* 3  */     BIT(3),
  /* 4  */     BIT(4),
  /* 5  */     BIT(5),
  /* 6  */     BIT(6),
  /* 7  */     BIT(7),
  /* 8  */     BIT(8),
  /* 9  */     BIT(9),
  /* 10  */    BIT(10),
  /* 11  */    BIT(11),
  /* 12  */    BIT(12),
  /* 13  */    BIT(13),
  /* 14  */    BIT(14),
  /* 15  */    BIT(15),
  /* 16  */    BIT(16),
  /* 17  */    BIT(17),
  /* 18  */    BIT(18),
  /* 19  */    BIT(19),
  /* 20  */    BIT(20),
  /* 21  */    BIT(21),
  /* 22  */    BIT(22),
  /* 23  */    BIT(23),
  /* 24  */    BIT(24),
  /* 25  */    BIT(25),
  /* 26  */    BIT(26),
  /* 27  */    BIT(27),
  /* 28  */    BIT(28),
  /* 29  */    BIT(29),
  /* 30  */    BIT(30),
  /* 31  */    BIT(31),
  /* 32  */    BIT(0),
  /* 33  */    BIT(1),
  /* 34  */    BIT(2),
  /* 35  */    BIT(3),
  /* 36  */    BIT(4),
  /* 37  */    BIT(5),
  /* 38  */    BIT(6),
  /* 39  */    BIT(7)
};

#endif


/**
 * @brief   Table of all segments in a certain position
 *
 * @note    Used to clear char before writing a new one
 */
static const SegEncoding_t tablcdclear[12][8] = {
    /* Position  P00 */
    {
        /* C0  */       {   0, 0 },
        /* C1  */       {   0, 0 },
        /* C2  */       {   0, 0 },
        /* C3  */       {   0, 0 },
        /* C4  */       {   0, 0 },
        /* C5  */       {   0, 0 },
        /* C6  */       {   0, 0 },
        /* C7  */       {   0, 0 }
    },
    /* Position  P01 */
    {
        /* C0  */       {   0, 0 },
        /* C1  */       {   0, BIT(13)|BIT(14) },
        /* C2  */       {   0, BIT(13)|BIT(14) },
        /* C3  */       {   0, BIT(13)|BIT(14) },
        /* C4  */       {   0, BIT(13)|BIT(14) },
        /* C5  */       {   0, BIT(13)|BIT(14) },
        /* C6  */       {   0, BIT(13)|BIT(14) },
        /* C7  */       {   0, BIT(13)|BIT(14) }
    },
    /* Position  P02 */
    {
        /* C0  */       {   0, 0 },
        /* C1  */       {   0, BIT(15)|BIT(16) },
        /* C2  */       {   0, BIT(15)|BIT(16) },
        /* C3  */       {   0, BIT(15)|BIT(16) },
        /* C4  */       {   0, BIT(15)|BIT(16) },
        /* C5  */       {   0, BIT(15)|BIT(16) },
        /* C6  */       {   0, BIT(15)|BIT(16) },
        /* C7  */       {   0, BIT(15)|BIT(16) }
    },
    /* Position  P03 */
    {
        /* C0  */       {   0, 0 },
        /* C1  */       {   0, BIT(17)|BIT(18) },
        /* C2  */       {   0, BIT(17)|BIT(18) },
        /* C3  */       {   0, BIT(17)|BIT(18) },
        /* C4  */       {   0, BIT(17)|BIT(18) },
        /* C5  */       {   0, BIT(17)|BIT(18) },
        /* C6  */       {   0, BIT(17)|BIT(18) },
        /* C7  */       {   0, BIT(17)|BIT(18) }
    },
    /* Position  P04 */
    {
        /* C0  */       {   0, 0 },
        /* C1  */       {   0, BIT(19)|BIT(28) },
        /* C2  */       {   0, BIT(19)|BIT(28) },
        /* C3  */       {   0, BIT(19)|BIT(28) },
        /* C4  */       {   0, BIT(19)|BIT(28) },
        /* C5  */       {   0, BIT(19)|BIT(28) },
        /* C6  */       {   0, BIT(19)|BIT(28) },
        /* C7  */       {   0, BIT(19)|BIT(28) }
    },
    /* Position  P05 */
    {
        /* C0  */       {   0, BIT(29) },
        /* C1  */       {   0, BIT(29)|BIT(30) },
        /* C2  */       {   0, BIT(29)|BIT(30) },
        /* C3  */       {   0, BIT(29)|BIT(30) },
        /* C4  */       {   0, BIT(29)|BIT(30) },
        /* C5  */       {   0, BIT(29)|BIT(30) },
        /* C6  */       {   0, BIT(29)|BIT(30) },
        /* C7  */       {   0, BIT(30) }
    },
    /* Position  P06 */
    {
        /* C0  */       {   0,          BIT(31) },
        /* C1  */       {   BIT(32-32), BIT(31) },
        /* C2  */       {   BIT(32-32), BIT(31) },
        /* C3  */       {   BIT(32-32), BIT(31) },
        /* C4  */       {   BIT(32-32), BIT(31) },
        /* C5  */       {   BIT(32-32), BIT(31) },
        /* C6  */       {   BIT(32-32), BIT(31) },
        /* C7  */       {   BIT(32-32), 0 }
    },
    /* Position  P07 */
    {
        /* C0  */       {   0,                      0 },
        /* C1  */       {   BIT(34-32)|BIT(33-32),  0 },
        /* C2  */       {   BIT(34-32)|BIT(33-32),  0 },
        /* C3  */       {   BIT(34-32)|BIT(33-32),  0 },
        /* C4  */       {   BIT(34-32)|BIT(33-32),  0 },
        /* C5  */       {   BIT(34-32)|BIT(33-32),  0 },
        /* C6  */       {   BIT(34-32)|BIT(33-32),  0 },
        /* C7  */       {   BIT(34-32)|BIT(33-32),  0 }
    },
    /* Position  P08 */
    {
        /* C0  */       {   0,          0 },
        /* C1  */       {   BIT(38-32), 0 },
        /* C2  */       {   BIT(38-32), 0 },
        /* C3  */       {   BIT(38-32), 0 },
        /* C4  */       {   BIT(38-32), 0 },
        /* C5  */       {   BIT(38-32), 0 },
        /* C6  */       {   BIT(38-32), 0 },
        /* C7  */       {   BIT(38-32), 0 }
    },
    /* Position  P09 */
    {
        /* C0  */       {   0,          0 },
        /* C1  */       {   BIT(37-32), 0 },
        /* C2  */       {   BIT(37-32), 0 },
        /* C3  */       {   BIT(37-32), 0 },
        /* C4  */       {   BIT(37-32), 0 },
        /* C5  */       {   BIT(37-32), 0 },
        /* C6  */       {   BIT(37-32), 0 },
        /* C7  */       {   BIT(37-32), 0 }
    },
    /* Position  P10 */
    {
        /* C0  */       {   0,          0 },
        /* C1  */       {   BIT(36-32), 0 },
        /* C2  */       {   BIT(36-32), 0 },
        /* C3  */       {   BIT(36-32), 0 },
        /* C4  */       {   BIT(36-32), 0 },
        /* C5  */       {   BIT(36-32), 0 },
        /* C6  */       {   BIT(36-32), 0 },
        /* C7  */       {   BIT(36-32), 0 }
    },
    /* Position  P11 */
    {
        /* C0  */       {   0, 0 },
        /* C1  */       {   BIT(35-32), 0 },
        /* C2  */       {   BIT(35-32), 0 },
        /* C3  */       {   BIT(35-32), 0 },
        /* C4  */       {   BIT(35-32), 0 },
        /* C5  */       {   BIT(35-32), 0 },
        /* C6  */       {   BIT(35-32), 0 },
        /* C7  */       {   BIT(35-32), 0 }
    },
};

/*
 * This array emulates the LCD data registers
 */
#ifdef LCD_EMULATION
typedef struct {
    uint32_t hi;
    uint32_t lo;
} lcddata_t;

lcddata_t lcd[8];
#endif


/**
 * @brief   Set Clock for LCD module
 *
 * @note    LFACLK is the clock Source for the LCD module. Must be set before using LCD.
 *          It can be driven by LFRCOCLK, HFCORECLK/2, LFXOCLK and UFLCDOCLK
 *
 * @param   source can be
 *               LCD_CLK_DEFAULT : uses LFRCO as source
 *               LCD_CLK_LFRCO   : uses the internal RC oscillator with 32768 Hz
 *               LCD_CLK_ULRCO   : uses the internal RC oscillator with 1 or 2 kHz
 *               LCD_CLK_HFCORECLK_2 : user HFCORE clock signal divided by 2
 *               LCD_CLK_LFXO    : uses externa crystal oscillator with 32768 Hz
 *
 * @note     It disables clock for LCD, LETIMER0, RTC and LESENSE but restore them at the end.
 * @note     It disables the LCD module.
 */
uint32_t
SetLFAClock(uint32_t source) {

#ifdef LCD_EMULATION
    /* Set flag to signalize clock is set */
    lcdclock_set = 1;
    return 0;
#else
uint32_t oldlfaclken0,lfclksel;

    if( source == LCD_CLOCK_DEFAULT )
        source = LCD_CLOCK_LFRCO;

    /* If LCD active, disable it. It is not restored */
    if( LCD->CTRL&LCD_CTRL_EN ) {
        LCD->CTRL &= ~(LCD_CTRL_EN);
    }

    /*
     * LFACLK is the selected clock for these Low Energy A Peripherals:
     *    LCD, LETIMER0, RTC and LESENSE
     *
     * The clocks for these devices are disabled, but configuration is saved to be restored later
     */
    oldlfaclken0 = CMU->LFACLKEN0;
    CMU->LFACLKEN0 &= ~(CMU_LFACLKEN0_LCD|CMU_LFACLKEN0_LETIMER0
                         |CMU_LFACLKEN0_RTC|CMU_LFACLKEN0_LESENSE);

    /*
     * It can be driven by LFRCO (default), LFXO, HFCORECLK/2, HFCORECLK/4 or ULFRCO (BURTC)
     * Note that LFRCO is disabled at reset !!!!!! It is enabled bt writing a word with the
     * LFRCOEN bit set to the OSCENCMD register.
     *
     *
     * The source for the LFACLK in selected by the LFA and LFAE fields in CMU->LFCLKSEL
     * More information in section 11.5.11 of reference manual
     *
     *    LFA   LFAE      source
     *     00     0       disabled
     *     00     1       UFLRCO
     *     01     0       LFRCO
     *     10     0       LFXO
     *     11     0       HFCORECLK/2
     *
     * The LCD clock has:
     *    a prescaler (CMU->LFAPRESC0.LCD) and
     *    a frame rate controller (CMU->LCDCTRL.FDIV)
     *    a clock enable (CMU->LFACLKEN0.LCD)
     */

    /* ??
     * Ensure LE modules are accessible, by setting LE bit to enable the clock for the
     * low energy modules. It enables bus access to Low Energy peripherals.
     *
     * To use LCD module, the LE interface clock must be enabled in CMU_HFCORECLKEN0,
     * in addition to the module clock
     */
     CMU->HFCORECLKEN0 |= CMU_HFCORECLKEN0_LE;

    /*
     * First, configure and enable the clock source for the LCD module
     */
    switch ( source ) {
    case LCD_CLOCK_LFRCO:
        /*
         * Set LFRCO as LFACLK clock source
         *
         * LFRCO is a RC oscillator withe a nominal frequency of 32768 Hz.
         * Following contents of
         * http://embeddedelectrons.blogspot.com/2016/12/example-code-for-segment-lcd-on-efm32.html
         */

        /*
         * Set LFRCOEN bit to enable the low frequency RC oscillator (LFRCO)
         */
        CMU->OSCENCMD = CMU_OSCENCMD_LFRCOEN;
        /*
         * Wait until LFRCO is enabled and start-up time has exceeded
         */
        while ( !(CMU->STATUS&CMU_STATUS_LFRCORDY) ) {}

        /*
         * Set the LFACLK to use LFRCO as clock source
         *
         */
        lfclksel  = CMU->LFCLKSEL;
        lfclksel &= ~CMU_LFCLKSEL_LFAE_ULFRCO;  // Clear LFAE bit
        lfclksel &= ~_CMU_LFCLKSEL_LFA_MASK;    // Clear LFA field
        lfclksel |= CMU_LFCLKSEL_LFA_LFRCO;    // Set LFRCO as LFACLK clock source
        CMU->LFCLKSEL = lfclksel;
        break;
    case LCD_CLOCK_HFCORECLK_2:
        /*
         * Additional division factor for HFCORECLKLE
         * See 11.5.2
         * This can only be done when HFCORECLK <= 32 MHz
         */
        if( SystemCoreClock < 32000000 ) {
             CMU->HFCORECLKDIV &= ~CMU_HFCORECLKDIV_HFCORECLKLEDIV;
        }


        /*
         * Set HFCORECLK/2 as LFACLK clock source
         * HFCORECLK is derived from the HFCORECLK
         * This is configured in the LFCLKSEL register
         * See RM section 11.5.11
         */
        lfclksel  = CMU->LFCLKSEL;
        lfclksel &= ~CMU_LFCLKSEL_LFAE_ULFRCO;  // Clear LFAE bit
        lfclksel &= ~_CMU_LFCLKSEL_LFA_MASK;    // Clear LFA field
        lfclksel |= CMU_LFCLKSEL_LFA_HFCORECLKLEDIV2;
                                                // Set HFCOREPERCLK_2 as LFACLK clock source
        CMU->LFCLKSEL = lfclksel;
        break;
    case LCD_CLOCK_HFCORECLK_4:
        /*
         * Additional division factor for HFCORECLKLE
         * See 11.5.2
         */
         CMU->HFCORECLKDIV |= CMU_HFCORECLKDIV_HFCORECLKLEDIV;

        /*
         * Set HFCORECLK/4 as LFACLK clock source
         * HFCORECLK is derived from the HFCORECLK
         * This is configured in the LFCLKSEL register
         * See RM section 11.5.11
         */
        lfclksel  = CMU->LFCLKSEL;
        lfclksel &= ~CMU_LFCLKSEL_LFAE_ULFRCO;  // Clear LFAE bit
        lfclksel &= ~_CMU_LFCLKSEL_LFA_MASK;    // Clear LFA field
        lfclksel |= CMU_LFCLKSEL_LFA_HFCORECLKLEDIV2;
                                                // Set HFCOREPERCLK_2 as LFACLK clock source
        CMU->LFCLKSEL = lfclksel;
        break;
    case LCD_CLOCK_ULFRCO:
        /*
         * Set ULFRCO as LFACLK clock source
         *
         * ULFRCO is a RC oscillator with a nominal frequency of 1 or 2 KHz (depends on the chip??)
         * ULFRCO is enable by default. It can only be disabled by selection other
         * clock source in the OSC field of the EMU_EMF4CONF register
         *
         * NOTE: The instruction below changes the clock source for the Energy Mode 4 (EM4)!!!!!!
         *       This can be disable by defining DO_NOT_RECONFIGURE_EM4OSC compiler flag
         *
         */
#ifndef DO_NOT_RECONFIGURE_EM4OSC
        EMU->EM4CONF &= ~_EMU_EM4CONF_OSC_MASK;
#endif

        /*
         * Configure ULFRCO as LFACLK clock source
         * This is configured in the LFCLKSEL register
         * See RM section 11.5.11
         */
        lfclksel  = CMU->LFCLKSEL;
        lfclksel |= CMU_LFCLKSEL_LFAE_ULFRCO;  // Clear LFAE bit
        lfclksel &= ~_CMU_LFCLKSEL_LFA_MASK;   // Clear LFA field
        CMU->LFCLKSEL = lfclksel;
        break;

    case LCD_CLOCK_LFXO:
        /*
         * Set ULFRCO as LFACLK clock source
         *
         * LFXO is an oscillator that uses a external 32768 Hz crystal
         */

        // If LFXO is NOT enabled, enable it
        if ( (CMU->STATUS&CMU_STATUS_LFXOENS) == 0 ) {
            CMU->OSCENCMD  = CMU_OSCENCMD_LFXOEN;
        }

        lfclksel  = CMU->LFCLKSEL;
        lfclksel &= ~CMU_LFCLKSEL_LFAE_ULFRCO;  // Clear LFAE bit
        lfclksel &= ~_CMU_LFCLKSEL_LFA_MASK;    // Clear LFA field
        lfclksel |= CMU_LFCLKSEL_LFA_LFXO;      // Set LFXCO as LFACLK clock source
        CMU->LFCLKSEL = lfclksel;
        break;
    }

    /* if LFACK not active then return and signalizes error */
    if( (CMU->LFCLKSEL & (_CMU_LFCLKSEL_LFA_MASK|CMU_LFCLKSEL_LFAE)) == 0 ) {
        return 1;
    }

    /* Restore clocks for devices */
    CMU->LFACLKEN0 = oldlfaclken0;

    /* Set flag to signalize clock is set */
    lcdclock_set = 1;

    return 0;
#endif
}



/**
 *  @brief  SetLCDClock
 *
 */
uint32_t SetLCDClock(uint32_t presc, uint32_t div) {


    if( presc == LCD_PRESC_DEFAULT )
        presc = LCD_PRESC_DIV16;

    if( div == LCD_DIV_DEFAULT ) {
        div = 0;
    } else {
        div--;
        if ( div > 7 ) div = 7;
    }

    /* Configure LCD Clock */
    CMU->LFAPRESC0 =    (CMU->LFAPRESC0&(~_CMU_LFAPRESC0_LCD_MASK))
                    |   presc<<_CMU_LFAPRESC0_LCD_SHIFT;

    CMU->LCDCTRL =      (CMU->LCDCTRL&(~_CMU_LCDCTRL_FDIV_MASK))
                    |   (div<<_CMU_LCDCTRL_FDIV_SHIFT);

    return 0;
}
/**
 *  @brief  Initializes LCD
 */
uint32_t LCD_Init(void) {
uint32_t rc;

    rc = LCD_Config(LCD_PRESC_DEFAULT,LCD_DIV_DEFAULT);

    return rc;
}

/*
 *  @brief  Configure LCD
 *
 *  @param  div:  set the frame rate according the formulas below
 *
 *  @note   frame_rate = (LFACK_freq/PRESC)/DIV/(MUX*2)
 *          where LFACLK_freq is the frequency of the LFACLK clock signal
 *                PRESC is the prescaler (16,32,64 or 128) set on LFAPRESC0 register
 *                DIV is the divisor set in the FDIV field of LCDCTRL register  (= DIV-1)
 *                MUX is the divisor set by the multiplexing mode
 *                    = 1 static
 *                      2 duplex
 *                      3 triples
 *                      4 quadruplex
 *                      6 hexplex
 *                      8 octaplex
 *
 *  @note   DIV = (LFACLK_freq/PRESC)/(frame_rate*MUX*2)
 *  @note   FDIV = DIV - 1
 *
 *  @note   When LCD_EMULATION is set, all write operations happen to a 8 position array
 *
 *  @note The table below show values of PRESC and FDIC that gives frame rate between 24 and 150
 *
  LFACLK freq |PRESC| LFACLK_presc|   DIV | FDIV | LFACLK_lcd  | Multiplexing | MUX |  Frame rate
 -------------|-----|-------------|-------|------|-------------|--------------|-----|------------
      32678   | 16  |      2048   |     1 |   0  |      2.048  |    Octaplex  |   8 |      128
      32678   | 16  |      2048   |     2 |   1  |      1.024  |    Octaplex  |   8 |       64
      32678   | 16  |      2048   |     3 |   2  |        683  |    Octaplex  |   8 |       43
      32678   | 16  |      2048   |     4 |   3  |        512  |    Octaplex  |   8 |       32
      32678   | 16  |      2048   |     5 |   4  |        410  |    Octaplex  |   8 |       26
      32678   | 32  |      1024   |     1 |   0  |      1.024  |    Octaplex  |   8 |       64
      32678   | 32  |      1024   |     2 |   1  |        512  |    Octaplex  |   8 |       32
      32678   | 64  |       512   |     1 |   0  |        512  |    Octaplex  |   8 |       32
 *
 *  @note  The default is PRESC=16 and FDIV=0 resulting in a 128 Hz frame rate
 *
 */
uint32_t LCD_Config(uint32_t presc, uint32_t div) {
#ifdef LCD_EMULATION
int i;

    /* Initializes area for segment information */
    for(i=0;i<8;i++) {
        lcd[i].hi = 0;
        lcd[i].lo = 0;
    }
#else
uint32_t segen;
int i;
uint32_t dispctrl;
#ifdef TWO_STEPS_ENCODING
uint32_t m,m4;
SegEncoding_t used = { 0,0 };
#else
int j;
#endif

    /*
     * Disable LCD module before fussing around with it
     */
    LCD->CTRL &= ~(LCD_CTRL_EN);

    /* If clock no set, configure it */
    if( !lcdclock_set ) {
        SetLFAClock(LCD_CLOCK_DEFAULT);
        SetLCDClock(LCD_PRESC_DEFAULT,LCD_DIV_DEFAULT);
    }
#ifdef TWO_STEPS_ENCODING

    /*
     * Scan all character encoding and accumulates all bits used */
    for(i=0;i<sizeof(seg_encoding)/sizeof(SegEncoding_t);i++) {
        used.hi |= seg_encoding[i].hi;
        used.lo |= seg_encoding[i].lo;
    }

    /*
     * Configure pins
     * A bit in the segen register corresponds to 4 segment pins
     */
    m = 1;
    m4 = 0xF;
    segen = 0;
    while ( (m&BIT(8)) == 0 ) {
        if( (used.lo & m4) != 0 ) {
            segen |= m;
        }
        m <<= 1;
        m4 <<= 4;
    }
    while ( (m&BIT(10)) == 0 ) {
        if( (used.hi & m4) != 0 ) {
            segen |= m;
        }
        m <<= 1;
        m4 <<= 4;
    }
#else
    /* Accumulate all segments used in the tablcd table and configure the corresponding pins */
    segen = 0;
    for(i=0;i<15;i++) {
        for(j=0;j<12;j++) {
            segen |= (1<<(GET_SEG(tablcd[i][j]))/4);
        }
    }
#endif

    LCD->SEGEN = segen;

    dispctrl = LCD->DISPCTRL;
    dispctrl &= ~(   _LCD_DISPCTRL_MUXE_MASK
                   | _LCD_DISPCTRL_MUX_MASK
                   | _LCD_DISPCTRL_BIAS_MASK
                   | _LCD_DISPCTRL_WAVE_MASK
                   | _LCD_DISPCTRL_VLCDSEL_MASK
                   | _LCD_DISPCTRL_CONCONF_MASK   );

    // 8 common pins: LCD_COM7-LCD_COM4 (SEG23-SEG20) . LCD_COM3-LCD_COM0
    dispctrl  |=   LCD_DISPCTRL_MUX_QUADRUPLEX      // QUADRUPLEX+MUXE -> OCTAPLEX
                 | LCD_DISPCTRL_MUXE                //
                 | LCD_DISPCTRL_BIAS_ONEFOURTH      // 1/4 bias
                 | LCD_DISPCTRL_WAVE_NORMAL;        // Normal wave
    LCD->DISPCTRL = dispctrl;

    /*
     * Enable LCD after configuration
     */
    LCD->CTRL |= LCD_CTRL_EN;
#endif
    return 0;
}

/**
 *  @brief  Write an ASCII character at the specified position
 *
 *  @param  c:   character to be written
 *  @param  pos: position where character is to be written
 *
 *  @note   When LCD_EMULATION is set, all writes happen to a 8 position array
 *
 */
void LCD_WriteChar(uint8_t c, uint8_t pos) {
uint32_t segments,m;
uint8_t sn,com,seg;
SegEncoding_t s[8] = { {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0} };

    /*
     * looks for segments to be lit
     * table contains only character with
     */
    segments = segments14forchar[c-' '];

    sn = 0;
    while( segments ) {
        if( segments&1 ) {
            m = tablcd[sn][pos];
            com = GET_COMMON(m);
            seg = GET_SEG(m);
#ifdef TWO_STEPS_ENCODING
            com = com_encoding[com]; // it is reversed on STK3700
            s[com].hi |= seg_encoding[seg].hi;
            s[com].lo |= seg_encoding[seg].lo;
#else
            if( seg < 32 ) {
                s[com].lo |= seg_encoding[seg];
            } else {
                s[com].hi |= seg_encoding[seg-32];
            }
#endif
        }
        segments>>=1;
        sn++;
    }
#ifndef LCD_EMULATION
    LCD->FREEZE |= LCD_FREEZE_REGFREEZE;
    for(com=0;com<8;com++) {
        switch(com) {
        case 0:
            LCD->SEGD0L = (LCD->SEGD0L&~tablcdclear[com][pos].lo)|s[com].lo;
            LCD->SEGD0H = (LCD->SEGD0H&~tablcdclear[com][pos].hi)|s[com].hi;
            break;
        case 1:
            LCD->SEGD1L = (LCD->SEGD1L&~tablcdclear[com][pos].lo)|s[com].lo;
            LCD->SEGD1H = (LCD->SEGD1H&~tablcdclear[com][pos].hi)|s[com].hi;
            break;
        case 2:
            LCD->SEGD2L = (LCD->SEGD2L&~tablcdclear[com][pos].lo)|s[com].lo;
            LCD->SEGD2H = (LCD->SEGD2H&~tablcdclear[com][pos].hi)|s[com].hi;
            break;
        case 3:
            LCD->SEGD3L = (LCD->SEGD3L&~tablcdclear[com][pos].lo)|s[com].lo;
            LCD->SEGD3H = (LCD->SEGD3H&~tablcdclear[com][pos].hi)|s[com].hi;
            break;
        case 4:
            LCD->SEGD4L = (LCD->SEGD4L&~tablcdclear[com][pos].lo)|s[com].lo;
            LCD->SEGD4H = (LCD->SEGD4H&~tablcdclear[com][pos].hi)|s[com].hi;
            break;
        case 5:
            LCD->SEGD5L = (LCD->SEGD5L&~tablcdclear[com][pos].lo)|s[com].lo;
            LCD->SEGD5H = (LCD->SEGD5H&~tablcdclear[com][pos].hi)|s[com].hi;
            break;
        case 6:
            LCD->SEGD6L = (LCD->SEGD6L&~tablcdclear[com][pos].lo)|s[com].lo;
            LCD->SEGD6H = (LCD->SEGD6H&~tablcdclear[com][pos].hi)|s[com].hi;
            break;
        case 7:
            LCD->SEGD7L = (LCD->SEGD7L&~tablcdclear[com][pos].lo)|s[com].lo;
            LCD->SEGD7H = (LCD->SEGD7H&~tablcdclear[com][pos].hi)|s[com].hi;
            break;
        }
    }
    LCD->FREEZE &= ~LCD_FREEZE_REGFREEZE;
#else
    for(com=0;com<8;com++) {
        switch(com) {
        case 0:
            lcd[0].lo = (lcd[0].lo&~tablcdclear[com][pos].lo)|s[com].lo;
            lcd[0].hi = (lcd[0].hi&~tablcdclear[com][pos].hi)|s[com].hi;
            break;
        case 1:
            lcd[1].lo = (lcd[1].lo&~tablcdclear[com][pos].lo)|s[com].lo;
            lcd[1].hi = (lcd[1].hi&~tablcdclear[com][pos].hi)|s[com].hi;
            break;
        case 2:
            lcd[2].lo = (lcd[2].lo&~tablcdclear[com][pos].lo)|s[com].lo;
            lcd[2].hi = (lcd[2].hi&~tablcdclear[com][pos].hi)|s[com].hi;
            break;
        case 3:
            lcd[3].lo = (lcd[3].lo&~tablcdclear[com][pos].lo)|s[com].lo;
            lcd[3].hi = (lcd[3].hi&~tablcdclear[com][pos].hi)|s[com].hi;
            break;
        case 4:
            lcd[4].lo = (lcd[4].lo&~tablcdclear[com][pos].lo)|s[com].lo;
            lcd[4].hi = (lcd[4].hi&~tablcdclear[com][pos].hi)|s[com].hi;
            break;
        case 5:
            lcd[5].lo = (lcd[5].lo&~tablcdclear[com][pos].lo)|s[com].lo;
            lcd[5].hi = (lcd[5].hi&~tablcdclear[com][pos].hi)|s[com].hi;
            break;
        case 6:
            lcd[6].lo = (lcd[6].lo&~tablcdclear[com][pos].lo)|s[com].lo;
            lcd[6].hi = (lcd[6].hi&~tablcdclear[com][pos].hi)|s[com].hi;
            break;
        case 7:
            lcd[7].lo = (lcd[7].lo&~tablcdclear[com][pos].lo)|s[com].lo;
            lcd[7].hi = (lcd[7].hi&~tablcdclear[com][pos].hi)|s[com].hi;
            break;
        }
    }
#endif


}

/**
 *  @brief  Write a string to positions 1 to 7 (with 14 segments displays)
 *
 *  @param  s: string to be written
 *
 *  @note   if the string is shorten than 7, it is padded with spaces
 */

void LCD_WriteAlphanumericField(char *s) {
int pos;

    for(pos=1;pos<=7;pos++) {
        if( *s ) {
            LCD_WriteChar(*s++,pos);
        } else {
            LCD_WriteChar(' ',pos);
        }
    }

}

/**
 *  @brief  Write a string into positions 8 to 11 (with 7 segments displays)
 *
 *  @param  s: string to be written
 *
 *  @note   if the string is shorten than 12, it is padded with spaces
 *
 */
void LCD_WriteNumericField(char *s) {
int pos;

    for(pos=8;pos<=12;pos++) {
        if( *s ) {
            LCD_WriteChar(*s++,pos);
        } else {
            LCD_WriteChar(' ',pos);
        }
    }

}

/**
 *  @brief  Write a string to positions 1 to 12 (with 14 segments displays in position 1 to 7)
 *          and 7 segments display in position 8 to 12)
 *
 *  @param  s: string to be written
 *
 *  @note   if the string is shorten than 12, it is padded with spaces
 */

void LCD_WriteString(char *s) {
int pos;

    for(pos=1;pos<=12;pos++) {
        if( *s ) {
            LCD_WriteChar(*s++,pos);
        } else {
            LCD_WriteChar(' ',pos);
        }
    }

}



/**
 *  @brief  Turns on/off a special segment
 *
 */
void LCD_SetSpecial(uint8_t c, uint8_t v) {
    // TBD
}

/**
 *  @brief  Turns off all segments
 *
 */
void LCD_Clear(void) {
#ifdef LCD_EMULATION
int i;

    for(i=0;i<8;i++) {
        lcd[i].hi = 0;
        lcd[i].lo = 0;
    }
#else
    LCD->SEGD0L = 0;
    LCD->SEGD0H = 0;
    LCD->SEGD1L = 0;
    LCD->SEGD1H = 0;
    LCD->SEGD2L = 0;
    LCD->SEGD2H = 0;
    LCD->SEGD3L = 0;
    LCD->SEGD3H = 0;
    LCD->SEGD4L = 0;
    LCD->SEGD4H = 0;
    LCD->SEGD5L = 0;
    LCD->SEGD5H = 0;
    LCD->SEGD6L = 0;
    LCD->SEGD6H = 0;
    LCD->SEGD7L = 0;
    LCD->SEGD7H = 0;
#endif
}

/**
 *  @brief  Turns on all segments
 *
 */
void LCD_SetAll(void) {
#ifdef LCD_EMULATION
int i;

    for(i=0;i<8;i++) {
        lcd[i].hi = 0xFF;
        lcd[i].lo = 0xFFFFFFFF;
    }
#else
    LCD->SEGD0L = 0xFFFFFFFF;
    LCD->SEGD0H = 0xFF;
    LCD->SEGD1L = 0xFFFFFFFF;
    LCD->SEGD1H = 0xFF;
    LCD->SEGD2L = 0xFFFFFFFF;
    LCD->SEGD2H = 0xFF;
    LCD->SEGD3L = 0xFFFFFFFF;
    LCD->SEGD3H = 0xFF;
    LCD->SEGD4L = 0xFFFFFFFF;
    LCD->SEGD4H = 0xFF;
    LCD->SEGD5L = 0xFFFFFFFF;
    LCD->SEGD5H = 0xFF;
    LCD->SEGD6L = 0xFFFFFFFF;
    LCD->SEGD6H = 0xFF;
    LCD->SEGD7L = 0xFFFFFFFF;
    LCD->SEGD7H = 0xFF;
#endif
}
