/*
 * @File:   drv_cpu.h 
 * @Author: liuguang 
 * @Date:   2017-09-14 23:43:00 
 * 
 * @LICENSE: 
 * The code owner is liuguang. Other personnel may 
 * not be used or modified.
 * 
 * Change Logs: 
 * Date           Author       Notes 
 * 2017-09-14     liuguang     提交第一版本 
 */ 
 
#ifndef __DRV_CPU_H_
#define __DRV_CPU_H_

#include <rtthread.h>
#include <rthw.h>

int cpu_usage_init(void);
void cpu_usage_get(rt_uint8_t *major, rt_uint8_t *minor);

#endif
