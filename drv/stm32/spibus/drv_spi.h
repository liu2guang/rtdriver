/*
 * File      : drv_spi.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2017 RT-Thread Develop Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-08-17     liuguang     first implementation.
 */

#ifndef __DRV_SPI_H_
#define __DRV_SPI_H_

#include <rtthread.h>
#include <drivers/spi.h>

#include "stm32f1xx.h"

struct stm32f1_spi
{
    SPI_HandleTypeDef spi_handle;
};

struct stm32_spi_cs
{
    GPIO_TypeDef * GPIOx;
    rt_uint16_t GPIO_Pin;
};

/* public function */
rt_err_t stm32_spi_bus_register(SPI_TypeDef * SPI, const char * spi_bus_name);

#endif
