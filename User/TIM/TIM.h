/********************************** (C) COPYRIGHT *******************************
* File Name          : TIM.h
* Author             : siman
* Version            : V1.0.0
* Date               : 2024/02/22
* Description        : 
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/


#ifndef TIM_H
#define TIM_H

#include "debug.h"

void TIM1_PWMOut_Init(u16 arr, u16 psc, u16 ccp);
void TIM2_Init( void );


#endif