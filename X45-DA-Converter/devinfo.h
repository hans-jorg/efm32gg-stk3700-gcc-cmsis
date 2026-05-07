#ifndef DEVINFO_H
#define DEVINFO_H
/**
 * @file    devinfo.h
 * @brief   The structure in the efm32gg_devinfo.h does not corresponds to the
 *          one defined by the Reference Manual at page 23.
 *
 * @note    The address in the efm32gg990f1024.h is 0x0FE081B0 but the addresses
 *          in the reference manual starts at 0x0FE08020!!!!!!
 @ @note    Some values are missing too. Eg. DAC0_BIASPROG
 *
 * @version 1.0
 */

#define    DI_CMU_LFRCOCTRL            0x0FE08020      // Register reset value.
#define    DI_CMU_HFRCOCTRL            0x0FE08028      // Register reset value.
#define    DI_CMU_AUXHFRCOCTRL         0x0FE08030      // Register reset value.
#define    DI_ADC0_CAL                 0x0FE08040      // Register reset value.
#define    DI_ADC0_BIASPROG            0x0FE08048      // Register reset value.
#define    DI_DAC0_CAL                 0x0FE08050      // Register reset value.
#define    DI_DAC0_BIASPROG            0x0FE08058      // Register reset value.
#define    DI_ACMP0_CTRL               0x0FE08060      // Register reset value.
#define    DI_ACMP1_CTRL               0x0FE08068      // Register reset value.
#define    DI_CMU_LCDCTRL              0x0FE08078      // Register reset value.
#define    DI_DAC0_OPACTRL             0x0FE080A0      // Register reset value.
#define    DI_DAC0_OPAOFFSET           0x0FE080A8      // Register reset value.
#define    DI_EMU_BUINACT              0x0FE080B0      // Register reset value.
#define    DI_EMU_BUACT                0x0FE080B8      // Register reset value.
#define    DI_EMU_BUBODBUVINCAL        0x0FE080C0      // Register reset value.
#define    DI_EMU_BUBODUNREGCAL        0x0FE080C8      // Register reset value.
#define    DI_DI_CRC                   0x0FE081B0      // [15:0]: DI data CRC-16.
#define    DI_CAL_TEMP_0               0x0FE081B2      // [7:0] Calibration temperature (°C).
#define    DI_ADC0_CAL_1V25            0x0FE081B4      // [14:8]: Gain for 1V25 reference, [6:0]: Offset for 1V25 reference.
#define    DI_ADC0_CAL_2V5             0x0FE081B6      // [14:8]: Gain for 2V5 reference, [6:0]: Offset for 2V5 reference.
#define    DI_ADC0_CAL_VDD             0x0FE081B8      //  [14:8]: Gain for VDD reference, [6:0]: Offset for VDD reference.
#define    DI_ADC0_CAL_5VDIFF          0x0FE081BA      //  [14:8]: Gain for 5VDIFF reference, [6:0]: Offset for 5VDIFF reference.
#define    DI_ADC0_CAL_2XVDD           0x0FE081BC      //  [14:8]: Reserved (gain for this reference cannot be calibrated), [6:0]: Offset for 2XVDD reference.
#define    DI_ADC0_TEMP_0_READ_1V25    0x0FE081BE      //  [15:4] Temperature reading at 1V25 reference, [3:0] Reserved.
#define    DI_DAC0_CAL_1V25            0x0FE081C8      //  [22:16]: Gain for 1V25 reference, [13:8]: Channel 1 offset for 1V25 reference, [5:0]: Channel 0 offset for 1V25 reference.
#define    DI_DAC0_CAL_2V5             0x0FE081CC      //  [22:16]: Gain for 2V5 reference, [13:8]: Channel 1 offset for 2V5 reference, [5:0]: Channel 0 offset for 2V5 reference.
#define    DI_DAC0_CAL_VDD             0x0FE081D0      //  [22:16]: Reserved (gain for this reference cannot be calibrated), [13:8]: Channel 1 offset for VDD reference, [5:0]: Channel 0 offset for VDD reference.
#define    DI_AUXHFRCO_CALIB_BAND_1    0x0FE081D4      //  [7:0]: Tuning for the 1.2 MHz AUXHFRCO band.
#define    DI_AUXHFRCO_CALIB_BAND_7    0x0FE081D5      //  [7:0]: Tuning for the 6.6 MHz AUXHFRCO band.
#define    DI_AUXHFRCO_CALIB_BAND_11   0x0FE081D6      //  [7:0]: Tuning for the 11 MHz AUXHFRCO band.
#define    DI_AUXHFRCO_CALIB_BAND_14   0x0FE081D7      //  [7:0]: Tuning for the 14 MHz AUXHFRCO band.
#define    DI_AUXHFRCO_CALIB_BAND_21   0x0FE081D8      //  [7:0]: Tuning for the 21 MHz AUXHFRCO band.
#define    DI_AUXHFRCO_CALIB_BAND_28   0x0FE081D9      //  [7:0]: Tuning for the 28 MHz AUXHFRCO band.
#define    DI_HFRCO_CALIB_BAND_1       0x0FE081DC      //  [7:0]: Tuning for the 1.2 MHz HFRCO band.
#define    DI_HFRCO_CALIB_BAND_7       0x0FE081DD      //  [7:0]: Tuning for the 6.6 MHz HFRCO band.
#define    DI_HFRCO_CALIB_BAND_11      0x0FE081DE      //  [7:0]: Tuning for the 11 MHz HFRCO band.
#define    DI_HFRCO_CALIB_BAND_14      0x0FE081DF      //  [7:0]: Tuning for the 14 MHz HFRCO band.
#define    DI_HFRCO_CALIB_BAND_21      0x0FE081E0      //  [7:0]: Tuning for the 21 MHz HFRCO band.
#define    DI_HFRCO_CALIB_BAND_28      0x0FE081E1      //  [7:0]: Tuning for the 28 MHz HFRCO band.
#define    DI_MEM_INFO_PAGE_SIZE       0x0FE081E7      //  [7:0] Flash page size in bytes coded as 2 ^ ((MEM_INFO_PAGE_SIZE + 10) & 0xFF). Ie. the value 0xFF = 512 bytes.
#define    DI_UNIQUE_0                 0x0FE081F0      //  [31:0] Unique number.
#define    DI_UNIQUE_1                 0x0FE081F4      //  [63:32] Unique number.
#define    DI_MEM_INFO_FLASH           0x0FE081F8      //  [15:0]: Flash size, kbyte count as unsigned integer (eg.  128).
#define    DI_MEM_INFO_RAM             0x0FE081FA      //  [15:0]: Ram size, kbyte count as unsigned integer (eg. 16).
#define    DI_PART_NUMBER              0x0FE081FC      //  [15:0]: EFM32 part number as unsigned integer (eg. 230).
#define    DI_PART_FAMILY              0x0FE081FE      //  [7:0]: EFM32 part family number (Gecko = 71, Giant Gecko = 72, Tiny Gecko = 73, Leopard Gecko=74, Wonder Gecko=75).
#define    DI_PROD_REV                 0x0FE081FF      //  [7:0]: EFM32 Production ID.

/**
 * GetValueFromMemory (32 and 16 bit versions)
 *
 */
#define DI_GetValue32(ADDR)  *( (uint32_t *) (ADDR) )
#define DI_GetValue16(ADDR)  *( (uint16_t *) (ADDR) )
#define DI_GetValue8(ADDR)   *( (uint8_t *) (ADDR) )



#endif // devinfo.h
