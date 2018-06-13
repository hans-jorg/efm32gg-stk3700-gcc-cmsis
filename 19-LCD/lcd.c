/**
 * @file    lcd.c
 * @brief   LCD HAL for EFM32GG STK
 * @version 1.0
 *
 * @note    There are at least two segment numbering methods. One of the LCD device. Other of the LCD controller
 *          in the microcontroller.
 * @note    The same for common numbering. The numbering of the LCD device is the reverse of the
 *          numbering used in the LCD controller.
 */

//#define EMULATION


#include  <stdint.h>

#ifndef EMULATION
#include "em_device.h"/////////
//#include "efm32gg_lcd.h"
//#include "efm32gg_cmu.h"
#endif

/**
 * Flag. When set, clock is initialized.
 */

static uint8_t lcdclock_set = 0;

/**
 * Default divisor for LCD clock
 */
#ifndef LCDCLOCKDIV_DFLT
#define LCDCLOCKDIV_DFLT 100
#endif

#define BIT(N) (1U<<(N))


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
            0                                                          , /* (space) */
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

#if USE_7SEGMENTS_FOR_CHARS
#undef _DP

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



/**********************************************************************************************//**
 *
 *  @brief  Configuration for LCD Segments
 *
 *  @note   There are two segment information. One in the display and the other in the controller
 *
 **************************************************************************************************/
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

#define SEGS    20
#define COMMS   8

#define SEGMASK(X) ((X)<<8)
#define GET_SEG(M) ((M)>>8)
#define GET_COMMON(M) ((M)&0xFF)

#ifdef TWO_STEPS_ENCODING
/////////////////////////// BEGIN OF TWO STEPS ENCODING ////////////////////////////////////////////
//
// In Two Steps Encoding, the LCD Segment Pin and Common Pin is found
// in lookup tables seg_encoding and com_encoding
//

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



#ifndef EMULATION
/**
 * @brief   Data structure for the divisors used to generate LCD Clock
 */

typedef struct {
    uint16_t    lfapresc_lcd;   // div = (1<<(lfapresc_lcd+4)); lcd values in [0-3]
    uint16_t    ldcctrl_fdiv;   // div = (lcdctrl_fdiv+1); fdiv values in [0-7]
} LCD_divconfig_t;

/**
 * @brief   Find divisor configuration for LCD
 *
 *
 * @note    Possible values
 *      |    div1    |   div2   |   divisor    |
 *      |------------|----------|--------------|
 *      |      16    |     1    |      16      |
 *      |      16    |     2    |      32      |
 *      |      16    |     3    |      48      |
 *      |      16    |     4    |      64      |
 *      |      16    |     5    |      80      |
 *      |      16    |     6    |      96      |
 *      |      16    |     7    |     112      |
 *      |      16    |     8    |     128      |
 *      |      32    |     5    |     160      |
 *      |      32    |     6    |     192      |
 *      |      32    |     7    |     224      |
 *      |      32    |     8    |     256      |
 *      |      64    |     5    |     320      |
 *      |      64    |     6    |     384      |
 *      |      64    |     7    |     448      |
 *      |      64    |     8    |     512      |
 *      |     128    |     5    |     640      |
 *      |     128    |     6    |     768      |
 *      |     128    |     7    |     896      |
 *      |     128    |     8    |    1024      |
 */

static LCD_divconfig_t
finddivconfig(int n) {
int d,e,r,emin;
LCD_divconfig_t ret;

    emin = 1024;
    ret.lfapresc_lcd = 0;
    ret.ldcctrl_fdiv = 0;
    for(d=4;d<=7;d++) {
            r = n>>d;
            if( r >= 1 && r <= 8 ) {
                e = n - (r<<d);
                if( e < 0 ) e = -e;
                if( e < emin ) {
                    emin = e;
                    ret.ldcctrl_fdiv = r-1;
                    ret.lfapresc_lcd = d-4;
                }
            }
            r++;
            if( r >= 1 && r <= 8 ) {
                e = n - (r<<d);
                if( e < 0 ) e = -e;
                if( e < emin ) {
                    emin = e;
                    ret.ldcctrl_fdiv = r-1;
                    ret.lfapresc_lcd = d-4;
                }
            }
    }
    return ret;
}
#endif


/**
 * @brief   Set Clock for LCD
 *
 * @note    Clock Source is LFACLK. Must be set before.
 */
uint32_t
LCD_SetClock(uint32_t div) {
#ifdef EMULATION
    return 0;
#else
uint32_t oldctrl,oldlfaclken0;
LCD_divconfig_t divconfig;

    /* If LCD active stop it */
    oldctrl = LCD->CTRL;
    if( oldctrl&LCD_CTRL_EN ) {
        LCD->CTRL &= ~(LCD_CTRL_EN);
    }

    /* if LFACK no active then return */
    if( (CMU->LFCLKSEL & (_CMU_LFCLKSEL_LFA_MASK|CMU_LFCLKSEL_LFAE)) != 0 ) {
        return 0;
    }

    /* If clock for LCD enabled, disable it */
    oldlfaclken0 = CMU->LFACLKEN0;
    if( (oldlfaclken0 & CMU_LFACLKEN0_LCD) != 0 ) {
        CMU->LFACLKEN0 &= ~(CMU_LFACLKEN0_LCD);
    }
    /* Configure LCD Clock */
    divconfig = finddivconfig(div);
    CMU->LFAPRESC0 =    (CMU->LFAPRESC0&(~_CMU_LFAPRESC0_LCD_MASK))
                     |  (divconfig.lfapresc_lcd<<_CMU_LFAPRESC0_LCD_SHIFT);
    CMU->LCDCTRL   =    (CMU->LCDCTRL&(~_CMU_LCDCTRL_FDIV_MASK))
                     |  (divconfig.ldcctrl_fdiv<<_CMU_LCDCTRL_FDIV_SHIFT);

    /* IF clock for LCD was enabled, reenable it */
    CMU->LFACLKEN0 = oldlfaclken0;

    /* If LCD was active, reenable it */
    LCD->CTRL = oldctrl;

    /* Set flag to signalize clock is set */
    lcdclock_set = 1;
    return 0;
#endif
}

/**
 *  @brief  Initializes LCD
 *
 *  @note   When emulation is set, all writes happen to a 8 position array
 *
 */
void LCD_Init(void) {
#ifdef EMULATION
int i;

    for(i=0;i<8;i++) {
        lcd[i].hi = 0;
        lcd[i].lo = 0;
    }
#else
uint32_t segen;
#ifdef TWO_STEPS_ENCODING
uint32_t m,m4;
SegEncoding used = { 0,0 };

    /* If clock no set, set it to default */
    if( !lcdclock_set ) {
        LCD_SetClock(LCDCLOCKDIV_DFLT);
    }
    for(i=0;i<sizeof(seg_encoding)/sizeof(SegEncoding_t);i++) {
        used.hi |= seg_encoding[i].hi;
        used.lo |= seg_encoding[i].lo;
    }

    // Disable LCD Controller
    LCD->CTRL &= ~LCD_CTRL_EN;
    // Configure pins
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
int i,j;

    segen = 0;
    for(i=0;i<15;i++) {
        for(j=0;j<12;j++) {
            segen |= (1<<(GET_SEG(tablcd[i][j]))/4);
        }
    }

#endif

    LCD->SEGEN = segen;
    // 8 common pins
    // LCD_COM7-LCD_COM4 (SEG23-SEG20) . LCD_COM3-LCD_COM0
    LCD->DISPCTRL |= LCD_DISPCTRL_MUX_QUADRUPLEX|LCD_DISPCTRL_MUXE;
    // 1/4 bias
    LCD->DISPCTRL |= LCD_DISPCTRL_BIAS_ONEFOURTH;
    // Normal wave
    LCD->DISPCTRL |= LCD_DISPCTRL_WAVE_NORMAL;
    // Enable (After configuration)
    LCD->CTRL |= LCD_CTRL_EN;
#endif
}

/**
 *  @brief  Write an ASCII character
 *
 *  @note   When emulation is set, all writes happen to a 8 position array
 *
 */
void LCD_WriteChar(uint8_t c, uint8_t pos) {
uint32_t segments,m;
uint8_t s,com,seg;
SegEncoding_t lcd[8] = { {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0} };

    segments = segments14forchar[c-' '];

    s = 0;
    while( segments ) {
        if( segments&1 ) {
            m = tablcd[s][pos];
            com = GET_COMMON(m);
            seg = GET_SEG(m);
#ifdef TWO_STEPS_ENCODING
            com = com_encoding[com]; // it is reversed on STK3700
            lcd[com].hi |= seg_encoding[seg].hi;
            lcd[com].lo |= seg_encoding[seg].lo;
#else
            if( seg < 32 ) {
                lcd[com].lo |= seg_encoding[seg];
            } else {
                lcd[com].hi |= seg_encoding[seg-32];
            }
#endif
        }
        segments>>=1;
        s++;
    }
#ifndef EMULATION
    for(com=0;com<8;com++) {
        switch(com) {
        case 0:
            LCD->SEGD0L = (LCD->SEGD0L&~tablcdclear[com][pos].lo)|lcd[com].lo;
            LCD->SEGD0H = (LCD->SEGD0H&~tablcdclear[com][pos].hi)|lcd[com].hi;
            break;
        case 1:
            LCD->SEGD1L = (LCD->SEGD1L&~tablcdclear[com][pos].lo)|lcd[com].lo;
            LCD->SEGD1H = (LCD->SEGD1H&~tablcdclear[com][pos].hi)|lcd[com].hi;
            break;
        case 2:
            LCD->SEGD2L = (LCD->SEGD2L&~tablcdclear[com][pos].lo)|lcd[com].lo;
            LCD->SEGD2H = (LCD->SEGD2H&~tablcdclear[com][pos].hi)|lcd[com].hi;
            break;
        case 3:
            LCD->SEGD3L = (LCD->SEGD3L&~tablcdclear[com][pos].lo)|lcd[com].lo;
            LCD->SEGD3H = (LCD->SEGD3H&~tablcdclear[com][pos].hi)|lcd[com].hi;
            break;
        case 4:
            LCD->SEGD4L = (LCD->SEGD4L&~tablcdclear[com][pos].lo)|lcd[com].lo;
            LCD->SEGD4H = (LCD->SEGD4H&~tablcdclear[com][pos].hi)|lcd[com].hi;
            break;
        case 5:
            LCD->SEGD5L = (LCD->SEGD5L&~tablcdclear[com][pos].lo)|lcd[com].lo;
            LCD->SEGD5H = (LCD->SEGD5H&~tablcdclear[com][pos].hi)|lcd[com].hi;
            break;
        case 6:
            LCD->SEGD6L = (LCD->SEGD6L&~tablcdclear[com][pos].lo)|lcd[com].lo;
            LCD->SEGD6H = (LCD->SEGD6H&~tablcdclear[com][pos].hi)|lcd[com].hi;
            break;
        case 7:
            LCD->SEGD7L = (LCD->SEGD7L&~tablcdclear[com][pos].lo)|lcd[com].lo;
            LCD->SEGD7H = (LCD->SEGD7H&~tablcdclear[com][pos].hi)|lcd[com].hi;
            break;
        }
    }

#else
    (void) seg;
    (void) com;
    (void) tablcdclear;
#endif


}


/**
 *  @brief  Write a string into positions 1 to 7 (with 14 segments display)
 *
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
 *  @brief  Write a digit to positions 8 to 11 (with 7 segments displays)
 *
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

}


void LCD_Clear(void) {

}
