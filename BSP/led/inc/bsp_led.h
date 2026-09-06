/******************************************************************************
 * Copyright (C) 2024 EternalChip, Inc.(Gmbh) or its affiliates.
 * 
 * All Rights Reserved.
 * 
 * @file bsp_key.h
 * 
 * @par dependencies 
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
 * @version V1.0 2023-12-03
 *
 * @note 1 tab == 4 spaces!
 * 
 *****************************************************************************/

#ifndef __BSP_LED_H__
#define __BSP_LED_H__

//******************************** Includes *********************************//


#include <stdint.h>               // 编译器提供的通用库包含部分
#include <stdio.h>
#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
//#include "queue.h"
//#include "cmsis_os.h"
//******************************** Includes *********************************//

//******************************** Defines **********************************//

/*  function resturn status          */
typedef enum
{
  LED_OK                = 0,           /* Operation completed successfully.  */
  LED_ERROR             = 1,           /* Run-time error without case matched*/
  LED_ERRORTIMEOUT      = 2,           /* Operation failed with timeout      */
  LED_ERRORRESOURCE     = 3,           /* Resource not available.            */
  LED_ERRORPARAMETER    = 4,           /* Parameter error.                   */
  LED_ERRORNOMEMORY     = 5,           /* Out of memory.                     */
  LED_ERRORISR          = 6,           /* Not allowed in ISR context         */
  LED_RESERVED          = 0x7FFFFFFF   /* Reserved                           */
} led_status_t;

typedef enum
{
  LED_ON                = 0,           /* Operation completed successfully.  */
  LED_OFF               = 1,           /* Operation completed successfully.  */
  LED_TOGGLE            = 2,           /* Operation completed successfully.  */
  LED_BLINK_1_TIMES     = 3,           /* Operation completed successfully.  */
  LED_BLINK_3_TIMES     = 4,           /* Operation completed successfully.  */
  LED_BLINK_10_TIMES    = 5,           /* Operation completed successfully.  */
  LED_INITED_VALUE      = 0xFF         /* Inited value                    .  */
} led_operation_t;

//******************************** Defines **********************************//

//******************************** Declaring ********************************//

led_status_t led_on_off(led_operation_t led_operation);

led_status_t led_on_off_timer_irq(led_operation_t led_operation);

led_status_t led_callback_in_timer(void);

//******************************** Declaring ********************************//




#endif // End of __BSP_KEY_H__

