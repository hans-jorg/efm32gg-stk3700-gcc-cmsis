9  Buttons
==========


This is the 1st version of Button. It implements a polling method to read the push buttons. It uses the HAL for LEDs (STK3700) used in the last Blink example.

##Button API (Application Programming Interface)

The buttons are represented as bits in a 32 bit unsigned integer.

The functions implemented are:

    void Button_Init(uint32_t buttons);
    uint32_t Button_Read(void);
    uint32_t Button_ReadChanges(void);
    uint32_t Button_ReadPressed(void);
    uint32_t Button_ReadReleased(void);

As an example, the function *Button_ReadReleased* is shown. There in a static variable called *lastread*, updated every time a read is done. The expression *changes = newread&~lastread* returns 1 in the corresponding bit position when the present read value is 1 and the last read value is 0.

    uint32_t Button_ReadReleased(void) {
    uint32_t newread;
    uint32_t changes;

     newread = GPIOB->DIN;
     changes = newread&~lastread;
     lastread = newread;
     return changes&inputpins;
    }

##Main function

The main function is implemented in a very direct way.

    int main(void) {
    uint32_t b;

    /* Configure LEDs */
    LED_Init(LED1|LED2);
    /* Configure buttons */
    Button_Init(BUTTON0|BUTTON1);

    /* Blink loop */
    while (1) {
        b = Button_ReadReleased();
        if( b&BUTTON1 ) {
            LED_Toggle(LED1);
        }
        if( b&BUTTON2 ) {
            LED_Toggle(LED2);
        }
    }

}
