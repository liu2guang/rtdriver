/*
 * @File:   drv_rtc_stm32.c 
 * @Author: liuguang 
 * @Date:   2017-09-20 18:28:35 
 * 
 * @LICENSE: 
 * The code owner is liuguang. Other personnel may 
 * not be used or modified.
 * 
 * Change Logs: 
 * Date           Author       Notes 
 * 2017-09-20     liuguang     使用stm32f1内部RTC实现RTC驱动 
 */ 

#include "drv_rtc_stm32.h"

/* lib支持 */
#include "time.h"

#ifdef  RT_RTC_DEBUG
#define RT_RTC_LOG(format, args...)      rt_kprintf(format, ##args)
#else
#define RT_RTC_LOG(format, args...) 
#endif

static struct rt_device rtc_device;
static RTC_HandleTypeDef RTC_Handler;

static rt_err_t rtc_ll_init(void)
{
    rt_err_t ret = RT_EOK;

    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWR_EnableBkUpAccess();
    __HAL_RCC_RTC_ENABLE();

    RTC_Handler.Instance = RTC;

    RTC_Handler.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
    RTC_Handler.Init.OutPut       = RTC_OUTPUTSOURCE_NONE;
    
    ret = HAL_RTC_Init(&RTC_Handler);
    if(ret != RT_EOK)
    {
        return ret;
    }

    if(HAL_RTCEx_BKUPRead(&RTC_Handler, RTC_BKP_DR1) != 0x5A5A)
    {
        /* 默认时钟设置 */
        RT_RTC_LOG("[rtc] the first one config time\n");
        
        /* 初始化完成 */
        HAL_RTCEx_BKUPWrite(&RTC_Handler, RTC_BKP_DR1, 0x5A5A);
    }

    return RT_EOK;
}

void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc)
{

}

/* 获取rtc时间戳 */
static time_t get_rtc_timestamp(void)
{
    struct tm tm_new;
    RTC_TimeTypeDef RTC_TimeStruct;
    RTC_DateTypeDef RTC_DateStruct;

    HAL_RTC_GetTime(&RTC_Handler, &RTC_TimeStruct, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&RTC_Handler, &RTC_DateStruct, RTC_FORMAT_BIN);

    tm_new.tm_sec  = RTC_TimeStruct.Seconds; 
    tm_new.tm_min  = RTC_TimeStruct.Minutes; 
    tm_new.tm_hour = RTC_TimeStruct.Hours;

    tm_new.tm_mday = RTC_DateStruct.Date;
    tm_new.tm_mon  = RTC_DateStruct.Month - 1; 
    tm_new.tm_year = RTC_DateStruct.Year  + 100;

    return mktime(&tm_new);
}

/* 设置RTC时间戳 */
rt_err_t set_rtc_timestamp(time_t time_stamp)
{
    struct tm *p_tm;
    RTC_TimeTypeDef RTC_TimeStruct;
    RTC_DateTypeDef RTC_DateStruct;
    
    p_tm = localtime(&time_stamp);
    if(p_tm->tm_year < 100)
    {
        return RT_ERROR;
    }

    RTC_TimeStruct.Seconds = p_tm->tm_sec ; 
    RTC_TimeStruct.Minutes = p_tm->tm_min ; 
    RTC_TimeStruct.Hours   = p_tm->tm_hour;
    
    RTC_DateStruct.Date    = p_tm->tm_mday;
    RTC_DateStruct.Month   = p_tm->tm_mon  + 1; 
    RTC_DateStruct.Year    = p_tm->tm_year - 100;
    
    RTC_DateStruct.WeekDay = p_tm->tm_wday + 1;

    HAL_RTC_SetTime(&RTC_Handler, &RTC_TimeStruct, RTC_FORMAT_BIN);
    HAL_RTC_SetDate(&RTC_Handler, &RTC_DateStruct, RTC_FORMAT_BIN);
    
    HAL_RTCEx_BKUPWrite(&RTC_Handler, RTC_BKP_DR1, 0x5A5A);
  
    return RT_EOK;
}

static rt_err_t rt_rtc_open(rt_device_t dev, rt_uint16_t oflag)
{
    return RT_EOK;
}

static rt_size_t rt_rtc_read(rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
    return 0;
}

static rt_err_t rt_rtc_control(rt_device_t dev, rt_uint8_t cmd, void *args)
{
    rt_err_t ret = RT_EOK;

    RT_ASSERT(dev != RT_NULL);

    switch(cmd)
    {
        case RT_DEVICE_CTRL_RTC_GET_TIME:
        {
            *(rt_uint32_t *)args = get_rtc_timestamp();

            RT_RTC_LOG("[RTC] get rtc_time %x\n", *(rt_uint32_t *)args());
        }
        break;

        case RT_DEVICE_CTRL_RTC_SET_TIME:
        {
            ret = set_rtc_timestamp(*(rt_uint32_t *)args);

            RT_RTC_LOG("[RTC] set rtc_time %x\n", *(rt_uint32_t *)args);
        }
        break;
    }

    return ret;
}

/* 初始化RTC驱动 */
int rt_hw_rtc_init(void)
{
    rt_err_t ret = RT_EOK;

    /* 初始化RTC底层, 这里没有在设备的init接口中实现初始化是因为, 
       RTC在上电需要后尽早初始化 */
    ret = rtc_ll_init();
    if(ret != RT_EOK)
    {
        RT_RTC_LOG("[rtc] rtc low level init failed. errno = %d\n", ret);
        return ret;
    }

    rtc_device.type = RT_Device_Class_RTC;

    rtc_device.init    = RT_NULL;
    rtc_device.open    = rt_rtc_open;
    rtc_device.close   = RT_NULL;
    rtc_device.read    = rt_rtc_read;
    rtc_device.write   = RT_NULL;
    rtc_device.control = rt_rtc_control;

    rtc_device.rx_indicate = RT_NULL;
    rtc_device.tx_complete = RT_NULL;

    rtc_device.user_data = RT_NULL;

    /* 注册设备 */
    ret = rt_device_register(&rtc_device, "rtc", RT_DEVICE_FLAG_RDWR);
    if(ret != RT_EOK)
    {
        RT_RTC_LOG("[rtc] rtc low level init failed. errno = %d\n", ret);
        return ret;
    }

    return RT_EOK;
}
INIT_DEVICE_EXPORT(rt_hw_rtc_init);
