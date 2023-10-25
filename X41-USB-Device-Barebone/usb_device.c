/**
 * @file    usb_device.c
 *
 * @brief   Implementation of a USB Device on the STK3700.
 *
 * @note    For now, NO support for OTG.
 *
 * @note    For now, it supports ONLY Energy Level EM0 (Run mode).
 *
 * @note    This implementation is specific for the STK3700 board. It does not
 *          use the DMPU signal. So, it should not work as a low speed device
 *          but probably will work.
 *
 * @note    The core is designed to be *interrupt-driven*. Polling interrupt
 *          mechanism is not recommended: this may result in undefined
 *          resolutions.
 *
 * @note    According AN0065, the maximum packet size is 64.
 *
 * @note    Connections of the USB interface
 *
 * | Signal | Board signal     | Pin  | Description                     |
 * |--------|------------------|------|---------------------------------|
 * | VBUSEN | EFM_USB_VBUSEN   | PF5  | Output. Controls switch for OTG |
 * | FAULT  | EFM_USB_OC_FAULT | PF6  | Input. Overcurrent              |
 * | DM     | EFM_USB_DM       | PF10 |                                 |
 * | DP     | EFM_USB_DP       | PF11 |                                 |
 * | ID     | EFM_USB_ID       | PF12 |                                 |
 * | DMPU   |     -            | PD2  | Not used on the STK3700 Board   |
 * | VBUS   | EFM_USB_VBUS     |      | Connected to VREGI              |
 * | VREGO  | EFM_USB_VREGO    |      | Internal regulator output       |
 * | VREGI  | EFM_USB_VREGI    |      | Internal regulator output       |
 *
 * @note    The fact that the board does not use the DPMU connected to
 *          a 4K7 resistore prevent the use of the board as a Low Speed
 *          device. But it is possible that it will work despite it.
 *          Take note that cable specs are different for Low Speed.
 *          And there should be no connector on board side.
 *
 * @note    The DMPU is not used/touched in this code.
 *
 * @note    The USB controller has two parts: core and system.
 *          The DMA and FIFO are part of the core.
 *
 * @note    The USB interface uses two clocks
 *
 * | Clock           | Part      | Description                         |
 * |-----------------|-----------|-------------------------------------|
 * | HFCORECLK_USB   | System    | Divided by HCLKDIV and HFCORECLKDIV |
 * | HFCLORECLK_USBC | Core      | Undivided.                          |
 *
 * @note    The USB Core Part demands a 48 MHz clock for normal operation.
 *
 * @note    Core clock source is specified by the USBCCLKEL field.
 *
 * | Clock source | Value     | Use                                    |
 * |--------------|-----------|----------------------------------------|
 * | HFCLKNODIV   |  ?        | HFCLK w/o divider as defined by HFCLKSEL  |
 * | LFXO         | 32768 kHz | External Low Frequency Crystal Oscillator |
 * | LFRCO        | 32768 kHz | Internal Low Frequency RC Oscillator |
 *
 * The HCLKNODIV source is specified by the HFCLKSEL field:
 *  * LFXO:  External Low Frequency Crystal Oscillator
 *  * LFRCO: Internal Low Frequency RC Oscillator
 *  * HFRCO: Internal High Frequency RC Oscillator
 *  * HFXO:  External High Frequency Crystal Oscillator
 *
 * The values for the differentes clock signal are shown below
 *
 * | Clock    | Range                            | STK3700 Board       |
 * |----------|----------------------------------|---------------------|
 * | LFRCO    | 32768 Hz nom., 31.29-34.28 Hz    |                     |
 * | HFRCO    | 1-28 MHz, default 14 Mhz         |                     |
 * | LFXO     | 32768 Hz                         |  32768 Hz           |
 * | HFXO     | 4-48 MHz                         |     48 MHz          |
 *
 * HFRCO has a tolerance of approximately 15% and is calibrated during
 * manufacturing
 *
 * @note    The USB controller supports Full Speed ( 12 Mbps) and
 *          Low Speed (1.5 Mbps) as host and device
 *
  * @note    Mapping to core and system parts
 *
 * |     Parts        |  Mapped to            |
 * |------------------|-----------------------|
 * |    system        |  +0x00000-0x00018     |
 * |     core         |  +0x3C000-0x4A7FF     |
 * | FIFO RAM (Debug) |  +0x5C000-0x5C7FF     |
 *
 * @note    These are the only registers of the System part
 *
 * | Offset  |  Name        | Type  |  Description                      |
 * |---------|--------------|-------|-----------------------------------|
 * |   +0    | USB_CTRL     | RW    | System Control Register           |
 * |   +4    | USB_STATUS   | R     | System Status Register            |
 * |   +8    | USB_IF       | R     | Interrupt Flag Register           |
 * |  +12    | USB_IFS      | W1    |  Interrupt Flag Set Register      |
 * |  +16    | USB_IFC      | W1    |  Interrupt Flag Clear Register    |
 * |  +20    | USB_IEN      | RW    |  Interrupt Enable Register        |
 * |  +24    | USB_ROUTE    | RW    |  I/O Routing Register             |

 * @note    Convention for register naming
 *
 * | Function           |  Register names          |
 * |--------------------|--------------------------|
 * |   Host             |  USB_H*                  |
 * |   Device OUT       |  USB_DOEPx*              |
 * |   Device IN        |  USB_DIEPx*              |
 * |   Device           |  USB_D*                  |
 * |   Global           |  USB_G*                  |
 * |   Host channel     |  USB_HCx*                |
 * |   FIFO             |  USB_FIFOnDm             |
 * |   FIFO RAM (Debug) |  USB_FIFO*               |
 *
 * @note    Registers in System part not using name convention
 *
 * |  Register          |  Description                                 |
 * |--------------------|----------------------------------------------|
 * |   USB_PCGCCTL      | Power and Clock Gating Control Register      |
 *
 *
 *
 * @note    There are four header files related to USB in the EFM32 MCU
 *          headers
 *
 * | File                | Contents                                    |
 * |---------------------|---------------------------------------------|
 * | efm32gg_usb.h       | All registers from CTRL to  FIFORAM         |
 * | efm32gg_usb_hc.h    | Registers from DMA host channel             |
 * | efm32gg_usb_diep.h  | Registers for IN endpoints                  |
 * | efm32gg_usb_doep.h  | Registers for OUT endpoints                 |
 *
 *
 * | Structure           | File                                        |
 * |---------------------|---------------------------------------------|
 * | USB_TypeDef         | efm32gg_usb.h                               |
 * | All symbols         | efm32gg_usb.h                               |
 * | USB_HC_Typedef      | efm32gg_usb_hc.h                            |
 * | USB_DIEP_TypeDef    | efm32gg_usb_diep.h                          |
 * | USB_DOEP_TypeDef    | efm32gg_usb_doep.h                          |
 *
 * @note    For the EFM32GG990F1024
 *
 *  | Symbol        |  Defined as     | Description                    |
 *  |---------------|-----------------|--------------------------------|
 *  | USB_IRQn      |               5 |                                |
 *  | USB_BASE      |    0x400C4000UL | Register area                  |
 *  | USBC_MEM_BASE |    0x40100000UL | USB Core memory base address   |
 *  | USBC_MEM_END  |    0x4013FFFFUL | USB Core memory end address    |
 *  | USBC_MEM_SIZE |       0x40000UL | USB Core memory size  (=256 KB)|
 *  | USBC_MEM_BITS |          0x18UL | Used bits                      |
 *  | USB_PRESENT   |       (defined) |                                |
 *  | USB_COUNT     |               1 | #units                         |
 *  | USBC_PRESENT  |       (defined) |                                |
 *  | USBC_COUNT    |               1 | #units                         |
 *
 *
 * @note USB builtin voltage regulator
 *
 * There is a 5V to 3.3 voltage regulator in the USB interface.
 * The input is VREGI (5V) and the output is VREGO (3.3 V).
 * As default, it powers the PHY unit.
 *
 * There are two fields in the CTRL register that controls it operation.
 *
 *   * VREGDIS: Disables regulator
 *   * VREGOSEN: Enables voltage sensing
 *
 * @note Device configuration (RM Section 15.3.2)
 *
 * * Bus powered devices
 *   The VDD pins of the EFM32 is connected to the VREGO. So, the EFM32 is
 *   powered only when the device is connected to a USB host. VREGI is
 *   connected to VBUS. There is no need to monitor the VREGO and VREGOSEN
 *   should be disabled.
 *
 * * Self powered device
 *   The device has a 3.3 V power supply (e.g. a battery) for the EFM32.
 *   The EFM32 is running all the time. VREGI is  connected to VBUS and the
 *   PHY is only powered when the device is connected to a host.
 *   The VREGO must be monitored to detect the plug and unplug events.
 *
 * * Self powered device with a switch.
 *   Similar to the "Self powered device" case. The device has a 3.3 V power
 *   supply (e.g. a battery) for the EFM32. The EFM32 is running all the time.
 *   VREGI is  connected to VBUS and the PHY is only powered when the device is
 *   connected to a host. But there is a switch, controlled by a GPIO pin,
 *   that controls who is in charge to power the EFM32. The sources are a
 *   3.3 V supply or VREGI. So it is possible to extend battery life, draining
 *   when possible, power from the USB connection.  The VREGO must be monitored
 *   to detect the plug and unplug events.
 *
 * * Host
 *   A 5V power supply is needed (stepup regulator, battery, or power adapter).
 *   A signal VBUSEN is used to automatically power the VBUS. It is desirable
 *   to have an overcurrent detector. VREGI should be connected to 5 V and the
 *   VREGO can be used to power the EFM32. There is no need to monitor VREGO,
 *   so VREGOSEN should be disabled.
 *
 * * OTG (Dual role device) with 5 V supply
 *   The device must have a 5V power supply in order to be a host. The 5V must
 *   be connected to VREGI. VREGO is a 3.3V power supply, that can power the
 *   EFM32 and possibly, other devices. A signal VBUSEN drives a switch that
 *   connects or not, the 5V supply to VBUS when the device must operates as
 *   a host. The VREGO monitor should be disabled.  It is recommend that to
 *   have an overcurrent detector.
 *
 * * OTG (Dual role device) with a 3.3 V supply
 *   It is very similar to the OTG case.Instead of a switch, a step-up
 *   regulator is used. The EFM32 is powered by the 3.3 V power supply.
 *   The voltage regulador must be disabled (VREGDIS=1).
 *
 * * STK3700 configuration
 *   The board has a 5V power supply line and a switch controlled by VBUSEN
 *   is used to connect it to VBUS. The switch used can generate a
 *   overcurrent signal (connected to PF6).
 *   There is a mechanical switch that controls who powers the 3.3 V line.
 *   The alternatives are:
 *   - VBUS: From VREGO.
 *   - DBG: 3.3 V derived from 5 V at the Debug USB connector
 *   - BAT: a 3.0 V 2032 coin cell battery.
 *
 *
 * @author Hans

 * @version 0.1
 * @date 2021-11-19
 *
 */

#include "em_device.h"
#include "clock_efm32gg2.h"
#include "gpio.h"
#include "usb_device.h"
#include "usb_packets.h"
#include "usb_descriptors.h"
#include "usbcdc_descriptors.h"


/*
 * Should be in clock_efm32*.*
 */
#define USB_CORECLOCKFREQUENCY   48000000U

/**
 * @brief  USB  Device Information
 *
 */
USB_DevInfo_t USB_DevInfo = {
    .state       = USB_STATE_DISABLED,
    .saved_state = USB_STATE_DISABLED,
    .last_state  = USB_STATE_DISABLED,
    .power_status= USB_STATE_UNDEFINED,
};




/**
 * @brief USB Tick routine
 *
 * @note  Must be called every 1 ms
 */
static volatile uint32_t usb_tick = 0;

void USB_Tick(void) {
    usb_tick++;
}


/**
 * @brief Delay routine
 *
 * @param ms: delay in milliseconds
 *
 * @note  Do not use it when processing an interrupt
 */

void USB_Delay(unsigned ms) {
    usb_tick = 0;
    while ( usb_tick < ms ) {}
}


/**
 * @brief Simple timer for very small intervals
 *
 * @param n
 *
 * @note  Not calibrated
 */
static void USB_Timer_us(register volatile int n) {
    while( n-- ) {}
}


#define USB_CLKSRC_HCLKNODIV                0
#define USB_CLKSRC_LFXO                     1
#define USB_CLKSRC_LFRCO                    2




// Forward references
void USB_IRQ_Processing(void);
void USB_FaultIRQ_Processing(void);


#ifdef USB_IMPLEMENTS_GPIO_IRQ_ROUTINE
#if (USB_GPIO1PIN_FAULT%2)==0
void GPIO_EVEN_IRQHandler(void) {

    // TODO
}
#else
void GPIO_ODD_IRQHandler(void) {

    // TODO
}
#endif
#endif

/**
 * @brief Set clock source for USB interface
 *
 * @note  It does not changes the HCLK and derived HFCORECLK and HFPERCLK
 *        signals.
 *
 * @note  USB only works when the clock is 48 MHz. So the HCLK MUST be 48 MHz.
 *        When in suspended mode, it is possible to run USB with a 32768 Hz.
 *
 * @return 0 if OK, negative value when an error occurs
 */

static int USB_SetClockCoreSource(uint32_t clocksource) {
uint32_t clksrc;

    switch(clocksource) {
    case USB_CLKSRC_HCLKNODIV:
        clksrc = CMU_CMD_USBCCLKSEL_HFCLKNODIV;
        break;
    case USB_CLKSRC_LFXO:
        clksrc = CMU_CMD_USBCCLKSEL_LFXO;
        break;
    case USB_CLKSRC_LFRCO:
        clksrc = CMU_CMD_USBCCLKSEL_LFRCO;
        break;
    default:
        return -1;
    }
    CMU->CMD = clksrc;
    return 0;
}

/**
 * @brief Configure USB System Clock
 *
 * @return int
 */
static int USB_EnableSystemClock(void) {

    // Set system clock
    CMU->HFCORECLKEN0 |= CMU_HFCORECLKEN0_USB;

    return 0;
}

/**
 * @brief Configure USB Core Clock
 *
 * @note  It changes the source of the HCLK clock signal!!!!!!!!
 *        And its derived clocks signals: HFCORECLK and HFPÈRCLK.
 *
 * @return int
 */
static int USB_EnableCoreClock(void) {

    // HFCLK must be driven by HFXO
    if( (CMU->STATUS&CMU_STATUS_HFXOSEL)==0 ) {
        return -1;
    }
    // Set clock source to HCLKNODIV
    USB_SetClockCoreSource(USB_CLKSRC_HCLKNODIV);

    CMU->HFCORECLKEN0 |= CMU_HFCORECLKEN0_USBC;

    return 0;
}

#define BIT(N) (1UL<<(N))


#define USB_GPIO1PIN_VBUSEN_MASK       BIT(USB_GPIO1PIN_VBUSEN)
#define USB_GPIO1PIN_VBUSEN_MASK       BIT(USB_GPIO1PIN_VBUSEN)

/**
 * @brief USB Core Reset
 *
 * @note  USB core clock must be enabled
 */
void USB_ResetCore(void) {

    // Set the Power and Clock Gating to default (USB running)
    USB->PCGCCTL &= ~( USB_PCGCCTL_STOPPCLK
                      |USB_PCGCCTL_PWRCLMP
                      |USB_PCGCCTL_RSTPDWNMODULE);

    // Software reset
    USB->GRSTCTL |= USB_GRSTCTL_CSFTRST;
    // It is a self clearing bit. Wait until cleared
    while( (USB->GRSTCTL&USB_GRSTCTL_CSFTRST) != 0 ) {}
    // Wait until it enters IDLE state
    while( (USB->GRSTCTL&USB_GRSTCTL_AHBIDLE) == 0 ) {}

}

/**
 * @brief Initialization of the USB System part
 *
 */
static void USB_InitSystemPart(uint32_t conf) {

    // Enable clock for Core part
    USB_EnableSystemClock();

#ifdef USB_USE_VBUSEN
    // Configure VBUSEN to push-pull and its polarity
    GPIO_Init(GPIOF,0,USB_GPIO1PIN_VBUSEN_MASK);
    GPIO_ConfigPins(GPIOF,USB_GPIO1PIN_VBUSEN_MASK,GPIO_MODE_PUSHPULL);
    // Change VBUSEN polarity
    if( (conf&USB_CONF_VBUSENPOLARITY) != 0 ) {
        USB->CTRL |= USB_CTRL_VBUSENAP;
    }
    // Enable PHY pins and, if needed, the VBUSEN
    USB->ROUTE = USB_ROUTE_PHYPEN|USB_ROUTE_VBUSENPEN;
#else
    // Enable PHY pins
    USB->ROUTE = USB_ROUTE_PHYPEN;
#endif

    // PHY is powered from VBUS
    USB->CTRL |= USB_CTRL_VREGOSEN;

    // Enable VBUS when SELFPOWERED.
    if( (conf&USB_CONF_BUSPOWERED) != 0 ) {
        USB->ROUTE |= USB_ROUTE_VBUSENPEN;
    }
}


/**
 * @brief Initialization of the USB Core part (Host or Device or OTG)
 *
 * @note  Initialization procedure (RM Section 15.4.1)
 *
 * 1. Program the following fields in the Global AHB Configuration (USB_GAHBCFG)
 *    register.
 *    * DMA Mode bit
 *    * AHB Burst Length field
 *    * Global Interrupt Mask bit = 1
 *    * Non-periodic TxFIFO Empty Level (can be enabled only when the core is
 *      operating in Slave mode as a host.)
 *    * Periodic TxFIFO Empty Level (can be enabled only when the core is
 *      operating in Slave mode)
 * 2. Program the following field in the Global Interrupt Mask (USB_GINTMSK)
 *    register:
 *    * USB_GINTMSK.RXFLVLMSK = 0
 * 3. Program the following fields in USB_GUSBCFG register.
 *    * HNP Capable bit
 *    * SRP Capable bit
 *    * External HS PHY or Internal FS Serial PHY Selection bit
 *    * Time-Out Calibration field
 *    * USB Turnaround Time field
 * 4. The software must unmask the following bits in the USB_GINTMSK register.
 *    * OTG Interrupt Mask
 *    * Mode Mismatch Interrupt Mask
 * 5. The software can read the USB_GINTSTS.CURMOD bit to determine whether
 *    he core is operating in Host or Device mode. The software the follows
 *    either the Section 15.4.1.1 (p. 250) or Device Initialization (p. 251)
 *    sequence.
 */
static void USB_InitCorePart(uint32_t conf) {

    // TODO
}

/**
 * @brief USB Reset procedure to be run when device is reset
 *
 * @note  Should be processed
 *
 * @note  Procedure to be followed according RM Section 15.4.4.1.1
 *
 * 1. Set the NAK bit for all OUT endpoints
 *    * USB_DOEPx_CTL.SNAK = 1 (for all OUT endpoints)
 * 2. Unmask the following interrupt bits:
 *    * USB_USB_DAINTMSK.INEP0 = 1 (control 0 IN endpoint)
 *    * USB_USB_DAINTMSK.OUTEP0 = 1 (control 0 OUT endpoint)
 *    * USB_DOEPMSK.SETUP = 1
 *    * USB_DOEPMSK.XFERCOMPL = 1
 *    * USB_DIEPMSK.XFERCOMPL = 1
 *    * USB_DIEPMSK.TIMEOUTMSK = 1
 * 3. To transmit or receive data, the device must initialize more registers as
 *    specified in Device DMA/Slave Mode Initialization (p. 287).
 * 4. Set up the Data FIFO RAM for each of the FIFOs
 *    * Program the USB_GRXFSIZ Register, to be able to receive control OUT
 *      data and setup data. At a minimum, this must be equal to
 *      1 max packet size of control endpoint 0 + 2 DWORDs (for the status of
 *      the control OUT data packet) + 10 DWORDs (for setup packets).
 *    * Program the Device IN Endpoint Transmit FIFO size register (depending
 *      on the FIFO number chosen), to be able to transmit control IN data.
 *      At a minimum, this must be equal to 1 max packet size of control
 *      endpoint 0.
 * 5. Program the following fields in the endpoint-specific registers for
 *    control OUT endpoint 0 to receive a SETUP packet
 *    * USB_DOEP0TSIZ.SUPCNT = 3 (to receive up to 3 back-to-back SETUP packets)
 *    * In DMA mode, USB_DOEP0DMAADDR register with a memory address to store
 *      any SETUP packets received.
 *
 * @note After it, all initialization required to receive SETUP packets is done,
 *       except for enabling control OUT endpoint 0 in DMA mode.
 */
static void USB_InitAtReset(void) {

}


/**
 * @brief USB Initialization as device
 *
 * @note  Internal routine
 *
 * @note  Initialization procedure according RM Section 15.4.1.2
 *
 * 1. Program the following fields in USB_DCFG register.
 *    * Device Speed
 *    * Non-Zero-Length Status OUT Handshake
 *    * Periodic Frame Interval
 * 2. Program the USB_GINTMSK register to unmask the following interrupts.
 *    * USB Reset
 *    * Enumeration Done
 *    * Early Suspend
 *    * USB Suspend
 * 3. Wait for the USB_GINTSTS.USBRST interrupt, which indicates a reset has
 *    been detected on the USB and lasts for about 10 ms. On receiving this
 *    interrupt, the application must perform the steps listed in
 *    Initialization on USB Reset (p. 285)
 * 4. Wait for the USB_GINTSTS.ENUMDONE interrupt. This interrupt indicates
 *    the end of reset on the USB. On receiving this interrupt, the application
 *    must read the USB_DSTS register to determine the enumeration speed and
 *    perform the steps listed in Initialization on Enumeration Completion
 *    (p. 285)
 *
 * @note At this point, the device is ready to accept SOF packets and perform
 *    control transfers on control endpoint 0.
 *
 */


/**
 * @brief Initialize USB for operation as device
 *
 * @note  It tests the clock clock frequency, because USB needs
 *        a 48 MHz clock.
 *
 * @note  USB Initialization according RM 15.3.1
 *
 *        1.  Enable the clock to the system part by setting USB in
 *            CMU_HFCORECLKEN0.
 *        2.  If the internal USB regulator is bypassed (by applying 3.3V on
 *            USB_VREGI and USB_VREGO externally), disable the regulator by
 *            setting VREGDIS in USB_CTRL.
 *        3.  If the PHY is powered from VBUS using the internal regulator,
 *            the VREGO sense circuit should be enabled by setting VREGOSEN in
 *            USB_CTRL.
 *        4.  Enable the USB PHY pins by setting PHYPEN in USB_ROUTE.
 *        5.  If host or OTG dual-role device, set VBUSENAP in USB_CTRL to the
 *            desired value and then enable the USB_VBUSEN pin in USB_ROUTE.
 *            Set the MODE for the pin to PUSHPULL.
 *        6.  If low-speed device, set DMPUAP in USB_CTRL to the desired value
 *            and then enable the USB_DMPU pin in USB_ROUTE. Set the MODE for
 *            the pin to PUSHPULL.
 *        7.  Make sure HFXO is ready and selected. The core part requires the
 *            undivided HFCLK to be 48 MHz when USB is active (during
 *            suspend/session-off a 32 kHz clock is used)..
 *        8.  Enable the clock to the core part by setting USBC in
 *            CMU_HFCORECLKEN0.
 *        9.  Wait for the core to come out of reset. This is easiest done by
 *            polling a core register with non-zero reset value until it reads
 *            a non-zero value. This takes approximately 20 48-MHz cycles.
 *        10. Start initializing the USB core as described in USB Core
 *            Description.
 *
 * @param conf
 * @return int
 */

int USB_Init(uint32_t conf) {
ClockConfiguration_t clockconf;

    // No Low Speed as a device on the STK3700
    if( conf&USB_CONF_LOWSPEED )
        return -1;

    // Get Clock configuration and test it
    ClockGetConfiguration(&clockconf);
    if( clockconf.basefreq != USB_CORECLOCKFREQUENCY )
        return -2;

    // Enable clock for USB interface
    USB_EnableSystemClock();

    // Initialization of System part
    USB_InitSystemPart(conf);


    // Enable USB Core Clock
    USB_EnableCoreClock();

    // Wait until core comes out of reset.
    // A register with non-zero reset value is read a zero until
    // core comes out of reset
    while( USB->IF==0 ) {}

    // Reset USB Core
    USB_ResetCore();

    // Initializing the USB Core
    USB_InitCorePart(conf);

    // Both Host and Device



    /*
     * Device specific initialization
     */

    /*
     * 1. Program the following fields in USB_DCFG register.
     *    * Device Speed
     *    * Non-Zero-Length Status OUT Handshake
     *    * Periodic Frame Interval
     */

// TODO Just to compile
#define DEVADDR (0)


    uint32_t dcfg = USB->DCFG;
    // Clear fields
    dcfg &= ~(_USB_DCFG_DEVSPD_MASK|_USB_DCFG_DEVADDR_MASK|_USB_DCFG_PERFRINT_MASK);
    // Set speed (must be HS)
    dcfg |= (3<<_USB_DCFG_DEVSPD_SHIFT);
    // Set device address
    dcfg |= (DEVADDR<<_USB_DCFG_DEVADDR_SHIFT);
    // Set Periodic Frame Interval
    // TODO ????????
    // Set Non-Zero-Length Status OUT Handshake
    dcfg |= USB_DCFG_NZSTSOUTHSHK;
    // Set register
    USB->DCFG = dcfg;

    /*
     * 2. Program the USB_GINTMSK register to unmask the following interrupts.
     *    * USB Reset
     *    * Enumeration Done
     *    * Early Suspend
     *    * USB Suspend
     */
    uint32_t gintmsk = USB->GINTMSK;



    /*
     * 3. Wait for the USB_GINTSTS.USBRST interrupt, which indicates a reset
     *    has been detected on the USB and lasts for about 10 ms. On receiving
     *    this interrupt, the application must perform the steps listed in
     *    Initialization on USB Reset (p. 285)
     */

    /*
     * 4. Wait for the USB_GINTSTS.ENUMDONE interrupt. This interrupt
     *    indicates the end of reset on the USB. On receiving this interrupt,
     *    the application must read the USB_DSTS register to determine the
     *    enumeration speed and perform the steps listed in Initialization on
     *    Enumeration Completion (p. 285)
     */


    // Clear debug flag if not in debug mode. By default, the debug flag is set
    if( (conf&USB_CONF_DEBUG) == 0 ) {
        USB->GINTSTS &= USB_GINTSTS_SOF;
    }
}




/**
 * @brief USB_Stop
 *
 * @note  Stop all operations, turn off pullup resistors, interrupts and pins
 */

void USB_Stop(void) {

    // Disconnect all

    // Disable IRQ
    NVIC_DisableIRQ(USB_IRQn);

    // Disable Global Interrupts
    USB->GAHBCFG &= ~USB_GAHBCFG_GLBLINTRMSK;

    // Disable USB interrupts
    USB->IEN = _USB_IEN_RESETVALUE;

    // Reset pins to default, effectively disabling them
    USB->ROUTE = _USB_ROUTE_RESETVALUE;

    // Turn off clocks
    CMU->HFCORECLKEN0 &= ~(CMU_HFCORECLKEN0_USB | CMU_HFCORECLKEN0_USBC);

}

/**
 * @brief  Shut down USB and enter
 *
 */

static void USB_PowerDown(void) {

    // TBD

}


/**
 * @brief Adjust clock when entering interrupt
 *
 * @note  When running at 32 KHz, switch to 48 MHz
 *
 */
void AdjustClock(void) {


}

/**
 * @brief  Set State
 *
 */
void USB_SetState( USB_State_t state) {

    if( state == USB_DevInfo.state )
        return;
    if( state == USB_DevInfo.last_state )
        return;
    USB_DevInfo.last_state = USB_DevInfo.state;
    USB_DevInfo.state = state;
}





/**
 * @brief  Get State
 *
 */

USB_State_t
USB_GetState(void) {

    return USB_DevInfo.state;
}


/**
 * @brief   Save State
 *
 */

void USB_SaveState(void) {

    if( USB_DevInfo.state == USB_DevInfo.saved_state )
        return;

    USB_DevInfo.saved_state = USB_DevInfo.state;

}

/**
 * @brief  Restore State
 *
 */

void USB_RestoreState(void) {

    USB_DevInfo.state = USB_DevInfo.saved_state;
}

/**
 * @brief  Get Last State
 *
 */
USB_State_t
USB_GetLastState(void) {

    return USB_DevInfo.last_state;
}



/**
 * @brief Process VBUS Interrupts
 *
 * @note  The are two interrupts related to VBUS\: VREGOSH and VREGOSL.
 *
 */
void ProcessVBUSInterrupt(void) {

    // If using internal regulator
    if ( USB->CTRL & USB_CTRL_VREGOSEN ) {

        // Test if VBUS > 1.8 V
        if( (USB->IF&USB_IF_VREGOSH)!=0 ) {
            // Clear interrupt flag
            USB->IFC = USB_IFC_VREGOSH;
            USB_DevInfo.power_status = USB_POWER_UP;
#if 0
            if( (USB->STATUS&USB_STATUS_VREGOS) != 0 ) {
                USB_SetState(USB_STATE_POWERED);
            }
#endif
        }
        // Test if VBUS < 1.8 V
        if( (USB->IF&USB_IF_VREGOSL)!=0 ) {
            // Clear interrupt flag
            USB->IFC = USB_IFC_VREGOSL;
            USB_DevInfo.power_status = USB_POWER_DOWN;
#if 0
            if( (USB->STATUS&USB_STATUS_VREGOS) != 0 ) {
                // Mask all interrupts
                USB->GINTMSK = 0;
                USB->GINTSTS = (uint32_t) (-1);
                // Shut down USB interface
                USB_PowerDown();
                // Set state to initial
                USB_SetState(USB_STATE_INITIAL);
            }

#endif
        }
    }

}


/**
 * @brief Process Reset Interrupt when in suspended mode
 *
 * @note
 * The RESETDET is asserted when the USB is suspended (EM2)
 * and an USB Reset is detected
 */
void ProcessResetWhenSuspendedInterrupt(void) {

    // TBD
}


/**
 * @brief Process Wakeup Interrupt
 *
 * @note
 * The WKUPINT is asserted when the USB is suspended (EM2)
 * and a Host Initiated Resume is detected
 */
void ProcessWakeupInterrupt(void) {

    // TBD
}


/**
 * @brief  Process USB Suspend Interrupt
 *
 * @note
 * The USBSUSP is asserted when a suspended is detected.
 * The core enters the Suspended state when there is no
 * activity on the bus for an extended period of time.
 */
void ProcessSuspendInterrupt(void) {

    // TBD
}


/**
 * @brief  Process Start Of Frame Interrupt
 *
 * @note
 * The core sets this bit to indicate that an SOF token has been received on
 * the USB. The application can read the Device Status register to get the
 * current frame number. This interrupt is seen only when the core is
 * operating at full-speed (FS). This bit can be set only by the core and
 * the application should write 1 to clear it.
 */
void ProcessStartOfFrameInterrupt(void) {

}


/**
 * @brief Process Enumeration Done Interrupt
 *
 * @note
 * The core sets this bit to indicate that speed enumeration is complete.
 * The application must read the Device Status (USB_DSTS) register to
 * obtain the enumerated speed.
 */
void ProcessEnumerationDoneInterrupt(void) {

}


/*
 * The core sets this bit to indicate that a reset is detected on the USB.
 * Only in device mode.
 */
void ProcessUSBResetInterrupt(void) {

}


/**
 * @brief Process IN Endpoints Interrupts
 *
 * @note
 * The core sets this bit to indicate that an interrupt is pending on one
 * of the IN endpoints of the core (in Device mode). The application
 * must read the Device All Endpoints Interrupt (USB_DAINT) register
 * to determine the exact number of the IN endpoint on Device IN
 * Endpoint-x Interrupt (USB_DIEP0INT/USB_DIEPx_INT) register to
 * determine the exact cause of the interrupt. The application must
 * clear the appropriate status bit in the corresponding
 * USB_DIEP0INT/USB_DIEPx_INT register to clear this bit.
 */
void ProcessINEndpointsInterrupt(void) {
uint32_t ints;
int epn;
uint32_t epmsk;

    if( USB_GetState() == USB_STATE_SUSPENDED ) {
        USB_SetState(USB_STATE_SUSPENDED);   /// TBV
    }

    // Get only IN endpoint interrupts
    ints  = USB->DAINT&USB->DAINTMSK&0xFFFF;

    for(epn=0,epmsk=1;ints;epn++,epmsk<<=1) {
        if( (ints&epmsk) != 0 ) {
            ints &= ~epmsk;

        }
    }


}


/**
 * @brief Process OUT Endpoints Interrupts
 *
 * @note
 * The core sets this bit to indicate that an interrupt is pending on one of
 * the OUT endpoints of the core (in Device mode). The application
 * must read the Device All Endpoints Interrupt (USB_DAINT) register to
 * determine the exact number of the OUT endpoint on which the interrupt
 * occurred, and then read the corresponding Device OUT Endpoint-x
 * Interrupt (USB_DOEP0INT/USB_DOEPx_INT) register to determine the exact
 * cause of the interrupt. The application must clear the appropriate
 * status bit in the corresponding USB_DOEP0INT/USB_DOEPx_INT register
 * to clear this bit.
 */
void ProcessOUTEndpointsInterrupt(void) {

}



/**
 * @brief USB Interrupt routine
 *
 * @note  Processing interrupts when USB interface runs as a device
 */
void USB_IRQHandler(void) {
uint32_t enabledints;


    // CoreEnterAtomic

    // Adjust clock, eventually switching to HFXOCLK
    AdjustClock();

    // Process VBUS interrupts
    ProcessVBUSInterrupt();

    enabledints = USB->GINTSTS&USB->GINTMSK;

    if( enabledints == 0 ) {
        goto exitirqhandler;
    }
    /*
     * The RESETDET is asserted when the USB is suspended (EM2)
     * and an USB Reset is detected
     */

    if( enabledints&USB_GINTSTS_RESETDET ) {
        ProcessResetWhenSuspendedInterrupt();
        enabledints &= ~USB_GINTSTS_RESETDET;
    }

    /*
     * The WKUPINT is asserted when the USB is suspended (EM2)
     * and a Host Initiated Resume is detected
     */

    if( enabledints&USB_GINTSTS_WKUPINT ) {
        ProcessWakeupInterrupt();
        enabledints &= ~USB_GINTSTS_WKUPINT;
    }

    /*
     * The USBSUSP is asserted when a suspended is detected.
     * The core enters the Suspended state when there is no
     * activity on the bus for an extended period of time.
     */

    if( enabledints&USB_GINTSTS_USBSUSP ) {
        ProcessSuspendInterrupt();
        enabledints &= ~USB_GINTSTS_USBSUSP;
    }

    /*
     * The core sets this bit to indicate that an SOF token has been received on
     * the USB. The application can read the Device Status register to get the
     * current frame number. This interrupt is seen only when the core is
     * operating at full-speed (FS). This bit can be set only by the core and
     *  the application should write 1 to clear it.
     */

    if( enabledints&USB_GINTSTS_SOF ) {
        ProcessStartOfFrameInterrupt();
        enabledints &= ~USB_GINTSTS_USBSUSP;
    }

    /*
     * The core sets this bit to indicate that speed enumeration is complete.
     * The application must read the Device Status (USB_DSTS) register to
     * obtain the enumerated speed.
     */

    if( enabledints&USB_GINTSTS_ENUMDONE ) {
        ProcessEnumerationDoneInterrupt();
        enabledints &= ~USB_GINTSTS_ENUMDONE;
    }

    /*
     * The core sets this bit to indicate that a reset is detected on the USB.
     * Only in device mode.
     */

    if( enabledints&USB_GINTSTS_USBRST ) {
        ProcessUSBResetInterrupt();
        enabledints &= ~USB_GINTSTS_USBRST;
    }

    /*
     * The core sets this bit to indicate that an interrupt is pending on one
     * of the IN endpoints of the core (in Device mode). The application
     * must read the Device All Endpoints Interrupt (USB_DAINT) register
     * to determine the exact number of the IN endpoint on Device IN
     * Endpoint-x Interrupt (USB_DIEP0INT/USB_DIEPx_INT) register to
     * determine the exact cause of the interrupt. The application must
     * clear the appropriate status bit in the corresponding
     * USB_DIEP0INT/USB_DIEPx_INT register to clear this bit.
     */

    if( enabledints&USB_GINTSTS_IEPINT ) {
        ProcessINEndpointsInterrupt();
        enabledints &= ~USB_GINTSTS_IEPINT;
    }

    /*
     * The core sets this bit to indicate that an interrupt is pending on one of
     * the OUT endpoints of the core (in Device mode). The application
     * must read the Device All Endpoints Interrupt (USB_DAINT) register to
     * determine the exact number of the OUT endpoint on which the interrupt
     * occurred, and then read the corresponding Device OUT Endpoint-x
     * Interrupt (USB_DOEP0INT/USB_DOEPx_INT) register to determine the exact
     * cause of the interrupt. The application must clear the appropriate
     * status bit in the corresponding USB_DOEP0INT/USB_DOEPx_INT register
     * to clear this bit.
     */

    if( enabledints&USB_GINTSTS_OEPINT ) {
        ProcessOUTEndpointsInterrupt();
        enabledints &= ~USB_GINTSTS_OEPINT;
    }

    int x = 0;
    /*
     * Exiting IRQ Handler
     */
    exitirqhandler:
     //CORE_EXIT_ATOMIC;
    return;
}

/**
 * @brief  USB_Read
 *
 * @note   Get data from host
 *
 * @note   It uses an OUT endpoint
 *
 */

int
USB_Read( uint8_t *data, uint16_t len, uint16_t addr) {



    return 0;
}

/**
 * @brief USB_Write
 *
 * @note  Write
 *
 */

int
USB_Write( uint8_t*data, uint16_t maxlen, uint16_t addr) {



    return 0;
 }
