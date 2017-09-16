#include "drv_sm5386b.h"

#ifdef RT_USING_FINSH
#include "finsh.h"
#endif

#include "mb.h"
#include "mb_m.h"
#include "user_mb_app.h"

static rt_mutex_t  windspeed_mutex = RT_NULL;
static rt_uint32_t windspeed = 0;

/* 后台轮询函数 */
void mb_poll_thread(void *p)
{
    while(1)
    {
        /* freemodbus master poll */
        eMBMasterPoll();
        
        rt_thread_delay(10);
    }
}

void ws_poll_thread(void *p)
{
    eMBMasterReqErrCode ret = MB_MRE_NO_ERR;
    
    while(1)
    {
        ret = eMBMasterReqReadHoldingRegister(RT_SM5385B_MODBUS_ADDR, 0x0000, 1, RT_TICK_PER_SECOND);
        
        if(ret != MB_MRE_NO_ERR)
        {
            rt_mutex_take(windspeed_mutex, RT_WAITING_FOREVER);
            windspeed = 0xFFFFFFFF;
            rt_mutex_release(windspeed_mutex);
        }
        
        rt_mutex_take(windspeed_mutex, RT_WAITING_FOREVER);
        windspeed = usMRegHoldBuf[3][0x0000];
        rt_mutex_release(windspeed_mutex);
        
        rt_thread_delay(RT_TICK_PER_SECOND/20);
    }
}

/* 获取风速 */
rt_uint32_t rt_sm5385b_get_windspeed(void)
{
    rt_uint32_t windspeed_tmp = 0;

    rt_mutex_take(windspeed_mutex, RT_WAITING_FOREVER);
    windspeed_tmp = windspeed;
    // rt_kprintf("windspeed = %d.%.1dm/s\n", windspeed_tmp/10, windspeed_tmp%10);
    rt_mutex_release(windspeed_mutex);
    
    return windspeed_tmp; 
}
FINSH_FUNCTION_EXPORT_ALIAS(rt_sm5385b_get_windspeed, windspeed, get speed wind);

/* 初始化风速传感器 */
int rt_hw_sm5385b_init(void)
{
    rt_thread_t mb_poll = RT_NULL;
    rt_thread_t ws_poll = RT_NULL;
    
    /* 初始化freemodbus主机 */
    eMBMasterInit(MB_RTU, 1, 9600, MB_PAR_NONE);
    
    eMBMasterEnable();
    
    /* 创建互斥量 */
    windspeed_mutex = rt_mutex_create("windspeed_mutex", RT_IPC_FLAG_FIFO);
    if(windspeed_mutex == RT_NULL)
    {
        rt_kprintf("windspeed_mutex create failed\n");
        return RT_ERROR;
    }
    
    mb_poll = rt_thread_create("mb-poll", mb_poll_thread, RT_NULL, 512, 6, 10);
    if(mb_poll == RT_NULL)
    {
        rt_kprintf("mb poll create failed\n");
        return RT_ERROR;
    }
    
    ws_poll = rt_thread_create("ws-poll", ws_poll_thread, RT_NULL, 1024, 15, 10);
    if(ws_poll == RT_NULL)
    {
        rt_kprintf("ws poll create failed\n");
        return RT_ERROR;
    }
    
    /* 启动线程 */
    rt_thread_startup(mb_poll);
    rt_thread_startup(ws_poll);
    
    return RT_EOK;
}
INIT_APP_EXPORT(rt_hw_sm5385b_init);
