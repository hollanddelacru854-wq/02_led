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

#include "bsp_led.h"

static uint32_t g_blink_times = 0; //点亮的次数
static uint32_t g_blink_order = 0; //



/**
 * @brief Instantiates the bsp_led_handler_t target.
 * 
 * Steps:
 *  1.doing the specific operations
 *  
 * @param[in] void 
 * 
 * @return led_handler_status_t : Status of the function.
 * 
 * */
led_status_t led_on_off(led_operation_t led_operation)
{
    led_status_t ret = LED_OK;
    
    if ( LED_ON     == led_operation )
    {
        //1. Make the LED ON
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET); 
    }
    
    if ( LED_OFF    == led_operation )
    {
        //1. Make the LED OFF
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
    }
    
    if ( LED_TOGGLE == led_operation )
    {
        //1. Make the LED toggle
        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    }
	
	if ( LED_BLINK_3_TIMES == led_operation )
    {
        //1. Make the LED blink 3 times
        for(int i = 0; i < 6; i++)
        {
            HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
            //vTaskDelay(300);
			HAL_Delay(300);
        }
    }

    return ret;
}


/**
 * @brief Instantiates the bsp_led_handler_t target.
 * 
 * Steps:
 *  1.doing the specific operations
 *  
 * @param[in] void 
 * 
 * @return led_handler_status_t : Status of the function.
 * 
 * */
led_status_t led_on_off_timer_irq(led_operation_t led_operation)
{
    led_status_t ret = LED_OK;
    
//    if ( LED_ON            == led_operation )
//    {
//        //1. Make the LED ON
//        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET); 
//    }
//    
//    if ( LED_OFF           == led_operation )
//    {
//        //1. Make the LED OFF
//        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
//    }
    
//    if ( LED_TOGGLE        == led_operation )
//    {
//        //1. Make the LED toggle
//        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
//    }
    if ( LED_BLINK_1_TIMES  == led_operation )
    {
       g_blink_times = 1; // 1: blink 1, 5： blink 5
       g_blink_order = 0;
    }
    if ( LED_BLINK_10_TIMES == led_operation )
    {
       g_blink_times = 10; // 1: blink 1, 5： blink 5
       g_blink_order = 0;
    }

    return ret;
}


/**
 * @brief led_callback_in_timer2 for timer to run.
 * 
 * Steps:
 *  1.doing the specific operations
 *  
 * @param[in] void 
 * 
 * @return led_handler_status_t : Status of the function.
 * 
 * */
led_status_t led_callback_in_timer(void)
{
    //1.if the g_blink_times is not zero, then start blink
    if (g_blink_times > 0)
    {
        if (g_blink_order % 2 == 0)
        {
            led_on_off(LED_ON);
        }
        else
        {
            led_on_off(LED_OFF);
            g_blink_times--;
        }
        g_blink_order++;
    }
    else
    {
        g_blink_order = 0;
    }

    return LED_OK;



}







