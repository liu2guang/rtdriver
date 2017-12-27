#include "drv_ds1302.h"

#ifdef  RT_RTC_DEBUG
#define RT_RTC_LOG(...)   rt_kprintf(__VA_ARGS__)
#else
#define RT_RTC_LOG(...) 
#endif

static struct rt_device rtc_device;
static RTC_HandleTypeDef RTC_Handler;

static void delay(void)
{
    uint8_t i;
    
    for(i = 0; i < 20; i++);
}

static void Set_SCLK(uint8_t level)
{
    level ? HAL_GPIO_WritePin(DS1302_GPIO, DS1302_SCLK,  GPIO_PIN_SET) : HAL_GPIO_WritePin(DS1302_GPIO, DS1302_SCLK, GPIO_PIN_RESET);
}

static void Set_RST(uint8_t level)
{
    level ? HAL_GPIO_WritePin(DS1302_GPIO, DS1302_RST,  GPIO_PIN_SET) : HAL_GPIO_WritePin(DS1302_GPIO, DS1302_RST, GPIO_PIN_RESET);
}

static uint8_t DS1302_IO_BIT(uint8_t out_in, uint8_t value)
{
    uint8_t bit;

    if(out_in == 0)
    {
        value ? HAL_GPIO_WritePin(DS1302_GPIO, DS1302_SDA,  GPIO_PIN_SET) : HAL_GPIO_WritePin(DS1302_GPIO, DS1302_SDA,  GPIO_PIN_RESET);
    }
    else if(out_in == 1)
    {
        bit = HAL_GPIO_ReadPin(DS1302_GPIO, DS1302_SDA);
    }

    return bit;
}

static void DS1302_GpioDirection(uint8_t Direction)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.Pin = DS1302_SDA;
    GPIO_InitStructure.Mode =  (0 == Direction) ? GPIO_MODE_OUTPUT_PP : GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(DS1302_GPIO, &GPIO_InitStructure);
}

static void write_bus(uint8_t value)
{
    uint8_t i;

    for(i = 0; i < 8; i++)
    {
        DS1302_IO_BIT(0, value & 0x01);
        value >>= 1;
        delay();
        Set_SCLK(1);
        delay();
        Set_SCLK(0);
    }
}


static void write_reg(uint8_t addr, uint8_t data)
{
    rt_enter_critical();
    Set_RST(1);
    DS1302_GpioDirection(0);
    write_bus(addr);   // Sends address
    write_bus(data);   // Sends data
    DS1302_GpioDirection(1);
    Set_RST(0);
    delay();
    rt_exit_critical();
}


static uint8_t read_reg(uint8_t addr) 
{
    uint8_t i, temp = 0x00;
    
    rt_enter_critical();
    Set_RST(1);
    DS1302_GpioDirection(0);
    write_bus(addr);
    DS1302_GpioDirection(1);
    for(i = 0; i < 8; i++)
    {
        delay();
        temp >>= 1;
        if(DS1302_IO_BIT(1, 0)) temp |= 0x80;
        Set_SCLK(1);
        delay();
        Set_SCLK(0);
    }
    Set_RST(0);  
    delay();
    
    rt_exit_critical();
    return temp;
}


rt_err_t set_rtc_timestamp(time_t time)
{
    struct tm* _tm;
    uint8_t n;
    uint32_t years;

     _tm = localtime(&time);

    write_reg(0x8E, 0x00);
    n = _tm->tm_sec;
    n = ((n / 10) << 4) + (n % 10);
    write_reg(0x80, n);
    n = _tm->tm_min;
    n = ((n / 10) << 4) + (n % 10);
    write_reg(0x82, n);
    n = _tm->tm_hour;
    n = ((n / 10) << 4) + (n % 10);
    write_reg(0x84, n);
    n = _tm->tm_mday;
    n = ((n / 10) << 4) + (n % 10);
    write_reg(0x86, n);
    n = _tm->tm_mon + 1;
    n = ((n / 10) << 4) + (n % 10);
    write_reg(0x88, n);
    n = _tm->tm_year % 100;
    n = ((n / 10) << 4) + (n % 10);
    write_reg(0x8C, n);

    years = _tm->tm_year / 100;
    write_reg(0xC0, years & 0xFF);
    years >>= 8;
    write_reg(0xC2, years & 0xFF);
    years >>= 8;
    write_reg(0xC4, years & 0xFF);
    years >>= 8;
    write_reg(0xC6, years & 0xFF);

    write_reg(0x90, 0xA5);
    write_reg(0xC8, 0xAA);

    write_reg(0x8e, 0x80);
    
    return RT_EOK;
}
FINSH_FUNCTION_EXPORT_ALIAS(set_rtc_timestamp, settime, set rtc time);

time_t get_rtc_timestamp(void)
{
    struct tm _tm = {0};
    uint8_t n;
    uint32_t years;

    n = read_reg(0x81);
    _tm.tm_sec = (n >> 4) * 10 + (n & 0x0F);
    n = read_reg(0x83);
    _tm.tm_min = (n >> 4) * 10 + (n & 0x0F);
    n = read_reg(0x85);
    _tm.tm_hour = (n >> 4) * 10 + (n & 0x0F);
    n = read_reg(0x87);
    _tm.tm_mday = (n >> 4) * 10 + (n & 0x0F);
    n = read_reg(0x89);
    _tm.tm_mon = (n >> 4) * 10 + (n & 0x0F) - 1;
    n = read_reg(0x8D);
    _tm.tm_year = (n >> 4) * 10 + (n & 0x0F);

    years = read_reg(0xC7);
    years <<= 8;
    years += read_reg(0xC5);
    years <<= 8;
    years += read_reg(0xC3);
    years <<= 8;
    years += read_reg(0xC1);

    _tm.tm_year += years * 100;

    return mktime(&_tm);
}

int gettime()
{
    time_t time;
    time = get_rtc_timestamp();
    struct tm *_tm = localtime(&time);
    
    rt_kprintf("time_t:%d\n",time);
    rt_kprintf("%d-%d-%d-%d-%d-%d\n",_tm->tm_year+1900,_tm->tm_mon+1,_tm->tm_mday,_tm->tm_hour,_tm->tm_min,_tm->tm_sec);
    
    return RT_EOK;
}
FINSH_FUNCTION_EXPORT_ALIAS(gettime, gettime, get rtc time);

static rt_err_t rt_rtc_open(rt_device_t dev, rt_uint16_t oflag)
{
    return RT_EOK;
}

static rt_size_t rt_rtc_read(rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
    return 0;
}

static rt_err_t rt_rtc_control(rt_device_t dev, int cmd, void *args)
{
    rt_err_t ret = RT_EOK;

    RT_ASSERT(dev != RT_NULL);

    switch(cmd)
    {
        case RT_DEVICE_CTRL_RTC_GET_TIME:
        {
            *(rt_uint32_t *)args = get_rtc_timestamp();

            RT_RTC_LOG("[RTC] get rtc_time %x\n", *(rt_uint32_t *)args);
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

/* Initialization */
static rt_err_t ds1302_init(rt_device_t rtc_device)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.Pin = DS1302_SCLK | DS1302_SDA | DS1302_RST;
    GPIO_InitStructure.Mode =  GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(DS1302_GPIO, &GPIO_InitStructure);

    write_reg(DS1302_CHARGER_W,0xA5);      // Enable Trickle Charger 
        
    HAL_GPIO_WritePin(DS1302_GPIO, DS1302_RST,  GPIO_PIN_RESET);
    HAL_GPIO_WritePin(DS1302_GPIO, DS1302_SCLK,  GPIO_PIN_RESET);
    
    return RT_EOK;
}
//INIT_APP_EXPORT(DS1302_Init);

void  Trickle_charge(uint8_t en){
    en ? write_reg(DS1302_CHARGER_W,0xA6): write_reg(DS1302_CHARGER_W,0x00);
}
FINSH_FUNCTION_EXPORT_ALIAS(Trickle_charge, charge, Trickle charge);

/*×¢²áRCTÉè±¸*/
int rt_hw_rtc_init(void)
{
    rt_err_t ret = RT_EOK;
    
    rtc_device.type = RT_Device_Class_RTC;

    rtc_device.init    = ds1302_init;
    rtc_device.open    = rt_rtc_open;
    rtc_device.close   = RT_NULL;
    rtc_device.read    = rt_rtc_read;
    rtc_device.write   = RT_NULL;
    rtc_device.control = rt_rtc_control;

    rtc_device.rx_indicate = RT_NULL;
    rtc_device.tx_complete = RT_NULL;

    rtc_device.user_data = RT_NULL;

    /* ???? */
    ret = rt_device_register(&rtc_device, "rtc", RT_DEVICE_FLAG_RDWR);
    if(ret != RT_EOK)
    {
        RT_RTC_LOG("[rtc] rtc low level init failed. errno = %d\n", ret);
        return ret;
    }

    return RT_EOK;
}
INIT_BOARD_EXPORT(rt_hw_rtc_init);
