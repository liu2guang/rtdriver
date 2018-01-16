/*
 * @File:   drv_spi_flash.c 
 * @Author: liu2guang 
 * @Date:   2018-01-11 16:10:56 
 * 
 * @LICENSE: MIT License Copyright (c) 2017 liu2guang
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
 * 2018-01-11     liu2guang    第一版本. 
 */ 
#include "drv_spi.h" 
#include "drv_spi_flash.h"

#include "rtthread.h" 
#include "rtdevice.h" 
#include "spi_flash.h"
#include "spi_flash_sfud.h"

/* SFUD模式SPI FLASH初始化 */
int rt_hw_spi_flash_with_sfud_init(void)
{
    rt_err_t result; 
    
    result = stm32_spi_bus_attach_device(RT_SPI_FLASH_CS_PIN, "spi1", "spi10"); 
    if(result != RT_EOK) 
    {
        return result; 
    }
    
    if(rt_sfud_flash_probe("flash0", "spi10") == RT_NULL) 
    {
        return RT_ERROR;
    }

    return RT_EOK; 
}
INIT_DEVICE_EXPORT(rt_hw_spi_flash_with_sfud_init); 
