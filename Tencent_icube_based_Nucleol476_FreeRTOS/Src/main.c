/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "stdio.h"
#include "main.h"
#include "cmsis_os.h"
#include "board.h"
#include "version.h"
#include "config.h"
#include "app_x-cube-mems1.h"
#include "nucleo_l476rg_errno.h"


/*extern function*/
extern void mqtt_demo_task(void);
extern void icube_data_template_demo_task(void);
extern void light_demo_task(void);
extern void shell_handle(void);

osThreadId exampleTaskHandle;
osThreadId shellTaskHandle;

/* @brief  show memory info.
*
* @retval None
*/
void mem_info(void)
{
#ifdef OS_USED
    printf("\n\rTotal_mem:%d freeMem:%d", configTOTAL_HEAP_SIZE, xPortGetFreeHeapSize());
#endif
}

/*mqtt example for iot platform*/
void mqtt_sample(char *cmd)
{
#define MQTT_TASK_STACK     4096

    printf("mqtt_sample entry\r\n");

#ifdef OS_USED
    osThreadDef(mqttTask, (os_pthread)mqtt_demo_task, osPriorityNormal, 0, MQTT_TASK_STACK);
    exampleTaskHandle = osThreadCreate(osThread(mqttTask), NULL);
    mem_info();
#else
    mqtt_demo_task();
#endif

#undef MQTT_TASK_STACK
}

/*iCube data_template example for iot explorer*/
void icube_data_template_sample(char *cmd)
{
#define DATA_TEMPLATE_TASK_STACK     6144
#ifdef OS_USED
    osThreadDef(data_template_Task, (os_pthread)icube_data_template_demo_task, osPriorityNormal, 0, DATA_TEMPLATE_TASK_STACK);
    exampleTaskHandle = osThreadCreate(osThread(data_template_Task), NULL);
#else
    icube_data_template_demo_task();
#endif
#undef DATA_TEMPLATE_TASK_STACK
}

/*light_data_template_sample for iot explorer*/
void light_data_template_sample(char *cmd)
{
#define LIGHT_DATA_TEMPLATE_TASK_STACK     6144
#ifdef OS_USED
    osThreadDef(light_data_template_Task, (os_pthread)light_demo_task, osPriorityNormal, 0, LIGHT_DATA_TEMPLATE_TASK_STACK);
    exampleTaskHandle = osThreadCreate(osThread(light_data_template_Task), NULL);
#else
    light_demo_task();
#endif
#undef LIGHT_DATA_TEMPLATE_TASK_STACK

}

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */


/* USER CODE BEGIN PV */

/* USER CODE END PV */


/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    BoardInit();

#ifdef USE_IKS01A3_BOARD
    if (BSP_ERROR_NONE != MX_MEMS_Init()) {
        printf("mems sensor of IKS01A3 init fail");
    }
#endif

    printf("\n\r==================================================================================================");
    printf("\n\r======================Tencent iCube example based on NucleoL476 and esp8266 with freeRTOS=========");
    printf("\n\r======================FW Version:%s                            ================================", FW_VERSION_STR);
    printf("\n\r======================Build Time:%s %s             ================================", BUILDING_DATE, BUILDING_TIME);
    printf("\n\r==================================================================================================");
    printf("\n\rBoard init over");
    printf("\n\rSysclk[%d]", HAL_RCC_GetHCLKFreq());

    shell_handle();

#ifdef OS_USED
    /* Start scheduler */
    osKernelStart();
#endif

    /* We should never get here as control is now taken by the scheduler */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1) {
        /* USER CODE END WHILE */
        printf("\n\rsomething goes wrong, wait for reset");
        NVIC_SystemReset();
        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}


/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(char *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
