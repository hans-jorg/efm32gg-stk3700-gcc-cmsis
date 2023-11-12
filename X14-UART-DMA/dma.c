
/**
 * @file    dma.c
 *
 * @brief   HAL for DMA in the EFM32GG
 *
 * @note    It implements a simple HAL for DMA, basically to support UART
 *
 * @note    It only implements normal mode. It does not implement ping-pong, loop, 2D transfers.
 *
 * @author  Hans
 *
 * @version 1.0
 *
 * @date    07/06/2020
 */

#include <stdint.h>
#include "em_device.h"
#include "dma.h"

//#include "efm32gg_dma.h"
//#include "efm32gg_dma_descriptor.h"

/* These files are include because the address used to read/write data is needed to configure DMA */
/*#include "efm32gg_adc.h"*/
/*#include "efm32gg_dac.h"*/
/*#include "efm32gg_usart.h"*/
/*#include "efm32gg_timer.h"*/
/*#include "efm32gg_uart.h"*/
/*#include "efm32gg_msc.h"*/
/*#include "efm32gg_aes.h"*/
/*#include "efm32gg_ebi.h"*/



/**
 * The EFM32GG DMA peripheral supports 12 channels. Each channel has two descriptors.
 *
 * The channels can be:
 *      memory to memory
 *      peripheral to memory
 *      memory to peripheral
 *
 *
 *
 * The descriptors must be stored in a descriptor area, that must be alligned to a 256 byte address
 * boundary.
 *
 * Each DMA channel has one primary and one alternate descriptor.
 * The descriptors must be stored in a single area, accessible to both peripherals.
 *
 * Note that the alternate descriptors must start at a 128 byte address boundary.
 *
 *    +01B0   Alternate descriptor for channel 11
 *    +01A0   Alternate descriptor for channel 10
 *    +0190	Alternate descriptor for channel 9
 *    +0180	Alternate descriptor for channel 8
 *    +0170	Alternate descriptor for channel 7
 *    +0160   Alternate descriptor for channel 6
 *    +0150	Alternate descriptor for channel 5
 *    +0140	Alternate descriptor for channel 4
 *    +0130	Alternate descriptor for channel 3
 *    +0120   Alternate descriptor for channel 2
 *    +0110	Alternate descriptor for channel 1
 *    +0100	Alternate descriptor for channel 0
 *    +00F0   Not used
 *    +00E0   Not used
 *    +00D0   Not used
 *    +00C0   Not used
 *    +00B0	Descriptor for channel 11
 *    +00A0	Descriptor for channel 10
 *    +0090	Descriptor for channel 9
 *    +0080	Descriptor for channel 8
 *    +0070	Descriptor for channel 7
 *    +0060	Descriptor for channel 6
 *    +0050	Descriptor for channel 5
 *    +0040	Descriptor for channel 4
 *    +0030	Descriptor for channel 3
 *    +0020	Descriptor for channel 2
 *    +0010	Descriptor for channel 1
 *    +0000	Descriptor for channel 0
 *    */
#define NDMACHANNELS     DMA_CHAN_COUNT
#if NDMACHANNELS >= 8
#define DESCTABLESIZE  16
#elif NDMACHANNELS 8
#endif
#define NELEMTABLE      (DESCTABLESIZE+NDMACHANNELS)





static DMA_DESCRIPTOR_TypeDef dma_descriptors[NELEMTABLE]
// This form of specifying attributes is from GCC. Other compilers have other forms.
#if __GNUC__
  __attribute__((aligned(256)))
#else
#error "This area must be alligned to a 256 boundary"
#endif
;

/**
 * @brief   Data Source Index
 *
 * @note    Used as parameter for DMA_ConfigChannel.
 *          The actual encoding is in the table channelconfig below
 */
 /* Input signal selection */
#define DMA_CHNOPND_NONE                         0    // No source selected and no input signal used
#define DMA_CHNOPND_MEMORY                       1    // Memory to Memory (same as 0?)
#define DMA_CHNOPND_ADC0_SINGLE                  2    // Analog to Digital Converter 0 Single mode
#define DMA_CHNOPND_ADC0_SCAN                    3    // Analog to Digital Converter 0 Scan mode
#define DMA_CHNOPND_DAC0_CH0                     4    // Digital to Analog Converter 0 Channel 0
#define DMA_CHNOPND_DAC0_CH1                     5    // Digital to Analog Converter 0 Channel 1
#define DMA_CHNOPND_USART0_RXDATAV               6    // USART 0 RX Data Available REQ/SREQ
#define DMA_CHNOPND_USART0_TXBL                  7    // USART 0 TX Buffer Level REQ/SREQ
#define DMA_CHNOPND_USART0_TXEMPTY               8    // USART 0 TX Buffer Empty
#define DMA_CHNOPND_USART1_RXDATAV               9    // USART 1 RX Data Available REQ/SREQ
#define DMA_CHNOPND_USART1_TXBL                 10    // USART 1 TX TX Buffer Level REQ/SREQ
#define DMA_CHNOPND_USART1_TXEMPTY              11    // USART 1 TX Buffer Empty
#define DMA_CHNOPND_USART1_RXDATAVRIGH          12    // USART 1 RX Data Available REQ/SREQ ??
#define DMA_CHNOPND_USART1_TXBLRIGHT            13    // USART 1 TX Buffer Level  REQ/SREQ
#define DMA_CHNOPND_USART2_RXDATAV              14    // USART 2 RX Data Available REQ/SREQ
#define DMA_CHNOPND_USART2_TXBL                 15    // USART 2 TX TX ?? REQ/SREQ
#define DMA_CHNOPND_USART2_TXEMPTY              16    // USART 2 TX Buffer Empty
#define DMA_CHNOPND_USART2_RXDATAVRIGH          17    // USART 2 RX Data Available REQ/SREQ ??
#define DMA_CHNOPND_USART2_TXBLRIGHT            18    // USART 2 TX Buffer Level  REQ/SREQ
#define DMA_CHNOPND_LEUART0_RXDATAV             19    // LEUART 0 RX Data Available REQ/SREQ
#define DMA_CHNOPND_LEUART0_TXBL                20    // LEUART 0 TX Buffer Level REQ/SREQ
#define DMA_CHNOPND_LEUART0_TXEMPTY             21    // LEUART 0 TX Buffer Empty
#define DMA_CHNOPND_LEUART1_RXDATAV             22    // LEUART 1 RX Data Available REQ/SREQ
#define DMA_CHNOPND_LEUART1_TXBL                23    // LEUART 1 TX Buffer Level REQ/SREQ
#define DMA_CHNOPND_LEUART1_TXEMPTY             24    // LEUART 1 TX Buffer Empty
#define DMA_CHNOPND_I2C0_RXDATAV                25    // I2C 0
#define DMA_CHNOPND_I2C0_TXBL                   26    // I2C 0 TX Buffer Level
#define DMA_CHNOPND_I2C1_RXDATAV                27    // I2C 1
#define DMA_CHNOPND_I2C1_TXBL                   28    // I2C 1 TX Buffer Level
#define DMA_CHNOPND_TIMER0_UFOF                 29    // Timer 0 Underflow/Overflow
#define DMA_CHNOPND_TIMER0_CC0                  30    // Timer 0 Compare/Capture Channel 0
#define DMA_CHNOPND_TIMER0_CC1                  31    // Timer 0 Compare/Capture Channel 1
#define DMA_CHNOPND_TIMER0_CC2                  32    // Timer 0 Compare/Capture Channel 2
#define DMA_CHNOPND_TIMER1_UFOF                 33    // Timer 1 Underflow/Overflow
#define DMA_CHNOPND_TIMER1_CC0                  34    // Timer 1 Compare/Capture Channel 0
#define DMA_CHNOPND_TIMER1_CC1                  35    // Timer 1 Compare/Capture Channel 1
#define DMA_CHNOPND_TIMER1_CC2                  36    // Timer 1 Compare/Capture Channel 2
#define DMA_CHNOPND_TIMER2_UFOF                 37    // Timer 2 Underflow/Overflow
#define DMA_CHNOPND_TIMER2_CC0                  38    // Timer 2 Compare/Capture Channel 0
#define DMA_CHNOPND_TIMER2_CC1                  39    // Timer 2 Compare/Capture Channel 1
#define DMA_CHNOPND_TIMER2_CC2                  40    // Timer 2 Compare/Capture Channel 2
#define DMA_CHNOPND_TIMER3_UFOF                 40    // Timer 3 Underflow/Overflow
#define DMA_CHNOPND_TIMER3_CC0                  41    // Timer 3 Compare/Capture Channel 0
#define DMA_CHNOPND_TIMER3_CC1                  42    // Timer 3 Compare/Capture Channel 1
#define DMA_CHNOPND_TIMER3_CC2                  43    // Timer 3 Compare/Capture Channel 2
#define DMA_CHNOPND_UART0_RXDATAV               44    // UART 0 RX Data Available REQ/SREQ
#define DMA_CHNOPND_UART0_TXBL                  45    // UART 0 TX Buffer Level REQ/SREQ
#define DMA_CHNOPND_UART0_TXEMPTY               46    // UART 0 TX Buffer Empty
#define DMA_CHNOPND_UART1_RXDATAV               47    // UART 1 RX Data Available REQ/SREQ
#define DMA_CHNOPND_UART1_TXBL                  48    // UART 1 TX Buffer Level REQ/SREQ
#define DMA_CHNOPND_UART1_TXEMPTY               49    // UART 1 TX Buffer Empty
#define DMA_CHNOPND_MSC_WDATA                   50    // Memory System Controller
#define DMA_CHNOPND_AES_DATAWR                  52    // AES Data Write
#define DMA_CHNOPND_AES_XORDATAWR               53    // AES Data Write
#define DMA_CHNOPND_AES_DATARD                  54    // AES Data Write
#define DMA_CHNOPND_AES_DATAKEYWR               55    // AES Data Write
#define DMA_CHNOPND_LESENSE_BUFDATAV            56    // Low Energy Sensor Interface
#define DMA_CHNOPND_EBI_PXL0EMPTY               57    // EBI
#define DMA_CHNOPND_EBI_PXL1EMPTY               58    // EBI
#define DMA_CHNOPND_EBI_PXL0FULL                59    // EBI
#define DMA_CHNOPND_EBI_DDEMPTY                 60    // EBI

/**
 * @brief   Structure to store information about channel configuration
 *
 * @note
 */
struct ChannelConfig {
    uint32_t        chnctrl;
    void           *ioaddr;
    uint16_t        sourceconfig;
    uint16_t        destconfig;
    uint16_t        type;
};

#define SIZE_Pos        (0)
#define SIZE8           ((0)<<SIZE_Pos)
#define SIZE16          ((1)<<SIZE_Pos)
#define SIZE32          ((2)<<SIZE_Pos)
#define SIZE_MASK       (0x3<<SIZE_Pos)

#define INC_Pos         (3)
#define NOINC           ((0)<<INC_Pos)
#define INC0            NOINC
#define INC1            ((1)<<INC_Pos)
#define INC2            ((2)<<INC_Pos)
#define INC3            ((3)<<INC_Pos)
#define INC4            ((4)<<INC_Pos)
#define INC_MASK        (0x7<<INC_Pos)


#define TYPE_Pos        (0)
#define TYPE_MASK       (0x3)
#define MEMMEM          (0)
#define MEMPER          (1) // Write to peripheral
#define PERMEM          (2) // Read from peripheral

static const struct ChannelConfig channelconfig[] = {
/* NONE               */
    {   DMA_CH_CTRL_SOURCESEL_NONE,
        0x0000,
        0x0000,
        0x0000,
        MEMMEM  },
/* MEMORY             */
    {   DMA_CH_CTRL_SOURCESEL_NONE,
        0x0000,
        0x0000,
        0x0000,
        MEMMEM  },
/* ADC0_SINGLE        */
    {   DMA_CH_CTRL_SOURCESEL_ADC0|DMA_CH_CTRL_SIGSEL_ADC0SINGLE,
        (void *) &ADC0->SINGLEDATA,
        SIZE16|NOINC,
        SIZE16|INC2,
        PERMEM  },
/* ADC0_SCAN          */
    {   DMA_CH_CTRL_SOURCESEL_ADC0|DMA_CH_CTRL_SIGSEL_ADC0SCAN,
        (void *) &ADC0->SCANDATA,
        SIZE16|NOINC,
        SIZE16|INC2,
        PERMEM  },
/* DAC0_CH0           */
    {   DMA_CH_CTRL_SOURCESEL_DAC0|DMA_CH_CTRL_SIGSEL_DAC0CH0,
        (void *) &DAC0->CH0DATA,
        SIZE16|INC2,
        SIZE16|NOINC,
        MEMPER  },
/* DAC0_CH1           */
    {   DMA_CH_CTRL_SOURCESEL_DAC0|DMA_CH_CTRL_SIGSEL_DAC0CH1,
        (void *) &DAC0->CH1DATA,
        SIZE16|INC2,
        SIZE16|NOINC,
        MEMPER  },
/* USART0_RXDATAV     */
    {   DMA_CH_CTRL_SOURCESEL_USART0|DMA_CH_CTRL_SIGSEL_USART0RXDATAV,
        (void *) &USART0->RXDATA,
        SIZE8|NOINC,
        SIZE8|INC1,
        PERMEM  },
/* USART0_TXBL        */
    {   DMA_CH_CTRL_SOURCESEL_USART0|DMA_CH_CTRL_SIGSEL_USART0TXBL,
        (void *) &USART0->TXDATA,
        SIZE8|INC1,
        SIZE8|NOINC,
        MEMPER  },
/* USART0_TXEMPTY     */
    {   DMA_CH_CTRL_SOURCESEL_USART0|DMA_CH_CTRL_SIGSEL_USART0TXEMPTY,
        (void *) &USART0->TXDATA,
        SIZE8|INC1,
        SIZE8|NOINC,
        MEMPER  },
/* USART1_RXDATAV     */
    {   DMA_CH_CTRL_SOURCESEL_USART1|DMA_CH_CTRL_SIGSEL_UART1RXDATAV,
        (void *) &USART1->RXDATA,
        SIZE8|NOINC,
        SIZE8|INC1,
        PERMEM  },
/* USART1_TXBL        */
    {   DMA_CH_CTRL_SOURCESEL_USART1|DMA_CH_CTRL_SIGSEL_USART1TXBL,
        (void *) &USART1->TXDATA,
        SIZE8|INC1,
        SIZE8|NOINC,
        MEMPER  },
/* USART1_TXEMPTY     */
    {   DMA_CH_CTRL_SOURCESEL_USART1|DMA_CH_CTRL_SIGSEL_USART1TXEMPTY,
        (void *) &USART1->TXDATA,
        SIZE8|INC1,
        SIZE8|NOINC,
        MEMPER  },
/* USART1_RXDATAVRIGH */
    {   DMA_CH_CTRL_SOURCESEL_USART1|DMA_CH_CTRL_SIGSEL_USART1RXDATAVRIGHT,
        (void *) &USART1->RXDATA,
        SIZE8|NOINC,
        SIZE8|INC1,
        PERMEM  },
/* USART1_TXBLRIGHT   */
    {   DMA_CH_CTRL_SOURCESEL_USART1|DMA_CH_CTRL_SIGSEL_USART1TXBLRIGHT,
        (void *) &USART1->TXDATA,
        SIZE8|INC1,
        SIZE8|NOINC,
        MEMPER  },
/* USART2_RXDATAV     */
    {   DMA_CH_CTRL_SOURCESEL_USART2|DMA_CH_CTRL_SIGSEL_USART2RXDATAV,
        (void *) &USART2->RXDATA,
        SIZE8|NOINC,
        SIZE8|INC1,
        PERMEM  },
/* USART2_TXBL        */
    {   DMA_CH_CTRL_SOURCESEL_USART2|DMA_CH_CTRL_SIGSEL_USART2TXBL,
        (void *) &USART2->TXDATA,
        SIZE8|INC1,
        SIZE8|NOINC,
        MEMPER  },
/* USART2_TXEMPTY     */
    {   DMA_CH_CTRL_SOURCESEL_USART1|DMA_CH_CTRL_SIGSEL_USART1RXDATAVRIGHT,
        (void *) &USART2->TXDATA,
        SIZE8|INC1,
        SIZE8|NOINC,
        MEMPER  },
/* USART2_RXDATAVRIGH */
    {   DMA_CH_CTRL_SOURCESEL_USART2|DMA_CH_CTRL_SIGSEL_USART2RXDATAVRIGHT,
        (void *) &USART2->RXDATA,
        SIZE8|NOINC,
        SIZE8|INC1,
        MEMPER  },
/* USART2_TXBLRIGHT   */
    {   DMA_CH_CTRL_SOURCESEL_USART2|DMA_CH_CTRL_SIGSEL_USART2TXBLRIGHT,
        (void *) &USART1->TXDATA,
        SIZE8|INC1,
        SIZE8|NOINC,
        MEMPER  },
/* LEUART0_RXDATAV    */
    {   DMA_CH_CTRL_SOURCESEL_LEUART0|DMA_CH_CTRL_SIGSEL_LEUART0RXDATAV,
        (void *) &LEUART0->RXDATA,
        SIZE8|NOINC,
        SIZE8|INC1,
        MEMPER  },
/* LEUART0 TXBL       */
    {   DMA_CH_CTRL_SOURCESEL_LEUART0|DMA_CH_CTRL_SIGSEL_LEUART0TXBL,
        (void *) &LEUART0->TXDATA,
        SIZE8|NOINC,
        SIZE8|INC1,
        MEMPER  },
/* LEUART0_TXEMPTY    */
    {   DMA_CH_CTRL_SOURCESEL_LEUART0|DMA_CH_CTRL_SIGSEL_LEUART0TXEMPTY,
        (void *) &LEUART0->TXDATA,
        SIZE8|NOINC,
        SIZE8|INC1,
        MEMPER  },
/* LEUART1_RXDATAV    */
    {   DMA_CH_CTRL_SOURCESEL_LEUART1|DMA_CH_CTRL_SIGSEL_LEUART1RXDATAV,
        (void *) &LEUART0->RXDATA,
        SIZE8|NOINC,
        SIZE8|INC1,
        PERMEM  },
/* LEUART1_TXBL       */
    {   DMA_CH_CTRL_SOURCESEL_LEUART1|DMA_CH_CTRL_SIGSEL_LEUART1TXBL,
        (void *) &LEUART1->TXDATA,
        SIZE8|NOINC,
        SIZE8|INC1,
        MEMPER  },
/* LEUART1_TXEMPTY    */
    {   DMA_CH_CTRL_SOURCESEL_LEUART1|DMA_CH_CTRL_SIGSEL_LEUART1TXEMPTY,
        (void *) &LEUART1->TXDATA,
        SIZE8|NOINC,
        SIZE8|INC1,
        MEMPER  },
/* I2C0_RXDATAV       */
    {   DMA_CH_CTRL_SOURCESEL_I2C0|DMA_CH_CTRL_SIGSEL_I2C0RXDATAV,
        (void *) &I2C0->RXDATA,
        SIZE8|NOINC,
        SIZE8|INC1,
        PERMEM  },
/* I2C0_TXBL          */
    {   DMA_CH_CTRL_SOURCESEL_I2C0|DMA_CH_CTRL_SIGSEL_I2C0TXBL,
        (void *) &I2C0->TXDATA,
        SIZE8|INC1,
        SIZE8|NOINC,
        MEMPER  },
/* I2C1_RXDATAV       */
    {   DMA_CH_CTRL_SOURCESEL_I2C1|DMA_CH_CTRL_SIGSEL_I2C1RXDATAV,
        (void *) &I2C0->RXDATA,
        SIZE8|NOINC,
        SIZE8|INC1,
        PERMEM  },
/* I2C1_TXBL          */
    {   DMA_CH_CTRL_SOURCESEL_I2C1|DMA_CH_CTRL_SIGSEL_I2C1TXBL,
        (void *) &I2C1->TXDATA,
        SIZE8|INC1,
        SIZE8|NOINC,
        MEMPER  },
/* TIMER0_UFOF        */
    {   DMA_CH_CTRL_SOURCESEL_TIMER0|DMA_CH_CTRL_SIGSEL_TIMER0UFOF,
        (void *) &TIMER0->CNT,              // To be verified
        SIZE16|NOINC,
        SIZE16|INC2 ,
        PERMEM  },
/* TIMER0_CC0         */
    {   DMA_CH_CTRL_SOURCESEL_TIMER0|DMA_CH_CTRL_SIGSEL_TIMER0CC0,
        (void *) &TIMER0->CC[0].CCV,        // To be verified
        SIZE16|NOINC,
        SIZE16|INC2 ,
        PERMEM  },
/* TIMER0_CC1         */
    {   DMA_CH_CTRL_SOURCESEL_TIMER0|DMA_CH_CTRL_SIGSEL_TIMER0CC1,
        (void *) &TIMER0->CC[1].CCV,        // To be verified
        SIZE16|NOINC,
        SIZE16|INC2 ,
        PERMEM  },
/* TIMER0_CC2         */
    {   DMA_CH_CTRL_SOURCESEL_TIMER0|DMA_CH_CTRL_SIGSEL_TIMER0CC2,
        (void *) &TIMER0->CC[2].CCV,        // To be verified
        SIZE16|NOINC,
        SIZE16|INC2 ,
        PERMEM  },
/* TIMER1_UFOF        */
    {   DMA_CH_CTRL_SOURCESEL_TIMER1|DMA_CH_CTRL_SIGSEL_TIMER1UFOF,
        (void *) &TIMER1->CNT,              // To be verified
        SIZE16|NOINC,
        SIZE16|INC2 ,
        PERMEM  },
/* TIMER1_CC0         */
    {   DMA_CH_CTRL_SOURCESEL_TIMER1|DMA_CH_CTRL_SIGSEL_TIMER1CC0,
        (void *) &TIMER1->CC[0].CCV,        // To be verified
        SIZE16|NOINC,
        SIZE16|INC2 ,
        PERMEM  },
/* TIMER1_CC1         */
    {   DMA_CH_CTRL_SOURCESEL_TIMER1|DMA_CH_CTRL_SIGSEL_TIMER1CC1,
        (void *) &TIMER1->CC[1].CCV,        // To be verified
        SIZE16|NOINC,
        SIZE16|INC2 ,
        PERMEM  },
/* TIMER1_CC2         */
    {   DMA_CH_CTRL_SOURCESEL_TIMER1|DMA_CH_CTRL_SIGSEL_TIMER1CC2,
        (void *) &TIMER1->CC[2].CCV,        // To be verified
        SIZE16|NOINC,
        SIZE16|INC2 ,
        PERMEM  },
/* TIMER2_UFOF        */
    {   DMA_CH_CTRL_SOURCESEL_TIMER2|DMA_CH_CTRL_SIGSEL_TIMER2UFOF,
        (void *) &TIMER2->CNT,              // To be verified
        SIZE16|NOINC,
        SIZE16|INC2 ,
        PERMEM  },
/* TIMER2_CC0         */
    {   DMA_CH_CTRL_SOURCESEL_TIMER2|DMA_CH_CTRL_SIGSEL_TIMER2CC0,
        (void *) &TIMER2->CC[0].CCV,        // To be verified
        SIZE16|NOINC,
        SIZE16|INC2 ,
        PERMEM  },
/* TIMER2_CC1         */
    {   DMA_CH_CTRL_SOURCESEL_TIMER2|DMA_CH_CTRL_SIGSEL_TIMER2CC1,
        (void *) &TIMER2->CC[1].CCV,        // To be verified
        SIZE16|NOINC,
        SIZE16|INC2 ,
        PERMEM  },
/* TIMER2_CC2         */
    {   DMA_CH_CTRL_SOURCESEL_TIMER2|DMA_CH_CTRL_SIGSEL_TIMER2CC2,
        (void *) &TIMER2->CC[2].CCV,        // To be verified
        SIZE16|NOINC,
        SIZE16|INC2 ,
        PERMEM  },
/* TIMER3_UFOF        */
    {   DMA_CH_CTRL_SOURCESEL_TIMER3|DMA_CH_CTRL_SIGSEL_TIMER3UFOF,
        (void *) &TIMER3->CNT,              // To be verified
        SIZE16|NOINC,
        SIZE16|INC2 ,
        PERMEM  },
/* TIMER3_CC0         */
    {   DMA_CH_CTRL_SOURCESEL_TIMER3|DMA_CH_CTRL_SIGSEL_TIMER3CC0,
        (void *) &TIMER3->CC[0].CCV,        // To be verified
        SIZE16|NOINC,
        SIZE16|INC2 ,
        PERMEM  },
/* TIMER3_CC1         */
    {   DMA_CH_CTRL_SOURCESEL_TIMER3|DMA_CH_CTRL_SIGSEL_TIMER3CC1,
        (void *) &TIMER3->CC[1].CCV,        // To be verified
        SIZE16|NOINC,
        SIZE16|INC2 ,
        PERMEM  },
/* TIMER3_CC2         */
    {   DMA_CH_CTRL_SOURCESEL_TIMER3|DMA_CH_CTRL_SIGSEL_TIMER3CC2,
        (void *) &TIMER3->CC[2].CCV,        // To be verified
        SIZE16|NOINC,
        SIZE16|INC2 ,
        PERMEM  },
/* UART0_RXDATAV      */
    {   DMA_CH_CTRL_SOURCESEL_UART0|DMA_CH_CTRL_SIGSEL_UART0RXDATAV,
        (void *) &UART0->RXDATA,
        SIZE8|NOINC,
        SIZE8|INC1,
        PERMEM  },
/* UART0_TXBL         */
    {   DMA_CH_CTRL_SOURCESEL_UART0|DMA_CH_CTRL_SIGSEL_UART0TXBL,
        (void *) &UART0->TXDATA,
        SIZE8|INC1,
        SIZE8|NOINC,
        MEMPER  },
/* UART0_TXEMPTY      */
    {   DMA_CH_CTRL_SOURCESEL_UART0|DMA_CH_CTRL_SIGSEL_UART0TXEMPTY,
        (void *) &UART0->TXDATA,
        SIZE8|INC1,
        SIZE8|NOINC,
        MEMPER  },
/* UART1_RXDATAV      */
    {   DMA_CH_CTRL_SOURCESEL_UART1|DMA_CH_CTRL_SIGSEL_UART1RXDATAV,
        (void *) &UART1->RXDATA,
        SIZE8|NOINC,
        SIZE8|INC1,
        PERMEM  },
/* UART1_TXBL         */
    {   DMA_CH_CTRL_SOURCESEL_UART1|DMA_CH_CTRL_SIGSEL_UART1TXBL,
        (void *) &UART1->TXDATA,
        SIZE8|INC1,
        SIZE8|NOINC,
        MEMPER  },
/* UART1_TXEMPTY      */
    {   DMA_CH_CTRL_SOURCESEL_UART1|DMA_CH_CTRL_SIGSEL_UART1TXEMPTY,
        (void *) &UART1->TXDATA,
        SIZE8|INC1,
        SIZE8|NOINC,
        MEMPER  },
/* MSC_WDATA          */
    {   DMA_CH_CTRL_SOURCESEL_MSC|DMA_CH_CTRL_SIGSEL_MSCWDATA,
        (void *) &MSC->WDATA,
        SIZE32|INC4,
        SIZE32|NOINC,
        MEMPER  },
/* AES_DATAWR         */
    {   DMA_CH_CTRL_SOURCESEL_AES|DMA_CH_CTRL_SIGSEL_AESDATAWR,
        (void *) &AES->DATA,
        SIZE32|INC4,
        SIZE32|NOINC,
        MEMPER  },
/* AES_XORDATAWR      */
    {   DMA_CH_CTRL_SOURCESEL_AES|DMA_CH_CTRL_SIGSEL_AESXORDATAWR,
        (void *) &AES->DATA,
        SIZE32|INC4,
        SIZE32|NOINC,
        MEMPER  },
/* AES_DATARD         */
    {   DMA_CH_CTRL_SOURCESEL_AES|DMA_CH_CTRL_SIGSEL_AESDATARD,
        (void *) &AES->DATA,
        SIZE32|NOINC,
        SIZE32|INC4,
        PERMEM  },
/* AES_DATAKEYWR      */
    {   DMA_CH_CTRL_SOURCESEL_AES|DMA_CH_CTRL_SIGSEL_AESKEYWR,
        (void *) &AES->KEYLA,
        SIZE32|INC4,
        SIZE32|INC4,
        MEMPER  },
/* LESENSE_BUFDATAV   */
    {   DMA_CH_CTRL_SOURCESEL_LESENSE|DMA_CH_CTRL_SIGSEL_LESENSEBUFDATAV,
        (void *) &LESENSE->BUFDATA,         // To be verified
        SIZE16|NOINC,
        SIZE16|INC2,
        PERMEM  },
/* EBI_PXL0EMPTY      */
    {   DMA_CH_CTRL_SOURCESEL_EBI|DMA_CH_CTRL_SIGSEL_EBIPXL0EMPTY,
        (void *) &EBI->TFTPIXEL0,           // To be verified
        SIZE16|INC2,
        SIZE16|NOINC,
        MEMPER  },
/* EBI_PXL1EMPTY      */
    {   DMA_CH_CTRL_SOURCESEL_EBI|DMA_CH_CTRL_SIGSEL_EBIPXL1EMPTY,
        (void *) &EBI->TFTPIXEL1,           // To be verified
        SIZE16|INC2,
        SIZE16|NOINC,
        MEMPER  },
/* EBI_PXL0FULL       */
    {   DMA_CH_CTRL_SOURCESEL_EBI|DMA_CH_CTRL_SIGSEL_EBIPXLFULL,
        (void *) &EBI->TFTPIXEL,            // To be verified
        SIZE16|INC2,
        SIZE16|NOINC,
        MEMPER  },
/* EBI_DDEMPTY        */
    {   DMA_CH_CTRL_SOURCESEL_EBI|DMA_CH_CTRL_SIGSEL_EBIDDEMPTY,
        (void *) &EBI->TFTDD,               // To be verified
        SIZE16|INC2,
        SIZE16|NOINC,
        MEMPER  },
};

/**
 * @brief   Data structure with information about channel
 */
struct ChannelInfo {
    uint32_t    desccontrol;
    uint32_t    type;
    uint32_t    size;
    void        *data1;
    void        *data2;
    void        (*callback)(int chn);
};
static struct ChannelInfo channelinfo[NDMACHANNELS] = {0};


/**
 * @brief  Process DMA interrupts
 *
 * @note   Cancels the pending interrupt for the channel and
 *         calls the callback function if it is defined.
 *
 * @note   Callback is called during the interrupt processing. Be small and fast.
 *
 * Layout of pending variable
 * N-1 to 0: channels with high priority
 * 2N-1 to N: channels with low priority
 *
 * N=NDMACHANNELS
 *
 */

void DMA_IRQHandler(void) {
uint32_t prio;
uint32_t priopending,norpending,pending;
int chn;

    prio = DMA->CHPRIS;         // Get channels with high priority
    pending = DMA->IF&DMA->IEN; // Only enabled interrupts

    priopending = pending&prio; // Only channels with high priority pending
    norpending = pending&~prio; // The remaining priority pending;
    // pending variable will contain the interrupts pending as a bit vector, with
    // the high priority interrupts in the lower NDMACCHANNELS bits and the lower priority interrupts in
    // the higher NDMACCHANNELS bits
    pending = (norpending<<NDMACHANNELS)|priopending;

    // process all interrupts from channels starting with the high priority
    chn = 0;
    while(pending) {
        if( pending&1 ) {
            DMA->IFC = (1<<chn);
            if( channelinfo[chn].callback )
                channelinfo[chn].callback(chn);  // Call callback function if defined
        }
        chn++;
        if( chn >= NDMACHANNELS ) {
            // Start processing the lower order interrupts
            chn = 0;
        }
        pending >>= 1;
    }

}


/**
 *  Enable/Disable DMA
 *  Both reset the CHPROT bit
*/
///{
static inline void DMA_Enable(void) { DMA->CONFIG = DMA_CONFIG_EN; }
static inline void DMA_Disable(void) { DMA->CONFIG = 0; }
///}


/**
 * @brief   Reset DMA
 *
 * @note    Reset DMA by
 */

int DMA_Reset(void) {
int i;
const uint32_t channelsmask = (1<<NDMACHANNELS)-1;
    DMA_Disable();

    // Clear Enable for all channels
    DMA->CHENC = channelsmask;
    // Clear bus error
    DMA->ERRORC = DMA_ERRORC_ERRORC;

    // Disable all interrupts
    DMA->IEN = 0;

    // Clear all interrupts
    DMA->IFC = channelsmask;

    // Reset control register
    DMA->CTRL = 0;

    // Disable loops
    DMA->LOOP0 = 0;
    DMA->LOOP1 = 0;

    // Clear Channel Control registers
    for(i=0;i<NDMACHANNELS;i++) {
        DMA->CH[i].CTRL = 0;
        channelinfo[i].callback = 0;
        channelinfo[i].type = 0;
        channelinfo[i].data1 = 0;
        channelinfo[i].data2 = 0;
        channelinfo[i].size = 0;
        channelinfo[i].desccontrol = 0;
    }

    // Clear Descriptor table
    for(i=0;i<NELEMTABLE;i++) {
        dma_descriptors[i].SRCEND = 0;
        dma_descriptors[i].DSTEND = 0;
        dma_descriptors[i].CTRL = 0;
        dma_descriptors[i].USER = 0;
    }

}

/**
 * @brief  Short description of function
 *
 * @note   Long description of function
 *
 * @param  Description of parameter
 *
 * @return Description of return parameters
 */


int DMA_Init(uint32_t config) {

    /* Reset DMA */
    DMA_Reset();

    /* Enable clock */
    CMU->HFCORECLKEN0 |= CMU_HFCORECLKEN0_DMA;

    /* Set descriptor table. This set the ALTCTRLBASE register too. */
    DMA->CTRLBASE = (uint32_t) dma_descriptors;

    DMA_Enable();
    return 0;
}



/**
 * @brief  Configure DMA Channel
 *
 * @note   Configure DMA channel
 *
 * @param  Description of parameter
 *
 * @return Description of return parameters
 */

#define DMA_CONFIGCHANNEL_ARBITRATION_POS               (0)
#define DMA_CONFIGCHANNEL_ALTERNATE                     (0x0001)<<DMA_CONFIGCHANNEL_ARBITRATION_POS)
#define DMA_CONFIGCHANNEL_ARBITRATION_1                 (0x0000)<<DMA_CONFIGCHANNEL_ARBITRATION_POS)
#define DMA_CONFIGCHANNEL_ARBITRATION_2                 (0x0001)<<DMA_CONFIGCHANNEL_ARBITRATION_POS)
#define DMA_CONFIGCHANNEL_ARBITRATION_4                 (0x0010)<<DMA_CONFIGCHANNEL_ARBITRATION_POS)
#define DMA_CONFIGCHANNEL_ARBITRATION_8                 (0x0011)<<DMA_CONFIGCHANNEL_ARBITRATION_POS)
#define DMA_CONFIGCHANNEL_ARBITRATION_16                (0x0100)<<DMA_CONFIGCHANNEL_ARBITRATION_POS)
#define DMA_CONFIGCHANNEL_ARBITRATION_32                (0x0101)<<DMA_CONFIGCHANNEL_ARBITRATION_POS)
#define DMA_CONFIGCHANNEL_ARBITRATION_64                (0x0110)<<DMA_CONFIGCHANNEL_ARBITRATION_POS)
#define DMA_CONFIGCHANNEL_ARBITRATION_128               (0x0111)<<DMA_CONFIGCHANNEL_ARBITRATION_POS)
#define DMA_CONFIGCHANNEL_ARBITRATION_256               (0x1000)<<DMA_CONFIGCHANNEL_ARBITRATION_POS)
#define DMA_CONFIGCHANNEL_ARBITRATION_512               (0x1001)<<DMA_CONFIGCHANNEL_ARBITRATION_POS)
#define DMA_CONFIGCHANNEL_ARBITRATION_1024              (0x1010)<<DMA_CONFIGCHANNEL_ARBITRATION_POS)
#define DMA_CONFIGCHANNEL_ARBITRATION_MASK              (0xFU<<DMA_CONFIGCHANNEL_ARBITRATION_POS)

#define DMA_CONFIGCHANNEL_USEALTERNATE_POS              (10)
#define DMA_CONFIGCHANNEL_USEALTERNATE                  (1<<(DMA_CONFIGCHANNEL_USEALTERNATE_POS))


#define DMA_CONFIGCHANNEL_MODE_POS                      (12)
#define DMA_CONFIGCHANNEL_MODE_STOP                     (0x0<<DMA_CONFIGCHANNEL_MODE_POS)
#define DMA_CONFIGCHANNEL_MODE_BASIC                    (0x1<<DMA_CONFIGCHANNEL_MODE_POS)
#define DMA_CONFIGCHANNEL_MODE_AUTOREQ                  (0x2<<DMA_CONFIGCHANNEL_MODE_POS)
#define DMA_CONFIGCHANNEL_MODE_PINGPONT                 (0x3<<DMA_CONFIGCHANNEL_MODE_POS)
#define DMA_CONFIGCHANNEL_MODE_MEM_SC                   (0x4<<DMA_CONFIGCHANNEL_MODE_POS)
#define DMA_CONFIGCHANNEL_MODE_MEM_SC_ALT               (0x5<<DMA_CONFIGCHANNEL_MODE_POS)
#define DMA_CONFIGCHANNEL_MODE_PER_SC                   (0x6<<DMA_CONFIGCHANNEL_MODE_POS)
#define DMA_CONFIGCHANNEL_MODE_PER_SC_ALT               (0x7<<DMA_CONFIGCHANNEL_MODE_POS)

#define DMA_CONFIGCHANNEL_MODE_MASK                     (0x7<<DMA_CONFIGCHANNEL_MODE_POS)

/**
 * @brief  Configure a DMA channel
 *
 * @note   Configure a DMA channel using the info in the table above
 *
 * @param  chn: channel number
 *         chopnd: channel type including operand type
 *
 *
 * @return 0 for normal processing and a negative for error
 *
 * @note   There are two conntrol fields for a channel. One in the Descriptor table and
 *         other in the CH block of registers.
 */

int DMA_ConfigChannel(int chn, unsigned chnopnd, unsigned params  ) {
DMA_DESCRIPTOR_TypeDef *pdesc;
const struct ChannelConfig *pchannelconfig;
struct ChannelInfo *pchannelinfo;
DMA_CH_TypeDef *pchconfig;

    if( (channelconfig[chnopnd].type&TYPE_MASK)!=MEMPER )
        return -1;

    if( chnopnd >= sizeof(channelconfig)/sizeof(struct ChannelConfig ) )
        return -1;

    if( chn >= NDMACHANNELS )
        return -1;

    pchannelconfig = &channelconfig[chnopnd];
    pchannelinfo = &channelinfo[chn];

    pdesc = &dma_descriptors[chn];
    pchconfig = &(DMA->CH[chn]);

    // Configure Control in the C region of the DMA registers
    pchconfig->CTRL = pchannelconfig->chnctrl;

    // Configure Control in the descriptor region
    unsigned ctrl = 0;
    switch(pchannelconfig->destconfig&SIZE_MASK) {
    case SIZE8:
        ctrl |= DMA_CTRL_DST_SIZE_BYTE;
        break;
    case SIZE16:
        ctrl |= DMA_CTRL_DST_SIZE_HALFWORD;
        break;
    case SIZE32:
        ctrl |= DMA_CTRL_DST_SIZE_WORD;
        break;
    }
    // Configure dest
    switch(pchannelconfig->destconfig&INC_MASK) {
    case INC0:
        ctrl |= DMA_CTRL_DST_INC_NONE;
        break;
    case INC1:
        ctrl |= DMA_CTRL_DST_INC_BYTE;
        break;
    case INC2:
        ctrl |= DMA_CTRL_DST_INC_HALFWORD;
        break;
    case INC3:
        ctrl |= DMA_CTRL_DST_INC_WORD;
        break;
    }
    // Configure source
    switch(pchannelconfig->sourceconfig&SIZE_MASK) {
    case SIZE8:
        ctrl |= DMA_CTRL_SRC_SIZE_BYTE;
        break;
    case SIZE16:
        ctrl |= DMA_CTRL_SRC_SIZE_HALFWORD;
        break;
    case SIZE32:
        ctrl |= DMA_CTRL_SRC_SIZE_WORD;
        break;
    }
    switch(pchannelconfig->sourceconfig&INC_MASK) {
    case INC0:
        ctrl |= DMA_CTRL_SRC_INC_NONE;
        break;
    case INC1:
        ctrl |= DMA_CTRL_SRC_INC_BYTE;
        break;
    case INC2:
        ctrl |= DMA_CTRL_SRC_INC_HALFWORD;
        break;
    case INC3:
        ctrl |= DMA_CTRL_SRC_INC_WORD;
        break;
    }

    pdesc->CTRL = ctrl;

    //if(pchconfig-> )
    return 0;
}

/**
 * @brief  Start DMA transfer on channel chn
 *
 * @note   Configure DMA channel
 *
 * @param  Description of parameter
 *
 * @return Description of return parameters
 */

int DMA_StartTransferMemMem(int chn, void *inbuf, void *outbuf, int cnt ) {

    return 0;
}
/**
 * @brief  Enable DMA Channel
 *
 * @note
 */
static inline int DMA_EnableChannel(int cha) {

    DMA->CHENS = (1<<cha);

    return 0;
}

/**
 * @brief  Enable DMA Channel
 *
 * @note
 */
static inline int DMA_DisableChannel(int cha) {

    DMA->CHENC = (1<<cha);

    return 0;
}
