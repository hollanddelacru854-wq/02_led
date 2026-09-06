/******************************************************************************
 * Copyright (C) 2024 EternalChip, Inc.(Gmbh) or its affiliates.
 * 
 * All Rights Reserved.
 * 
 * @file key.c
 * 
 * @par dependencies 
 * - bsp_key.h
 * - stdio.h
 * - stdint.h
 * 
 * @author Jack | R&D Dept. | EternalChip 立芯嵌入式
 * 
 * @brief Provide the HAL APIs of Key and corresponding opetions.
 * 
 * Processing flow:
 * 
 * call directly.
 * 
 * @version V1.0 2024-10-30
 *
 * @note 1 tab == 4 spaces!
 * 
 *****************************************************************************/

#include "bsp_key.h"

#define FALING_TYPE 0
#define RASING_TYPE 1


QueueHandle_t        inter_key_queue = NULL;



key_status_t key_scan(key_press_status_t * key_value)
{
    
    
    key_press_status_t key_status_value = KEY_NOT_PRESSED;
    
    
	//2.如果按键（PA0)的电平为低，则说明按键被按下了。
	//2.1  如果按键被按下，则发送对应的消息队列
	if(HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == GPIO_PIN_RESET)
	{
		key_status_value = KEY_PRESSED;
		*key_value = key_status_value;
		return KEY_OK;
	}
    *key_value = key_status_value;
    
    return KEY_ERRORTIMEOUT;//始终没有按键被按下，返回超时
}

key_status_t key_scan_short_long_press(key_press_status_t *  key_value, 
                                       uint32_t      short_press_time)
{
	/**Variables */
    key_status_t       ret_key_status =          KEY_OK;    /* return value  */
    key_press_status_t key_value_temp = KEY_NOT_PRESSED;    /* temporary     */
    uint32_t             counter_tick =               0;    /* counter       */
    /**Variables */

    // 1. check if the key is pressed
    ret_key_status = key_scan(&key_value_temp);
    // TBD : critical section


    // 1.1 if the key is pressed, then check if it is short pressed
    if ( KEY_OK == ret_key_status )
    {
        if ( KEY_PRESSED == key_value_temp )
        {
            // get the timestamp of the first key press
            counter_tick = HAL_GetTick(); 

            // 1.1.1 check the short press time
            while( HAL_GetTick() <  counter_tick + short_press_time )
                ;
            
            // 1.1.2 now could check if the user pressed the key 
            ret_key_status = key_scan(&key_value_temp);
            
            if ( KEY_NOT_PRESSED == key_value_temp )
            {
                // 1.1.3 the key is pressed with short press
                *key_value = KEY_SHORT_PRESSED;
                return KEY_OK;
            } 
            else 
            {
                // 1.2 the key is pressed with long press
                *key_value = KEY_LONG_PRESSED;
                
                // 1.2.1 keep focuing on the status of key to avoid the short
                // press
                while( KEY_OK == key_scan(&key_value_temp) ) // key is pressed
                    ;
                
                return KEY_OK;
            }

        }
    }

    return ret_key_status;
}


/**
 * @brief key_interuption_callback
 * 
 * Steps:
 *  1.if trigger first time with falling type,\
 *    send the event to the inter_key_queue \
 *    changing the interruption type to Raising
 *  
 *  2.if trigger first time with Raising type,\
 *    send the event to the inter_key_queue \
 *    changing the interruption type back to falling
 *  
 * @param[in] void 
 * 
 * @return void
 * 
 * */
KEY_CALLBACK 
{
    static uint32_t irq_type = FALING_TYPE;
   /*
    1.if trigger first time with falling type,\
      send the event to the inter_key_queue \
      changing the interruption type to Raising
    */
    BaseType_t xHigherPrioritTaskWoken;
    
    if ( FALING_TYPE == irq_type )
    {
        key_press_event_t key_press_event_1 = 
        {
            .edge_type    = FAILING,
            .trigger_tick = HAL_GetTick()
        };
        
        if ( NULL == inter_key_queue )
        {
            printf( "inter_key_queue not created"
                    " at [%d] tick \r\n", 
                                   HAL_GetTick());
        }        

        if ( pdTRUE == xQueueSendToFrontFromISR(            inter_key_queue, 
                                                         &key_press_event_1, 
                                                    &xHigherPrioritTaskWoken ))
        {
            printf( "key_press_event send FALING_event successfully"
                    " at [%d] tick \r\n", 
                                   HAL_GetTick());
        }
    /*
    1.1 changing the irq type
    */
        irq_type = RASING_TYPE;
    /*
    1.2 changing the GPIO irq trigger type
    */ 
        GPIO_InitTypeDef GPIO_InitStruct = {0};
            
        GPIO_InitStruct.Pin = KEY_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        HAL_GPIO_Init(KEY_GPIO_Port, &GPIO_InitStruct);
    }
    else if ( RASING_TYPE == irq_type )
    {
            
    /*  
    2.if trigger first time with Raising type,\
      send the event to the inter_key_queue \
      changing the interruption type back to falling
    */
         key_press_event_t key_press_event_2 = 
        {
            .edge_type    = RASING,
            .trigger_tick = HAL_GetTick()
        };
    
        if ( NULL == inter_key_queue )
        {
            printf( "inter_key_queue not created"
                    " at [%d] tick \r\n", 
                                   HAL_GetTick());
        }        
        if ( pdTRUE == xQueueSendToFrontFromISR(              inter_key_queue, 
                                                           &key_press_event_2, 
                                                     &xHigherPrioritTaskWoken ))
        {
            printf( "key_press_event send RASING_event successfully"
                    " at [%d] tick \r\n", 
                                   HAL_GetTick());
        }
        
    /*
    1.1 changing the irq type
    */
        irq_type = FALING_TYPE;
    /*
    1.2 changing the GPIO irq trigger type
    */ 
        GPIO_InitTypeDef GPIO_InitStruct = {0};
            
        GPIO_InitStruct.Pin = KEY_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        HAL_GPIO_Init(KEY_GPIO_Port, &GPIO_InitStruct);
    }
    

}



