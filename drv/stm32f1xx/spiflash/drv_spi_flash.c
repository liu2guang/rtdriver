/*
 * File      : drv_spi_flash.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009 RT-Thread Develop Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2012-01-01     aozima       first implementation.
 * 2012-07-27     aozima       fixed variable uninitialized.
 * 2017-08-19     liuguang     for stm32f103 + w25q64fv.
 */
#include <board.h>
#include "drv_spi.h"
#include "spi_flash.h"
#include "spi_flash_sfud.h"

#include <rthw.h>
#include <finsh.h>

int rt_hw_spi1_init(void)
{
    /* register spi bus */
    {
        GPIO_InitTypeDef GPIO_InitStructure;
		rt_err_t result;

		__HAL_RCC_GPIOA_CLK_ENABLE();

		GPIO_InitStructure.Pin = GPIO_PIN_5|GPIO_PIN_7;
		GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

		GPIO_InitStructure.Pin = GPIO_PIN_6;
		GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
		GPIO_InitStructure.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

		result = stm32_spi_bus_register(SPI1, "spi1");
        if (result != RT_EOK)
		{
			return result;
		}
    }

    /* attach cs */
    {
        static struct rt_spi_device spi_device;
        static struct stm32_spi_cs  spi_cs;
		rt_err_t result;

        GPIO_InitTypeDef GPIO_InitStructure;
		
		__HAL_RCC_GPIOA_CLK_ENABLE();
		
		GPIO_InitStructure.Mode  = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;

        spi_cs.GPIOx    = GPIOA;
        spi_cs.GPIO_Pin = GPIO_PIN_4;

        GPIO_InitStructure.Pin = spi_cs.GPIO_Pin;
        HAL_GPIO_WritePin(spi_cs.GPIOx, spi_cs.GPIO_Pin, GPIO_PIN_SET);
        HAL_GPIO_Init(spi_cs.GPIOx, &GPIO_InitStructure);

        result = rt_spi_bus_attach_device(&spi_device, "spi10", "spi1", (void*)&spi_cs);
		if (result != RT_EOK)
		{
			return result;
		}
    }

	return RT_EOK;
}

/* 注意在msh下使用sf查看flash的保护 */
int rt_hw_spi_flash_with_sfud_init(void)
{	
	rt_hw_spi1_init();
	
    if(RT_NULL == rt_sfud_flash_probe("flash0", "spi10"))
    {
        return RT_ERROR;
    };

	return RT_EOK;
}
INIT_DEVICE_EXPORT(rt_hw_spi_flash_with_sfud_init);
