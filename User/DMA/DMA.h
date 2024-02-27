/********************************** (C) COPYRIGHT *******************************
 * File Name          : dma.h
 * Author             : siman
 * Version            : V1.0.0
 * Date               : 2021/02/22
 * Description        : The head of dma conf
*********************************************************************************
*
*******************************************************************************/
#ifndef DMA_H
#define DMA_H
#include "debug.h"

void UART2_DMAInit( uint8_t type, uint8_t *pbuf, uint32_t len );
void DMA_P2M_Init(DMA_Channel_TypeDef *DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize);
void DMA_IT_Config(DMA_Channel_TypeDef *DMA_CHx, IRQn_Type DMA_IRQn);

#endif