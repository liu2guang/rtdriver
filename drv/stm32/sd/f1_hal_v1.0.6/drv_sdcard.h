#ifndef __DRV_SDCARD_H_ 
#define __DRV_SDCARD_H_ 

#include <board.h>
#include <rtthread.h>

#include "stm32f1xx.h"

#define RT_SDCARD_HOTPLUG_GPIO_INDEX (55) 

#define RT_SDCARD_PULLOUT      ((uint8_t)0x00)
#define RT_SDCARD_INTERJECT    ((uint8_t)0x01)

#define RT_USING_SDCARD_DMA_MODE
#define RT_SDCARD_WAIT_TIMEOUT ((uint32_t)100000000UL)

/* 导出API接口 */
int rt_hw_sdcard_init(void);
int rt_hw_sdcard_with_hotplug_init(void);
rt_uint8_t rt_sdcard_is_detected(void);

#endif
