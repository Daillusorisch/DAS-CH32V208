/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#include "TIM.h"
#include "ch32v20x_tim.h"
#include "UART.h"

/****************************WCH FAST INTERRUPT*****************************************/

void TIM2_IRQHandler( void )__attribute__((interrupt("WCH-Interrupt-fast")));
void TIM1_UP_IRQHandler( void )__attribute__((interrupt("WCH-Interrupt-fast")));  

/*********************************************************************
 * @fn      TIM1_PWMOut_Init
 *
 * @brief    高级定时1 创建激光激发波形
 *
 * @param   arr - the period value.
 * @param   psc - the prescaler value.
 * @param   ccp - the pulse value.
 *
 * @return  none
 */
void TIM1_PWMOut_Init(u16 arr, u16 psc, u16 ccp)
{
	GPIO_InitTypeDef GPIO_InitStructure={0};
	TIM_OCInitTypeDef TIM_OCInitStructure={0};
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure={0};

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_TIM1, ENABLE );

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOA, &GPIO_InitStructure );

	TIM_TimeBaseInitStructure.TIM_Period = arr;
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit( TIM1, &TIM_TimeBaseInitStructure);
    
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; // PWM2模式
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = ccp;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//高电平有效
	TIM_OC1Init( TIM1, &TIM_OCInitStructure );

	TIM_CtrlPWMOutputs(TIM1, ENABLE );
	TIM_OC1PreloadConfig( TIM1, TIM_OCPreload_Disable);
	TIM_ARRPreloadConfig( TIM1, ENABLE );

	/* Clear TIM1 update pending flag */
    TIM_ClearFlag( TIM1, TIM_FLAG_Update );
	/* TIM IT enable */
    TIM_ITConfig( TIM1, TIM_IT_Update, ENABLE );
    
    /* Enable Interrupt */
    NVIC_EnableIRQ( TIM1_UP_IRQn );

}


/*********************************************************************
 * @fn      TIM2_Init
 *
 * @brief   Main Timer
 *          144 * 100 * 13.8888 -----> 100uS
 *
 * @return  none
 */
void TIM2_Init( void )
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure = {0};
    //TIM_OCInitTypeDef TIM_OCInitStructure= {0};

    RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2, ENABLE );
    TIM_DeInit( TIM2 );

    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period = 200 - 1;                        // 20ms
    TIM_TimeBaseStructure.TIM_Prescaler = SystemCoreClock / 10000 - 1; // 100us
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit( TIM2, &TIM_TimeBaseStructure );

    // // 配置 TIM2 的 CC2 通道 硬件触发
    // TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
    // TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    // TIM_OCInitStructure.TIM_Pulse = 99; // 设置比较值为计数器的最大值，使得在计数器溢出时比较值匹配
    // TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    // TIM_OC2Init(TIM2, &TIM_OCInitStructure);


    /* Clear TIM2 update pending flag */
    TIM_ClearFlag( TIM2, TIM_FLAG_Update );

    /* TIM IT enable */
    TIM_ITConfig( TIM2, TIM_IT_Update, ENABLE );

    /* Enable Interrupt */
    NVIC_EnableIRQ( TIM2_IRQn );

}



/*********************************************************************
 * @fn      TIM1_UP_IRQHandler
 *
 * @brief   This function handles TIM1 exception.
 *
 * @return  none
 */
void TIM1_UP_IRQHandler( void )
{
    /* Test IO */
    static uint8_t tog1;
    tog1 ? (GPIOA->BSHR = GPIO_Pin_10):(GPIOA->BCR = GPIO_Pin_10);
    tog1 ^= 1;
    /* clear status */

    /* Clear TIM1 update pending flag */
    TIM_ClearFlag( TIM1, TIM_FLAG_Update );
}


/*********************************************************************
 * @fn      TIM2_IRQHandler
 *
 * @brief   This function handles TIM2 exception.
 *
 * @return  none
 */
void TIM2_IRQHandler( void )
{
    /* RESTART DMA */
    DMA_Cmd(DMA1_Channel1, DISABLE); // 首先关闭 DMA
    DMA_SetCurrDataCounter(DMA1_Channel1, 1888); // 设置 DMA 的数据传输数量
    DMA_Cmd(DMA1_Channel1, ENABLE); // 重启DMA
    
    /* START ADC */
    ADC_SoftwareStartConvCmd( ADC1, ENABLE );

    /*START TIM1*/
    TIM_Cmd( TIM1, ENABLE );

    /* Test IO */
    static uint8_t tog;
    tog ? (GPIOA->BSHR = GPIO_Pin_15):(GPIOA->BCR = GPIO_Pin_15);
    tog ^= 1; 

    /* clear status */
    TIM2->INTFR = (uint16_t)~TIM_IT_Update;
}
