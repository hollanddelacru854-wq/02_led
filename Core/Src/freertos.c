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
//#include <stdio.h>
#include "bsp_key.h"
#include "queue.h"
#include "bsp_led.h"

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
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

osThreadId_t key_TaskHandle;
const osThreadAttr_t key_Task_attributes = {
  .name = "key_Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};


osThreadId_t led_TaskHandle;
const osThreadAttr_t led_Task_attributes = {
  .name = "led_Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};




//*********************Queue_Handler ********************//
QueueHandle_t key_queue;
QueueHandle_t led_queue;


//*********************Queue_Handler ********************//



void KeyDefaultTask(void *argument);

void LedDefaultTask(void *argument);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

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

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
//  key_queue = xQueueCreate( 10, sizeof( uint32_t ) );
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  key_TaskHandle = osThreadNew(KeyDefaultTask, NULL, &key_Task_attributes);
  led_TaskHandle = osThreadNew(LedDefaultTask, NULL, &led_Task_attributes);
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  uint32_t         received_value =       0;
  led_operation_t   led_ops_value =  LED_ON;
  /* Infinite loop */
  for(;;)
  {
    printf("StartDefaultTask\r\n");
    if( key_queue != 0 )
	{
		// Receive a message on the created queue.  Block for 10 ticks if a
		// message is not immediately available.
		if( xQueueReceive( key_queue, &( received_value ), ( TickType_t ) 100 ) )
		{
			// pcRxedMessage now points to the struct AMessage variable posted
			// by vATask.
            printf("received queue value = [%d]" , received_value);
            
            
            // send the operation to LED
            led_ops_value = LED_TOGGLE;
            if ( pdTRUE == xQueueSendToFront(led_queue,&led_ops_value,0))
            {
                printf("led send successfully\r\n");
            }
            
		}
	}
    
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

void KeyDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  key_status_t  ket_ret          =          KEY_OK;
  key_press_status_t key_statues = KEY_NOT_PRESSED;
  key_queue = xQueueCreate( 10, sizeof( uint32_t ) );
  uint32_t counter_tick = 0;
  if (NULL == key_queue )
  {
      printf("key_queue created failed \r\n");
  } 
  else
  {
      printf("key_queue created successfully \r\n");
  }
  for(;;)
  {
    counter_tick++;
    //printf("Hellow Key thread\r\n");
    
    ket_ret = key_scan(&key_statues);
    
    if( KEY_OK == ket_ret)
    {
        if ( KEY_PRESSED == key_statues )
        {
            printf("Key_Pressed\r\n");
            if ( pdTRUE == xQueueSendToFront(key_queue,&counter_tick,0))
            {
                printf("send successfully\r\n");
            }
        }
    }
    if( KEY_OK != ket_ret)
    {
        printf("Key_not_Pressed\r\n");
    }        
    osDelay(100);
	
  }

  /* USER CODE END StartDefaultTask */
}



void LedDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  led_status_t      led_ret          =          LED_OK;
  led_operation_t led_value          =          LED_ON;
  led_queue = xQueueCreate( 10, sizeof( led_operation_t ) );
  if (NULL == led_queue )
  {
      printf("led_queue created failed \r\n");
  } 
  else
  {
      printf("led_queue created successfully \r\n");
  }
  for(;;)
  {
    printf("Hello led thread\r\n");
    
    if( led_queue != 0 )
	{
		// Receive a message on the created queue.  Block for 10 ticks if a
		// message is not immediately available.
		if( xQueueReceive( led_queue, &( led_value ), ( TickType_t ) 100 ) )
		{
			// pcRxedMessage now points to the struct AMessage variable posted
			// by vATask.
            printf("received led_queue value = [%d]" , led_value);
            led_on_off(led_value);
		}
	}
    
    osDelay(100);
  }
  /* USER CODE END StartDefaultTask */
}





/* USER CODE END Application */

