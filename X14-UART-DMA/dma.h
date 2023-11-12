#ifndef DMA_H
#define DMA_H

/**
 * @file    dma.h
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


int DMA_Init();



#endif // DMA_H
