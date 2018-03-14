/*
 * File      : drv_pin.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006-2013, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-03-13     Liuguang     the first version. 
 */
#include "drv_pin.h" 

#include "fsl_common.h" 
#include "fsl_iomuxc.h" 
#include "fsl_gpio.h" 

/* GPIO外设时钟会在GPIO_PinInit中自动配置, 如果定义了以下宏则不会自动配置 */ 
#if defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL
    #error "Please don't define 'FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL'!"
#endif

/* RT1052 PIN描述结构体 */
struct rt1052_pin
{
    rt_uint16_t   pin;      /* 引脚索引, 请按照实际芯片引脚顺序分配, 0为无效 */
    GPIO_Type    *gpio;     /* 引脚GPIO */
    rt_uint32_t   gpio_pin; /* 引脚GPIOPIN */
}; 

/* 计算数组长度 */
#define __ARRAY_LEN(array) (sizeof(array)/sizeof(array[0])) 
#define __RT1052_PIN_DEFAULT {0, 0, 0} 
#define __RT1052_PIN(INDEX, PORT, PIN) {INDEX, PORT, PIN} 

/* PIN表 */
static struct rt1052_pin rt1052_pin_map[] = 
{
    __RT1052_PIN_DEFAULT, 
    
    /* GPIO4 */
    __RT1052_PIN( 1, GPIO4,  0),    /* GPIO */
    __RT1052_PIN( 2, GPIO4,  1), 
    __RT1052_PIN( 3, GPIO4,  2), 
    __RT1052_PIN( 4, GPIO4,  3), 
    __RT1052_PIN( 5, GPIO4,  4), 
    __RT1052_PIN( 6, GPIO4,  5), 
    __RT1052_PIN( 7, GPIO4,  6), 
    __RT1052_PIN( 8, GPIO4,  7), 
    __RT1052_PIN( 9, GPIO4,  8), 
    __RT1052_PIN(10, GPIO4,  9), 
    __RT1052_PIN(11, GPIO4, 10), 
    __RT1052_PIN(12, GPIO4, 11), 
    __RT1052_PIN(13, GPIO4, 12), 
    __RT1052_PIN(14, GPIO4, 13), 
    __RT1052_PIN(15, GPIO4, 14), 
    __RT1052_PIN(16, GPIO4, 15), 
    __RT1052_PIN(17, GPIO4, 16), 
    __RT1052_PIN(18, GPIO4, 17), 
    __RT1052_PIN(19, GPIO4, 18), 
    __RT1052_PIN(20, GPIO4, 19), 
    __RT1052_PIN(21, GPIO4, 20), 
    __RT1052_PIN(22, GPIO4, 21), 
    __RT1052_PIN(23, GPIO4, 22), 
    __RT1052_PIN(24, GPIO4, 23), 
    __RT1052_PIN(25, GPIO4, 24), 
    __RT1052_PIN(26, GPIO4, 25), 
    __RT1052_PIN(27, GPIO4, 26), 
    __RT1052_PIN(28, GPIO4, 27), 
    __RT1052_PIN(29, GPIO4, 28), 
    __RT1052_PIN(30, GPIO4, 29), 
    __RT1052_PIN(31, GPIO4, 30), 
    __RT1052_PIN(32, GPIO4, 31), 
    
    __RT1052_PIN(33, GPIO3, 18), 
    __RT1052_PIN(34, GPIO3, 19), 
    __RT1052_PIN(35, GPIO3, 20), 
    __RT1052_PIN(36, GPIO3, 21), 
    __RT1052_PIN(37, GPIO3, 22), 
    __RT1052_PIN(38, GPIO3, 23), 
    __RT1052_PIN(39, GPIO3, 24), 
    __RT1052_PIN(40, GPIO3, 25), 
    __RT1052_PIN(41, GPIO3, 26), 
    __RT1052_PIN(42, GPIO3, 27), 
    __RT1052_PIN(43, GPIO3, 28), 
    __RT1052_PIN(44, GPIO3, 29), 
    __RT1052_PIN(45, GPIO3, 30), 
    __RT1052_PIN(46, GPIO3, 31), 
    
    /* GPIO1 */
    __RT1052_PIN(47, GPIO1,  0), 
    __RT1052_PIN(48, GPIO1,  1), 
    __RT1052_PIN(49, GPIO1,  2), 
    __RT1052_PIN(50, GPIO1,  3), 
    __RT1052_PIN(51, GPIO1,  4), 
    __RT1052_PIN(52, GPIO1,  5), 
    __RT1052_PIN(53, GPIO1,  6), 
    __RT1052_PIN(54, GPIO1,  7), 
    __RT1052_PIN(55, GPIO1,  8), 
    __RT1052_PIN(56, GPIO1,  9), 
    __RT1052_PIN(57, GPIO1, 10), 
    __RT1052_PIN(58, GPIO1, 11), 
    __RT1052_PIN(59, GPIO1, 12), 
    __RT1052_PIN(60, GPIO1, 13), 
    __RT1052_PIN(61, GPIO1, 14), 
    __RT1052_PIN(62, GPIO1, 15), 
    __RT1052_PIN(63, GPIO1, 16), 
    __RT1052_PIN(64, GPIO1, 17), 
    __RT1052_PIN(65, GPIO1, 18), 
    __RT1052_PIN(66, GPIO1, 19), 
    __RT1052_PIN(67, GPIO1, 20), 
    __RT1052_PIN(68, GPIO1, 21), 
    __RT1052_PIN(69, GPIO1, 22), 
    __RT1052_PIN(70, GPIO1, 23), 
    __RT1052_PIN(71, GPIO1, 24), 
    __RT1052_PIN(72, GPIO1, 25), 
    __RT1052_PIN(73, GPIO1, 26), 
    __RT1052_PIN(74, GPIO1, 27), 
    __RT1052_PIN(75, GPIO1, 28), 
    __RT1052_PIN(76, GPIO1, 29), 
    __RT1052_PIN(77, GPIO1, 30), 
    __RT1052_PIN(78, GPIO1, 31),

    /* GPIO2 */
    __RT1052_PIN( 79, GPIO2,  0), 
    __RT1052_PIN( 80, GPIO2,  1), 
    __RT1052_PIN( 81, GPIO2,  2), 
    __RT1052_PIN( 82, GPIO2,  3), 
    __RT1052_PIN( 83, GPIO2,  4), 
    __RT1052_PIN( 84, GPIO2,  5), 
    __RT1052_PIN( 85, GPIO2,  6), 
    __RT1052_PIN( 86, GPIO2,  7), 
    __RT1052_PIN( 87, GPIO2,  8), 
    __RT1052_PIN( 88, GPIO2,  9), 
    __RT1052_PIN( 89, GPIO2, 10), 
    __RT1052_PIN( 90, GPIO2, 11), 
    __RT1052_PIN( 91, GPIO2, 12), 
    __RT1052_PIN( 92, GPIO2, 13), 
    __RT1052_PIN( 93, GPIO2, 14), 
    __RT1052_PIN( 94, GPIO2, 15), 
    __RT1052_PIN( 95, GPIO2, 16), 
    __RT1052_PIN( 96, GPIO2, 17), 
    __RT1052_PIN( 97, GPIO2, 18), 
    __RT1052_PIN( 98, GPIO2, 19), 
    __RT1052_PIN( 99, GPIO2, 20), 
    __RT1052_PIN(100, GPIO2, 21), 
    __RT1052_PIN(101, GPIO2, 22), 
    __RT1052_PIN(102, GPIO2, 23), 
    __RT1052_PIN(103, GPIO2, 24), 
    __RT1052_PIN(104, GPIO2, 25), 
    __RT1052_PIN(105, GPIO2, 26), 
    __RT1052_PIN(106, GPIO2, 27), 
    __RT1052_PIN(107, GPIO2, 28), 
    __RT1052_PIN(108, GPIO2, 29), 
    __RT1052_PIN(109, GPIO2, 30), 
    __RT1052_PIN(110, GPIO2, 31), 
    
    /* GPIO3 */
    __RT1052_PIN(111, GPIO3,  0), 
    __RT1052_PIN(112, GPIO3,  1), 
    __RT1052_PIN(113, GPIO3,  2), 
    __RT1052_PIN(114, GPIO3,  3), 
    __RT1052_PIN(115, GPIO3,  4), 
    __RT1052_PIN(116, GPIO3,  5), 
    __RT1052_PIN(117, GPIO3,  6), 
    __RT1052_PIN(118, GPIO3,  7), 
    __RT1052_PIN(119, GPIO3,  8), 
    __RT1052_PIN(120, GPIO3,  9), 
    __RT1052_PIN(121, GPIO3, 10), 
    __RT1052_PIN(122, GPIO3, 11), 
    __RT1052_PIN(123, GPIO3, 12), 
    __RT1052_PIN(124, GPIO3, 13), 
    __RT1052_PIN(125, GPIO3, 14), 
    __RT1052_PIN(126, GPIO3, 15), 
    __RT1052_PIN(127, GPIO3, 16), 
    __RT1052_PIN(128, GPIO3, 17), 

    /* GPIO5 */
    __RT1052_PIN(129, GPIO5,  0), 
    __RT1052_PIN(130, GPIO5,  1), 
    __RT1052_PIN(131, GPIO5,  2), 
}; 

/* PIN通用输入输出模式配置 */ 
static void rt1052_pin_mode(rt_device_t dev, rt_base_t pin, rt_base_t mode)
{
    gpio_pin_config_t gpio; 
    rt_uint32_t config_value = 0; 
    
    /* 检查pin是否合法 */ 
    if((pin > __ARRAY_LEN(rt1052_pin_map)) || (pin == 0))
    {
        return; 
    }
    
    /* GPIO5和1-4的略有不同 */
    if(rt1052_pin_map[pin].gpio != GPIO5)
    {
        CLOCK_EnableClock(kCLOCK_Iomuxc); 
        
    }
    else
    {
        CLOCK_EnableClock(kCLOCK_IomuxcSnvs); 
    }
    
    /* 配置IOMUXC: 将IO配置为GPIO */ 
    IOMUXC_SetPinMux(0x401F8000U + pin*4, 0x5U, 0, 0, 0, 0); 
    
    gpio.outputLogic = 0; 
    gpio.interruptMode = kGPIO_NoIntmode; 
    
    switch(mode)
    {
        case PIN_MODE_OUTPUT:
        {
            config_value = 0x1030U;
            gpio.direction = kGPIO_DigitalOutput; 
        }
        break; 
        
        case PIN_MODE_INPUT:
        {
            config_value = 0x1030U;
            gpio.direction = kGPIO_DigitalInput; 
        }
        break;
        
        case PIN_MODE_INPUT_PULLDOWN:
        {
            config_value = 0x1030U;
            gpio.direction = kGPIO_DigitalInput; 
        }
        break; 
        
        case PIN_MODE_INPUT_PULLUP:
        {
            config_value = 0x5030U;
            gpio.direction = kGPIO_DigitalInput; 
        }
        break;
        
        case PIN_MODE_OUTPUT_OD:
        {
            config_value = 0x1830U;
            gpio.direction = kGPIO_DigitalOutput; 
        }
        break;
    }
    
    /* 配置GPIO模式: 上下拉模式, 开漏模式 (所有都默认为GPIO翻转速度为50MHz) */ 
    IOMUXC_SetPinConfig(0, 0, 0, 0, 0x401F8200U + pin*4, config_value); 
    
    /* 初始化GPIO */ 
    GPIO_PinInit(rt1052_pin_map[pin].gpio, rt1052_pin_map[pin].gpio_pin, &gpio); 
}

/* PIN读取状态 */ 
static int rt1052_pin_read(rt_device_t dev, rt_base_t pin)
{
    return GPIO_PinRead(rt1052_pin_map[pin].gpio, rt1052_pin_map[pin].gpio_pin); 
}

/* PIN写状态 */
static void rt1052_pin_write(rt_device_t dev, rt_base_t pin, rt_base_t value)
{
    GPIO_PinWrite(rt1052_pin_map[pin].gpio, rt1052_pin_map[pin].gpio_pin, value);
}

/* PIN驱动底层OPS */
static struct rt_pin_ops rt1052_pin_ops = 
{
    .pin_mode        = rt1052_pin_mode, 
    .pin_read        = rt1052_pin_read, 
    .pin_write       = rt1052_pin_write, 
}; 

/* PIN驱动初始化 */
int rt_hw_pin_init(void)
{
    int ret = RT_EOK; 
    
    ret = rt_device_pin_register("pin", &rt1052_pin_ops, RT_NULL);
    
    return ret;
}
INIT_BOARD_EXPORT(rt_hw_pin_init); 
