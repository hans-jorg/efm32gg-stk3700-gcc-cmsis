/**
 * @file    lcd.c
 * @brief   LCD HAL for EFM32GG STK3700
 * @version 1.0
 *
 * @note    To debug, there is a LCD_EMULATION compiler flag. By defining it, all writes of
 *          segments are directed to a 8x2 array in memory
 *
 * @note    There are at least two segment numbering methods. One of them uses the LCD device
 *          numbering. The other uses the LCD controller ()in the microcontroller) numbering.
 *          The same for common numbering. The numbering of the LCD device is the reverse of the
 *          numbering used in the LCD controller.
 *
 * @note    This implementation uses a two step approch: First a table is used to
 *          get the segments to be lit (from 7 and 14 bit displays), then for each segment the
 *          display segment and correspoding common numbers are found and finally another table
 *          is used to get the controller segment.
 *
 * @note    The display is divided in fields:
 *          Positions 1 to 7 correspond to the alphanumeric (14 segment) display
 *          Positions 8 to 11 correspond to the numeric (7 segment) display
 *          Position 12 to the pizza display (9 values, from 0 to 8)
 *          Position 13 to the battery level display (4 values, from 0 to 3)
 *          Position 14 to the target display (6 values from 0 to 4)
 *
 *
 * LCD Map (using display segment numbering)
 *
 * Seg   S0   S1  S2  S3  S4  S5  S6  S7  S8  S9  S10  S11 S12 S13 S14 S15 S16 S17 S18 S19
 * COM0  DP2  1E  1D  2E  2D  3E  3D  4E  4D  DP5 5D   DP6 6D  7E  7D  11A 10A 9A  8A  EM2
 * COM1  DP4  1Q  1N  2Q  2N  3Q  3N  4Q  4N  5E  5N   6E  6N  7Q  7N  11F 10F 9F  8F  EM4
 * COM2  DP3  1P  1C  2P  2C  3P  3C  4P  4C  5Q  5C   6Q  6C  7P  7C  11B 10B 9B  8B  COL10
 * COM3  COL3 1G  1M  2G  2M  3G  3M  4G  4M  5P  5M   6P  6M  7G  7M  11G 10G 9G  8G  DP10
 * COM4  MIN  1F  1J  2F  2J  3F  3J  4F  4J  5G  5J   6G  6J  7F  7J  11E 10E 9E  8E  PAD0
 * COM5  PAD1 1H  1K  2H  2K  3H  3K  4H  4K  5F  5K   6F  6K  7H  7K  11C 10C 9C  8C  EM3
 * COM6  GEK  1A  1B  2A  2B  3A  3B  4A  4B  5H  5B   6H  6B  7A  7B  11D 10D 9D  8D  EM1
 * COM7  A7   A6  A5  A4  A3  A2  A1  A0  EFM 5A  COL5 6A  ANT BAT .C  .F  B1  B0  B2  EM0
 *
 *
 * Controller pin Map
 *
 * Display Seg#          0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19
 * Controller Seg#      12 13 14 15 16 17 18 19 28 29 30 31 32 33 34 35 36 37 38 39
 *
 */

//#define LCD_EMULATION


#include  <stdint.h>
#include "lcd.h"

#ifndef LCD_EMULATION
#include "em_device.h"
#endif

/**
 * @brief  BIT macros
 * @note   generates a 1 bit in the N position
 * @note   BIT32 truncates, ie. BIT32(33) == BIT(1)
 */
#define BIT(N) (1U<<(N))
#define BITLONG(N) (1UL<<(N))
#define BITTRUNCATED(N) ( (N)>=32?(1UL<<((N)-32)):(1UL<<(N)) )
#define BIT32(N) (1UL<<((N)%32))


/**
 * @brief Macros to control Freeze/Unfreeze
 * @note  Can be overridden for debugging
 */
#ifdef DO_NOT_USE_FREEZE
#define LCD_FREEZE
#define LCD_UNFREEZE
#else
#define LCD_FREEZE   LCD->FREEZE |=  LCD_FREEZE_REGFREEZE
#define LCD_UNFREEZE LCD->FREEZE &= ~LCD_FREEZE_REGFREEZE
#endif

/**
 *  @brief  struct to hold information about segment
 *
 *  @note   used two 32-bit words to simplify operations with the segment registers
 *  @note   an alternative would be the use of 64 bit words
 */
typedef struct {
    uint32_t    hi;
    uint32_t    lo;
} SegEncoding_t;


/**
 * Flag. When set, LCD clock is initialized.
 */
static uint8_t lcdclock_set = 0;

/**
 *  @brief  Table for 14 segments displays
 *
 *  @note   For each character, it gives the segments to be lit
 *  @note   It uses a 16 bit word and each bit corresponds to a segment
 *  @note   0 index corresponds to space (=0x20=32)
 *  @note   To get the segments for character c use segments14forchar[c-' ']
 */

// bit-segment association
#define _DP 0x4000
#define _Q  0x2000
#define _P  0x1000
#define _N  0x800
#define _M  0x400
#define _K  0x200
#define _J  0x100
#define _H  0x80
#define _G  0x40
#define _F  0x20
#define _E  0x10
#define _D  0x8
#define _C  0x4
#define _B  0x2
#define _A  0x1

static const uint16_t segments14forchar[96] = {
0                                                                ,  /* SPC */
      _B |_C                                                 |_DP,  /*  ! */
      _B                         |_J                             ,  /*  " */
      _B |_C |_D         |_G     |_J         |_M     |_P         ,  /*  # */
  _A     |_C |_D     |_F |_G     |_J         |_M     |_P         ,  /*  $ */
          _C         |_F |_G |_H |_J |_K     |_M |_N |_P |_Q     ,  /*  % */
  _A         |_D |_E     |_G |_H |_J             |_N             ,  /*  & */
                                  _J                             ,  /*  ' */
                                      _K         |_N             ,  /*  ( */
                              _H                         |_Q     ,  /*  ) */
                          _G |_H |_J |_K     |_M |_N |_P |_Q     ,  /*  * */
                          _G     |_J         |_M     |_P         ,  /*  + */
                                                          _Q     ,  /*  , */
                          _G                 |_M                 ,  /*  - */
                                                              _DP,  /*  . */
                                      _K                 |_Q     ,  /*  / */
  _A |_B |_C |_D |_E |_F             |_K                 |_Q     ,  /*  0 */
      _B |_C                         |_K                         ,  /*  1 */
  _A |_B     |_D |_E     |_G                 |_M                 ,  /*  2 */
  _A |_B |_C |_D                             |_M                 ,  /*  3 */
      _B |_C         |_F |_G                 |_M                 ,  /*  4 */
  _A         |_D     |_F |_G                     |_N             ,  /*  5 */
  _A     |_C |_D |_E |_F |_G                 |_M                 ,  /*  6 */
  _A |_B |_C                                                     ,  /*  7 */
  _A |_B |_C |_D |_E |_F |_G                 |_M                 ,  /*  8 */
  _A |_B |_C |_D     |_F |_G                 |_M                 ,  /*  9 */
                                  _J                 |_P         ,  /*  : */
                                  _J                     |_Q     ,  /*  ; */
                          _G         |_K         |_N             ,  /*  < */
              _D         |_G                 |_M                 ,  /*  = */
                              _H             |_M         |_Q     ,  /*  > */
  _A |_B                                     |_M     |_P     |_DP,  /*  ? */
  _A |_B     |_D |_E |_F         |_J         |_M                 ,  /*  @ */
  _A |_B |_C     |_E |_F |_G                 |_M                 ,  /*  A */
  _A |_B |_C |_D                 |_J         |_M     |_P         ,  /*  B */
  _A         |_D |_E |_F                                         ,  /*  C */
  _A |_B |_C |_D                 |_J                 |_P         ,  /*  D */
  _A         |_D |_E |_F |_G                                     ,  /*  E */
  _A             |_E |_F |_G                                     ,  /*  F */
  _A     |_C |_D |_E |_F                     |_M                 ,  /*  G */
      _B |_C     |_E |_F |_G                 |_M                 ,  /*  H */
  _A         |_D                 |_J                 |_P         ,  /*  I */
      _B |_C |_D |_E                                             ,  /*  J */
                  _E |_F |_G         |_K         |_N             ,  /*  K */
              _D |_E |_F                                         ,  /*  L */
      _B |_C     |_E |_F     |_H     |_K                         ,  /*  M */
      _B |_C     |_E |_F     |_H                 |_N             ,  /*  N */
  _A |_B |_C |_D |_E |_F                                         ,  /*  O */
  _A |_B         |_E |_F |_G                 |_M                 ,  /*  P */
  _A |_B |_C |_D |_E |_F                         |_N             ,  /*  Q */
  _A |_B         |_E |_F |_G                 |_M |_N             ,  /*  R */
  _A     |_C |_D     |_F |_G                 |_M                 ,  /*  S */
  _A                             |_J                 |_P         ,  /*  T */
      _B |_C |_D |_E |_F                                         ,  /*  U */
                  _E |_F             |_K                 |_Q     ,  /*  V */
      _B |_C     |_E |_F                         |_N     |_Q     ,  /*  W */
                              _H     |_K         |_N     |_Q     ,  /*  X */
      _B |_C |_D     |_F |_G                 |_M                 ,  /*  Y */
  _A         |_D                     |_K                 |_Q     ,  /*  Z */
  _A         |_D |_E |_F                                         ,  /*  [ */
                              _H                 |_N             ,  /*  \ */
  _A |_B |_C |_D                                                 ,  /*  ] */
                                                  _N     |_Q     ,  /*  ^ */
              _D                                                 ,  /*  _ */
                              _H                                 ,  /*  ` */
              _D |_E     |_G                         |_P         ,  /*  a */
              _D |_E |_F |_G                     |_N             ,  /*  b */
              _D |_E     |_G                 |_M                 ,  /*  c */
      _B |_C |_D                             |_M         |_Q     ,  /*  d */
              _D |_E     |_G                             |_Q     ,  /*  e */
                          _G         |_K     |_M     |_P         ,  /*  f */
      _B |_C |_D                     |_K     |_M                 ,  /*  g */
                  _E |_F |_G                         |_P         ,  /*  h */
                                                      _P         ,  /*  i */
                  _E             |_J                     |_Q     ,  /*  j */
                                  _J |_K         |_N |_P         ,  /*  k */
                  _E |_F                                         ,  /*  l */
          _C     |_E     |_G                 |_M     |_P         ,  /*  m */
                  _E     |_G                         |_P         ,  /*  n */
          _C |_D |_E     |_G                 |_M                 ,  /*  o */
                  _E |_F |_G |_H                                 ,  /*  p */
      _B |_C                         |_K     |_M                 ,  /*  q */
                  _E     |_G                                     ,  /*  r */
              _D                             |_M |_N             ,  /*  s */
              _D |_E |_F |_G                                     ,  /*  t */
          _C |_D |_E                                             ,  /*  u */
                  _E                                     |_Q     ,  /*  v */
          _C     |_E                             |_N     |_Q     ,  /*  w */
                              _H     |_K         |_N     |_Q     ,  /*  x */
      _B |_C |_D                 |_J         |_M                 ,  /*  y */
              _D         |_G                             |_Q     ,  /*  z */
  _A         |_D         |_G |_H                         |_Q     ,  /*  { */
                                  _J                 |_P         ,  /*  | */
  _A         |_D                     |_K     |_M |_N             ,  /*  } */
                          _G         |_K     |_M         |_Q     ,  /*  ~ */
0                                                                   /* DEL */
};


/**
 *  @brief  Table for 7 segments displays
 *
 *  @note   Only used when trying to display alphabetic characters in the numeric display
 *  @note   For each character, it gives the segments to be lit
 *  @note   It uses a 16 bit word and each bit corresponds to a segment
 *  @note   0 position corresponds to space (=0x20=32)
 *  @note   To get the segments for character c use segments7forchar[c-' ']
 */

// bit-segment association. Symbols _A,_B,...,_G,_DP are reused!!!!

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

static const uint8_t segments7forchar[96] = {
0                                    ,  /* SPC */
      _B |_C                 |_DP    ,  /*  ! */
      _B             |_F             ,  /*  " */
      _B |_C |_D |_E |_F |_G         ,  /*  # */
  _A     |_C |_D     |_F |_G         ,  /*  $ */
      _B         |_E     |_G |_DP    ,  /*  % */
      _B |_C             |_G         ,  /*  & */
                      _F             ,  /*  ' */
  _A         |_D     |_F             ,  /*  ( */
  _A |_B     |_D                     ,  /*  ) */
  _A                 |_F             ,  /*  * */
                  _E |_F |_G         ,  /*  + */
                  _E                 ,  /*  , */
                          _G         ,  /*  - */
                              _DP    ,  /*  . */
      _B         |_E     |_G         ,  /*  / */
  _A |_B |_C |_D |_E |_F             ,  /*  0 */
      _B |_C                         ,  /*  1 */
  _A |_B     |_D |_E     |_G         ,  /*  2 */
  _A |_B |_C |_D         |_G         ,  /*  3 */
      _B |_C         |_F |_G         ,  /*  4 */
  _A     |_C |_D     |_F |_G         ,  /*  5 */
  _A     |_C |_D |_E |_F |_G         ,  /*  6 */
  _A |_B |_C                         ,  /*  7 */
  _A |_B |_C |_D |_E |_F |_G         ,  /*  8 */
  _A |_B |_C |_D     |_F |_G         ,  /*  9 */
  _A         |_D                     ,  /*  : */
  _A     |_C |_D                     ,  /*  ; */
  _A                 |_F |_G         ,  /*  < */
              _D         |_G         ,  /*  = */
  _A |_B                 |_G         ,  /*  > */
  _A |_B         |_E     |_G |_DP    ,  /*  ? */
  _A |_B |_C |_D |_E     |_G         ,  /*  @ */
  _A |_B |_C     |_E |_F |_G         ,  /*  A */
          _C |_D |_E |_F |_G         ,  /*  B */
  _A         |_D |_E |_F             ,  /*  C */
      _B |_C |_D |_E     |_G         ,  /*  D */
  _A         |_D |_E |_F |_G         ,  /*  E */
  _A             |_E |_F |_G         ,  /*  F */
  _A     |_C |_D |_E |_F             ,  /*  G */
      _B |_C     |_E |_F |_G         ,  /*  H */
                  _E |_F             ,  /*  I */
      _B |_C |_D |_E                 ,  /*  J */
  _A     |_C     |_E |_F |_G         ,  /*  K */
              _D |_E |_F             ,  /*  L */
  _A     |_C     |_E                 ,  /*  M */
  _A |_B |_C     |_E |_F             ,  /*  N */
  _A |_B |_C |_D |_E |_F             ,  /*  O */
  _A |_B         |_E |_F |_G         ,  /*  P */
  _A |_B     |_D     |_F |_G         ,  /*  Q */
  _A |_B         |_E |_F             ,  /*  R */
  _A     |_C |_D     |_F |_G         ,  /*  S */
              _D |_E |_F |_G         ,  /*  T */
      _B |_C |_D |_E |_F             ,  /*  U */
      _B |_C |_D |_E |_F             ,  /*  V */
      _B     |_D     |_F             ,  /*  W */
      _B |_C     |_E |_F |_G         ,  /*  X */
      _B |_C |_D     |_F |_G         ,  /*  Y */
  _A |_B     |_D |_E     |_G         ,  /*  Z */
  _A         |_D |_E |_F             ,  /*  [ */
          _C         |_F |_G         ,  /*  \ */
  _A |_B |_C |_D                     ,  /*  ] */
  _A |_B             |_F             ,  /*  ^ */
              _D                     ,  /*  _ */
      _B                             ,  /*  ` */
  _A |_B |_C |_D |_E     |_G         ,  /*  a */
          _C |_D |_E |_F |_G         ,  /*  b */
              _D |_E     |_G         ,  /*  c */
      _B |_C |_D |_E     |_G         ,  /*  d */
  _A |_B     |_D |_E |_F |_G         ,  /*  e */
  _A             |_E |_F |_G         ,  /*  f */
  _A |_B |_C |_D     |_F |_G         ,  /*  g */
          _C     |_E |_F |_G         ,  /*  h */
                  _E                 ,  /*  i */
          _C |_D                     ,  /*  j */
  _A     |_C     |_E |_F |_G         ,  /*  k */
                  _E |_F             ,  /*  l */
          _C     |_E                 ,  /*  m */
          _C     |_E     |_G         ,  /*  n */
          _C |_D |_E     |_G         ,  /*  o */
  _A |_B         |_E |_F |_G         ,  /*  p */
  _A |_B |_C         |_F |_G         ,  /*  q */
                  _E     |_G         ,  /*  r */
  _A     |_C |_D     |_F |_G         ,  /*  s */
              _D |_E |_F |_G         ,  /*  t */
          _C |_D |_E                 ,  /*  u */
          _C |_D |_E                 ,  /*  v */
          _C     |_E                 ,  /*  w */
      _B |_C     |_E |_F |_G         ,  /*  x */
      _B |_C |_D     |_F |_G         ,  /*  y */
  _A |_B     |_D |_E     |_G         ,  /*  z */
      _B |_C             |_G         ,  /*  { */
                  _E |_F             ,  /*  | */
                  _E |_F |_G         ,  /*  } */
  _A                                 ,  /*  ~ */
0                                       /* DEL */
};


/**
  *
  *  @brief  Configuration for LCD Segments
  *
  *  @note   There are two segment numbering schemas.
  *          One in the display and the other in the LCD controller
  *
  */

//
// LCD Matriz 20x8
//
//
//
//     Display Seg#  Display Pin Controller Seg#        MCU Port      MCU Pin
//        S0          1             Seg12      <--->    PA15           B1
//        S1          2             Seg13      <--->    PA0            C2
//        S2          3             Seg14      <--->    PA1            C1
//        S3          4             Seg15      <--->    PA2            D2
//        S4          5             Seg16      <--->    PA3            D1
//        S5          6             Seg17      <--->    PA4            E3
//        S6          7             Seg18      <--->    PA5            E2
//        S7          8             Seg19      <--->    PA6            E1
//        S8          9             Seg28      <--->    PD9            D6
//        S9         10             Seg29      <--->    PD10           A5
//        S10        11             Seg30      <--->    PD11           B5
//        S11        12             Seg31      <--->    PD12           C5
//        S12        13             Seg32      <--->    PB0            E4
//        S13        14             Seg33      <--->    PB1            F1
//        S14        15             Seg34      <--->    PB2            F2
//        S15        16             Seg35      <--->    PA7            H4
//        S16        17             Seg36      <--->    PA8            H5
//        S17        18             Seg37      <--->    PA9            J5
//        S18        19             Seg38      <--->    PA10           J6
//        S19        20             Seg39      <--->    PA11           K5
//        COM7       21             Com 0      <--->    PB6            G2
//        COM6       22             Com 1      <--->    PB5            G1
//        COM5       23             Com 2      <--->    PB4            F4
//        COM4       24             Com 3      <--->    PB3            F3
//        COM3       25             Com 4      <--->    PE7            D9
//        COM2       26             Com 5      <--->    PE6            C9
//        COM1       27             Com 6      <--->    PE5            B9
//        COM0       28             Com 7      <--->    PD4            J11
//
//                                  CAP+                PA12           J4
//                                  CAP-                PA13           K3
//                                  EXT                 PA14           L3
//


/*
 *  Segment information is encoded in a 16 bit word
 *  Each segment in the display is addressed by a segment signal AND a common signal
 *  The high order byte contains the segment number
 *  The lower order byte containts the common number
 */
#define SEGS    20
#define COMMS   8

#define SEGMASK(X) ((X)<<8)
#define GET_SEG(M) ((M)>>8)
#define GET_COMMON(M) ((M)&0xFF)



/**
 *  The display segment number is determined, and then the controller segment number is found.
 *  LCD Segment Pin and Common Pin are found in lookup tables seg_encoding and com_encoding
 *
 * S00-S19 symbols specifies the display segment number
 * C0-C7 specifies the display common number
 */
//{
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


/*
 * @brief table for LCD segments adjusted por position
 *
 * @note  It returns the information needed to turn on a LCD segment knowing its position
 * @note  tablcd[x][y] returns a 16 bit value, with segment and common number information
 *        to turn the display segment x (A-Q,DP) in position y
 * @note  Segment A corresponds to 0, B to 1 and so on
 * @note  It is used by one step encoding and two step encoding
 * @note   Position 1 to 7:  alphanumeric (14 segment) displays
 * @note   Position 8 to 11: numeric (7 segment) displays
 * @note   Position 12:      pizza display
 * @note   Position 13:      battery level display
 * @note   Position 14:      target display
 * @note   Invalid combinations returns C0|S0 -> DP2
 */
static const uint16_t tablcd[15][15] = {
//          P00/8   P01/9   P02/10  P03/11  P04/12  P05/13  P06/14  P07
/* A  */ { C7|S12, C6|S01, C6|S03, C6|S05, C6|S07, C7|S09, C7|S11, C6|S13,
           C0|S18, C0|S17, C0|S16, C0|S15, C7|S07, C7|S18, C7|S19 },
/* B  */ { C7|S13, C6|S02, C6|S04, C6|S06, C6|S08, C6|S10, C6|S12, C6|S14,
           C2|S18, C2|S17, C2|S16, C2|S15, C7|S06, C7|S17, C6|S19 },
/* C  */ { C7|S14, C2|S02, C2|S04, C2|S06, C2|S08, C2|S10, C2|S12, C2|S14,
           C5|S18, C5|S17, C5|S16, C5|S15, C7|S05, C7|S16, C0|S19 },
/* D  */ { C7|S15, C0|S02, C0|S04, C0|S06, C0|S08, C0|S10, C0|S12, C0|S14,
           C6|S18, C6|S17, C6|S16, C6|S15, C7|S04, C0|S00, C5|S19 },
/* E  */ { C6|S00, C0|S01, C0|S03, C0|S05, C0|S07, C1|S09, C1|S11, C0|S13,
           C4|S18, C4|S17, C4|S16, C4|S15, C7|S03, C0|S00, C1|S19 },
/* F  */ { C7|S17, C4|S01, C4|S03, C4|S05, C4|S07, C5|S09, C5|S11, C4|S13,
           C1|S18, C1|S17, C1|S16, C1|S15, C7|S02, C0|S00, C0|S00 },
/* G  */ { C7|S16, C3|S01, C3|S03, C3|S05, C3|S07, C4|S09, C4|S11, C3|S13,
           C3|S18, C3|S17, C3|S16, C3|S15, C7|S01, C0|S00, C0|S00 },
/* H  */ { C7|S18, C5|S01, C5|S03, C5|S05, C5|S07, C6|S09, C6|S11, C5|S13,
           C7|S19, C3|S00, C0|S00, C7|S07, C7|S00, C0|S00, C0|S00 },
/* J  */ { C0|S00, C4|S02, C4|S04, C4|S06, C4|S08, C4|S10, C4|S12, C4|S14,
           C6|S19, C0|S00, C0|S00, C7|S06, C0|S00, C0|S00, C0|S00 },
/* K  */ { C4|S19, C5|S02, C5|S04, C5|S06, C5|S08, C5|S10, C5|S12, C5|S14,
           C0|S19, C7|S10, C0|S00, C7|S05, C0|S00, C0|S00, C0|S00 },
/* M  */ { C5|S00, C3|S02, C3|S04, C3|S06, C3|S08, C3|S10, C3|S12, C3|S14,
           C5|S19, C2|S19, C0|S00, C7|S04, C0|S00, C0|S00, C0|S00 },
/* N  */ { C0|S00, C1|S02, C1|S04, C1|S06, C1|S08, C1|S10, C1|S12, C1|S14,
           C1|S19, C0|S00, C0|S00, C7|S03, C0|S00, C0|S00, C0|S00 },
/* P  */ { C0|S00, C2|S01, C2|S03, C2|S05, C2|S07, C3|S09, C3|S11, C2|S13,
           C0|S00, C0|S00, C0|S00, C7|S02, C0|S00, C0|S00, C0|S00 },
/* Q  */ { C0|S00, C1|S01, C1|S03, C1|S05, C1|S07, C2|S09, C2|S11, C1|S13,
           C4|S00, C0|S00, C0|S00, C7|S01, C0|S00, C0|S00, C0|S00 },
/* DP */ { C0|S00, C0|S00, C0|S00, C2|S00, C1|S00, C0|S09, C0|S11, C0|S00,
           C7|S08, C0|S00, C3|S19, C7|S00, C0|S00, C0|S00, C0|S00 }
};


/*
 * @brief table to find the encoding for a specified segment
 *
 * @note
 * @note
 */
static const SegEncoding_t seg_encoding[] = {
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
    {   BIT32(32),   0x00000000 },  //    S12      ->      Seg 32    SEGEN8
    {   BIT32(33),   0x00000000 },  //    S13      ->      Seg 33    SEGEN8
    {   BIT32(34),   0x00000000 },  //    S14      ->      Seg 34    SEGEN8
    {   BIT32(35),   0x00000000 },  //    S15      ->      Seg 35    SEGEN8
    {   BIT32(36),   0x00000000 },  //    S16      ->      Seg 36    SEGEN9
    {   BIT32(37),   0x00000000 },  //    S17      ->      Seg 37    SEGEN9
    {   BIT32(38),   0x00000000 },  //    S18      ->      Seg 38    SEGEN9
    {   BIT32(39),   0x00000000 },  //    S19      ->      Seg 39    SEGEN9
};

/**
 * @brief common pins encoding
 * @note  they are reversed
 */

static const uint8_t com_encoding[8] = { 7, 6, 5, 4, 3, 2, 1, 0 };



/**
 * @brief table for special characters
 * @note  the corresponding segment and common numbers are compressed into a 16 bit data
 *
 */
static const uint16_t tablcdspecial[] = {
    C6|S00,       // LCD_GECKO
    C4|S00,       // LCD_MINUS
    C4|S19,       // LCD_PAD0
    C5|S00,       // LCD_PAD1
    C7|S12,       // LCD_ANTENNA
    C7|S09,       // LCD_EMF32
    C7|S13,       // LCD_BATTERY
    C7|S17,       // LCD_BAT0
    C7|S16,       // LCD_BAT1
    C7|S18,       // LCD_BAT2
    C7|S07,       // LCD_ARC0
    C7|S06,       // LCD_ARC1
    C7|S05,       // LCD_ARC2
    C7|S04,       // LCD_ARC3
    C7|S03,       // LCD_ARC4
    C7|S02,       // LCD_ARC5
    C7|S01,       // LCD_ARC6
    C7|S00,       // LCD_ARC7
    C7|S19,       // LCD_TARGET0
    C6|S19,       // LCD_TARGET1
    C0|S19,       // LCD_TARGET2
    C5|S19,       // LCD_TARGET3
    C1|S19,       // LCD_TARGET4
    C7|S14,       // LCD_C
    C7|S15,       // LCD_F
    C3|S00,       // LCD_COLLON3
    C7|S10,       // LCD_COLLON5
    C2|S19,       // LCD_COLLON10
    C0|S00,       // LCD_DP2
    C2|S00,       // LCD_DP3
    C1|S00,       // LCD_DP4
    C0|S09,       // LCD_DP5
    C0|S11,       // LCD_DP6
    C3|S19        // LCD_DP10
};


/**
 * @brief  Table for clearing a position before writing a new character onto it
 * @note   Indexed by position and common
 * @note   Returns controller segments info
 * @note   Position 1 to 7:  alphanumeric (14 segment) displays
 * @note   Position 8 to 11: numeric (7 segment) displays
 * @note   Position 12:      pizza display
 * @note   Position 13:      battery level display
 * @note   Position 14:      target display
 */

static const SegEncoding_t tablcdclear[15][8] = {
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
    /* Position  P01 - 14 segment display  */
    {
        /* C0  */       {   0, BIT32(13)|BIT32(14) },           // Segments E and D
        /* C1  */       {   0, BIT32(13)|BIT32(14) },           // Segments Q and N
        /* C2  */       {   0, BIT32(13)|BIT32(14) },           // Segments P and C
        /* C3  */       {   0, BIT32(13)|BIT32(14) },           // Segments G and M
        /* C4  */       {   0, BIT32(13)|BIT32(14) },           // Segments F and J
        /* C5  */       {   0, BIT32(13)|BIT32(14) },           // Segments H and K
        /* C6  */       {   0, BIT32(13)|BIT32(14) },           // Segments A and B
        /* C7  */       {   0, 0 }                              //
    },
    /* Position  P02 - 14 segment display  */
    {
        /* C0  */       {   0, BIT32(15)|BIT32(16) },           // Segments E and D
        /* C1  */       {   0, BIT32(15)|BIT32(16) },           // Segments Q and N
        /* C2  */       {   0, BIT32(15)|BIT32(16) },           // Segments P and C
        /* C3  */       {   0, BIT32(15)|BIT32(16) },           // Segments G and M
        /* C4  */       {   0, BIT32(15)|BIT32(16) },           // Segments F and J
        /* C5  */       {   0, BIT32(15)|BIT32(16) },           // Segments H and K
        /* C6  */       {   0, BIT32(15)|BIT32(16) },           // Segments A and B
        /* C7  */       {   0, 0 }                              //
    },
    /* Position  P03 - 14 segment display  */
    {
        /* C0  */       {   0, BIT32(17)|BIT32(18) },           // Segments E and D
        /* C1  */       {   0, BIT32(17)|BIT32(18) },           // Segments Q and N
        /* C2  */       {   0, BIT32(17)|BIT32(18) },           // Segments P and C
        /* C3  */       {   0, BIT32(17)|BIT32(18) },           // Segments G and M
        /* C4  */       {   0, BIT32(17)|BIT32(18) },           // Segments F and J
        /* C5  */       {   0, BIT32(17)|BIT32(18) },           // Segments H and K
        /* C6  */       {   0, BIT32(17)|BIT32(18) },           // Segments A and B
        /* C7  */       {   0, 0 }                              //
    },
    /* Position  P04 - 14 segment display  */
    {
        /* C0  */       {   0, BIT32(19)|BIT32(28) },           // Segments E and D
        /* C1  */       {   0, BIT32(19)|BIT32(28) },           // Segments Q and N
        /* C2  */       {   0, BIT32(19)|BIT32(28) },           // Segments P and C
        /* C3  */       {   0, BIT32(19)|BIT32(28) },           // Segments G and M
        /* C4  */       {   0, BIT32(19)|BIT32(28) },           // Segments F and J
        /* C5  */       {   0, BIT32(19)|BIT32(28) },           // Segments H and K
        /* C6  */       {   0, BIT32(19)|BIT32(28) },           // Segments A and B
        /* C7  */       {   0, 0 }                              //
    },
    /* Position  P05 - 14 segment display  */
    {
        /* C0  */       {   0, BIT32(30)           },           // Segment D
        /* C1  */       {   0, BIT32(29)|BIT32(30) },           // Segments E and N
        /* C2  */       {   0, BIT32(29)|BIT32(30) },           // Segments Q and C
        /* C3  */       {   0, BIT32(29)|BIT32(30) },           // Segments P and M
        /* C4  */       {   0, BIT32(29)|BIT32(30) },           // Segments G and J
        /* C5  */       {   0, BIT32(29)|BIT32(30) },           // Segments F and K
        /* C6  */       {   0, BIT32(29)|BIT32(30) },           // Segments H and B
        /* C7  */       {   0, BIT32(29)           }            // Segment A
    },
    /* Position  P06 - 14 segment display  */
    {
        /* C0  */       {   BIT32(32), 0           },           // Segment D
        /* C1  */       {   BIT32(32), BIT32(31)   },           // Segments E and N
        /* C2  */       {   BIT32(32), BIT32(31)   },           // Segments Q and C
        /* C3  */       {   BIT32(32), BIT32(31)   },           // Segments P and M
        /* C4  */       {   BIT32(32), BIT32(31)   },           // Segments G and J
        /* C5  */       {   BIT32(32), BIT32(31)   },           // Segments F and K
        /* C6  */       {   BIT32(32), BIT32(31)   },           // Segments H and B
        /* C7  */       {   0,         BIT32(31)   }            // Segment A

    },
    /* Position  P07 - 14 segment display */
    {
        /* C0  */       {   BIT32(33)|BIT32(34), 0 },           // Segments E and D
        /* C1  */       {   BIT32(33)|BIT32(34), 0 },           // Segments Q and N
        /* C2  */       {   BIT32(33)|BIT32(34), 0 },           // Segments P and C
        /* C3  */       {   BIT32(33)|BIT32(34), 0 },           // Segments G and M
        /* C4  */       {   BIT32(33)|BIT32(34), 0 },           // Segments F and J
        /* C5  */       {   BIT32(33)|BIT32(34), 0 },           // Segments H and K
        /* C6  */       {   BIT32(33)|BIT32(34), 0 },           // Segments A and B
        /* C7  */       {   0, 0                   }            //
    },
    /* Position  P08 - 14 segment display  */
    {
        /* C0  */       {  BIT32(38),0             },           // Segment A
        /* C1  */       {  BIT32(38),0             },           // Segment F
        /* C2  */       {  BIT32(38),0             },           // Segment B
        /* C3  */       {  BIT32(38),0             },           // Segment G
        /* C4  */       {  BIT32(38),0             },           // Segment E
        /* C5  */       {  BIT32(38),0             },           // Segment C
        /* C6  */       {  BIT32(38),0             },           // Segment D
        /* C7  */       {  0, 0                    }
    },
    /* Position  P09 - 7 segment display  */
    {
        /* C0  */       {  BIT32(37),0             },           // Segment A
        /* C1  */       {  BIT32(37),0             },           // Segment F
        /* C2  */       {  BIT32(37),0             },           // Segment B
        /* C3  */       {  BIT32(37),0             },           // Segment G
        /* C4  */       {  BIT32(37),0             },           // Segment E
        /* C5  */       {  BIT32(37),0             },           // Segment C
        /* C6  */       {  BIT32(37),0             },           // Segment D
        /* C7  */       {   0, 0                   }
    },
    /* Position  P10 - 7 segment display   */
    {
        /* C0  */       {  BIT32(36),0             },           // Segment A
        /* C1  */       {  BIT32(36),0             },           // Segment F
        /* C2  */       {  BIT32(36),0             },           // Segment B
        /* C3  */       {  BIT32(36),0             },           // Segment G
        /* C4  */       {  BIT32(36),0             },           // Segment E
        /* C5  */       {  BIT32(36),0             },           // Segment C
        /* C6  */       {  BIT32(36),0             },           // Segment D
        /* C7  */       {   0, 0                   }
    },
    /* Position  P11 - 7 segment display   */
    {
        /* C0  */       {  BIT32(35),0             },           // Segment A
        /* C1  */       {  BIT32(35),0             },           // Segment F
        /* C2  */       {  BIT32(35),0             },           // Segment B
        /* C3  */       {  BIT32(35),0             },           // Segment G
        /* C4  */       {  BIT32(35),0             },           // Segment E
        /* C5  */       {  BIT32(35),0             },           // Segment C
        /* C6  */       {  BIT32(35),0             },           // Segment D
        /* C7  */       {   0, 0                   }
    },
    /* Position  P12 - 8 segments pizza display   */
    {
        /* C0  */       {  0,0                     },           //
        /* C1  */       {  0,0                     },           //
        /* C2  */       {  0,0                     },           //
        /* C3  */       {  0,0                     },           //
        /* C4  */       {  0,0                     },           //
        /* C5  */       {  0,0                     },           //
        /* C6  */       {  0,0                     },           //
        /* C7  */       {  0,BIT(19)|BIT(18)|BIT(17)            // Segments A0, A1, A2
                            |BIT(16)|BIT(15)|BIT(14)            // Segments A3, A4, A5
                            |BIT(13)|BIT(12)       }            // Segments A6, A7
    },
    /* Position  P13 - 3 levels battery display   */
    {
        /* C0  */       {  0,0                     },           //
        /* C1  */       {  0,0                     },           //
        /* C2  */       {  0,0                     },           //
        /* C3  */       {  0,0                     },           //
        /* C4  */       {  0,0                     },           //
        /* C5  */       {  0,0                     },           //
        /* C6  */       {  0,0                     },           //
        /* C7  */       {  BIT32(38)|BIT32(37)                  // Segment B2, B0
                          |BIT32(36),0             }            // Segment B1
    },
    /* Position  P14 - 8 segments target display   */
    {
        /* C0  */       {  BIT32(39),0             },           // Segment EM2
        /* C1  */       {  BIT32(39),0             },           // Segment EM4
        /* C2  */       {  0,0                     },           //
        /* C3  */       {  0,0                     },           //
        /* C4  */       {  0,0                     },           //
        /* C5  */       {  BIT32(39),0             },           // Segment EM3
        /* C6  */       {  BIT32(39),0             },           // Segment EM1
        /* C7  */       {  BIT32(39),0             }            // Segment EM0
    },
};

/**
 *  @brief This array emulates the LCD data registers for debugging
 */
#ifdef LCD_EMULATION
typedef struct {
    uint32_t hi;
    uint32_t lo;
} lcddata_t;

lcddata_t lcd[8];
#endif


/**
 * @brief pointer to segment data registers
 */
//{
volatile uint32_t  * const segdatalow[8] = {
    &(LCD->SEGD0L),&(LCD->SEGD1L),&(LCD->SEGD2L),&(LCD->SEGD3L),
    &(LCD->SEGD4L),&(LCD->SEGD5L),&(LCD->SEGD6L),&(LCD->SEGD7L)
    };
volatile uint32_t  * const segdatahigh[8] = {
    &(LCD->SEGD0H),&(LCD->SEGD1H),&(LCD->SEGD2H),&(LCD->SEGD3H),
    &(LCD->SEGD4H),&(LCD->SEGD5H),&(LCD->SEGD6H),&(LCD->SEGD7H)
    };
// To simplify assignements
#define SEGDATALOW  *segdatalow
#define SEGDATAHIGH *segdatahigh
//}

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
    CMU->LFACLKEN0 = oldlfaclken0|CMU_LFACLKEN0_LCD;

    /* Set flag to signalize clock is set */
    lcdclock_set = 1;

    return 0;
#endif
}



/**
 *  @brief  SetLCDClock
 *
 */
uint32_t LCD_SetClock(uint32_t presc, uint32_t div) {


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

/**
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
 * LFACLK freq |PRESC| LFACLK_presc|   DIV | FDIV | LFACLK_lcd  | Multiplexing | MUX |  Frame
 * ------------|-----|-------------|-------|------|-------------|--------------|-----|----------
 *     32678   | 16  |      2048   |     1 |   0  |      2.048  |    Octaplex  |   8 |      128
 *     32678   | 16  |      2048   |     2 |   1  |      1.024  |    Octaplex  |   8 |       64
 *     32678   | 16  |      2048   |     3 |   2  |        683  |    Octaplex  |   8 |       43
 *     32678   | 16  |      2048   |     4 |   3  |        512  |    Octaplex  |   8 |       32
 *     32678   | 16  |      2048   |     5 |   4  |        410  |    Octaplex  |   8 |       26
 *     32678   | 32  |      1024   |     1 |   0  |      1.024  |    Octaplex  |   8 |       64
 *     32678   | 32  |      1024   |     2 |   1  |        512  |    Octaplex  |   8 |       32
 *     32678   | 64  |       512   |     1 |   0  |        512  |    Octaplex  |   8 |       32
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
uint32_t m,m4;
SegEncoding_t used = { 0,0 };

    /*
     * Disable LCD module before fussing around
     */
    LCD->CTRL &= ~(LCD_CTRL_EN);

    /* If clock no set, configure it */
    if( !lcdclock_set ) {
        SetLFAClock(LCD_CLOCK_DEFAULT);
        LCD_SetClock(LCD_PRESC_DEFAULT,LCD_DIV_DEFAULT);
        lcdclock_set = 1;
    }

    /*
     * Configure Voltage and contrast
     */
    LCD_SetVoltage(LCD_USE_VBOOST,1,7);
    LCD_SetContrast(25,LCD_REF_VLCD);

    /*
     * Configure Pins
     */

    // Scan all character encoding and accumulates all bits used
    for(i=0;i<sizeof(seg_encoding)/sizeof(SegEncoding_t);i++) {
        used.hi |= seg_encoding[i].hi;
        used.lo |= seg_encoding[i].lo;
    }

    // Pins are configured in groups of four
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
    m4 = 0xFF;
    while ( (m&BIT(10)) == 0 ) {
        if( (used.hi & m4) != 0 ) {
            segen |= m;
        }
        m <<= 1;
        m4 <<= 4;
    }

    LCD->SEGEN   = segen;

    dispctrl     = LCD->DISPCTRL;
    dispctrl     &= ~(
                     _LCD_DISPCTRL_MUXE_MASK
                     | _LCD_DISPCTRL_MUX_MASK
                     | _LCD_DISPCTRL_BIAS_MASK
                     | _LCD_DISPCTRL_WAVE_MASK
                     | _LCD_DISPCTRL_VLCDSEL_MASK
                     | _LCD_DISPCTRL_CONCONF_MASK   );

    // 8 common pins: LCD_COM7-LCD_COM4 (SEG23-SEG20) . LCD_COM3-LCD_COM0
    dispctrl     |= ( LCD_DISPCTRL_MUX_QUADRUPLEX         // QUADRUPLEX+MUXE -> OCTAPLEX
                     |   LCD_DISPCTRL_MUXE                //
                     |   LCD_DISPCTRL_BIAS_ONEFOURTH      // 1/4 bias
                     |   LCD_DISPCTRL_WAVE_NORMAL );      // Normal wave
    LCD->DISPCTRL = dispctrl;

    /*
     * Enable LCD after configuration
     */
    LCD->CTRL |= LCD_CTRL_EN;
#endif
    return 0;
}

/**
 * @brief  Generates a bit mask with n bit 1 in the least significant position
 */
#define GENBITMASK1(N)  ((1UL<<((N)+1))-1)

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
uint8_t sn,comn,segn,comnrev;
SegEncoding_t s[8] = { {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0} };

    if( pos < 1 || pos > 14 )
        return;
    /*
     * looks for segments to be lit
     * table contains only character with
     */
    if( pos >=1 && pos <= 7 )           segments = segments14forchar[c-' '];
    else if( pos >= 8 && pos <= 11 )    segments = segments7forchar[c-' '];
    else if( pos == 12 ) segments = GENBITMASK1(c-'0');
    else if( pos == 13 ) segments = GENBITMASK1(c-'0');
    else if( pos == 14 ) segments = GENBITMASK1(c-'0');
    else segments = 0;


    /*
     * scan segments bit mask and set corresponding bits in the segment encoding variable 's'
     */
    sn = 0;
    while( segments ) {
        if( segments&1 ) {
            m = tablcd[sn][pos];
            comn = GET_COMMON(m);
            segn = GET_SEG(m);
            comn = com_encoding[comn]; // it is reversed on STK3700
            s[comn].hi |= seg_encoding[segn].hi;
            s[comn].lo |= seg_encoding[segn].lo;
        }
        segments>>=1;
        sn++;
    }
    /*
     * Set LCD registers with data from s and erase field simultaneously
     */
#ifndef LCD_EMULATION
    LCD_FREEZE;
    for(comn=0;comn<8;comn++) {
        comnrev=com_encoding[comn];
        SEGDATALOW[comn]  = (SEGDATALOW[comn] &~tablcdclear[pos][comnrev].lo) | s[comn].lo;
        SEGDATAHIGH[comn] = (SEGDATAHIGH[comn]&~tablcdclear[pos][comnrev].hi) | s[comn].hi;
//        SEGDATALOW[comn]  = SEGDATALOW[comn] | s[comn].lo;
//        SEGDATAHIGH[comn] = SEGDATAHIGH[comn]| s[comn].hi;
     }
    LCD_UNFREEZE;
#else
    for(comn=0;comn<8;comn++) {
        comnrev=com_encoding[comn];
        lcd[comn].lo = (lcd[comn].lo&~tablcdclear[pos][comnrev].lo)|s[comn].lo;
        lcd[comn].hi = (lcd[comn].hi&~tablcdclear[pos][comnrev].hi)|s[comn].hi;
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

void LCD_WriteAlphanumericDisplay(char *s) {
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
void LCD_WriteNumericDisplay(char *s) {
int pos;

    for(pos=8;pos<=11;pos++) {
        if( *s ) {
            LCD_WriteChar(*s++,pos);
        } else {
            LCD_WriteChar(' ',pos);
        }
    }

}

/**
 *  @brief  Write a string to positions 1 to 12 (with 14 segments displays in position 1 to 7)
 *          and 7 segments display in position 8 to 11)
 *
 *  @param  s: string to be written
 *
 *  @note   if the string is shorten than 12, it is padded with spaces
 */

void LCD_WriteString(char *s) {
int pos;

    for(pos=1;pos<=11;pos++) {
        if( *s ) {
            LCD_WriteChar(*s++,pos);
        } else {
            LCD_WriteChar(' ',pos);
        }
    }

}

/**
 *  @brief  Turns off all segments
 *
 */
void LCD_ClearAll(void) {
int comn;
    LCD_FREEZE;
    for(comn=0;comn<8;comn++) {
#ifdef LCD_EMULATION
        lcd[comn].hi = 0;
        lcd[comn].lo = 0;
#else
        SEGDATALOW[comn]  = 0;
        SEGDATAHIGH[comn] = 0;
#endif
    LCD_UNFREEZE;
    }
}

/**
 *  @brief  Turns on all segments
 *
 */
void LCD_SetAll(void) {
int comn;
    LCD_FREEZE;
    for(comn=0;comn<8;comn++) {
#ifdef LCD_EMULATION
        lcd[comn].hi = 0xFF;
        lcd[comn].lo = 0xFFFFFFFF;
#else
        SEGDATALOW[comn]  = 0xFFFFFFFF;
        SEGDATAHIGH[comn] = 0xFF;
#endif
    LCD_UNFREEZE;
    }
}

/**
 *  @brief find the log2 of an integer
 *  @note  y = ln2(x)  <=>  2^x = y
 *  @note  if y is not a power of 2, returns the largest x such that 2^x < y
 *  @note  used to find the encoding needed for a divisor
 */
static uint32_t ln2(uint32_t x) {
uint32_t n = 0;
uint32_t v;

    v = 1;
    while( v < x ) {
        v <<= 1;
        n++;
    }

    return n;
}

/**
 *  @brief Set LCD Voltage
 *
 *  @param source: LCD_USEVDD our LCD_USEVBOOST.
 *  @param div:    a power of 2 between 1 and 128.
 *  @param level:  a number between 0 and 7 to indicate the voltage output of booster
 *
 *  @note  There are two alternatives. Use MCU VDD, i.e., the main power. Other alternative is
 *         to use a VBOOST power. This is done by using an external 1 uF capacitor between
 *         LCD_BEXT pin and VSS, a 22 nF between LCD_BCAP_P and LCD_BCAP_N. Both capacitors
 *         are mounted in the STK3700 board.
 *  @note  There is a possibility to configure the VBOOST frequency. A low frequency leads to
 *         small current consumption. The boost frequency is LFACLK/div.
 *  @note
 *   level    |     0 |     1 |     2 |     3 |     4 |     5 |     6 |     7
 *   ---------|-------|-------|-------|-------|-------|-------|-------|--------
 *   voltage  |   3.00|   3.08|   3.17|   3.26|   3.34|   3.43|   3.52|   3.60
 *  @endverbatim
 */
void LCD_SetVoltage(uint32_t source, uint32_t div, uint32_t level ) {
uint32_t d;
uint32_t lcdctrl, dispctrl;

    if( source == LCD_USE_VDD ) {
        // Set LCD to use VDD
        LCD->DISPCTRL = (LCD->DISPCTRL&~_LCD_DISPCTRL_VLCDSEL_MASK)|_LCD_DISPCTRL_VLCDSEL_VDD;
        // Disable VBoost
        CMU->LCDCTRL &= ~CMU_LCDCTRL_VBOOSTEN;
        return;
    }

    /*
     * Configure VBoost: frequency in CMU->LCDCTRL and level in LCD->DISPCTRL
     */
    d = ln2(div);
    // Set LCD momentarily to use VDD
    LCD->DISPCTRL = (LCD->DISPCTRL&~_LCD_DISPCTRL_VLCDSEL_MASK)|_LCD_DISPCTRL_VLCDSEL_VDD;
    // Disable VBoost
    CMU->LCDCTRL &= ~CMU_LCDCTRL_VBOOSTEN;
    // Read registers
    lcdctrl  = CMU->LCDCTRL;
    dispctrl = LCD->DISPCTRL;
    // Clear fields
    lcdctrl &= ~_CMU_LCDCTRL_VBFDIV_MASK;
    dispctrl &= _LCD_DISPCTRL_VBLEV_MASK;
    // Configure frequency
    lcdctrl |= (d<<_CMU_LCDCTRL_VBFDIV_SHIFT)&_CMU_LCDCTRL_VBFDIV_MASK;
    // Configure level
    dispctrl |= (level<<_LCD_DISPCTRL_VBLEV_SHIFT)&_LCD_DISPCTRL_VBLEV_MASK;
    // Write registers
    LCD->DISPCTRL = dispctrl;
    CMU->LCDCTRL  = lcdctrl;
    // Before exiting, reenable
    CMU->LCDCTRL |= CMU_LCDCTRL_VBOOSTEN;   // Reenable VBoost

    // Set LCD to use Boost
    LCD->DISPCTRL = (LCD->DISPCTRL&~_LCD_DISPCTRL_VLCDSEL_MASK)|_LCD_DISPCTRL_VLCDSEL_VEXTBOOST;
}



/**
 *  @brief Set Contrast of LCD display
 *
 *  @param level:  In the range 0 to 31
 *  @param ref:    LCD_REF_GND or LCD_REF_VLCD: Reference is GND or VLCD
 *
 *  @note  VLCDout = 0.61*VLCD to VLCD in 0.61 steps
 *
 */
void LCD_SetContrast(uint32_t level, uint32_t ref) {
uint32_t dispctrl;

    // Read register
    dispctrl = LCD->DISPCTRL;
    // Clear bits
    dispctrl &= ~(_LCD_DISPCTRL_CONCONF_MASK|_LCD_DISPCTRL_CONLEV_MASK);
    // Set level
    dispctrl |= (level<<_LCD_DISPCTRL_CONLEV_SHIFT)&_LCD_DISPCTRL_CONLEV_MASK;
    // Set reference
    dispctrl |= (ref<<_LCD_DISPCTRL_CONCONF_SHIFT)&_LCD_DISPCTRL_CONCONF_MASK;
    // Write register
    LCD->DISPCTRL = dispctrl;

}

/**
 * @brief Write a segment using segment mask
 */
void LCD_WriteSegmentMask(uint32_t com, SegEncoding_t s, uint32_t v) {

    com = com_encoding[com]; // it is reversed on STK3700
    if( v ) { /* Set */
        SEGDATALOW[com]  |= s.lo;
        SEGDATAHIGH[com] |= s.hi;
    } else { /* Clear */
        SEGDATALOW[com]  &= ~s.lo;
        SEGDATAHIGH[com] &= ~s.hi;
    }
}

/**
 * @brief Write a segment using segment number
 * @note  Uses controller segment number
 */
void LCD_WriteSegment2(uint32_t comn, uint32_t segn, uint32_t v) {
SegEncoding_t s;

    if( segn >= 32 ) {
        s.hi = BIT(segn-32);
        s.lo = 0;
    } else {
        s.hi = 0;
        s.lo = BIT(segn);
    }
    LCD_WriteSegmentMask(comn,s,v);
}

/**
 * @brief Write a segment using segment number
 * @note  Uses display segment number
 */
void LCD_WriteSegment(uint32_t comn, uint32_t segn, uint32_t v) {
SegEncoding_t s;

    s = seg_encoding[segn];
    LCD_WriteSegmentMask(comn,s,v);
}

/**
 *  @brief  Turns on/off a special segment
 *
 */
void LCD_WriteSpecial(LCD_Code_t code, uint8_t v) {
uint16_t m;
uint8_t segn,comn,sn;
SegEncoding_t seg;
SegEncoding_t s[8] = { 0 };
uint8_t pos=0;
uint32_t segments;

    if( code < LCD_GROUP ) {
        m = tablcdspecial[code];
        comn = GET_COMMON(m);
        segn = GET_SEG(m);
        seg = seg_encoding[segn];
        LCD_WriteSegmentMask(comn,seg,v);
    } else {
        switch(code) {
        case LCD_ARC:
            pos = 12;
            segments = GENBITMASK1(v);
            break;
        case LCD_BAT:
            pos = 13;
            segments = GENBITMASK1(v);
            break;
        case LCD_TARGET:
            pos = 14;
            segments = GENBITMASK1(v);
            break;
        default:
            segments = 0;
        }
        /*
         * scan segments bit mask and set corresponding bits in the segment encoding variable 's'
         */
        sn = 0;
        while( segments ) {
            if( segments&1 ) {
                m = tablcd[sn][pos];
                comn = GET_COMMON(m);
                segn = GET_SEG(m);
                comn = com_encoding[comn]; // it is reversed on STK3700
                s[comn].hi |= seg_encoding[segn].hi;
                s[comn].lo |= seg_encoding[segn].lo;
            }
            segments>>=1;
            sn++;
        }
        /*
         * Set LCD registers with data from s and erase field simultaneously
         */
#ifndef LCD_EMULATION
        LCD_FREEZE;
        for(comn=0;comn<8;comn++) {
            SEGDATALOW[comn]  = (SEGDATALOW[comn] &~tablcdclear[comn][pos].lo) | s[comn].lo;
            SEGDATAHIGH[comn] = (SEGDATAHIGH[comn]&~tablcdclear[comn][pos].hi) | s[comn].hi;
        }
        LCD_UNFREEZE;
#else
        for(comn=0;comn<8;comn++) {
            lcd[comn].lo = (lcd[comn].lo&~tablcdclear[comn][pos].lo)|s[comn].lo;
            lcd[comn].hi = (lcd[comn].hi&~tablcdclear[comn][pos].hi)|s[comn].hi;
        }
#endif
    }
}

/**
 * @brief Freeze LCD controller. All update are postponed
 */
void LCD_Freeze(void) {
    LCD_FREEZE;
}


/**
 * @brief Unfreeze LCD controller. All updates can be made
 */
void LCD_Unfreeze(void) {
    LCD_UNFREEZE;
}
