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

/*
 *@Note
 *Example routine to emulate a simulate USB-CDC Device, USE USART2(PA2/PA3);
 *Please note: This code uses the default serial port 1 for debugging,
 *if you need to modify the debugging serial port, please do not use USART2
*/

//usb
#include "ch32v20x_usbfs_device.h"

//timer
#include "TIM/TIM.h"
//adc
#include "ADC/ADC.h"
//dma
#include "DMA/DMA.h"
//debug
#include "debug.h"

//MACRO
#define DATA_PACK_LEN 3779	 // 12bit adc：1888 * 2 + info:3
#define ADC_SAMPLE_NUM 1888  // 12bit adc: 1888
#define TIM_PERIOD 1905	     // 激光器周期 1905时钟周期
#define TIM_HIGH_WIDTH 5	 // 激光器脉宽 5时钟周期

#define USB_MAX_PACKET_SIZE 63

//global FLAG
volatile uint8_t DMA_FINISH = 0;

uint16_t send_seq = 0;
uint8_t send_flag = 0;

//global data
uint16_t adc_data_buf[ADC_SAMPLE_NUM] = {
	
};

uint16_t adc_calibration = 0;

uint8_t send_data[DATA_PACK_LEN] = {
	
};

//function declare
void USB_Send_Data( uint8_t *buf, uint16_t len );

/*********************************************************************
 * @fn      DataProcess
 *
 * @brief   拼装发送数据	
 * @param 	data - 数据指针
 * @param 	len  - 数据长度 写死为1888
 * @return  none
 */
void DataProcess( uint16_t *data, uint16_t len){
	
	uint8_t point_lenth = 17;
	uint8_t point_width = 111;
	static uint8_t pack_seq = 0;
	uint16_t temp;

	if(DMA_FINISH != 0){
		pack_seq++;
		send_data[0] = pack_seq;
		send_data[1] = point_lenth;
		send_data[2] = point_width;
		for(int i=4, j=4; i < len + 4; i++, j=j+2){
			temp = data[i-4] + adc_calibration;
			if ( temp >= 4095 ){
				send_data[j] = 0x0F;
				send_data[j+1] = 0xFF;
			}
			else if ( temp <= 0 ){
				send_data[j] = 0x00;
				send_data[j+1] = 0x00;
			}
			else{
				send_data[j] = (uint8_t)(temp >> 8);
				send_data[j+1] = (uint8_t)(temp & 0xff);
			}
		}
		/* debug */
		// printf("MESSAGE: \r\n");
		// for(int i=0; i<DATA_PACK_LEN; i++){
		// 	printf("%02x ", send_data[i]);
		// }
		// printf("\r\n");
		//以下printf起延迟作用 不想计算延迟时间了
		DMA_FINISH = 0;
		send_flag = 1; 
	}

}

/*********************************************************************
 * @fn      DataSend
 *
 * @brief   发送数据	
 * 
 * @return  none
 */
void DataSend( void ){

	if(send_flag == 1 && (USBFS_Endp_Busy[DEF_UEP3] == 0x00)){
		if(send_seq < DATA_PACK_LEN / USB_MAX_PACKET_SIZE){
			USB_Send_Data(&send_data[send_seq * USB_MAX_PACKET_SIZE], USB_MAX_PACKET_SIZE);
			send_seq++;
		}else if(send_seq == DATA_PACK_LEN / USB_MAX_PACKET_SIZE){
			USB_Send_Data(&send_data[send_seq * USB_MAX_PACKET_SIZE], DATA_PACK_LEN % USB_MAX_PACKET_SIZE);
			send_seq = 0;
			send_flag = 0;
		}else if(send_seq > DATA_PACK_LEN / USB_MAX_PACKET_SIZE){
			send_seq = 0;
			send_flag = 0;
		}
	}


}

/*********************************************************************
 * @fn      USB_Send_Data
 *
 * @brief   send data to USB	
 *
 * @param   buf - the data buffer.
 * @param   len - the data length.
 * 
 * @return  none
 */

void USB_Send_Data( uint8_t *buf, uint16_t len )
{
	USBFS_Endp_DataUp(DEF_UEP3, buf, len, DEF_UEP_CPY_LOAD);
}

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
	Delay_Init( );
	USART_Printf_Init( 115200 );

	printf("SystemClk:%d\r\n",SystemCoreClock);
	
	RCC_Configuration( );

	/* Tim2 init 采样计时器*/
    TIM2_Init( );
	TIM_Cmd( TIM2, ENABLE );

	/* Tim1 init 脉冲发生器*/
	TIM1_PWMOut_Init( TIM_PERIOD, 0, TIM_HIGH_WIDTH );
	TIM_Cmd( TIM1, ENABLE );

	/* Usart2 init */
    //UART2_Init( 1, DEF_UARTx_BAUDRATE, DEF_UARTx_STOPBIT, DEF_UARTx_PARITY );

    /* USB20 device init */
    USBFS_RCC_Init( );
    USBFS_Device_Init( ENABLE );

	/* DMA init*/
	DMA_P2M_Init( DMA1_Channel1, (u32)&ADC1->RDATAR, (u32)adc_data_buf, ADC_SAMPLE_NUM );
	DMA_IT_Config( DMA1_Channel1, DMA1_Channel1_IRQn );
	DMA_Cmd(DMA1_Channel1, ENABLE);

	/*ADC init*/
	ADC_Function_Init();

	printf("system init ok1\r\n");

	Delay_Ms(5000);
	
	while(1)
	{
		// UART2_DataRx_Deal( );
		// UART2_DataTx_Deal( );
        //sendTxData( USB_Tx_buf, 32 );
		DataProcess(adc_data_buf, ADC_SAMPLE_NUM);
		DataSend();
		
	}
}
