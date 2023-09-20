I2C
===

Introduction
------------

I2C is an asynchronous protocol that supports multi master communication between a master and devices.

It uses only two lines: Data (SDA) and Clock (SCL).

Due to the pull up resistors on both lines, the normal status is high on both lines.

Setting SDA low while SCL is high indicates a START condition.
Setting SDA high while SCL is high indicates a STOP condition.

On normal operation, SDA only changes when SCL is low.

### Addresses

I2C uses two forms of addressing

* 7-bit addressing. It uses a 7-bit field in the first byte. The 8th bit is the direction bit. 
* 10-bit addressing. It uses 7 bits in a second byte to build a 11 bit addressing field.

Combinations that start with 11110 signal that there are additional address bits in a second byte. There are other reserved address ranges that start with 0000 and 1111.


### Velocity

The velocity can range from 10 Kbps to 1 Gpbs, but the standard speeds are shown in the table below.

| Speed    | Clock frequency  | 
|----------|------------------|
| Standard |   100 KHz        |
| Fast     |   400 KHz        |
| Fast plus|  1000 KHz        |



I2C on EFM32 Giant Gecko MCU
----------------------------

The EMF32GG microcontrollers have two I2C peripherals.

| Unit   |   SDA                        |  SCL                           |
|--------|------------------------------|--------------------------------|
|  I2C0  |PA0 PD6 PC6 PD14 PC0 PF0 PE12 |PA1 PD7 PC7 PD15 PC1 PF1 PE13   |
|  I2C1  |PC4 PB11 PE0                  |PC5 PB12 PE1                    |

The pins are listed ordered by the LOCATION number.

Both support standard, fast and fast plus mode of operation.

The I2C1 can be accessed thru the expansion connectors.

| Signal        | MCU Pin | EXP Pin    |
|---------------|---------|------------|
| I2C1_SDA      |    PC4  |     7      |
| I2C2_SCL      |    PC5  |     9      |




Operation modes
---------------


* Polling
* Interrupt
* Direct Memory Access


Polling mode
------------

The API is show below.

    int I2CMaster_Init(unit,speed,pinconf);
    int I2CMaster_Send(unit,address,data,size);
    int I2CMaster_Received(unit,address,data,maxsize);
    int I2CMaster_Status(unit);
    int I2CMaster_ConfigureClock(unit,speed);


Initialization
--------------


### Clock

	f_SCL  = 1/(T_low+T_high)
	
The formulas below does not work for N less than 3

	T_high = (N_high x (CLKDIV+1))/f_HFPERCLK
	T_low  = (N_low x (CLKDIV+1))/f_HFPERCLK
	
See Table 16.3

### Interrupt

I2C0_IRQn (9)
I2C1_IRQn (10)


### Master/Slave mode

Send START condition.

	i2c->CMD |= I2C_CMD_START;
	

### Enable

	i2c->CTRL |= I2C_CTRL_EN;
	
### Transmit

For transmission, I2C peripherals uses a double buffer. TXDATA and shift registers.

There is an automatic ACK generation controlled by the AUTOACK bit of CTRL register.

See 16.3.7.4 



References
----------

[1] EFM32GG Reference Manual
[2] EFM32GG990 Datasheet
[3] Starter Kit EFM32GG-STK3700 User Manual
[4] I2C Master and Slave Operation. AN0011 - Application Note
