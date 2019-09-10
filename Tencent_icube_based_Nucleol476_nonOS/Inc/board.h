/* @brief		  : board drivers head file
******************************************************************************
* @attention
*
* <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
* All rights reserved.</center></h2>
*
* This software component is licensed by ST under BSD 3-Clause license,
* the "License"; You may not use this file except in compliance with the
* License. You may obtain a copy of the License at:
*						 opensource.org/licenses/BSD-3-Clause
*
******************************************************************************
*/

#ifndef __BOARD_H_
#define __BOARD_H_

#include "config.h"
#include "stm32l4xx_hal.h"


#define  USE_IKS01A3_BOARD		//use X-Nucleo-IKS01A3 board for sensor data or not
#undef 	 USE_IKS01A3_BOARD	

#ifdef 	USE_IKS01A3_BOARD
#include "iks01a3_env_sensors.h"
#include "iks01a3_motion_sensors.h"
#endif




#define I2C_TIME_OUT				(0x1000)


#define B1_Pin 						GPIO_PIN_13
#define B1_GPIO_Port 				GPIOC
#define USART_TX_Pin 				GPIO_PIN_2
#define USART_TX_GPIO_Port 			GPIOA
#define USART_RX_Pin 				GPIO_PIN_3
#define USART_RX_GPIO_Port 			GPIOA
#define LD2_Pin 					GPIO_PIN_5
#define LD2_GPIO_Port 				GPIOA
#define TMS_Pin 					GPIO_PIN_13
#define TMS_GPIO_Port 				GPIOA
#define TCK_Pin 					GPIO_PIN_14
#define TCK_GPIO_Port 				GPIOA
#define SWO_Pin 					GPIO_PIN_3
#define SWO_GPIO_Port 				GPIOB

typedef enum {
	eLD2_OFF	= 0,
	eLD2_ON  	= 1,
}eLd2StateType;


void SystemClock_Config(void);
void BoardInit(void);
void AT_Uart_Init(void);
void Error_Handler(void);
uint8_t GetButtonPressedCount(void);
void ClearButtonPressedCount(void);
void Led2Ctrl(eLd2StateType type);
eLd2StateType GetLed2State(void);



#endif

