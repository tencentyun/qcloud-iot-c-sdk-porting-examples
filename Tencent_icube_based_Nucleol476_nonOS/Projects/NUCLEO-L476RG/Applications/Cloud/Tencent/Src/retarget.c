/* @brief         : retarget stdin and stdout for printf
******************************************************************************
* @attention
*
* <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
* All rights reserved.</center></h2>
*
* This software component is licensed by ST under BSD 3-Clause license,
* the "License"; You may not use this file except in compliance with the
* License. You may obtain a copy of the License at:
*                        opensource.org/licenses/BSD-3-Clause
*
******************************************************************************
*/


/* Includes ------------------------------------------------------------------*/

#include <stdio.h>
#include <stdint.h>
#include "stm32l4xx_hal.h"

extern UART_HandleTypeDef huart2;
static UART_HandleTypeDef *pDebugUart = &huart2;

#if !defined(__ICCARM__)
struct __FILE {
    int handle;
};
FILE __stdout;
FILE __stdin;
#endif


#if defined(__CC_ARM) ||  defined(__ICCARM__)
int fgetc(FILE * p_file)
{
    uint8_t input;

    if (HAL_UART_Receive(pDebugUart, &input, 1, 0xFFFF) == HAL_OK) {
        return input;
    } else {
        return -1;
    }
}


int fputc(int ch, FILE * p_file)
{

    HAL_UART_Transmit(pDebugUart, (uint8_t *)&ch, 1, 0xFFFF);

    return ch;
}

#elif defined(__GNUC__)


int _write(int file, const char * p_char, int len)
{
    int i;

    for (i = 0; i < len; i++) {
        HAL_UART_Transmit(pDebugUart, (uint8_t *)p_char, 1, 0xFFFF);
    }

    return len;
}


int _read(int file, char * p_char, int len)
{

    HAL_UART_Transmit(pDebugUart, (uint8_t *)p_char, 1, 0xFFFF);

    return 1;
}
#endif
