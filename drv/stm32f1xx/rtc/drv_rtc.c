/*
 * File      : rtc.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 * 2011-11-26     aozima       implementation time.
 * 2017-08-09     liuguang     create for stm32f103 HAL version
 */
 
#include "drv_rtc.h"

#include <rtthread.h>
#include <stm32f1xx.h>
#include <time.h>

#ifdef RT_USING_FINSH
#include <finsh.h>
#endif

static struct rt_device rtc;
static RTC_HandleTypeDef hrtc;
static RTC_TimeTypeDef RTC_TimeStructure;
static RTC_DateTypeDef RTC_DateStructure;

#define MINUTE (60)
#define HOUR   (60*MINUTE)
#define DAY    (24*HOUR)
#define YEAR   (365*DAY)

static int month[12] =
{
    0,
    DAY*(31),
    DAY*(31+29),
    DAY*(31+29+31),
    DAY*(31+29+31+30),
    DAY*(31+29+31+30+31),
    DAY*(31+29+31+30+31+30),
    DAY*(31+29+31+30+31+30+31),
    DAY*(31+29+31+30+31+30+31+31),
    DAY*(31+29+31+30+31+30+31+31+30),
    DAY*(31+29+31+30+31+30+31+31+30+31),
    DAY*(31+29+31+30+31+30+31+31+30+31+30)
};

static time_t rt_mktime(struct tm *tm)
{
    long res;
    int year;
    year = tm->tm_year - 70;

    res = YEAR * year + DAY * ((year + 1) / 4);
    res += month[tm->tm_mon];

    if (tm->tm_mon > 1 && ((year + 2) % 4))
    res -= DAY;
    res += DAY * (tm->tm_mday - 1);
    res += HOUR * tm->tm_hour;
    res += MINUTE * tm->tm_min;
    res += tm->tm_sec;
    return res;
}

static rt_err_t rt_rtc_init(rt_device_t dev)
{
    HAL_PWR_EnableBkUpAccess();
    __HAL_RCC_BKP_CLK_ENABLE();
    __HAL_RCC_RTC_ENABLE();

    hrtc.Instance = RTC;
    hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
    hrtc.Init.OutPut = RTC_OUTPUTSOURCE_ALARM;
    
    /* Init bkp sram */
    if(HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) != 0xAA55)
    {
        rt_kprintf("Init Config RTC\n");

        if(HAL_RTC_Init(&hrtc) != HAL_OK)
        {
            while(1);
        }
        
        RTC_TimeStructure.Hours   = 0x12;
        RTC_TimeStructure.Minutes = 0x0;
        RTC_TimeStructure.Seconds = 0x0;
        
        if(HAL_RTC_SetTime(&hrtc, &RTC_TimeStructure, RTC_FORMAT_BCD) != HAL_OK)
        {
            while(1);
        }
        
        RTC_DateStructure.WeekDay = RTC_WEEKDAY_MONDAY;
        RTC_DateStructure.Month   = RTC_MONTH_JANUARY;
        RTC_DateStructure.Date    = 0x1;
        RTC_DateStructure.Year    = 0x0;
        
        if(HAL_RTC_SetDate(&hrtc, &RTC_DateStructure, RTC_FORMAT_BCD) != HAL_OK)
        {
            while(1);
        }
        
        HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR1,0xAA55);
    }

    return RT_EOK;
}

static rt_err_t rt_rtc_open(rt_device_t dev, rt_uint16_t oflag)
{
    if(dev->rx_indicate != RT_NULL)
    {
        /* Open Interrupt */
    }

    return RT_EOK;
}

static rt_size_t rt_rtc_read(rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
    return 0;
}

static rt_err_t rt_rtc_control(rt_device_t dev, rt_uint8_t cmd, void *args)
{
    time_t *time;
    struct tm ti,*to;
    RT_ASSERT(dev != RT_NULL);

    switch(cmd)
    {
        case RT_DEVICE_CTRL_RTC_GET_TIME:
        {
            time = (time_t *)args;
            HAL_RTC_GetTime(&hrtc, &RTC_TimeStructure, RTC_FORMAT_BIN);
            HAL_RTC_GetDate(&hrtc, &RTC_DateStructure, RTC_FORMAT_BIN);
            
            ti.tm_sec  = RTC_TimeStructure.Seconds;
            ti.tm_min  = RTC_TimeStructure.Minutes;
            ti.tm_hour = RTC_TimeStructure.Hours;
            
            ti.tm_mday = RTC_DateStructure.Date;
            ti.tm_mon  = RTC_DateStructure.Month - 1;
            ti.tm_year = RTC_DateStructure.Year + 100;
            
            *time = rt_mktime(&ti);
        }
        break;

        case RT_DEVICE_CTRL_RTC_SET_TIME:
        {
            time = (time_t *)args;
            
            to = localtime(time);
            
            RTC_TimeStructure.Seconds = to->tm_sec;
            RTC_TimeStructure.Minutes = to->tm_min;
            RTC_TimeStructure.Hours	  = to->tm_hour;
            
            RTC_DateStructure.Date  = to->tm_mday;
            RTC_DateStructure.Month = to->tm_mon  + 1;
            RTC_DateStructure.Year  = to->tm_year - 100;
            
            HAL_RTC_SetTime(&hrtc, &RTC_TimeStructure, RTC_FORMAT_BIN);
            HAL_RTC_SetDate(&hrtc, &RTC_DateStructure, RTC_FORMAT_BIN);
            
            HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR1,0xAA55);
        }
        break;
    }

    return RT_EOK;
}

int rt_hw_rtc_init(void)
{
    rtc.type = RT_Device_Class_RTC;

    /* register rtc device */
    rtc.init 	= RT_NULL;
    rtc.open 	= rt_rtc_open;
    rtc.close	= RT_NULL;
    rtc.read 	= rt_rtc_read;
    rtc.write	= RT_NULL;
    rtc.control = rt_rtc_control;

    /* no private */
    rtc.user_data = RT_NULL;

    rt_device_register(&rtc, "rtc", RT_DEVICE_FLAG_RDWR);
    
    rt_rtc_init(&rtc);

    return RT_EOK;
}
INIT_DEVICE_EXPORT(rt_hw_rtc_init);
