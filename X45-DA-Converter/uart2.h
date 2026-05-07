/**************************************************************************//**
 * @file    UART.h
 * @brief   UART HAL for EFM32GG STK3200
 * @version 1.0
******************************************************************************/
#ifndef UART_H
#define UART_H

#ifndef UART_BIT
#define UART_BIT(N) (1U<<(N))
#endif

#define UART_TXREADY    UART_BIT(6)
#define UART_RXDATAV    UART_BIT(7)
#define UART_RXFULL     UART_BIT(8)

#define UART_TXENS      UART_BIT(1)
#define UART_RXENS      UART_BIT(0)

void UART_Init(void);
void UART_ReconfigureClock(void);

unsigned UART_GetStatus(void);
void UART_SendChar(char c);
void UART_SendString(char *s);
void UART_PutCharPolling(char ch);


unsigned UART_GetChar(void);
unsigned UART_GetCharNoWait(void);

int UART_Flush(void);


/// Parameter for UART_EnableInterrupts/DisableInterrupts
#define UART_RXINT              0x1
#define UART_TXINT              0x2
void UART_EnableInterrupts(uint32_t m);
void UART_DisableInterrupts(uint32_t m);

#endif // UART_H
