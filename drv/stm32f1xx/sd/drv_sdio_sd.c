/*
 * @File:   drv_sdio_sd.c 
 * @Author: liuguang 
 * @Date:   2017-09-14 00:22:15 
 * 
 * @LICENSE: 
 * The code owner is liuguang. Other personnel may 
 * not be used or modified.
 * 
 * Change Logs: 
 * Date           Author       Notes 
 * 2017-09-13     liuguang     the first version. 
 * 2017-09-14     liuguang     add sdcard hotplug.
 * 2017-09-15     liuguang     解决2G以上卡无法识别, 提高SD识别稳定性, 提高热拔插稳定性
 */ 

#include "drv_sdio_sd.h"
#include "drv_gpio.h"
#include "drv_led.h"
#include "multi_button.h"
#include <dfs_fs.h>

struct rt_device sdcard_device;
struct rt_semaphore sd_lock;

SD_HandleTypeDef hsd;
HAL_SD_CardInfoTypedef SDCardInfo;

#ifdef RT_USING_SDCARD_DMA_MODE
DMA_HandleTypeDef hdma_sdio;
#endif

static rt_err_t rt_sdcard_ll_init(void)
{
    {
        GPIO_InitTypeDef GPIO_InitStruct;
        
        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_GPIOD_CLK_ENABLE();
        
        GPIO_InitStruct.Mode  = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull  = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        
        /*  */
        GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 |
            GPIO_PIN_11 | GPIO_PIN_12;
        
        //HAL_GPIO_DeInit(GPIOC, GPIO_InitStruct.Pin);
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
        
        /*  */
        GPIO_InitStruct.Pin = GPIO_PIN_2;
        
        //HAL_GPIO_DeInit(GPIOD, GPIO_InitStruct.Pin);
        HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    }
    
#ifdef RT_USING_SDCARD_DMA_MODE
    {
        __HAL_RCC_DMA2_CLK_ENABLE();
        
        hdma_sdio.Instance                 = DMA2_Channel4;
        hdma_sdio.Init.Direction           = DMA_PERIPH_TO_MEMORY;
        hdma_sdio.Init.PeriphInc           = DMA_PINC_DISABLE;
        hdma_sdio.Init.MemInc              = DMA_MINC_ENABLE;
        hdma_sdio.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
        hdma_sdio.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
        hdma_sdio.Init.Mode                = DMA_NORMAL;
        hdma_sdio.Init.Priority            = DMA_PRIORITY_VERY_HIGH;

        //HAL_DMA_Abort(&hdma_sdio);
        //HAL_DMA_DeInit(&hdma_sdio);
        
        __HAL_LINKDMA(&hsd, hdmarx, hdma_sdio);
        
        HAL_NVIC_SetPriority(DMA2_Channel4_5_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ  (DMA2_Channel4_5_IRQn);
    }
#endif
    
    {
        __HAL_RCC_SDIO_CLK_ENABLE();
        
        hsd.Instance                 = SDIO;
        hsd.Init.ClockEdge           = SDIO_CLOCK_EDGE_RISING;   
        hsd.Init.ClockBypass         = SDIO_CLOCK_BYPASS_DISABLE; 
        hsd.Init.ClockPowerSave      = SDIO_CLOCK_POWER_SAVE_DISABLE; 
        hsd.Init.BusWide             = SDIO_BUS_WIDE_1B; 
        hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_ENABLE;
        hsd.Init.ClockDiv            = 2;
        
        HAL_NVIC_SetPriority(SDIO_IRQn, 4, 0);
        HAL_NVIC_EnableIRQ  (SDIO_IRQn);
    }
    
    {
        HAL_SD_ErrorTypedef sd_state = SD_OK;
    
#ifdef RT_USING_SDCARD_DMA_MODE
        if(HAL_DMA_Init(&hdma_sdio) != HAL_OK)
        {
            return RT_ERROR;
        }
#endif
        
        sd_state = HAL_SD_Init(&hsd, &SDCardInfo);
        if(sd_state != SD_OK)
        {
            return RT_ERROR;
        }
        
        while(HAL_SD_WideBusOperation_Config(&hsd, SDIO_BUS_WIDE_4B) != SD_OK);
    }
    
    return RT_EOK;
}

static rt_err_t rt_sdcard_ll_deinit(void)
{
    __HAL_RCC_SDIO_CLK_DISABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    
    HAL_SD_DeInit(&hsd);
    
#ifdef RT_USING_SDCARD_DMA_MODE
    HAL_DMA_DeInit(&hdma_sdio);
#endif
    
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11 
                          |GPIO_PIN_12);
    
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_2);

    HAL_NVIC_DisableIRQ(SDIO_IRQn);
    
#ifdef RT_USING_SDCARD_DMA_MODE
    HAL_NVIC_DisableIRQ(DMA2_Channel4_5_IRQn);
#endif
    
    return RT_EOK;
}

void SDIO_IRQHandler(void)
{
    rt_interrupt_enter();
    
    HAL_SD_IRQHandler(&hsd);

    rt_interrupt_leave();
}

#ifdef RT_USING_SDCARD_DMA_MODE	
void DMA2_Channel4_5_IRQHandler(void)
{
    rt_interrupt_enter();
    
    HAL_DMA_IRQHandler(&hdma_sdio);

    rt_interrupt_leave();
}
#endif

static rt_err_t rt_sdcard_init(rt_device_t dev)
{
    return RT_EOK;
}

rt_uint8_t SD_ReadBlocks(uint32_t *buf, uint64_t sector, uint32_t cnt)
#ifdef RT_USING_SDCARD_DMA_MODE
{
    rt_uint8_t err = SD_OK;
    
    err = HAL_SD_ReadBlocks_DMA(&hsd, buf, (uint64_t)sector, SECTOR_SIZE, cnt);
    
    if(err == SD_OK)
    {
        err = HAL_SD_CheckReadOperation(&hsd, (uint32_t)RT_SDCARD_WAIT_TIMEOUT);
    }

    return err;
}
#else
{
    rt_uint8_t err = SD_OK;
    
    err = HAL_SD_ReadBlocks(&hsd, buf, (uint64_t)sector, SECTOR_SIZE, cnt);
    
    return err;
}
#endif

rt_uint8_t SD_WriteBlocks(uint32_t *buf,uint64_t sector,uint32_t cnt)
{
    return HAL_SD_WriteBlocks(&hsd, buf, (uint64_t)sector, SECTOR_SIZE, cnt);
}

static rt_err_t rt_sdcard_open(rt_device_t dev, rt_uint16_t oflag)
{
    return RT_EOK;
}

static rt_err_t rt_sdcard_close(rt_device_t dev)
{
    return RT_EOK;
}

static uint32_t sdio_buffer[512/sizeof(uint32_t)];
static rt_size_t rt_sdcard_read(rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
    rt_uint8_t status = SD_OK;

    rt_sem_take(&sd_lock, RT_WAITING_FOREVER);

    if( ((uint32_t)buffer & 0x03) != 0 )
    {
        uint32_t i;
        uint64_t sector_adr;
        uint8_t* copy_buffer;

        sector_adr = (uint64_t)pos*SECTOR_SIZE;
        copy_buffer = (uint8_t*)buffer;
        
        for(i=0; i<size; i++)
        {
            status = SD_ReadBlocks(sdio_buffer, (uint64_t)sector_adr, 1);
            
            rt_memcpy(copy_buffer, sdio_buffer, SECTOR_SIZE);

            sector_adr  += SECTOR_SIZE;
            copy_buffer += SECTOR_SIZE;
        }
    }
    else
    {
        status = SD_ReadBlocks(buffer, (uint64_t)((uint64_t)pos*SECTOR_SIZE), size);
    }

    rt_sem_release(&sd_lock);

    if(status == SD_OK) 
    {
        return size;
    }

    rt_kprintf("read failed: %d, buffer 0x%08x\n", status, buffer);

    return 0;
}

static rt_size_t rt_sdcard_write(rt_device_t dev, rt_off_t pos, const void* buffer, rt_size_t size)
{
    rt_uint8_t status = SD_OK;
    
    rt_sem_take(&sd_lock, RT_WAITING_FOREVER);

    if( ((uint32_t)buffer & 0x03) != 0 )
    {
        uint32_t i;
        uint64_t sector_adr;
        uint8_t* copy_buffer;

        sector_adr = (uint64_t)pos*SECTOR_SIZE;
        copy_buffer = (uint8_t*)buffer;
        
        for(i=0; i<size; i++)
        {
            rt_memcpy(sdio_buffer, copy_buffer, SECTOR_SIZE);
            
            status = SD_WriteBlocks((uint32_t*)sdio_buffer, (uint64_t)sector_adr, 1);

            sector_adr  += SECTOR_SIZE;
            copy_buffer += SECTOR_SIZE;
            
        }
    }
    else
    {
        status = SD_WriteBlocks((uint32_t*)buffer, (uint64_t)((uint64_t)pos*SECTOR_SIZE), size);
    }
    
    rt_sem_release(&sd_lock);
    
    if(status == SD_OK) 
    {
        return size;
    }

    rt_kprintf("write failed: %d, buffer 0x%08x\n", status, buffer);
    
    return 0;
}

static rt_err_t rt_sdcard_control(rt_device_t dev, rt_uint8_t cmd, void *args)
{
    RT_ASSERT(dev != RT_NULL);

    if(cmd == RT_DEVICE_CTRL_BLK_GETGEOME)
    {
        struct rt_device_blk_geometry *geometry;

        geometry = (struct rt_device_blk_geometry *)args;
        
        if(geometry == RT_NULL) 
        {
            return -RT_ERROR;
        }

        geometry->bytes_per_sector = 512;
        geometry->block_size = SDCardInfo.CardBlockSize;
        geometry->sector_count = SDCardInfo.CardCapacity/SDCardInfo.CardBlockSize;
    }

    return RT_EOK;
}

int rt_hw_sdcard_init(void)
{
    rt_err_t ret = RT_EOK;
    
    ret = rt_sdcard_ll_init();
    
    if(ret != RT_EOK)
    {
        rt_kprintf("[Filesystem]sdcard ll init failed : %d\n", ret);
        return ret;
    }
    
    sdcard_device.type = RT_Device_Class_Block;
    
    sdcard_device.init    = rt_sdcard_init;
    sdcard_device.open    = rt_sdcard_open;
    sdcard_device.close   = rt_sdcard_close;
    sdcard_device.read    = rt_sdcard_read;
    sdcard_device.write   = rt_sdcard_write;
    sdcard_device.control = rt_sdcard_control;
    
    sdcard_device.user_data = &SDCardInfo;
    
    ret = rt_device_register(&sdcard_device, "sd0",
        RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_REMOVABLE | RT_DEVICE_FLAG_STANDALONE);
    
    if(ret != RT_EOK)
    {
        rt_kprintf("[Filesystem]register sdcard device failed : %d\n", ret);
        return ret;
    }
    
    return RT_EOK;
}

/* 热拔插部分 */

static struct Button sdcard_hotplug;
static __IO uint8_t sdcard_hotplug_status = RT_SDCARD_PULLOUT;

/* 热拔插引脚电平读取函数 */
static uint8_t read_pin_level(void)
{
    return rt_pin_read(RT_SDCARD_HOTPLUG_GPIO_INDEX);
}

/* sdcard热拔插回调函数 */
static void sdcard_hotplug_callback(void* btn)
{
    int ret = 0;
    rt_uint8_t index = 0;
    rt_device_t device = RT_NULL;
    
    switch(((struct Button *)btn)->event)
    {
        /* 热拔插按键弹起, SD卡拔出动作处理 */
        case PRESS_UP:
        {
            /* 拔出的情况下需要第一时间通知其他任务, SD已经拔出 */
            sdcard_hotplug_status = RT_SDCARD_PULLOUT;

            /* 让出CPU权限给低优先级任务, 但是再操作SD的任务进行处理 */
            rt_thread_delay(10);

            /* 移除SD卡 */
            ret = dfs_unmount("/sdcard");
            if(ret != 0)
            {
                rt_kprintf("[Filesystem]sdcard unmount failed.\n");
                break;
            }
            else
            {
                rt_kprintf("[Filesystem]sdcard unmount succeed.\n");
            }
            
            /* 移除SD卡设备 */
            device = rt_device_find("sd0");
            if(device == RT_NULL)
            {
                rt_kprintf("[Filesystem]sdcard device unregister failed\n");
                break;
            }

            rt_device_unregister(device);
            
            rt_sdcard_ll_deinit();

            /* 移除成功采用闪烁2次LED表示移除成功 */
            for(index = 0; index < 4; index++)
            {
                stm32_hw_led_toggle(0);
                rt_thread_delay(RT_TICK_PER_SECOND/20);
            }
        }
        break;
        
        /* 热拔插按键按下, SD卡插入动作处理 */
        case PRESS_DOWN:
        {
            /* 延时20ms等待SD卡插入后, SD卡接触稳定 */
            rt_thread_delay(RT_TICK_PER_SECOND/2);

            /* 初始化SD底层并注册SD卡设备 */
            rt_hw_sdcard_init();
            
            /* 延时20ms等待SD卡初始化完成后稳定 */
            rt_thread_delay(RT_TICK_PER_SECOND/50);
            
            /* 挂载SD卡到"/sdcard"目录 */
            ret = dfs_mount("sd0", "/sdcard", "elm", 0, RT_NULL);
            if(ret != 0)
            {
                rt_kprintf("[Filesystem]sdcard mount failed.\n");
                break;
            }
            else
            {
                rt_kprintf("[Filesystem]sdcard mount succeed.\n");
            }

            /* 移除成功采用闪烁2次LED表示移除成功 */
            for(index = 0; index < 4; index++)
            {
                stm32_hw_led_toggle(0);
                rt_thread_delay(RT_TICK_PER_SECOND/20);
            }
            
            /* 最后等所有准备工作做完, 再通知其他任务已经插入SD卡 */
            sdcard_hotplug_status = RT_SDCARD_INTERJECT;
        }
        break;
    }
}

/* 读取当前SD是否已经插入状态 */
rt_uint8_t rt_sdcard_is_detected(void)
{
    return sdcard_hotplug_status;
}

/* 带热拔插机制初始化SD卡 */
int rt_hw_sdcard_with_hotplug_init(void)
{
    /* 创建SD卡读写互斥量 */
    if(rt_sem_init(&sd_lock, "sdlock", 1, RT_IPC_FLAG_FIFO) != RT_EOK)
    {
        rt_kprintf("[Filesystem]Init sdcard lock semaphore failed\n");
    }
    
    /* 初始化SD卡热拔插识别引脚 */
    rt_pin_mode(RT_SDCARD_HOTPLUG_GPIO_INDEX, GPIO_MODE_INPUT);
    
    /* 将热拔插识别引脚创建为按键 */
    button_init(&sdcard_hotplug, read_pin_level, 0);
    
    /* 注册热拔插插入和拔出动作对应回调函数 */
    button_attach(&sdcard_hotplug, PRESS_UP,   sdcard_hotplug_callback);
    button_attach(&sdcard_hotplug, PRESS_DOWN, sdcard_hotplug_callback);
    
    /* 启动热拔插按键 */
    button_start(&sdcard_hotplug);
    
    return RT_EOK;
}
