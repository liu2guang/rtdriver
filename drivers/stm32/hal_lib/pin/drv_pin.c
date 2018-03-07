/*
 * @File:   drv_pin.c 
 * @Author: liu2guang 
 * @Date:   2018-01-05 01:12:56 
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
 * 2018-01-05     liu2guang    第一版本. 
 * 2018-01-09     liu2guang    优化结构体结构. 
 */ 
#include "drv_pin.h"
#include "rtthread.h" 
#include "rtdevice.h" 

/* STM32 PIN描述结构体 */
struct stm32_pin
{
    rt_uint16_t   pin;              /* 引脚索引, 请按照实际芯片引脚顺序分配, 0为无效 */
    rt_uint32_t   rcc;              /* 引脚时钟 */
    GPIO_TypeDef *gpio;             /* 引脚GPIO */
    rt_uint32_t   gpio_pin;         /* 引脚GPIOPIN */
}; 

/* STM32 IRQ描述结构体 */
struct stm32_irq
{
    rt_uint16_t           enable;   /* 中断使能 */
    IRQn_Type             irq_num;  /* 中断编号 */
    struct rt_pin_irq_hdr irq_info; /* 中断信息: GPIOPIN, 中断触发模式, 回调函数, 回调参数 */
};

/* 计算数组长度 */
#define __ARRAY_LEN(array) (sizeof(array)/sizeof(array[0])) 
    
#define __STM32_PIN_DEFAULT          {0, 0, 0, 0} 
#define __PIN(INDEX, RCC, PORT, PIN) {INDEX, RCC##PORT##EN, GPIO##PORT, GPIO_PIN_##PIN} 
#define __GPIO_CLK_ENABLE(RCC, BIT)  \
do {                                 \
    __IO uint32_t tmpreg = 0x00U;    \
    SET_BIT(RCC, BIT);               \
    tmpreg = READ_BIT(RCC, BIT);     \
    UNUSED(tmpreg);                  \
}while(0)

#if defined(STM32F0) || defined(STM32F3) || defined(STM32L1)
#define __STM32_GPIO_CLK_ENABLE(BIT)  __GPIO_CLK_ENABLE(RCC->AHBENR, BIT)
#define __STM32_PIN(INDEX, PORT, PIN) __PIN(INDEX, RCC_AHB2ENR_GPIO, PORT, PIN)
#endif /* STM32F0 STM32F3 STM32L1 */ 

#if defined(STM32F1)
#define __STM32_GPIO_CLK_ENABLE(BIT)  __GPIO_CLK_ENABLE(RCC->APB2ENR, BIT)
#define __STM32_PIN(INDEX, PORT, PIN) __PIN(INDEX, RCC_APB2ENR_IOP, PORT, PIN)
#endif /* STM32F1 */ 

#if defined(STM32F2) || defined(STM32F4) || defined(STM32F7)
#define __STM32_GPIO_CLK_ENABLE(BIT)  __GPIO_CLK_ENABLE(RCC->AHB1ENR, BIT)
#define __STM32_PIN(INDEX, PORT, PIN) __PIN(INDEX, RCC_AHB1ENR_GPIO, PORT, PIN)
#endif /* STM32F2 STM32F4 STM32F7 */ 

#if defined(STM32L0)
#define __STM32_GPIO_CLK_ENABLE(BIT)  __GPIO_CLK_ENABLE(RCC->IOPENR, BIT)
#define __STM32_PIN(INDEX, PORT, PIN) __PIN(INDEX, RCC_IOPENR_GPIO, PORT, PIN)
#endif /* STM32L0 */ 

#if defined(STM32L4)
#define __STM32_GPIO_CLK_ENABLE(BIT)  __GPIO_CLK_ENABLE(RCC->AHB2ENR, BIT)
#define __STM32_PIN(INDEX, PORT, PIN) __PIN(INDEX, RCC_AHB2ENR_GPIO, PORT, pin)
#endif /* STM32L4 */ 

/* PIN表 */
static struct stm32_pin stm32_pin_map[] = 
{
#if defined(STM32FX_PACKAGE_EWLCSP49)   /* OK */
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT,                /* VDD_USB */
    __STM32_PIN(2, A, 15),
    __STM32_PIN(3, B, 3),
    __STM32_PIN(4, B, 5),
    __STM32_PIN_DEFAULT,                /* BOOT0 */
    __STM32_PIN(6, B, 9),
    __STM32_PIN_DEFAULT,                /* VDD */
    __STM32_PIN(8, A, 12),
    __STM32_PIN(9, A, 14),
    __STM32_PIN(10, B, 4),
    __STM32_PIN(11, B, 6),
    __STM32_PIN(12, B, 8),
    __STM32_PIN_DEFAULT,                /* VDD */
    __STM32_PIN(14, C, 13),
    __STM32_PIN(15, A, 10),
    __STM32_PIN(16, A, 13),
    __STM32_PIN(17, B, 7),
    __STM32_PIN(18, C, 1),
    __STM32_PIN(19, C, 0),
    __STM32_PIN(20, C, 14),             /* OSC32_IN */
    __STM32_PIN(21, C, 15),             /* OSC32_OUT */
    __STM32_PIN(22, A, 8),
    __STM32_PIN(23, A, 11),
    __STM32_PIN(24, B, 1),
    __STM32_PIN_DEFAULT,                /* VSS */
    __STM32_PIN_DEFAULT,                /* NRST */
    __STM32_PIN(27, H, 0),              /* OSC_IN */
    __STM32_PIN(28, H, 1),              /* OSC_OUT */
    __STM32_PIN(29, B, 15),
    __STM32_PIN(30, A, 9),
    __STM32_PIN(31, B, 2),
    __STM32_PIN(32, A, 1),
    __STM32_PIN(33, A, 0),
    __STM32_PIN_DEFAULT,                /* VREF+ */
    __STM32_PIN(35, C, 2),
    __STM32_PIN(36, B, 14),
    __STM32_PIN(37, B, 13),
    __STM32_PIN(38, B, 11),
    __STM32_PIN(39, A, 7),
    __STM32_PIN(40, A, 4),
    __STM32_PIN(41, A, 2),
    __STM32_PIN_DEFAULT,                /* VDDA */
    __STM32_PIN(43, B, 12),
    __STM32_PIN_DEFAULT,                /* VDD */
    __STM32_PIN(45, B, 10),
    __STM32_PIN(46, B, 0),
    __STM32_PIN(47, A, 6),
    __STM32_PIN(48, A, 5),
    __STM32_PIN(49, A, 3)
#endif /* STM32FX_PACKAGE_EWLCSP49 */

#if defined(STM32FX_PACKAGE_EWLCSP66)   /* OK */
    __STM32_PIN_DEFAULT,                /* None */
    __STM32_PIN(1, A, 14),
    __STM32_PIN(2, A, 15),
    __STM32_PIN(3, C, 12),
    __STM32_PIN(4, B, 3),
    __STM32_PIN(5, B, 5),
    __STM32_PIN(6, B, 7),
    __STM32_PIN(7, B, 9),
    __STM32_PIN_DEFAULT,                /* VDD */
    __STM32_PIN_DEFAULT,                /* VBAT */
    __STM32_PIN_DEFAULT,                /* VSS */
    __STM32_PIN(11, A, 13),
    __STM32_PIN(12, C, 10),
    __STM32_PIN(13, B, 4),
    __STM32_PIN(14, B, 6),
    __STM32_PIN_DEFAULT,                /* BOOT0 */
    __STM32_PIN(16, B, 8),
    __STM32_PIN(17, C, 13),
    __STM32_PIN(18, C, 14),             /* OSC32_IN */
    __STM32_PIN(19, A, 12),
    __STM32_PIN_DEFAULT,                /* VCAP_2 */
    __STM32_PIN(21, C, 11),
    __STM32_PIN(22, D, 2),
    __STM32_PIN_DEFAULT,                /* IRROFF */
    __STM32_PIN(24, C, 15),             /* OSC32_OUT */
    __STM32_PIN(25, C, 9),
    __STM32_PIN(26, A, 11),
    __STM32_PIN(27, A, 10),
    __STM32_PIN(28, C, 2),
    __STM32_PIN_DEFAULT,                /* VSS */
    __STM32_PIN_DEFAULT,                /* VDD */
    __STM32_PIN_DEFAULT,                /* VDD */
    __STM32_PIN(32, A, 8),
    __STM32_PIN(33, A, 9),
    __STM32_PIN(34, A, 0),
    __STM32_PIN_DEFAULT,                /* NRST */
    __STM32_PIN(36, H, 0),              /* OSC_IN */
    __STM32_PIN_DEFAULT,                /* VSS */
    __STM32_PIN(38, C, 7),
    __STM32_PIN(39, C, 8),
    __STM32_PIN_DEFAULT,                /* VREF+ */
    __STM32_PIN(41, C, 1),
    __STM32_PIN(42, H, 1),              /* OSC_OUT */
    __STM32_PIN(43, B, 15),
    __STM32_PIN(44, C, 6),
    __STM32_PIN(45, C, 5),
    __STM32_PIN(46, A, 3),
    __STM32_PIN(47, C, 3),
    __STM32_PIN(48, C, 0),
    __STM32_PIN(49, B, 14),
    __STM32_PIN(50, B, 13),
    __STM32_PIN(51, B, 10),
    __STM32_PIN(52, C, 4),
    __STM32_PIN(53, A, 6),
    __STM32_PIN(54, A, 5),
    __STM32_PIN_DEFAULT,                /* REGOFF */
    __STM32_PIN(56, A, 1),
    __STM32_PIN_DEFAULT,                /* VSS */
    __STM32_PIN(58, B, 12),
    __STM32_PIN(59, B, 11),
    __STM32_PIN_DEFAULT,                /* VCAP_1 */
    __STM32_PIN(61, B, 2),
    __STM32_PIN(62, B, 1),
    __STM32_PIN(63, B, 0),
    __STM32_PIN(64, A, 7),
    __STM32_PIN(65, A, 4),
    __STM32_PIN(66, A, 2)
#endif /* STM32FX_PACKAGE_EWLCSP66 */

#if defined(STM32FX_PACKAGE_LFBGA100)
    #error "STM32FX_PACKAGE_LFBGA100 Unrealized." 
#endif /* STM32FX_PACKAGE_LFBGA100 */

#if defined(STM32FX_PACKAGE_LFBGA144)
    #error "STM32FX_PACKAGE_LFBGA144 Unrealized." 
#endif /* STM32FX_PACKAGE_LFBGA144 */

#if defined(STM32FX_PACKAGE_LQFP32)
    #error "STM32FX_PACKAGE_LQFP32 Unrealized." 
#endif /* STM32FX_PACKAGE_LQFP32 */

#if defined(STM32FX_PACKAGE_LQFP48)     /* OK */
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT,
    __STM32_PIN(2, C, 13),
    __STM32_PIN(3, C, 14),
    __STM32_PIN(4, C, 15),
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT,
    __STM32_PIN(10, A, 0),
    __STM32_PIN(11, A, 1),
    __STM32_PIN(12, A, 2),
    __STM32_PIN(13, A, 3),
    __STM32_PIN(14, A, 4),
    __STM32_PIN(15, A, 5),
    __STM32_PIN(16, A, 6),
    __STM32_PIN(17, A, 7),
    __STM32_PIN(18, B, 0),
    __STM32_PIN(19, B, 1),
    __STM32_PIN(20, B, 2),
    __STM32_PIN(21, B, 10),
    __STM32_PIN(22, B, 11),
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT,
    __STM32_PIN(25, B, 12),
    __STM32_PIN(26, B, 13),
    __STM32_PIN(27, B, 14),
    __STM32_PIN(28, B, 15),
    __STM32_PIN(29, A, 8),
    __STM32_PIN(30, A, 9),
    __STM32_PIN(31, A, 10),
    __STM32_PIN(32, A, 11),
    __STM32_PIN(33, A, 12),
    __STM32_PIN(34, A, 13),
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT,
    __STM32_PIN(37, A, 14),
    __STM32_PIN(38, A, 15),
    __STM32_PIN(39, B, 3),
    __STM32_PIN(40, B, 4),
    __STM32_PIN(41, B, 5),
    __STM32_PIN(42, B, 6),
    __STM32_PIN(43, B, 7),
    __STM32_PIN_DEFAULT,
    __STM32_PIN(45, B, 8),
    __STM32_PIN(46, B, 9),
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT
#endif /* STM32FX_PACKAGE_LQFP48 */

#if defined(STM32FX_PACKAGE_LQFP64)     /* OK */
    __STM32_PIN_DEFAULT, 
    __STM32_PIN_DEFAULT,                /* VBAT */
    __STM32_PIN(2, C, 13), 
    __STM32_PIN(3, C, 14),
    __STM32_PIN(4, C, 15),
    __STM32_PIN(5, D, 0),               /* OSC_IN */
    __STM32_PIN(6, D, 1),               /* OSC_OUT */
    __STM32_PIN_DEFAULT,
    __STM32_PIN(8, C, 0), 
    __STM32_PIN(9, C, 1), 
    __STM32_PIN(10, C, 2), 
    __STM32_PIN(11, C, 3), 
    __STM32_PIN_DEFAULT,                /* VSSA */
    __STM32_PIN_DEFAULT,                /* VDDA */
    __STM32_PIN(14, A, 0),
    __STM32_PIN(15, A, 1),
    __STM32_PIN(16, A, 2),
    __STM32_PIN(17, A, 3),
    __STM32_PIN_DEFAULT,                /* VSSA */
    __STM32_PIN_DEFAULT,                /* VDDA */
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
    __STM32_PIN_DEFAULT,                /* VCAP_1 */
    __STM32_PIN_DEFAULT,                /* VDD */
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
    __STM32_PIN_DEFAULT,                /* VCAP_2 */
    __STM32_PIN_DEFAULT,                /* VDD */
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
    __STM32_PIN_DEFAULT,                /* BOOT0 */
    __STM32_PIN(61, B, 8),
    __STM32_PIN(62, B, 9),
    __STM32_PIN_DEFAULT,                /* VSS */
    __STM32_PIN_DEFAULT                 /* VDD */
#endif /* STM32FX_PACKAGE_LQFP64 */

#if defined(STM32FX_PACKAGE_LQFP100)    /* OK */
    __STM32_PIN_DEFAULT,
    __STM32_PIN(1,  E, 2),
    __STM32_PIN(2,  E, 3),
    __STM32_PIN(3,  E, 4),
    __STM32_PIN(4,  E, 5),
    __STM32_PIN(5,  E, 6),
    __STM32_PIN_DEFAULT,                /* VBAT */
    __STM32_PIN(7,  C, 13),
    __STM32_PIN(8,  C, 14),
    __STM32_PIN(9,  C, 15),
    __STM32_PIN_DEFAULT,                /* VSS */
    __STM32_PIN_DEFAULT,                /* VDD */
    __STM32_PIN(12, H, 0),              /* OSC_IN */
    __STM32_PIN(13, H, 1),              /* OSC_OUT */
    __STM32_PIN_DEFAULT,                /* NRST */
    __STM32_PIN(15, C, 0),
    __STM32_PIN(16, C, 1),
    __STM32_PIN(17, C, 2),
    __STM32_PIN(18, C, 3),
    __STM32_PIN_DEFAULT,                /* VDD */
    __STM32_PIN_DEFAULT,                /* VSSA */
    __STM32_PIN_DEFAULT,                /* VREF+ */
    __STM32_PIN_DEFAULT,                /* VDDA */
    __STM32_PIN(23, A, 0),
    __STM32_PIN(24, A, 1),
    __STM32_PIN(25, A, 2),
    __STM32_PIN(26, A, 3),
    __STM32_PIN_DEFAULT,                /* VSS */
    __STM32_PIN_DEFAULT,                /* VDD */
    __STM32_PIN(29, A, 4),
    __STM32_PIN(30, A, 5),
    __STM32_PIN(31, A, 6),
    __STM32_PIN(32, A, 7),
    __STM32_PIN(33, C, 4),
    __STM32_PIN(34, C, 5),
    __STM32_PIN(35, B, 0),
    __STM32_PIN(36, B, 1),
    __STM32_PIN(37, B, 2),
    __STM32_PIN(38, E, 7),
    __STM32_PIN(39, E, 8),
    __STM32_PIN(40, E, 9),
    __STM32_PIN(41, E, 10),
    __STM32_PIN(42, E, 11),
    __STM32_PIN(43, E, 12),
    __STM32_PIN(44, E, 13),
    __STM32_PIN(45, E, 14),
    __STM32_PIN(46, E, 15),
    __STM32_PIN(47, B, 10),
    __STM32_PIN(48, B, 11),
    __STM32_PIN_DEFAULT,                /* VCAP_1 */
    __STM32_PIN_DEFAULT,                /* VDD */
    __STM32_PIN(51, B, 12),
    __STM32_PIN(52, B, 13),
    __STM32_PIN(53, B, 14),
    __STM32_PIN(54, B, 15),
    __STM32_PIN(55, D, 8),
    __STM32_PIN(56, D, 9),
    __STM32_PIN(57, D, 10),
    __STM32_PIN(58, D, 11),
    __STM32_PIN(59, D, 12),
    __STM32_PIN(60, D, 13),
    __STM32_PIN(61, D, 14),
    __STM32_PIN(62, D, 15),
    __STM32_PIN(63, C, 6),
    __STM32_PIN(64, C, 7),
    __STM32_PIN(65, C, 8),
    __STM32_PIN(66, C, 9),
    __STM32_PIN(67, A, 8),
    __STM32_PIN(68, A, 9),
    __STM32_PIN(69, A, 10),
    __STM32_PIN(70, A, 11),
    __STM32_PIN(71, A, 12),
    __STM32_PIN(72, A, 13),
    __STM32_PIN_DEFAULT,                /* VCAP_2 */
    __STM32_PIN_DEFAULT,                /* VSS */
    __STM32_PIN_DEFAULT,                /* VDD */
    __STM32_PIN(76, A, 14),
    __STM32_PIN(77, A, 15),
    __STM32_PIN(78, C, 10),
    __STM32_PIN(79, C, 11),
    __STM32_PIN(80, C, 12),
    __STM32_PIN(81, D, 0),
    __STM32_PIN(82, D, 1),
    __STM32_PIN(83, D, 2),
    __STM32_PIN(84, D, 3),
    __STM32_PIN(85, D, 4),
    __STM32_PIN(86, D, 5),
    __STM32_PIN(87, D, 6),
    __STM32_PIN(88, D, 7),
    __STM32_PIN(89, B, 3),
    __STM32_PIN(90, B, 4),
    __STM32_PIN(91, B, 5),
    __STM32_PIN(92, B, 6),
    __STM32_PIN(93, B, 7),
    __STM32_PIN_DEFAULT,                /* BOOT0 */
    __STM32_PIN(95, B, 8),
    __STM32_PIN(96, B, 9),
    __STM32_PIN(97, E, 0),
    __STM32_PIN(98, E, 1),
    __STM32_PIN_DEFAULT,                /* RFU */
    __STM32_PIN_DEFAULT                 /* VDD */
#endif /* STM32FX_PACKAGE_LQFP100 */

#if defined(STM32FX_PACKAGE_LQFP144)    /* OK */
    __STM32_PIN_DEFAULT,
    __STM32_PIN(1, E, 2),
    __STM32_PIN(2, E, 3),
    __STM32_PIN(3, E, 4),
    __STM32_PIN(4, E, 5),
    __STM32_PIN(5, E, 6),
    __STM32_PIN_DEFAULT,
    __STM32_PIN(7, C, 13),
    __STM32_PIN(8, C, 14),
    __STM32_PIN(9, C, 15),
    __STM32_PIN(10, F, 0),
    __STM32_PIN(11, F, 1),
    __STM32_PIN(12, F, 2),
    __STM32_PIN(13, F, 3),
    __STM32_PIN(14, F, 4),
    __STM32_PIN(15, F, 5),
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT,
    __STM32_PIN(18, F, 6),
    __STM32_PIN(19, F, 7),
    __STM32_PIN(20, F, 8),
    __STM32_PIN(21, F, 9),
    __STM32_PIN(22, F, 10),
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT,
    __STM32_PIN(26, C, 0),
    __STM32_PIN(27, C, 1),
    __STM32_PIN(28, C, 2),
    __STM32_PIN(29, C, 3),
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT,
    __STM32_PIN(34, A, 0),
    __STM32_PIN(35, A, 1),
    __STM32_PIN(36, A, 2),
    __STM32_PIN(37, A, 3),
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT,
    __STM32_PIN(40, A, 4),
    __STM32_PIN(41, A, 5),
    __STM32_PIN(42, A, 6),
    __STM32_PIN(43, A, 7),
    __STM32_PIN(44, C, 4),
    __STM32_PIN(45, C, 5),
    __STM32_PIN(46, B, 0),
    __STM32_PIN(47, B, 1),
    __STM32_PIN(48, B, 2),
    __STM32_PIN(49, F, 11),
    __STM32_PIN(50, F, 12),
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT,
    __STM32_PIN(53, F, 13),
    __STM32_PIN(54, F, 14),
    __STM32_PIN(55, F, 15),
    __STM32_PIN(56, G, 0),
    __STM32_PIN(57, G, 1),
    __STM32_PIN(58, E, 7),
    __STM32_PIN(59, E, 8),
    __STM32_PIN(60, E, 9),
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT,
    __STM32_PIN(63, E, 10),
    __STM32_PIN(64, E, 11),
    __STM32_PIN(65, E, 12),
    __STM32_PIN(66, E, 13),
    __STM32_PIN(67, E, 14),
    __STM32_PIN(68, E, 15),
    __STM32_PIN(69, B, 10),
    __STM32_PIN(70, B, 11),
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT,
    __STM32_PIN(73, B, 12),
    __STM32_PIN(74, B, 13),
    __STM32_PIN(75, B, 14),
    __STM32_PIN(76, B, 15),
    __STM32_PIN(77, D, 8),
    __STM32_PIN(78, D, 9),
    __STM32_PIN(79, D, 10),
    __STM32_PIN(80, D, 11),
    __STM32_PIN(81, D, 12),
    __STM32_PIN(82, D, 13),
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT,
    __STM32_PIN(85, D, 14),
    __STM32_PIN(86, D, 15),
    __STM32_PIN(87, G, 2),
    __STM32_PIN(88, G, 3),
    __STM32_PIN(89, G, 4),
    __STM32_PIN(90, G, 5),
    __STM32_PIN(91, G, 6),
    __STM32_PIN(92, G, 7),
    __STM32_PIN(93, G, 8),
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT,
    __STM32_PIN(96, C, 6),
    __STM32_PIN(97, C, 7),
    __STM32_PIN(98, C, 8),
    __STM32_PIN(99, C, 9),
    __STM32_PIN(100, A, 8),
    __STM32_PIN(101, A, 9),
    __STM32_PIN(102, A, 10),
    __STM32_PIN(103, A, 11),
    __STM32_PIN(104, A, 12),
    __STM32_PIN(105, A, 13),
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT,
    __STM32_PIN(109, A, 14),
    __STM32_PIN(110, A, 15),
    __STM32_PIN(111, C, 10),
    __STM32_PIN(112, C, 11),
    __STM32_PIN(113, C, 12),
    __STM32_PIN(114, D, 0),
    __STM32_PIN(115, D, 1),
    __STM32_PIN(116, D, 2),
    __STM32_PIN(117, D, 3),
    __STM32_PIN(118, D, 4),
    __STM32_PIN(119, D, 5),
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT,
    __STM32_PIN(122, D, 6),
    __STM32_PIN(123, D, 7),
    __STM32_PIN(124, G, 9),
    __STM32_PIN(125, G, 10),
    __STM32_PIN(126, G, 11),
    __STM32_PIN(127, G, 12),
    __STM32_PIN(128, G, 13),
    __STM32_PIN(129, G, 14),
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT,
    __STM32_PIN(132, G, 15),
    __STM32_PIN(133, B, 3),
    __STM32_PIN(134, B, 4),
    __STM32_PIN(135, B, 5),
    __STM32_PIN(136, B, 6),
    __STM32_PIN(137, B, 7),
    __STM32_PIN_DEFAULT,
    __STM32_PIN(139, B, 8),
    __STM32_PIN(140, B, 9),
    __STM32_PIN(141, E, 0),
    __STM32_PIN(142, E, 1),
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT
#endif /* STM32FX_PACKAGE_LQFP144 */

#if defined(STM32FX_PACKAGE_LQFP176)
    #error "STM32FX_PACKAGE_LQFP176 Unrealized." 
#endif /* STM32FX_PACKAGE_LQFP176 */

#if defined(STM32FX_PACKAGE_LQFP208)    /* OK */
    __STM32_PIN_DEFAULT,                /* NONE */
    __STM32_PIN(1, E, 2),               /*  */
    __STM32_PIN(2, E, 3),
    __STM32_PIN(3, E, 4),
    __STM32_PIN(4, E, 5),
    __STM32_PIN(5, E, 6),
    __STM32_PIN_DEFAULT,                /* VBAT */
    __STM32_PIN(7, I, 8),
    __STM32_PIN(8, C, 13),
    __STM32_PIN_DEFAULT,                /* OSC32_IN */
    __STM32_PIN_DEFAULT,                /* OSC32_OUT */
    __STM32_PIN(11, I, 9),
    __STM32_PIN(12, I, 10),
    __STM32_PIN(13, I, 11),
    __STM32_PIN_DEFAULT,                /* VSS */
    __STM32_PIN_DEFAULT,                /* VDD */
    __STM32_PIN(15, F, 0),
    __STM32_PIN(16, F, 1),
    __STM32_PIN(17, F, 2),
    __STM32_PIN(18, I, 12),
    __STM32_PIN(20, I, 13),
    __STM32_PIN(21, I, 14),
    __STM32_PIN(22, F, 3),
    __STM32_PIN(23, F, 4),
    __STM32_PIN(24, F, 5),
    __STM32_PIN_DEFAULT,                /* VSS */
    __STM32_PIN_DEFAULT,                /* VDD */
    __STM32_PIN(27, F, 6),
    __STM32_PIN(28, F, 7),
    __STM32_PIN(29, F, 8),
    __STM32_PIN(30, F, 9),
    __STM32_PIN(31, F, 10),
    __STM32_PIN_DEFAULT,                /* OSC_IN */
    __STM32_PIN_DEFAULT,                /* OSC_OUT */
    __STM32_PIN_DEFAULT,                /* RESET */
    __STM32_PIN(35, C, 0),
    __STM32_PIN(36, C, 1),
    __STM32_PIN(37, C, 2),
    __STM32_PIN(38, C, 3),
    __STM32_PIN_DEFAULT,                /* VDD */
    __STM32_PIN_DEFAULT,                /* VSSA */
    __STM32_PIN_DEFAULT,                /* VREF+ */
    __STM32_PIN_DEFAULT,                /* VDDA */
    __STM32_PIN(43, A, 0),
    __STM32_PIN(44, A, 1),
    __STM32_PIN(45, A, 2),
    __STM32_PIN(46, H, 2),
    __STM32_PIN(47, H, 3),
    __STM32_PIN(48, H, 4),
    __STM32_PIN(49, H, 5),
    __STM32_PIN(50, A, 3),
    __STM32_PIN_DEFAULT,                /* VSS */
    __STM32_PIN_DEFAULT,                /* VDD */
    __STM32_PIN(53, A, 4),
    __STM32_PIN(54, A, 5),
    __STM32_PIN(55, A, 6),
    __STM32_PIN(56, A, 7),
    __STM32_PIN(57, C, 4),
    __STM32_PIN(58, C, 5),
    __STM32_PIN_DEFAULT,                /* VDD */
    __STM32_PIN_DEFAULT,                /* VSS */
    __STM32_PIN(61, B, 0),
    __STM32_PIN(62, B, 1),
    __STM32_PIN_DEFAULT,                /* BOOT1 */
    __STM32_PIN(64, I, 15),
    __STM32_PIN(65, J, 0),
    __STM32_PIN(66, J, 1),
    __STM32_PIN(67, J, 2),
    __STM32_PIN(68, J, 3),
    __STM32_PIN(69, J, 4),
    __STM32_PIN(70, F, 11),
    __STM32_PIN(71, F, 12),
    __STM32_PIN_DEFAULT,                /* VSS */
    __STM32_PIN_DEFAULT,                /* VDD */
    __STM32_PIN(74, F, 13),
    __STM32_PIN(75, F, 14),
    __STM32_PIN(76, F, 15),
    __STM32_PIN(77, G, 0),
    __STM32_PIN(78, G, 1),
    __STM32_PIN(79, E, 7),
    __STM32_PIN(80, E, 8),
    __STM32_PIN(81, E, 9),
    __STM32_PIN_DEFAULT,                /* VSS */
    __STM32_PIN_DEFAULT,                /* VDD */
    __STM32_PIN(84, E, 10),
    __STM32_PIN(85, E, 11),
    __STM32_PIN(86, E, 12),
    __STM32_PIN(87, E, 13),
    __STM32_PIN(88, E, 14),
    __STM32_PIN(89, E, 15),
    __STM32_PIN(90, B, 10),
    __STM32_PIN(91, B, 11),
    __STM32_PIN_DEFAULT,                /* VCAP_1 */
    __STM32_PIN_DEFAULT,                /* VSS */
    __STM32_PIN_DEFAULT,                /* VDD */
    __STM32_PIN(95, J, 5),
    __STM32_PIN(96, H, 6),
    __STM32_PIN(97, H, 7),
    __STM32_PIN(98, H, 8),
    __STM32_PIN(99, H, 9),
    __STM32_PIN(100, H, 10),
    __STM32_PIN(101, H, 11),
    __STM32_PIN(102, H, 12),
    __STM32_PIN_DEFAULT,                /* VDD */
    __STM32_PIN(104, B, 12),
    __STM32_PIN(105, B, 13),
    __STM32_PIN(106, B, 14),
    __STM32_PIN(107, B, 15),
    __STM32_PIN(108, D, 8),
    __STM32_PIN(109, D, 9),
    __STM32_PIN(110, D, 10),
    __STM32_PIN(111, D, 11),
    __STM32_PIN(112, D, 12),
    __STM32_PIN(113, D, 13),
    __STM32_PIN_DEFAULT,                /* VSS */
    __STM32_PIN_DEFAULT,                /* VDD */
    __STM32_PIN(116, D, 14),
    __STM32_PIN(117, D, 15),
    __STM32_PIN(118, J, 6),
    __STM32_PIN(119, J, 7),
    __STM32_PIN(120, J, 8),
    __STM32_PIN(121, J, 9),
    __STM32_PIN(122, J, 10),
    __STM32_PIN(123, J, 11),
    __STM32_PIN_DEFAULT,                /* VDD */
    __STM32_PIN_DEFAULT,                /* VSS */
    __STM32_PIN(126, K, 0),
    __STM32_PIN(127, K, 1),
    __STM32_PIN(128, K, 2),
    __STM32_PIN(129, G, 2),
    __STM32_PIN(130, G, 3),
    __STM32_PIN(131, G, 4),
    __STM32_PIN(132, G, 5),
    __STM32_PIN(133, G, 6),
    __STM32_PIN(134, G, 7),
    __STM32_PIN(135, G, 8),
    __STM32_PIN_DEFAULT,                /* VSS */
    __STM32_PIN_DEFAULT,                /* VDD */
    __STM32_PIN(138, C, 6),
    __STM32_PIN(139, C, 7),
    __STM32_PIN(140, C, 8),
    __STM32_PIN(141, C, 9),
    __STM32_PIN(142, A, 8),
    __STM32_PIN(143, A, 9),
    __STM32_PIN(144, A, 10),
    __STM32_PIN(145, A, 11),
    __STM32_PIN(146, A, 12),
    __STM32_PIN(147, A, 13),
    __STM32_PIN_DEFAULT,                /* VCAP_2 */
    __STM32_PIN_DEFAULT,                /* VSS */
    __STM32_PIN_DEFAULT,                /* VDD */
    __STM32_PIN(151, H, 13),
    __STM32_PIN(152, H, 14),
    __STM32_PIN(153, H, 15),
    __STM32_PIN(154, I, 0),
    __STM32_PIN(155, I, 1),
    __STM32_PIN(156, I, 2),
    __STM32_PIN(157, I, 3),
    __STM32_PIN_DEFAULT,                /* VDD */
    __STM32_PIN(159, A, 14),
    __STM32_PIN(160, A, 15),
    __STM32_PIN(161, C, 10),
    __STM32_PIN(162, C, 11),
    __STM32_PIN(163, C, 12),
    __STM32_PIN(164, D, 0),
    __STM32_PIN(165, D, 1),
    __STM32_PIN(166, D, 2),
    __STM32_PIN(167, D, 3),
    __STM32_PIN(168, D, 4),
    __STM32_PIN(169, D, 5),
    __STM32_PIN_DEFAULT,                /* VSS */
    __STM32_PIN_DEFAULT,                /* VDD */
    __STM32_PIN(172, D, 6),
    __STM32_PIN(173, D, 7),
    __STM32_PIN(174, J, 12),
    __STM32_PIN(175, J, 13),
    __STM32_PIN(176, J, 14),
    __STM32_PIN(177, J, 15),
    __STM32_PIN(178, G, 9),
    __STM32_PIN(179, G, 10),
    __STM32_PIN(180, G, 11),
    __STM32_PIN(181, G, 12),
    __STM32_PIN(182, G, 13),
    __STM32_PIN(183, G, 14),
    __STM32_PIN_DEFAULT,                /* VSS */
    __STM32_PIN_DEFAULT,                /* VDD */
    __STM32_PIN(186, K, 3),
    __STM32_PIN(187, K, 4),
    __STM32_PIN(188, K, 5),
    __STM32_PIN(189, K, 6),
    __STM32_PIN(190, K, 7),
    __STM32_PIN(191, G, 15),
    __STM32_PIN(192, B, 3),
    __STM32_PIN(193, B, 4),
    __STM32_PIN(194, B, 5),
    __STM32_PIN(195, B, 6),
    __STM32_PIN(196, B, 7),
    __STM32_PIN_DEFAULT,                /* BOOT0 */
    __STM32_PIN(198, B, 8),
    __STM32_PIN(199, B, 9),
    __STM32_PIN(200, E, 0),
    __STM32_PIN(201, E, 1),
    __STM32_PIN_DEFAULT,                /* VSS */
    __STM32_PIN_DEFAULT,                /* PDR_ON */
    __STM32_PIN_DEFAULT,                /* VDD */
    __STM32_PIN(205, I, 4),
    __STM32_PIN(206, I, 5),
    __STM32_PIN(207, I, 6),
    __STM32_PIN(208, I, 7)
#endif /* STM32FX_PACKAGE_LQFP208 */

#if defined(STM32FX_PACKAGE_TFBGA64)
    #error "STM32FX_PACKAGE_TFBGA64 Unrealized." 
#endif /* STM32FX_PACKAGE_TFBGA64 */

#if defined(STM32FX_PACKAGE_TFBGA100)
    #error "STM32FX_PACKAGE_TFBGA100 Unrealized." 
#endif /* STM32FX_PACKAGE_TFBGA100 */

#if defined(STM32FX_PACKAGE_TFBGA216)
    #error "STM32FX_PACKAGE_TFBGA216 Unrealized." 
#endif /* STM32FX_PACKAGE_TFBGA216 */

#if defined(STM32FX_PACKAGE_TFBGA240)
    #error "STM32FX_PACKAGE_TFBGA240 Unrealized." 
#endif /* STM32FX_PACKAGE_TFBGA240 */

#if defined(STM32FX_PACKAGE_TSSOP14)
    #error "STM32FX_PACKAGE_TSSOP14 Unrealized." 
#endif /* STM32FX_PACKAGE_TSSOP14 */

#if defined(STM32FX_PACKAGE_TSSOP20)    /* OK */
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT,
    __STM32_PIN(2, F, 0),
    __STM32_PIN(3, F, 1),
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT,
    __STM32_PIN(6, A, 0),
    __STM32_PIN(7, A, 1),
    __STM32_PIN(8, A, 2),
    __STM32_PIN(9, A, 3),
    __STM32_PIN(10, A, 4),
    __STM32_PIN(11, A, 5),
    __STM32_PIN(12, A, 6),
    __STM32_PIN(13, A, 7),
    __STM32_PIN(14, B, 1),
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT,
    __STM32_PIN(17, A, 9),
    __STM32_PIN(18, A, 10),
    __STM32_PIN(19, A, 13),
    __STM32_PIN(20, A, 14)
#endif /* STM32FX_PACKAGE_TSSOP20 */

#if defined(STM32FX_PACKAGE_UFBGA100)
    #error "STM32FX_PACKAGE_UFBGA100 Unrealized." 
#endif /* STM32FX_PACKAGE_UFBGA100 */

#if defined(STM32FX_PACKAGE_UFBGA132)
    #error "STM32FX_PACKAGE_UFBGA132 Unrealized." 
#endif /* STM32FX_PACKAGE_UFBGA132 */

#if defined(STM32FX_PACKAGE_UFBGA144)
    #error "STM32FX_PACKAGE_UFBGA144 Unrealized." 
#endif /* STM32FX_PACKAGE_UFBGA144 */

#if defined(STM32FX_PACKAGE_UFBGA169)
    #error "STM32FX_PACKAGE_UFBGA169 Unrealized." 
#endif /* STM32FX_PACKAGE_UFBGA169 */

#if defined(STM32FX_PACKAGE_UFBGA176)
    #error "STM32FX_PACKAGE_UFBGA176 Unrealized." 
#endif /* STM32FX_PACKAGE_UFBGA176 */

#if defined(STM32FX_PACKAGE_UFQFPN20)
    #error "STM32FX_PACKAGE_UFQFPN20 Unrealized." 
#endif /* STM32FX_PACKAGE_UFQFPN20 */

#if defined(STM32FX_PACKAGE_UFQFPN28)   /* OK */
    __STM32_PIN_DEFAULT,                /* None */
    __STM32_PIN_DEFAULT,                /* BOOT0 */
    __STM32_PIN(2, F, 0),               /* OSC_IN */
    __STM32_PIN(3, F, 1),               /* OSC_OUT */
    __STM32_PIN_DEFAULT,                /* NRST */
    __STM32_PIN_DEFAULT,                /* VDDA */
    __STM32_PIN(6, A, 0),
    __STM32_PIN(7, A, 1),
    __STM32_PIN(8, A, 2),
    __STM32_PIN(9, A, 3),
    __STM32_PIN(10, A, 4),
    __STM32_PIN(11, A, 5),
    __STM32_PIN(12, A, 6),
    __STM32_PIN(13, A, 7),
    __STM32_PIN(14, B, 0),
    __STM32_PIN(15, B, 1),
    __STM32_PIN_DEFAULT,                /* VSS */
    __STM32_PIN_DEFAULT,                /* VDD */
    __STM32_PIN(18, A, 8),
    __STM32_PIN(19, A, 9),
    __STM32_PIN(20, A, 10),
    __STM32_PIN(21, A, 13),
    __STM32_PIN(22, A, 14),
    __STM32_PIN(23, A, 15),
    __STM32_PIN(24, B, 3),
    __STM32_PIN(25, B, 4),
    __STM32_PIN(26, B, 5),
    __STM32_PIN(27, B, 6),
    __STM32_PIN(28, B, 7)
#endif /* STM32FX_PACKAGE_UFQFPN28 */

#if defined(STM32FX_PACKAGE_UFQFPN32)
    #error "STM32FX_PACKAGE_UFQFPN32 Unrealized." 
#endif /* STM32FX_PACKAGE_UFQFPN32 */

#if defined(STM32FX_PACKAGE_UFQFPN36)
    #error "STM32FX_PACKAGE_UFQFPN36 Unrealized." 
#endif /* STM32FX_PACKAGE_UFQFPN36 */

#if defined(STM32FX_PACKAGE_UFQFPN48)
    #error "STM32FX_PACKAGE_UFQFPN48 Unrealized." 
#endif /* STM32FX_PACKAGE_UFQFPN48 */

#if defined(STM32FX_PACKAGE_WLCSP25)    /* OK */
    __STM32_PIN_DEFAULT,
    __STM32_PIN(1, A, 13),
    __STM32_PIN(2, A, 14),
    __STM32_PIN(3, B, 7),
    __STM32_PIN_DEFAULT,
    __STM32_PIN(5, F, 0),
    __STM32_PIN(6, A, 10),
    __STM32_PIN(7, B, 6),
    __STM32_PIN(8, A, 4),
    __STM32_PIN(9, A, 0),
    __STM32_PIN(10, F, 1),
    __STM32_PIN(11, A, 9),
    __STM32_PIN(12, B, 5),
    __STM32_PIN(13, A, 5),
    __STM32_PIN(14, A, 1),
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT,
    __STM32_PIN(17, A, 8),
    __STM32_PIN(18, A, 6),
    __STM32_PIN(19, A, 2),
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT,
    __STM32_PIN(22, B, 1),
    __STM32_PIN(23, B, 0),
    __STM32_PIN(24, A, 7),
    __STM32_PIN(25, A, 3)
#endif /* STM32FX_PACKAGE_WLCSP25 */

#if defined(STM32FX_PACKAGE_WLCSP36)
    #error "STM32FX_PACKAGE_WLCSP36 Unrealized." 
#endif /* STM32FX_PACKAGE_WLCSP36 */

#if defined(STM32FX_PACKAGE_WLCSP49)
    #error "STM32FX_PACKAGE_WLCSP49 Unrealized." 
#endif /* STM32FX_PACKAGE_WLCSP49 */

#if defined(STM32FX_PACKAGE_WLCSP63)
    #error "STM32FX_PACKAGE_WLCSP63 Unrealized." 
#endif /* STM32FX_PACKAGE_WLCSP63 */

#if defined(STM32FX_PACKAGE_WLCSP64)
    #error "STM32FX_PACKAGE_WLCSP64 Unrealized." 
#endif /* STM32FX_PACKAGE_WLCSP64 */

#if defined(STM32FX_PACKAGE_WLCSP66)
    #error "STM32FX_PACKAGE_WLCSP66 Unrealized." 
#endif /* STM32FX_PACKAGE_WLCSP66 */

#if defined(STM32FX_PACKAGE_WLCSP72)
    #error "STM32FX_PACKAGE_WLCSP72 Unrealized." 
#endif /* STM32FX_PACKAGE_WLCSP72 */

#if defined(STM32FX_PACKAGE_WLCSP81)
    #error "STM32FX_PACKAGE_WLCSP81 Unrealized." 
#endif /* STM32FX_PACKAGE_WLCSP81 */

#if defined(STM32FX_PACKAGE_WLCSP90)
    #error "STM32FX_PACKAGE_WLCSP90 Unrealized." 
#endif /* STM32FX_PACKAGE_WLCSP90 */

#if defined(STM32FX_PACKAGE_WLCSP104)
    #error "STM32FX_PACKAGE_WLCSP104 Unrealized." 
#endif /* STM32FX_PACKAGE_WLCSP104 */

#if defined(STM32FX_PACKAGE_WLCSP143)
    #error "STM32FX_PACKAGE_WLCSP143 Unrealized." 
#endif /* STM32FX_PACKAGE_WLCSP143 */

#if defined(STM32FX_PACKAGE_WLCSP144)
    #error "STM32FX_PACKAGE_WLCSP144 Unrealized." 
#endif /* STM32FX_PACKAGE_WLCSP144 */

#if defined(STM32FX_PACKAGE_WLCSP168)
    #error "STM32FX_PACKAGE_WLCSP168 Unrealized." 
#endif /* STM32FX_PACKAGE_WLCSP168 */

#if defined(STM32FX_PACKAGE_WLCSP180)
    #error "STM32FX_PACKAGE_WLCSP180 Unrealized." 
#endif /* STM32FX_PACKAGE_WLCSP180 */
}; 

/* IRQ表 */
static struct stm32_irq stm32_irq_map[] = 
{
    {PIN_IRQ_DISABLE, EXTI0_IRQn,     {PIN_IRQ_PIN_NONE, PIN_IRQ_MODE_RISING, RT_NULL, RT_NULL}}, 
    {PIN_IRQ_DISABLE, EXTI1_IRQn,     {PIN_IRQ_PIN_NONE, PIN_IRQ_MODE_RISING, RT_NULL, RT_NULL}}, 
    {PIN_IRQ_DISABLE, EXTI2_IRQn,     {PIN_IRQ_PIN_NONE, PIN_IRQ_MODE_RISING, RT_NULL, RT_NULL}}, 
    {PIN_IRQ_DISABLE, EXTI3_IRQn,     {PIN_IRQ_PIN_NONE, PIN_IRQ_MODE_RISING, RT_NULL, RT_NULL}}, 
    {PIN_IRQ_DISABLE, EXTI4_IRQn,     {PIN_IRQ_PIN_NONE, PIN_IRQ_MODE_RISING, RT_NULL, RT_NULL}}, 
    {PIN_IRQ_DISABLE, EXTI9_5_IRQn,   {PIN_IRQ_PIN_NONE, PIN_IRQ_MODE_RISING, RT_NULL, RT_NULL}}, 
    {PIN_IRQ_DISABLE, EXTI9_5_IRQn,   {PIN_IRQ_PIN_NONE, PIN_IRQ_MODE_RISING, RT_NULL, RT_NULL}}, 
    {PIN_IRQ_DISABLE, EXTI9_5_IRQn,   {PIN_IRQ_PIN_NONE, PIN_IRQ_MODE_RISING, RT_NULL, RT_NULL}}, 
    {PIN_IRQ_DISABLE, EXTI9_5_IRQn,   {PIN_IRQ_PIN_NONE, PIN_IRQ_MODE_RISING, RT_NULL, RT_NULL}}, 
    {PIN_IRQ_DISABLE, EXTI9_5_IRQn,   {PIN_IRQ_PIN_NONE, PIN_IRQ_MODE_RISING, RT_NULL, RT_NULL}}, 
    {PIN_IRQ_DISABLE, EXTI15_10_IRQn, {PIN_IRQ_PIN_NONE, PIN_IRQ_MODE_RISING, RT_NULL, RT_NULL}}, 
    {PIN_IRQ_DISABLE, EXTI15_10_IRQn, {PIN_IRQ_PIN_NONE, PIN_IRQ_MODE_RISING, RT_NULL, RT_NULL}}, 
    {PIN_IRQ_DISABLE, EXTI15_10_IRQn, {PIN_IRQ_PIN_NONE, PIN_IRQ_MODE_RISING, RT_NULL, RT_NULL}}, 
    {PIN_IRQ_DISABLE, EXTI15_10_IRQn, {PIN_IRQ_PIN_NONE, PIN_IRQ_MODE_RISING, RT_NULL, RT_NULL}}, 
    {PIN_IRQ_DISABLE, EXTI15_10_IRQn, {PIN_IRQ_PIN_NONE, PIN_IRQ_MODE_RISING, RT_NULL, RT_NULL}}, 
    {PIN_IRQ_DISABLE, EXTI15_10_IRQn, {PIN_IRQ_PIN_NONE, PIN_IRQ_MODE_RISING, RT_NULL, RT_NULL}}
};

/* 内部函数 */
/* 通过PIN引脚索引获取PIN描述结构体 */
static struct stm32_pin* get_pinmap_from_pin(rt_uint16_t pin)
{
    if(pin >= __ARRAY_LEN(stm32_pin_map) || pin == 0)
    {
        return RT_NULL;
    }
    
    if(stm32_pin_map[pin].pin == 0)
    {
        return RT_NULL;
    }
    
    return &stm32_pin_map[pin];
}

/* 通过GPIO PIN获取IRQ描述结构体 */
static struct stm32_irq* get_irqmap_from_pin(rt_uint16_t pin)
{
    rt_uint32_t index;
    struct stm32_pin *pin_map = RT_NULL;
    
    pin_map = get_pinmap_from_pin(pin);
    if(pin_map == RT_NULL)
    {
        return RT_NULL;
    }
    
    for(index = 0; index < 32; index++)
    {
        if((0x01 << index) == pin_map->gpio_pin)
        {
            return &stm32_irq_map[index];
        }
    }
    
    return RT_NULL;
}

/* 中断相关 */
void EXTI0_IRQHandler(void)
{
    rt_interrupt_enter();
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
    rt_interrupt_leave();
}
void EXTI1_IRQHandler(void)
{
    rt_interrupt_enter();
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
    rt_interrupt_leave();
}
void EXTI2_IRQHandler(void)
{
    rt_interrupt_enter();
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
    rt_interrupt_leave();
}
void EXTI3_IRQHandler(void)
{
    rt_interrupt_enter();
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
    rt_interrupt_leave();
}
void EXTI4_IRQHandler(void)
{
    rt_interrupt_enter();
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
    rt_interrupt_leave();
}
void EXTI9_5_IRQHandler(void)
{
    rt_interrupt_enter();
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_6);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_7);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);
    rt_interrupt_leave();
}
void EXTI15_10_IRQHandler(void)
{
    rt_interrupt_enter();
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
    rt_interrupt_leave();
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    rt_uint32_t index;
    
    for(index = 0; index < 32; index++)
    {
        if((0x01 << index) == GPIO_Pin)
        {
            break;
        }
    }
    
    if(stm32_irq_map[index].irq_info.hdr != RT_NULL)
    {
        stm32_irq_map[index].irq_info.hdr(stm32_irq_map[index].irq_info.args);
    }
}

/* PIN读取状态 */ 
static int stm32_pin_read(rt_device_t dev, rt_base_t pin)
{
    struct stm32_pin *pin_map = RT_NULL;
    
    pin_map = get_pinmap_from_pin(pin); 
    if(pin_map == RT_NULL)
    {
        return (-RT_ENOSYS);
    }
    
    return HAL_GPIO_ReadPin(pin_map->gpio, pin_map->gpio_pin); 
}

/* PIN写状态 */
static void stm32_pin_write(rt_device_t dev, rt_base_t pin, rt_base_t value)
{
    struct stm32_pin *pin_map = RT_NULL;
    
    pin_map = get_pinmap_from_pin(pin); 
    if(pin_map == RT_NULL)
    {
        return; 
    }
    
    HAL_GPIO_WritePin(pin_map->gpio, pin_map->gpio_pin, (GPIO_PinState)value); 
} 

/* PIN通用输入输出模式配置 */ 
static void stm32_pin_mode(rt_device_t dev, rt_base_t pin, rt_base_t mode)
{
    GPIO_InitTypeDef  gpio;
    struct stm32_pin* pin_map = RT_NULL;
    
    pin_map = get_pinmap_from_pin(pin);
    if(pin_map == RT_NULL)
    {
        return;
    }
    
    __STM32_GPIO_CLK_ENABLE(pin_map->rcc);
    
    gpio.Pin   = pin_map->gpio_pin; 
    gpio.Speed = GPIO_SPEED_FREQ_HIGH; 
    
    switch(mode)
    {
        case PIN_MODE_OUTPUT:
        {
            gpio.Mode = GPIO_MODE_OUTPUT_PP;
            gpio.Pull = GPIO_NOPULL;
        }
        break;
        
        case PIN_MODE_OUTPUT_OD:
        {
            gpio.Mode = GPIO_MODE_OUTPUT_OD;
            gpio.Pull = GPIO_NOPULL;
        }
        break;
        
        case PIN_MODE_INPUT:
        {
            gpio.Mode = GPIO_MODE_INPUT;
            gpio.Pull = GPIO_NOPULL;
        }
        break;
        
        case PIN_MODE_INPUT_PULLUP:
        {
            gpio.Mode = GPIO_MODE_INPUT;
            gpio.Pull = GPIO_PULLUP;
        }
        break;
        
        case PIN_MODE_INPUT_PULLDOWN:
        {
            gpio.Mode = GPIO_MODE_INPUT;
            gpio.Pull = GPIO_PULLDOWN;
        }
        break;
    }
    
    HAL_GPIO_Init(pin_map->gpio, &gpio); 
}

/* 注册中断 */ 
rt_err_t stm32_pin_attach_irq(struct rt_device *device, rt_int32_t pin,
    rt_uint32_t mode, void (*hdr)(void *args), void *args)
{
    struct stm32_irq* irq_map = RT_NULL; 
    
    irq_map = get_irqmap_from_pin(pin); 
    if(irq_map == RT_NULL) 
    {
        return RT_ENOSYS; 
    }
    
    /* 检测是否已经注册 */
    if(irq_map->enable == PIN_IRQ_ENABLE)
    {
        return RT_EBUSY; 
    }
    
    irq_map->irq_info.pin  = pin; 
    irq_map->irq_info.hdr  = hdr; 
    irq_map->irq_info.mode = mode; 
    irq_map->irq_info.args = args; 
    
    return RT_EOK;
} 

/* 注销中断 */
rt_err_t stm32_pin_dettach_irq(struct rt_device *device, rt_int32_t pin)
{
    struct stm32_irq* irq_map = RT_NULL; 
    
    irq_map = get_irqmap_from_pin(pin); 
    if(irq_map == RT_NULL) 
    {
        return RT_ENOSYS; 
    }
    
    /* 检测是否已经注册 */
    if(irq_map->enable == PIN_IRQ_DISABLE)
    {
        return RT_EOK; 
    }
    
    irq_map->irq_info.pin  = PIN_IRQ_PIN_NONE; 
    irq_map->irq_info.hdr  = RT_NULL; 
    irq_map->irq_info.mode = PIN_IRQ_MODE_RISING; 
    irq_map->irq_info.args = RT_NULL; 
    
    return RT_EOK;
} 

/* 启动中断 */
rt_err_t stm32_pin_irq_enable(struct rt_device *device, rt_base_t pin, rt_uint32_t enabled)
{
    GPIO_InitTypeDef  gpio; 
    struct stm32_pin* pin_map = RT_NULL; 
    struct stm32_irq* irq_map = RT_NULL; 
    
    pin_map = get_pinmap_from_pin(pin); 
    irq_map = get_irqmap_from_pin(pin); 
    if(pin_map == RT_NULL || irq_map == RT_NULL) 
    {
        return RT_ENOSYS; 
    }
    
    if(enabled == PIN_IRQ_ENABLE)
    {
        /* 判断中断是否已经使能 */
        if(irq_map->enable == PIN_IRQ_ENABLE)
        {
            return RT_EBUSY; 
        }
        
        /* 判断注册的中断是否是该PIN */
        if(irq_map->irq_info.pin != pin)
        {
            return RT_EIO; 
        }
        
        irq_map->enable = PIN_IRQ_ENABLE;
        
        __STM32_GPIO_CLK_ENABLE(pin_map->rcc);
        
        gpio.Pin   = pin_map->gpio_pin; 
        gpio.Speed = GPIO_SPEED_FREQ_HIGH; 
        
        switch(irq_map->irq_info.mode)
        {
            case PIN_IRQ_MODE_RISING:
            {
                gpio.Pull = GPIO_PULLDOWN;
                gpio.Mode = GPIO_MODE_IT_RISING;
            }
            break;
            
            case PIN_IRQ_MODE_FALLING:
            {
                gpio.Pull = GPIO_PULLUP;
                gpio.Mode = GPIO_MODE_IT_FALLING;
            }
            break;
            
            case PIN_IRQ_MODE_RISING_FALLING:
            {
                gpio.Pull = GPIO_NOPULL;
                gpio.Mode = GPIO_MODE_IT_RISING_FALLING;
            }
            break;
        }
        
        HAL_NVIC_SetPriority(irq_map->irq_num, 5, 0);
        HAL_NVIC_EnableIRQ  (irq_map->irq_num);
        
        HAL_GPIO_Init(pin_map->gpio, &gpio);
    }
    else if(enabled == PIN_IRQ_DISABLE)
    {
        if(irq_map->enable == PIN_IRQ_DISABLE)
        {
            return RT_EOK; 
        }
        
        irq_map->enable = PIN_IRQ_DISABLE;
        
        HAL_NVIC_DisableIRQ(irq_map->irq_num);
    }
    else
    {
        return RT_EINVAL; 
    }
    
    return RT_EOK;
}

/* PIN驱动底层OPS */
static struct rt_pin_ops stm32_pin_ops = 
{
    .pin_mode        = stm32_pin_mode, 
    .pin_read        = stm32_pin_read, 
    .pin_write       = stm32_pin_write, 
    .pin_attach_irq  = stm32_pin_attach_irq,
    .pin_dettach_irq = stm32_pin_dettach_irq,
    .pin_irq_enable  = stm32_pin_irq_enable
}; 

/* PIN驱动初始化 */
int rt_hw_pin_init(void)
{
    int ret = RT_EOK; 
    
    ret = rt_device_pin_register("pin", &stm32_pin_ops, RT_NULL);
    
    return ret;
}
INIT_BOARD_EXPORT(rt_hw_pin_init); 
