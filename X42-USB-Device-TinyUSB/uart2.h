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

void UART_Init(USART_TypeDef *uart);

unsigned UART_GetStatus(USART_TypeDef *uart);
void UART_SendChar(USART_TypeDef *uart, char c);
void UART_SendString(USART_TypeDef *uart, char *s);
int  UART_Flush(USART_TypeDef *uart);

unsigned UART_GetChar(USART_TypeDef *uart);
unsigned UART_GetCharNoWait(USART_TypeDef *uart);

#define UART_TXINT  0x01
#define UART_RXINT  0x02

#endif // UART_H
