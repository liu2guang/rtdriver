#ifndef __DRV_SM5386B_H_
#define __DRV_SM5386B_H_

#include <rtthread.h>
#include "stm32f1xx.h"

#define RT_SM5385B_MODBUS_ADDR 0x04

/* 导出外部API接口 */
int rt_hw_sm5385b_init(void);

rt_uint32_t rt_sm5385b_get_windspeed(void);

#endif
