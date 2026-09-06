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
	osDelay(500);
	key_press_status_t key_result = KEY_SHORT_PRESSED;
    for(;;)
    {
        printf("APP task is living\r\n");
        
        if ( pdTRUE == xQueueReceive(        key_queue,
                                           &key_result,
                                                    0) )
        {
            printf( "key_result receive successfully"
                    " at [%d] tick \r\n", 
                                    HAL_GetTick());
            
            //Send the message to the led
            if( KEY_SHORT_PRESSED == key_result)
            {
                led_operation_t led_value          =       LED_BLINK_1_TIMES;

                if ( NULL == led_queue )
                {
                    printf( "led_queue not created"
                            " at [%d] tick \r\n", 
                                          HAL_GetTick());
                }
                if ( pdTRUE == xQueueSendToFront(          led_queue,
                                                      &( led_value ),
                                                    ( TickType_t )0))
                {
                    printf( "led_queue send led_value successfully"
                            " at [%d] tick \r\n", 
                                          HAL_GetTick());
                }
            }
            if( KEY_LONG_PRESSED == key_result)
            {
                led_operation_t led_value          =       LED_BLINK_10_TIMES;

                if ( NULL == led_queue )
                {
                    printf( "led_queue not created"
                            " at [%d] tick \r\n", 
                                          HAL_GetTick());
                }
                if ( pdTRUE == xQueueSendToFront(          led_queue,
                                                      &( led_value ),
                                                    ( TickType_t )0))
                {
                    printf( "led_queue send led_value successfully"
                            " at [%d] tick \r\n", 
                                          HAL_GetTick());
                }
            }
        }

      osDelay(100);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

void KeyDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  /**     Variables (in task stack)                              **/
    uint32_t           event_index         =                        0;
    uint32_t           frist_trigger_tick  =                        0;
    uint32_t           short_press_time    =         SHORT_PRESS_TIME;
    key_press_event_t  key_press_event     =     
    { 
     .edge_type = RASING,
     .trigger_tick = 0
    };
    /**     Variables (in task stack)                              **/

    /**     Variables (in OS heap )                                **/
    key_queue       = xQueueCreate( 10, sizeof( key_press_status_t ));
    inter_key_queue = xQueueCreate( 10, sizeof( key_press_event_t  ));
    /**     Variables (in OS heap )                                **/

    // check if the queue has been created successfully
    if (NULL == key_queue     || 
        NULL == inter_key_queue)
    {
        printf("key_queue created failed \r\n");
        return;
    } 
    else
    {
        printf("key_queue created successfully \r\n");
    }

    for(;;)
    {
        printf( "key_task_func running "
                " at [%d] tick \r\n",  HAL_GetTick());
        //1. check if there is new data about the key press in the queue
        if( pdTRUE == xQueueReceive(           inter_key_queue,  
                                          &( key_press_event ), 
                                           ( TickType_t ) 0 ) )
        {
            printf("key_press_event.trigger_tick = [%d]\r\n",
                                 key_press_event.trigger_tick);
            printf( "inter_key_queue receive key event "
                    " at [%d] tick \r\n",  HAL_GetTick());
            //1.1 if there is the new data about the key, 
            //then update it in state machine
            if( RASING  == key_press_event.edge_type &&
                0       ==                event_index )
            {
                printf("key RASING fetched! error!");
            }
            if( FAILING == key_press_event.edge_type &&
                0       ==                event_index)
            {
                printf("key FAILING fetched! first\r\n");
                //chang the index for chaging the state machine
                event_index += 1;     

                //Mark the first tick when event coming
                frist_trigger_tick  = key_press_event.trigger_tick;
            }
            if( RASING  == key_press_event.edge_type &&
                1       ==                event_index )
            {
                printf("key RASING after the falling ");
                //1.1.1 if the interval in new key event between two key 
                // is less than 10ms, 
                if ( key_press_event.trigger_tick - frist_trigger_tick < 10)
                {
                    // 1.1.1.1 the new key press event is not valid
                    printf( "Invalid key fetched "
                            " at [%d] tick \r\n",  HAL_GetTick());
                    continue;
                }

                //1.1.2 if the interval in new key event between two key
                // is more than 10ms, then the key press event is valid 
                // 1.1.2.1 the new key press event is valid
                // 1.1.2.1.1 if the interval is less than the short_press time
                // then it should be short press.
                if ( (key_press_event.trigger_tick - frist_trigger_tick) \
                                                           < short_press_time )
                {
                    // 1.1.2.1.1.1 send the short press message to key_queue
                    key_press_status_t key_result = KEY_SHORT_PRESSED;

                    if ( pdTRUE == xQueueSendToFront(        key_queue,
                                                           &key_result,
                                                           0))
                    {
                        printf( "key_result send short press successfully"
                                " at [%d] tick \r\n", 
                                               HAL_GetTick());
                        event_index = 0;
                    }
                    else
                    {
                        printf("key_result send short press failed" 
                               "at [%d] tick \r\n", 
                                         HAL_GetTick());
                    }
                }
                
                
                // 1.1.2.1.2 if the interval is more than the short_press time
                // then it should be long press.
                if ( (key_press_event.trigger_tick - frist_trigger_tick) \
                                                           > short_press_time )
                {
                    // 1.1.2.1.2.1 send the short press message to key_queue
                    key_press_status_t key_result = KEY_LONG_PRESSED;

                    if ( pdTRUE == xQueueSendToFront(        key_queue,
                                                           &key_result,
                                                           0))
                    {
                        printf( "key_result send long press successfully"
                                " at [%d] tick \r\n", 
                                               HAL_GetTick());
                        event_index = 0;
                    }
                    else
                    {
                        printf("key_result send long press failed" 
                               "at [%d] tick \r\n", 
                                         HAL_GetTick());
                    }
                }
                
            }
                

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
		if( pdTRUE == xQueueReceive(                led_queue, 
                                               &( led_value ), 
                                        ( TickType_t ) 0 ) )
		{
			// pcRxedMessage now points to the struct AMessage variable posted
			// by vATask.
            printf("received led_queue value = [%d] at time [%d] \r\n " , 
                                                                     led_value,
                                                                HAL_GetTick());
            led_ret = led_on_off_timer_irq(led_value);
            if ( LED_OK == led_ret )
            {
                printf("led_on_off successfully at time [%d] \r\n", \
                                                               HAL_GetTick() );
            }
		}
	}
	osDelay(100);
  }
  /* USER CODE END StartDefaultTask */
}





/* USER CODE END Application */

