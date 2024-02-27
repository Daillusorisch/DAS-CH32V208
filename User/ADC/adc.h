/********************************** (C) COPYRIGHT *******************************
 * File Name          : ADC.h
 * Author             : siman
 * Version            : V1.0.0
 * Date               : 2024/02/22
 * Description        : This file contains the headers of the operation of dual ADC 
 *                      fast interleaved.
*********************************************************************************
*
*******************************************************************************/

#ifndef __ADC_H__
#define __ADC_H__
#include "debug.h"

u16 Get_ConversionVal1(s16 val);
u16 Get_ConversionVal2(s16 val);
void ADC_Function_Init(void);



#endif