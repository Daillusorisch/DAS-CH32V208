/********************************** (C) COPYRIGHT *******************************
 * File Name          : adc.c
 * Author             : siman
 * Version            : V1.0.0
 * Date               : 2021/02/22
 * Description        : Init and run or stop dual ADC fast interleaved.
*********************************************************************************
*
*******************************************************************************/

/*
 *@Note
  dual ADC fast interleaved sampling routine:
 ADC1 channel 1 (PA1), ADC2 channel 1 (PA1)), the rule group channel obtains
 dual ADC conversion data through ADC interrupt.

 Note:only applied to CH32V203
*/

#include "ADC.h"
#include "ch32v20x_adc.h"
#include "main.h"

/* Global Variable */
u16 Adc_Val[2];
u32 temp;
s16 Calibrattion_Val1 = 0;
s16 Calibrattion_Val2 = 0;

void ADC1_2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      ADC_Function_Init
 *
 * @brief   Initializes ADC collection.
 *
 * @return  none
 */
void ADC_Function_Init(void)
{
    ADC_InitTypeDef  ADC_InitStructure = {0};
    GPIO_InitTypeDef GPIO_InitStructure = {0};
   //NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);

    RCC_ADCCLKConfig(RCC_PCLK2_Div8);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    ADC_DeInit(ADC1);

    // NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
    // NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    // NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    // NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    // NVIC_Init(&NVIC_InitStructure);

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_InitStructure.ADC_OutputBuffer = ADC_OutputBuffer_Disable;
    ADC_InitStructure.ADC_Pga = ADC_Pga_1;

    ADC_Init(ADC1, &ADC_InitStructure);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_1Cycles5);
    
    //ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE); //EOC中断开启
    ADC_DMACmd(ADC1, ENABLE);   //DMA开启
    ADC_Cmd(ADC1, ENABLE);
    
    ADC_BufferCmd(ADC1, DISABLE); //disable buffer
    
    //ADC校准
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
    adc_calibration = Get_CalibrationValue(ADC1);

}

/*********************************************************************
 * @fn      Get_ConversionVal1
 *
 * @brief   Get Conversion Value.
 *
 * @param   val - Sampling value
 *
 * @return  val+Calibrattion_Val - Conversion Value.
 */
u16 Get_ConversionVal1(s16 val)
{
    if((val + Calibrattion_Val1) < 0)
        return 0;
    if((Calibrattion_Val1 + val) > 4095||val==4095)
        return 4095;
    return (val + Calibrattion_Val1);
}

/*********************************************************************
 * @fn      Get_ConversionVal2
 *
 * @brief   Get Conversion Value.
 *
 * @param   val - Sampling value
 *
 * @return  val+Calibrattion_Val - Conversion Value.
 */
u16 Get_ConversionVal2(s16 val)
{
    if((val + Calibrattion_Val2) < 0)
        return 0;
    if((Calibrattion_Val2 + val) > 4095||val==4095)
        return 4095;
    return (val + Calibrattion_Val2);
}

// /*********************************************************************
//  * @fn      main
//  *
//  * @brief   Main program.
//  *
//  * @return  none
//  */
// int main(void)
// {
//     USART_Printf_Init(115200);
//     Delay_Init();
//     printf("SystemClk:%d\r\n", SystemCoreClock);
//     ADC_Function_Init();
//     printf("CalibrattionValue1:%d\n", Calibrattion_Val1);
//     printf("CalibrattionValue2:%d\n", Calibrattion_Val2);

//     while(1)
//     {
//         ADC_SoftwareStartConvCmd(ADC1, ENABLE);

//         Delay_Ms(500);
//     }
// }

/*********************************************************************
 * @fn      ADC1_2_IRQHandler
 *
 * @brief   ADC1_2 Interrupt Service Function.
 *
 * @return  none
 */
void ADC1_2_IRQHandler()
{
    if(ADC_GetITStatus(ADC1, ADC_IT_EOC))
    {
        temp = ADC1->RDATAR;
        Adc_Val[0] = temp & 0xffff;
        Adc_Val[1] = (temp >> 16) & 0xffff;

        printf("\r\nADC2 ch2=%d\r\n", Get_ConversionVal2(Adc_Val[1]));
        printf("\r\nADC1 ch2=%d\r\n", Get_ConversionVal1(Adc_Val[0]));

        ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
        ADC_ClearITPendingBit(ADC2, ADC_IT_EOC);
    }

}
