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
 
#include "drv_cpu.h"

#ifdef RT_USING_FINSH
#include <finsh.h>
#endif

#define CPU_USAGE_CALC_TICK    10
#define CPU_USAGE_LOOP        100

static rt_uint8_t  cpu_usage_major = 0, cpu_usage_minor= 0;
static rt_uint32_t total_count = 0;

static void cpu_usage_idle_hook()
{
    rt_tick_t tick;
    rt_uint32_t count;
    volatile rt_uint32_t loop;

    if (total_count == 0)
    {
        /* get total count */
        rt_enter_critical();
        
        tick = rt_tick_get();
        while(rt_tick_get() - tick < CPU_USAGE_CALC_TICK)
        {
            total_count++;
            loop = 0;
            while (loop < CPU_USAGE_LOOP) 
            {
                loop++;
            }
        }
        rt_exit_critical();
    }

    count = 0;
    
    /* get CPU usage */
    tick = rt_tick_get();
    while (rt_tick_get() - tick < CPU_USAGE_CALC_TICK)
    {
        count ++;
        loop  = 0;
        while (loop < CPU_USAGE_LOOP) loop ++;
    }

    /* calculate major and minor */
    if (count < total_count)
    {
        count = total_count - count;
        cpu_usage_major = (count * 100) / total_count;
        cpu_usage_minor = ((count * 100) % total_count) * 100 / total_count;
    }
    else
    {
        total_count = count;

        /* no CPU usage */
        cpu_usage_major = 0;
        cpu_usage_minor = 0;
    }
}

void cpu_usage_get(rt_uint8_t *major, rt_uint8_t *minor)
{
    RT_ASSERT(major != RT_NULL);
    RT_ASSERT(minor != RT_NULL);

    *major = cpu_usage_major;
    *minor = cpu_usage_minor;
}

static void cpu_usage_print(void)
{
    rt_kprintf("current cpu usage %d.%.2d%%\n", cpu_usage_major, cpu_usage_minor);
}
FINSH_FUNCTION_EXPORT_ALIAS(cpu_usage_print, cpu, print current cpu usage);

int cpu_usage_init(void)
{
    rt_thread_idle_sethook(cpu_usage_idle_hook);
    
    return RT_EOK;
}
INIT_BOARD_EXPORT(cpu_usage_init);
