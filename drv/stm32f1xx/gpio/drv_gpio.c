/*
 * File      : gpio.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2015, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2016-08-29     Aubr.Cool      the first version
 */
#include <rthw.h>
#include <rtdevice.h>
#include <board.h>

/* 暂时只支持stm32f103xE(64引脚)系列 */
#define STM32F10X_PIN_NUMBERS 64

/* f1头文件 */
#include "stm32f1xx.h"

/* GPIO时钟 */
#define __RCC_GPIO_CLK_ENABLE(BIT)          \
do {                                        \
    __IO uint32_t tmpreg;                   \
    SET_BIT(RCC->APB2ENR, BIT);             \
    tmpreg = READ_BIT(RCC->APB2ENR, BIT);   \
    UNUSED(tmpreg);                         \
}while(0)

/* 引脚定义 */
#define __STM32_PIN(index, gpio, gpio_index) \
    {index, RCC_APB2ENR_IOP##gpio##EN, GPIO##gpio, GPIO_PIN_##gpio_index}
#define __STM32_PIN_DEFAULT {-1, 0, 0, 0}

/* GPIO驱动句柄结构体 */
struct pin_index
{
    int index;
    unsigned int clk;
    GPIO_TypeDef *gpio;
    uint32_t pin;
};

/* 引脚 */
static const struct pin_index pins[] =
{
#if (STM32F10X_PIN_NUMBERS == 64)
    __STM32_PIN_DEFAULT,        /* ռλ */
    __STM32_PIN_DEFAULT,        /* VBAT */
    __STM32_PIN(2, C, 13),
    __STM32_PIN_DEFAULT,        /* OSC32_IN */
    __STM32_PIN_DEFAULT,        /* OSC32_OUT */
    __STM32_PIN_DEFAULT,        /* OSC_IN */
    __STM32_PIN_DEFAULT,        /* OSC_OUT */
    __STM32_PIN_DEFAULT,        /* NRST */
    __STM32_PIN(8, C, 0),
    __STM32_PIN(9, C, 1),
    __STM32_PIN(10, C, 2),
    __STM32_PIN(11, C, 3),
    __STM32_PIN_DEFAULT,        /* VSSA */
    __STM32_PIN_DEFAULT,        /* VDDA */
    __STM32_PIN(14, A, 0),
    __STM32_PIN(15, A, 1),
    __STM32_PIN(16, A, 2),
    
    __STM32_PIN(17, A, 3),
    __STM32_PIN_DEFAULT,        /* VSS */
    __STM32_PIN_DEFAULT,        /* VDD */
    __STM32_PIN(20, A, 4),
    __STM32_PIN(21, A, 5),
    __STM32_PIN(22, A, 6),
    __STM32_PIN(23, A, 7),
    __STM32_PIN(24, C, 4),
    __STM32_PIN(25, C, 5),
    __STM32_PIN(26, B, 0),
    __STM32_PIN(27, B, 1),
    __STM32_PIN(28, B, 2),
    __STM32_PIN(29, B, 10),
    __STM32_PIN(30, B, 11),
    __STM32_PIN_DEFAULT,        /* VSS */
    __STM32_PIN_DEFAULT,        /* VDD */
    
    __STM32_PIN(33, B, 12),
    __STM32_PIN(34, B, 13),
    __STM32_PIN(35, B, 14),
    __STM32_PIN(36, B, 15),
    __STM32_PIN(37, C, 6),
    __STM32_PIN(38, C, 7),
    __STM32_PIN(39, C, 8),
    __STM32_PIN(40, C, 9),
    __STM32_PIN(41, A, 8),
    __STM32_PIN(42, A, 9),
    __STM32_PIN(43, A, 10),
    __STM32_PIN(44, A, 11),
    __STM32_PIN(45, A, 12),
    __STM32_PIN(46, A, 13),
    __STM32_PIN_DEFAULT,        /* VSS */
    __STM32_PIN_DEFAULT,        /* VDD */
    
    __STM32_PIN(49, A, 14),
    __STM32_PIN(50, A, 15),
    __STM32_PIN(51, C, 10),
    __STM32_PIN(52, C, 11),
    __STM32_PIN(53, C, 12),
    __STM32_PIN(54, D, 2),
    __STM32_PIN(55, B, 3),
    __STM32_PIN(56, B, 4),
    __STM32_PIN(57, B, 5),
    __STM32_PIN(58, B, 6),
    __STM32_PIN(59, B, 7),
    __STM32_PIN_DEFAULT,        /* BOOT0 */
    __STM32_PIN(61, B, 8),
    __STM32_PIN(62, B, 9),
    __STM32_PIN_DEFAULT,        /* VSS */
    __STM32_PIN_DEFAULT,        /* VDD */
#endif
};

/* 通过引脚顺序获取对应GPIO句柄 */
#define ITEM_NUM(items) sizeof(items)/sizeof(items[0])
const struct pin_index *get_pin(uint8_t pin)
{
    const struct pin_index *index;

    if(pin < ITEM_NUM(pins))
    {
        index = &pins[pin];
        if (index->index == -1)
            index = RT_NULL;
    }
    else
    {
        index = RT_NULL;
    }

    return index;
};

/* 底层GPIO读 */
static int stm32_pin_read(rt_device_t dev, rt_base_t pin)
{
    int value;
    const struct pin_index *index;

    value = PIN_LOW;

    index = get_pin(pin);
    if (index == RT_NULL)
    {
        return value;
    }
    value = HAL_GPIO_ReadPin(index->gpio, index->pin);
    return value;
}

/* 底层GPIO写 */
static void stm32_pin_write(rt_device_t dev, rt_base_t pin, rt_base_t value)
{
    const struct pin_index *index;

    index = get_pin(pin);
    if (index == RT_NULL)
    {
        return;
    }
    HAL_GPIO_WritePin(index->gpio, index->pin, (GPIO_PinState)value);
}

/* 底层GPIO模式选择 */
static void stm32_pin_mode(rt_device_t dev, rt_base_t pin, rt_base_t mode)
{
    const struct pin_index *index;
    GPIO_InitTypeDef  GPIO_InitStructure;

    /* 获取GPIO句柄 */
    index = get_pin(pin);
    if (index == RT_NULL)
    {
        return;
    }
    
    /* 初始化时钟 */
    __RCC_GPIO_CLK_ENABLE(index->clk);

    /* 配置GPIO结构体 */
    GPIO_InitStructure.Pin   = index->pin;
    GPIO_InitStructure.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;

    /* GPIO模式选择 */
    if(mode == GPIO_MODE_OUTPUT_PP)
    {
        /* output setting */
        GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStructure.Pull = GPIO_NOPULL;
    } 
    else if(mode == GPIO_MODE_OUTPUT_OD)
    {
        GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
        GPIO_InitStructure.Pull = GPIO_NOPULL;
    }
    else if(mode == GPIO_MODE_INPUT)
    {
        /* input setting: not pull. */
        GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
        GPIO_InitStructure.Pull = GPIO_NOPULL;
    } 
    else if((mode & 0xFF) == GPIO_MODE_AF_PP)
    {
        GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStructure.Pull = GPIO_NOPULL;
    }
    else if((mode & 0xFF) == GPIO_MODE_AF_OD)
    {
        GPIO_InitStructure.Mode = GPIO_MODE_AF_OD;
        GPIO_InitStructure.Pull = GPIO_NOPULL;
    }
    else if(mode == GPIO_MODE_ANALOG)
    {
        GPIO_InitStructure.Mode  = GPIO_MODE_ANALOG;
    } 
    else 
    {
        /* 默认输入, 降低功耗 */
        GPIO_InitStructure.Mode  = GPIO_MODE_INPUT;
        GPIO_InitStructure.Pull  = GPIO_NOPULL;
    }

    /* 初始化GPIO */
    HAL_GPIO_Init(index->gpio, &GPIO_InitStructure);
}

/* 底层gpio操作函数集合 */
const static struct rt_pin_ops _stm32_pin_ops =
{
    stm32_pin_mode,
    stm32_pin_write,
    stm32_pin_read,
};

/* 初始化pin驱动 */
int stm32_hw_pin_init(void)
{
    int result;

    result = rt_device_pin_register("pin", &_stm32_pin_ops, RT_NULL);
    
    return result;
}
INIT_BOARD_EXPORT(stm32_hw_pin_init);
