/*
 * @File:   drv_rtc_ds1302.c 
 * @Author: liu2guang 
 * @Date:   2018-01-14 02:05:26 
 * 
 * @LICENSE: MIT License Copyright (c) 2017 liu2guangb 
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Change Logs: 
 * Date           Author       Notes 
 * 2018-01-1     liu2guang    The first version. 
 */ 
#include "drv_rtc_ds1302.h"
#include "rtthread.h" 
#include "rtdevice.h" 
#include "time.h"

/* 引脚定义 */
#define DS1302_SCK_PIN  2 
#define DS1302_SDA_PIN  3 
#define DS1302_RST_PIN  4 

/* 内部函数 */
static void ds1302_delay(void)
{
    uint8_t index;
    
    for(index = 0; index < 20; index++);
}

static void ds1302_set_sck(uint8_t level)
{
    rt_pin_write(DS1302_SCK_PIN, level);
}

static void ds1302_set_sda(uint8_t level)
{
    rt_pin_mode (DS1302_SDA_PIN, PIN_MODE_OUTPUT); 
    rt_pin_write(DS1302_SDA_PIN, level); 
}

static void ds1302_set_rst(uint8_t level)
{
    rt_pin_write(DS1302_RST_PIN, level);
}

static uint8_t ds1302_get_sda(void)
{
    rt_pin_mode(DS1302_SDA_PIN, PIN_MODE_INPUT); 
    return rt_pin_read(DS1302_SDA_PIN);
}

static void ds1302_write8(uint8_t c)
{
    uint8_t index; 
    
    rt_enter_critical();
    for(index = 0; index < 8; index++)
    {
        ds1302_set_sda(c & 0x01); c >>= 1; 
        ds1302_delay(); 
        
        ds1302_set_sck(PIN_HIGH); 
        ds1302_delay(); 
        ds1302_set_sck(PIN_LOW); 
        ds1302_delay(); 
    }
    rt_exit_critical();
}

static uint8_t ds1302_read8(void)
{
    uint8_t index, c; 
    
    rt_enter_critical();
    for(index = 0; index < 8; index++)
    {
        c >>= 1;
        
        if(ds1302_get_sda() == PIN_HIGH)
        {
            c |= 0x80; 
        } 
        ds1302_delay(); 
        
        ds1302_set_sck(PIN_HIGH); 
        ds1302_delay(); 
        ds1302_set_sck(PIN_LOW); 
        ds1302_delay(); 
    }
    rt_exit_critical();
    
    return c; 
}

static void ds1302_write_reg(uint8_t addr, uint8_t data)
{
    ds1302_set_rst(PIN_HIGH); 
    ds1302_delay(); 
    
    ds1302_write8(addr);
    ds1302_delay(); 
    ds1302_write8(data);
    ds1302_delay(); 
    
    ds1302_set_rst(PIN_LOW); 
    ds1302_delay(); 
}

static uint8_t ds1302_read_reg(uint8_t addr)
{
    uint8_t data;
    
    ds1302_set_rst(PIN_HIGH); 
    ds1302_delay(); 
    
    ds1302_write8(addr); 
    ds1302_delay(); 
    data = ds1302_read8(); 
    ds1302_delay();
    
    ds1302_set_rst(PIN_LOW); 
    ds1302_delay(); 
    
    return data; 
}

static time_t ds1302_get_rtc_with_timestamp(void)
{
    uint8_t  temp = 0;
    uint32_t year = 0;
    struct tm tm_new = {0}; 
    
    temp = ds1302_read_reg(0x81); tm_new.tm_sec  = (temp >> 4) * 10 + (temp & 0x0F); 
    temp = ds1302_read_reg(0x83); tm_new.tm_min  = (temp >> 4) * 10 + (temp & 0x0F); 
    temp = ds1302_read_reg(0x85); tm_new.tm_hour = (temp >> 4) * 10 + (temp & 0x0F); 
    temp = ds1302_read_reg(0x87); tm_new.tm_mday = (temp >> 4) * 10 + (temp & 0x0F); 
    temp = ds1302_read_reg(0x89); tm_new.tm_mon  = (temp >> 4) * 10 + (temp & 0x0F) - 1;
    temp = ds1302_read_reg(0x8D); tm_new.tm_year = (temp >> 4) * 10 + (temp & 0x0F); 
    
    year  = ds1302_read_reg(0xC7); year <<= 8;
    year += ds1302_read_reg(0xC5); year <<= 8;
    year += ds1302_read_reg(0xC3); year <<= 8;
    year += ds1302_read_reg(0xC1); 
    
    tm_new.tm_year += year * 100; 
    
    return mktime(&tm_new);
}

static int ds1302_set_rtc_with_timestamp(time_t timestamp)
{
    uint8_t  temp = 0;
    uint32_t year = 0;
    struct tm* tm_new; 
    
    tm_new = localtime(&timestamp); 
    
    ds1302_write_reg(0x8E, 0);    temp = tm_new->tm_sec;        temp = ((temp / 10) << 4) + (temp % 10); 
    ds1302_write_reg(0x80, temp); temp = tm_new->tm_min;        temp = ((temp / 10) << 4) + (temp % 10); 
    ds1302_write_reg(0x82, temp); temp = tm_new->tm_hour;       temp = ((temp / 10) << 4) + (temp % 10); 
    ds1302_write_reg(0x84, temp); temp = tm_new->tm_mday;       temp = ((temp / 10) << 4) + (temp % 10); 
    ds1302_write_reg(0x86, temp); temp = tm_new->tm_mon  + 1;   temp = ((temp / 10) << 4) + (temp % 10); 
    ds1302_write_reg(0x88, temp); temp = tm_new->tm_year % 100; temp = ((temp / 10) << 4) + (temp % 10); 
    
    ds1302_write_reg(0x8C, temp);        year = tm_new->tm_year / 100; 
    ds1302_write_reg(0xC0, year & 0xFF); year >>= 8; 
    ds1302_write_reg(0xC2, year & 0xFF); year >>= 8; 
    ds1302_write_reg(0xC3, year & 0xFF); year >>= 8; 
    ds1302_write_reg(0xC6, year & 0xFF); 
    
    ds1302_write_reg(0x90, 0xA5); 
    ds1302_write_reg(0xC8, 0xAA); 
    ds1302_write_reg(0x8E, 0x80); 
    
    return RT_EOK; 
}

/* 驱动接口 */
static rt_err_t ds1302_rtc_init(rt_device_t dev)
{
    /* PIN初始化 */
    rt_pin_mode(DS1302_SCK_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(DS1302_SDA_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(DS1302_RST_PIN, PIN_MODE_OUTPUT);
    
    /* 开启DS1302涡流充电 */
    ds1302_write_reg(0x90, 0xA5);
    
    /* PIN默认电平 */
    rt_pin_write(DS1302_SCK_PIN, PIN_LOW); 
    rt_pin_write(DS1302_SDA_PIN, PIN_LOW);
    rt_pin_write(DS1302_RST_PIN, PIN_LOW); 
    
    return RT_EOK; 
}

static rt_err_t ds1302_rtc_open(rt_device_t dev, rt_uint16_t oflag)
{
    return RT_EOK; 
}

static rt_err_t ds1302_rtc_close(rt_device_t dev) 
{
    return RT_EOK; 
} 

static rt_size_t ds1302_rtc_read(rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
    return 0; 
}

static rt_size_t ds1302_rtc_write(rt_device_t dev, rt_off_t pos, const void* buffer, rt_size_t size)
{
    return 0; 
}

static rt_err_t ds1302_rtc_control(rt_device_t dev, int cmd, void *args)
{
    RT_ASSERT(dev != RT_NULL);
    
    switch(cmd)
    {
        case RT_DEVICE_CTRL_RTC_GET_TIME: 
        {
            *(uint32_t *)args = ds1302_get_rtc_with_timestamp(); 
        }
        break;
            
        case RT_DEVICE_CTRL_RTC_SET_TIME: 
        {
            ds1302_set_rtc_with_timestamp(*(time_t *)args); 
        }
        break;
            
        /* 无效参数 */
        default:
            return RT_EINVAL; 
    }
    
    return RT_EOK; 
}

static struct rt_device device = 
{
    .type    = RT_Device_Class_RTC, 
    .init    = ds1302_rtc_init, 
    .open    = ds1302_rtc_open, 
    .close   = ds1302_rtc_close, 
    .read    = ds1302_rtc_read,
    .write   = ds1302_rtc_write,
    .control = ds1302_rtc_control, 
    
    .rx_indicate = RT_NULL,
    .tx_complete = RT_NULL,
    .user_data   = RT_NULL
};

int rt_hw_rtc_ds1302_init(void)
{
    rt_err_t ret = RT_EOK;
    
    ret = rt_device_register(&device, "rtc", RT_DEVICE_FLAG_RDWR); 
    if(ret != RT_EOK)
    {
        return ret; 
    }
    
    return RT_EOK; 
}
INIT_BOARD_EXPORT(rt_hw_rtc_ds1302_init); 
