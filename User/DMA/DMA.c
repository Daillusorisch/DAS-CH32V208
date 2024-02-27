
/********************************** (C) COPYRIGHT *******************************
 * File Name          : dma.c
 * Author             : siman
 * Version            : V1.0.0
 * Date               : 2021/02/22
 * Description        : DMA configuration.
*********************************************************************************
*
*******************************************************************************/
#include "DMA.h"
#include "ch32v20x_dma.h"
#include "main.h"

/********************************WCH FAST INTERRUPT***************************************/
void DMA1_Channel1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));


/*********************************************************************
 * @fn      UART2_DMAInit
 *
 * @brief   Uart2 DMA configuration initialization
 *          type = 0 : USART2_TX
 *          type = 1 : USART2_RX
 *          pbuf     : Tx/Rx Buffer, should be aligned(4)
 *          len      : buffer size of Tx/Rx Buffer
 *
 * @return  none
 */
void UART2_DMAInit( uint8_t type, uint8_t *pbuf, uint32_t len )
{
    DMA_InitTypeDef DMA_InitStructure = {0};

    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_DMA1, ENABLE );

    if( type == 0x00 )
    {
        /* UART2 Tx-DMA configuration */
        DMA_DeInit( DMA1_Channel7 );
        DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART2->DATAR);
        DMA_InitStructure.DMA_MemoryBaseAddr = (u32)pbuf;
        DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
        DMA_InitStructure.DMA_BufferSize = len;
        DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
        DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
        DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
        DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
        DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
        DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
        DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
        DMA_Init( DMA1_Channel7, &DMA_InitStructure );

        DMA_Cmd( DMA1_Channel7, ENABLE );
    }
    else
    {
        /* UART2 Rx-DMA configuration */
        DMA_DeInit( DMA1_Channel6 );
        DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART2->DATAR);
        DMA_InitStructure.DMA_MemoryBaseAddr = (u32)pbuf;
        DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
        DMA_InitStructure.DMA_BufferSize = len;
        DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
        DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
        DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
        DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
        DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
        DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
        DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
        DMA_Init( DMA1_Channel6, &DMA_InitStructure );

        DMA_Cmd( DMA1_Channel6, ENABLE );
    }
}


/*********************************************************************
 * @fn      DMA_P2M_Init
 *
 * @brief   Initializes the DMAy Channelx configuration.
 *
 * @param   DMA_CHx - DMA channel. x can be 1-7 | CAREFUL OF CHANNAL
 * @param   ppadr   - Peripheral base address.
 * @param   memadr  - Memory base address.
 * @param   bufsize - DMA channel buffer size.
 *
 * @return  none
 */
void DMA_P2M_Init(DMA_Channel_TypeDef *DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize)
{
    DMA_InitTypeDef DMA_InitStructure = {0};

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_DeInit (DMA_CHx);
    DMA_InitStructure.DMA_PeripheralBaseAddr = ppadr;
    DMA_InitStructure.DMA_MemoryBaseAddr = memadr;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = bufsize;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA_CHx, &DMA_InitStructure);
}

/*********************************************************************
 * @fn      DMA_IT_Config
 *
 * @brief   Initializes the DMAy Channelx IT configuration.
 *
 * @param   DMA_CHx - DMA channel. x can be 1-7
 * @param   DMA_IRQn - according to your DMA channel
 *
 * @return  none
 */

void DMA_IT_Config(DMA_Channel_TypeDef *DMA_CHx, IRQn_Type DMA_IRQn)
{
    //NVIC_InitTypeDef NVIC_InitStructure = {0};
    // DMA IT Config
    DMA_ITConfig(DMA_CHx, DMA_IT_TC, ENABLE);
    
    // // NVIC configuration
    // NVIC_InitStructure.NVIC_IRQChannel = DMA_IRQn; 
    // NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    // NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    // NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    // NVIC_Init(&NVIC_InitStructure);

    // Enable the 1 global Interrupt
    NVIC_EnableIRQ( DMA_IRQn );

}   //DMA IT config

void DMA1_Channel1_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_IT_TC1))
    {
        DMA_ClearITPendingBit(DMA1_IT_TC1);
        /* STOP ADC */
        ADC_SoftwareStartConvCmd(ADC1, DISABLE);
        /* STOP TIM1 */
        TIM_Cmd(TIM1, DISABLE); 
        /* SET FLAG */
        DMA_FINISH = 1;
    }
}