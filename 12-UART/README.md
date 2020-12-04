12  Serial Communication using polling
======================================

##Serial interface using USB

Before developing application that uses the serial-USB bridge, it is necessary to update the firmware in the board controller.

##Using Simplicity

1.  With the board disconnected, start Simplicity
2.  If there is a message, suggesting to upgrade, accept it.
3.  Connect the board
4.  In the *Device* panel, right click over the *J-Link Silicon Labs* line and select *Device Configuration*.
5.  Accept the offer to upgrade the firmware.

##Using J-Link

TBD!!!

##Serial interface


                    +------------+                           +-------------------+
                    |  Board     |        Serial             |                   |
    USB ------------| Controller |---------------------------| EFM32GG990F1024   |
    COMx            |            |         TX/RX             |                   |
                    +------------+                           +-------------------+


In Linux, a device named *ttyACM0* appears in the */dev* folder. In Windows, a new COM device appears. There is a permission problem in some Linux machines. There are two ways to solve it. One is add the user to the dialout group with the following command

	sudo usermod -a -G dialout $(USER)

The dialout group can access the serial devices (*/dev/tty\*\* *). Other way is add a file (*50-segger.rules*) with following rules to /etc/udev/rules.d folder.

	KERNEL=="ttyACM[0-9]*",MODE="0666"

The serial interface between EFM32GG and the board controller has a fixed configuration: 115200 bps, 8 bits, no parity, 1 stop bit, no hardware flow control.

The board is wired to use the *PE0 (TX)* and *PE1 (RX)* pins. There is an enable signal on PF7. It must be set to 1 to communication between the EFM32GG and Board Controller happens. It signals a TS3A4751 SPST (Single Pole - Single Throw ) Analog Switch to connect the EFM32GG and the Board Controller.

The I/O pins used for the UART0 can be choosen from three sets (Location) and one of them include PE0 and PE1. Attention: they are controlled by UART0, not USART0.

|      Signal          |     0        |      1       |       2      |
|----------------------|--------------|--------------|--------------|
|       U0_RX          |    PF7       |     PE1      |      PA4     |
|       U0_TX          |    PF6       |     PE0      |      PA3     |



The same device is used to implement a serial bootloader. See [AN0042](https://www.silabs.com/application-notes/an0042-efm32-usb-uart-bootloader.pdf)[13] and [AN0042-KB](https://www.silabs.com/community/mcu/32-bit/knowledge-base.entry.html/2017/09/01/use_an0042_bootloade-N9Zn)[14].

The UART uses the HFPERCLK clock signal, which is derived from the HFClock with a divisor specified by the field *HFPERCLKDIV* in register *CMU_HFPERCLKDIV*. The signal must be enable by setting the *HFPERCLKEN* in register *CMU_HFPERCLKDIV*. The divisor is a power of 2 in the range 1 to 512, specified by a value 0 to 8 in the field *HFPERCLKDIV*.

The HFClock source can be the HFXO (external high frequency crystal oscillator), HFRCO (internal high frequency RC oscillator, the default) or a low frequency source: LFXO (external low frequency crystal oscillator) or LFRCO (internal low frequency RC oscillator).

| Clock source    |   Frequency                                     |
|-----------------|-------------------------------------------------|
|  HFRCO          |  1-28 MHz (default: 14 MHz nominal )            |
|  HFXO           |  4-48 MHz (crystal)                             |
|  LFRCO          |   32768 Hz nominal                              |
|  LFXO           |   32768 Hz (crystal)                            |

The crystal on the STK3700 has a 48 MHz frequency, the maximal frequency of the device.

There is an important note in the Item 13.1 of Application Note *USART/UART - Asynchronous mode* (AN0045).

Often, the HFRCO is too unprecise to be used for communications. So using the HFXO with an external crystal is recommended when using the EFM32 UART/USART.

In some cases, the internal HFRCO can be used. But then careful considerations should be taken to ensure that the clock performance is acceptable for the communication link.

The reason for this note is hidden in the page 24 of the datasheet of the EFM32GG990F1024. RC based oscillators are inherently not very precise. They have a thermal drift, which must be compensated by calibration.

|   Clock source  |  Nominal frequency  |  Minimum frequency    | Maximum frequency  |           |
|-----------------|---------------------|-----------------------|--------------------|           |
|    LFXO         |   32768 Hz          |      31290 Hz         |    34280 Hz        |           |
|    HFRCO        |       1 MHz         |       1.15 MHz        |     1.25 MHz       |           |
|    HFRCO        |       7 MHz         |       6.48 MHz        |     6.72 MHz       |           |
|    HFRCO        |      11 MHz         |       10.8 MHz        |     11.2 MHz       |           |
|    HFRCO        |      14 MHz         |       13.7 MHz        |     14.3 MHz       |  default  |
|    HFRCO        |      21 MHz         |       20.6 MHz        |     21.4 MHz       |           |
|    HFRCO        |      28 MHz         |       27.5 MHz        |     28.5 MHz       |           |


To configure UART0:

1.  Configure pins PE0 and PE1 to be controlled by UART0. Configure UART0 to use LOC1 for RX and TX pins.
2.  Configure oversampling factor 16 in *UART0_CTRL* (higher is better), setting OVS to 00.
3.  Speed is configured by setting the *CLKDIV* field (in *UART0_CLKDIV*) according the formula
                            f_HFPERCLK
        speed =  -------------------------------------
                      Oversampling ( 1 + CLKVDIV/4 )

    or

                    /       f_HFPERCLK            \
        CLKDIV = 4 | ------------------------ - 1 |
                    \  Oversampling x Speed       /
or

4.  The formulas are different from EMF32GG Reference Manual, which use the whole register value. The formulas above use field values.
5.  Configure stop bits setting *STOPBITS* field in *UART0_CTRL* to 01.
6.  Configure 8 bits data setting *DATABITS* field in *UART0_CTRL* to 0101.
7.  Configure no parity by setting *PARITY* field in *UART0_CTRL* to 00.
8.  Enable transmit operations by writing *TXEN* to *UART0_CMD*.
9.  Enable receiving operations by writing *RXEN* to *UART0_CMD*.

To use it in polling mode:

1.  To transmit: test *TXC* in *UART0_STATUS*. If set, write data to *UART0_TXDATA*.
2.  Yo receive: test *RXDATAV* in *UART0_STATUS*. If set, get data from *UART0_RXDATA*.


## ITM Serial interface


## Semihosting

## More information

[EFM32 STK Virtual COM port](https://www.silabs.com/community/mcu/32-bit/knowledge-base.entry.html/2015/07/06/efm32_stk_virtualco-aT2m)[15]

[Using stdio on Silicon Labs platforms](https://os.mbed.com/teams/SiliconLabs/wiki/Using-stdio-on-Silicon-Labs-platforms)[16]
