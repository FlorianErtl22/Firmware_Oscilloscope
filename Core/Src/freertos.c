/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "app_main.h"
#include "adc_cntrl.h"
#include "communication.h"
#include "protocol.h"
#include "trigger_detection.h"
#include "tim.h"
#include "adc.h"
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

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for vCommandTask */
osThreadId_t vCommandTaskHandle;
const osThreadAttr_t vCommandTask_attributes = {
  .name = "vCommandTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for vTransmitTask */
osThreadId_t vTransmitTaskHandle;
const osThreadAttr_t vTransmitTask_attributes = {
  .name = "vTransmitTask",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for xDataReadySemaphore */
osSemaphoreId_t xDataReadySemaphoreHandle;
const osSemaphoreAttr_t xDataReadySemaphore_attributes = {
  .name = "xDataReadySemaphore"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartTask02(void *argument);

extern void MX_LWIP_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of xDataReadySemaphore */
  xDataReadySemaphoreHandle = osSemaphoreNew(1, 0, &xDataReadySemaphore_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of vCommandTask */
  vCommandTaskHandle = osThreadNew(StartDefaultTask, NULL, &vCommandTask_attributes);

  /* creation of vTransmitTask */
  vTransmitTaskHandle = osThreadNew(StartTask02, NULL, &vTransmitTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the vCommandTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* init code for LWIP */
  MX_LWIP_Init();
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    // App_Loop();
    osDelay(1000);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartTask02 */
/**
* @brief Function implementing the vTransmitTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void StartTask02(void *argument)
{
  /* USER CODE BEGIN StartTask02 */
  for(;;)
  {
    // Wait forever for Python to send 0x02
    osThreadFlagsWait(0x01, osFlagsWaitAny, osWaitForever);

    // Eat any stale semaphore tokens from previous runs
    while (osSemaphoreAcquire(xDataReadySemaphoreHandle, 0) == osOK) {}

    // Free-Run vs Triggered Capture
    if (trigger_params.type == TRIGGER_NONE)
    {
        // Free-Run: Kill watchdogs and instantly transmit
        __HAL_ADC_DISABLE_IT(&hadc1, ADC_IT_AWD1);
        __HAL_ADC_DISABLE_IT(&hadc1, ADC_IT_AWD2);
        tx_data(); 
    }
    else
    {
        // Triggered: Arm the hardware Watchdog
        __HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_AWD1);
        __HAL_ADC_ENABLE_IT(&hadc1, ADC_IT_AWD1);

        // Wait up to 3000ms for the hardware to fire the semaphore
        osStatus_t status = osSemaphoreAcquire(xDataReadySemaphoreHandle, 3000);

        if (status == osOK)
        {
            // --- REAL TRIGGER ---
            uint32_t start_index = (com_params.trigger_index + TX_BUFFER_SIZE - trigger_params.pre_trigger_samples) % TX_BUFFER_SIZE;
            uint32_t elements_part1 = TX_BUFFER_SIZE - start_index;
            uint32_t elements_part2 = start_index;

            if (elements_part1 > 0) {
                HAL_UART_Transmit(&huart3, (uint8_t *)&tx_buf[start_index], (elements_part1 * sizeof(uint16_t)), 10000);
            }
            if (elements_part2 > 0) {
                HAL_UART_Transmit(&huart3, (uint8_t *)&tx_buf[0], (elements_part2 * sizeof(uint16_t)), 10000);
            }
        }
        else 
        {
            // --- 3-SECOND TIMEOUT ---
            HAL_TIM_Base_Stop_IT(&htim4);
            __HAL_ADC_DISABLE_IT(&hadc1, ADC_IT_AWD1);
            __HAL_ADC_DISABLE_IT(&hadc1, ADC_IT_AWD2);
            tx_data();
        }
    }

    // Clear Overrun flag and restart hardware
    __HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_OVR);
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)tx_buf, TX_BUFFER_SIZE);
    HAL_TIM_Base_Start(&htim3);
  }
  /* USER CODE END StartTask02 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

