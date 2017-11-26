#ifndef __DRV_PIN_H_ 
#define __DRV_PIN_H_ 

#include <rthw.h>
#include <rtdevice.h>

#define STM32F1
#define STM32FX_PIN_NUMBERS 64 

#if defined(STM32F0)
#include "stm32f0xx.h"
#endif /* STM32F0 */ 
#if defined(STM32F1)
#include "stm32f1xx.h"
#endif /* STM32F1 */ 
#if defined(STM32F2)
#include "stm32f2xx.h"
#endif /* STM32F2 */ 
#if defined(STM32F3)
#include "stm32f3xx.h"
#endif /* STM32F3 */ 
#if defined(STM32F4)
#include "stm32f4xx.h"
#endif /* STM32F4 */ 
#if defined(STM32F7)
#include "stm32f7xx.h"
#endif /* STM32F7 */ 

#define GET_ARRAY_LENGTH(array) (sizeof(array)/sizeof(array[0]))

typedef struct pin_map
{
    rt_int16_t    index;
    rt_uint32_t   clk;
    GPIO_TypeDef *port;
    rt_uint32_t   pin;
} *pin_map_t;

typedef struct pin_irq_map
{
    rt_uint16_t pinbit;
    IRQn_Type   irqno;
} *pin_irq_map_t;

#endif
