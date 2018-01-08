#ifndef __DRV_PIN_H_ 
#define __DRV_PIN_H_ 

#include <rthw.h> 
#include <rtdevice.h> 

#define STM32F1 
// #define STM32FX_PACKAGE_EWLCSP49 
// #define STM32FX_PACKAGE_EWLCSP66 
// #define STM32FX_PACKAGE_LFBGA100 
// #define STM32FX_PACKAGE_LFBGA144 
// #define STM32FX_PACKAGE_LQFP32 
// #define STM32FX_PACKAGE_LQFP48 
#define STM32FX_PACKAGE_LQFP64 
// #define STM32FX_PACKAGE_LQFP100 
// #define STM32FX_PACKAGE_LQFP144 
// #define STM32FX_PACKAGE_LQFP176 
// #define STM32FX_PACKAGE_LQFP208 
// #define STM32FX_PACKAGE_TFBGA64 
// #define STM32FX_PACKAGE_TFBGA100 
// #define STM32FX_PACKAGE_TFBGA216 
// #define STM32FX_PACKAGE_TFBGA240 
// #define STM32FX_PACKAGE_TSSOP14 
// #define STM32FX_PACKAGE_TSSOP20 
// #define STM32FX_PACKAGE_UFBGA100 
// #define STM32FX_PACKAGE_UFBGA132 
// #define STM32FX_PACKAGE_UFBGA144 
// #define STM32FX_PACKAGE_UFBGA169 
// #define STM32FX_PACKAGE_UFBGA176 
// #define STM32FX_PACKAGE_UFQFPN20 
// #define STM32FX_PACKAGE_UFQFPN28 
// #define STM32FX_PACKAGE_UFQFPN32 
// #define STM32FX_PACKAGE_UFQFPN36 
// #define STM32FX_PACKAGE_UFQFPN48 
// #define STM32FX_PACKAGE_WLCSP25 
// #define STM32FX_PACKAGE_WLCSP36 
// #define STM32FX_PACKAGE_WLCSP49 
// #define STM32FX_PACKAGE_WLCSP63 
// #define STM32FX_PACKAGE_WLCSP64 
// #define STM32FX_PACKAGE_WLCSP66 
// #define STM32FX_PACKAGE_WLCSP72 
// #define STM32FX_PACKAGE_WLCSP81 
// #define STM32FX_PACKAGE_WLCSP90 
// #define STM32FX_PACKAGE_WLCSP100 
// #define STM32FX_PACKAGE_WLCSP104 
// #define STM32FX_PACKAGE_WLCSP143 
// #define STM32FX_PACKAGE_WLCSP144 
// #define STM32FX_PACKAGE_WLCSP168 
// #define STM32FX_PACKAGE_WLCSP180 

/* Í·ÎÄ¼þ */
#if   defined(STM32F0)
#include "stm32f0xx.h"
#elif defined(STM32F1)
#include "stm32f1xx.h"
#elif defined(STM32F2)
#include "stm32f2xx.h"
#elif defined(STM32F3)
#include "stm32f3xx.h"
#elif defined(STM32F4)
#include "stm32f4xx.h"
#elif defined(STM32F7)
#include "stm32f7xx.h"
#elif defined(STM32L0)
#include "stm32l0xx.h"
#elif defined(STM32L1)
#include "stm32l1xx.h"
#elif defined(STM32L4)
#include "stm32l4xx.h"
#elif 
#error "The PIN driver does not specify the type of the chip."
#endif

#endif /* __DRV_PIN_H_ */ 
