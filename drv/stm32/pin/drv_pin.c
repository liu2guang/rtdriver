#include "drv_pin.h"

#if defined(RT_USING_PIN)

#define __STM32_PIN_DEFAULT {-1, 0, 0, 0}
#if defined(STM32F0) || defined(STM32F2) 
#define __RCC_GPIO_CLK_ENABLE(BIT)           \
do {                                         \
    __IO uint32_t tmpreg = 0x00U;            \
    SET_BIT(RCC->AHBENR, BIT);               \
    tmpreg = READ_BIT(RCC->AHBENR, BIT);     \
    UNUSED(tmpreg);                          \
}while(0)
#define __STM32_PIN(index, gpio, gpio_index) \
    {index, RCC_AHBENR_GPIOP##gpio##EN, GPIO##gpio, GPIO_PIN_##gpio_index}
#endif /* STM32F0 and STM32F2 */ 

#if defined(STM32F1)
#define __RCC_GPIO_CLK_ENABLE(BIT)           \
do {                                         \
    __IO uint32_t tmpreg = 0x00U;            \
    SET_BIT(RCC->APB2ENR, BIT);              \
    tmpreg = READ_BIT(RCC->APB2ENR, BIT);    \
    UNUSED(tmpreg);                          \
}while(0)
#define __STM32_PIN(index, gpio, gpio_index) \
    {index, RCC_APB2ENR_IOP##gpio##EN, GPIO##gpio, GPIO_PIN_##gpio_index}
#endif /* STM32F1 */ 

#if defined(STM32F2) || defined(STM32F4) || defined(STM32F7)
#define __RCC_GPIO_CLK_ENABLE(BIT)           \
do {                                         \
    __IO uint32_t tmpreg = 0x00U;            \
    SET_BIT(RCC->AHB1ENR, BIT);              \
    tmpreg = READ_BIT(RCC->AHB1ENR, BIT);    \
    UNUSED(tmpreg);                          \
}while(0)
#define __STM32_PIN(index, gpio, gpio_index) \
    {index, RCC_AHB1ENR_GPIO##gpio##EN, GPIO##gpio, GPIO_PIN_##gpio_index}
#endif /* STM32F2 and STM32F4 and STM32F7 */ 
    
/* STM32F0 F1 F2 F3 F4 F7 GPIO MAP MARCO */
#if defined(STM32F1)
static const struct pin_map pins[] = 
{
#if (STM32FX_PIN_NUMBERS == 36)
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT,
    __STM32_PIN(7,  A, 0),
    __STM32_PIN(8,  A, 1),
    __STM32_PIN(9,  A, 2),
    __STM32_PIN(10, A, 3),
    __STM32_PIN(11, A, 4),
    __STM32_PIN(12, A, 5),
    __STM32_PIN(13, A, 6),
    __STM32_PIN(14, A, 7),
    __STM32_PIN(15, B, 0),
    __STM32_PIN(16, B, 1),
    __STM32_PIN(17, B, 2),
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT,
    __STM32_PIN(20, A, 8),
    __STM32_PIN(21, A, 9),
    __STM32_PIN(22, A, 10),
    __STM32_PIN(23, A, 11),
    __STM32_PIN(24, A, 12),
    __STM32_PIN(25, A, 13),
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT,
    __STM32_PIN(28, A, 14),
    __STM32_PIN(29, A, 15),
    __STM32_PIN(30, B, 3),
    __STM32_PIN(31, B, 4),
    __STM32_PIN(32, B, 5),
    __STM32_PIN(33, B, 6),
    __STM32_PIN(34, B, 7),
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT
#endif /* STM32FX_PIN_NUMBERS = 36 */

#if (STM32FX_PIN_NUMBERS == 48)
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
#endif /* STM32FX_PIN_NUMBERS = 48 */
#if (STM32FX_PIN_NUMBERS == 64)
    __STM32_PIN_DEFAULT,        /* ?? */
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
    __STM32_PIN_DEFAULT         /* VDD */
#endif /* STM32FX_PIN_NUMBERS == 64 */
#if (STM32FX_PIN_NUMBERS == 100)
    __STM32_PIN_DEFAULT,
    __STM32_PIN(1,  E, 2),
    __STM32_PIN(2,  E, 3),
    __STM32_PIN(3,  E, 4),
    __STM32_PIN(4,  E, 5),
    __STM32_PIN(5,  E, 6),
    __STM32_PIN_DEFAULT,
    __STM32_PIN(7,  C, 13),
    __STM32_PIN(8,  C, 14),
    __STM32_PIN(9,  C, 15),
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT,
    __STM32_PIN(15, C, 0),
    __STM32_PIN(16, C, 1),
    __STM32_PIN(17, C, 2),
    __STM32_PIN(18, C, 3),
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT,
    __STM32_PIN(23, A, 0),
    __STM32_PIN(24, A, 1),
    __STM32_PIN(25, A, 2),
    __STM32_PIN(26, A, 3),
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT,
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
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT,
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
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT,
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
    __STM32_PIN_DEFAULT,
    __STM32_PIN(95, B, 8),
    __STM32_PIN(96, B, 9),
    __STM32_PIN(97, E, 0),
    __STM32_PIN(98, E, 1),
    __STM32_PIN_DEFAULT,
    __STM32_PIN_DEFAULT
#endif /* STM32FX_PIN_NUMBERS == 100 */
#if (STM32FX_PIN_NUMBERS == 144)
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
#endif /* STM32FX_PIN_NUMBERS == 144 */ 
};
#endif /* STM32F1 */

#if defined(STM32F4)
static const struct pin_map pins[] = 
{
#if (STM32FX_PIN_NUMBERS == 208)
    __STM32_PIN_DEFAULT,    /* NONE */
    __STM32_PIN(1, E, 2),   /*  */
    __STM32_PIN(2, E, 3),
    __STM32_PIN(3, E, 4),
    __STM32_PIN(4, E, 5),
    __STM32_PIN(5, E, 6),
    __STM32_PIN_DEFAULT,    /* VBAT */
    __STM32_PIN(7, I, 8),
    __STM32_PIN(8, C, 13),
    __STM32_PIN_DEFAULT,    /* OSC32_IN */
    __STM32_PIN_DEFAULT,    /* OSC32_OUT */
    __STM32_PIN(11, I, 9),
    __STM32_PIN(12, I, 10),
    __STM32_PIN(13, I, 11),
    __STM32_PIN_DEFAULT,    /* VSS */
    __STM32_PIN_DEFAULT,    /* VDD */
    __STM32_PIN(15, F, 0),
    __STM32_PIN(16, F, 1),
    __STM32_PIN(17, F, 2),
    __STM32_PIN(18, I, 12),
    __STM32_PIN(20, I, 13),
    __STM32_PIN(21, I, 14),
    __STM32_PIN(22, F, 3),
    __STM32_PIN(23, F, 4),
    __STM32_PIN(24, F, 5),
    __STM32_PIN_DEFAULT,    /* VSS */
    __STM32_PIN_DEFAULT,    /* VDD */
    __STM32_PIN(27, F, 6),
    __STM32_PIN(28, F, 7),
    __STM32_PIN(29, F, 8),
    __STM32_PIN(30, F, 9),
    __STM32_PIN(31, F, 10),
    __STM32_PIN_DEFAULT,    /* OSC_IN */
    __STM32_PIN_DEFAULT,    /* OSC_OUT */
    __STM32_PIN_DEFAULT,    /* RESET */
    __STM32_PIN(35, C, 0),
    __STM32_PIN(36, C, 1),
    __STM32_PIN(37, C, 2),
    __STM32_PIN(38, C, 3),
    __STM32_PIN_DEFAULT,    /* VDD */
    __STM32_PIN_DEFAULT,    /* VSSA */
    __STM32_PIN_DEFAULT,    /* VREF+ */
    __STM32_PIN_DEFAULT,    /* VDDA */
    __STM32_PIN(43, A, 0),
    __STM32_PIN(44, A, 1),
    __STM32_PIN(45, A, 2),
    __STM32_PIN(46, H, 2),
    __STM32_PIN(47, H, 3),
    __STM32_PIN(48, H, 4),
    __STM32_PIN(49, H, 5),
    __STM32_PIN(50, A, 3),
    __STM32_PIN_DEFAULT,    /* VSS */
    __STM32_PIN_DEFAULT,    /* VDD */
    __STM32_PIN(53, A, 4),
    __STM32_PIN(54, A, 5),
    __STM32_PIN(55, A, 6),
    __STM32_PIN(56, A, 7),
    __STM32_PIN(57, C, 4),
    __STM32_PIN(58, C, 5),
    __STM32_PIN_DEFAULT,    /* VDD */
    __STM32_PIN_DEFAULT,    /* VSS */
    __STM32_PIN(61, B, 0),
    __STM32_PIN(62, B, 1),
    __STM32_PIN_DEFAULT,    /* BOOT1 */
    __STM32_PIN(64, I, 15),
    __STM32_PIN(65, J, 0),
    __STM32_PIN(66, J, 1),
    __STM32_PIN(67, J, 2),
    __STM32_PIN(68, J, 3),
    __STM32_PIN(69, J, 4),
    __STM32_PIN(70, F, 11),
    __STM32_PIN(71, F, 12),
    __STM32_PIN_DEFAULT,    /* VSS */
    __STM32_PIN_DEFAULT,    /* VDD */
    __STM32_PIN(74, F, 13),
    __STM32_PIN(75, F, 14),
    __STM32_PIN(76, F, 15),
    __STM32_PIN(77, G, 0),
    __STM32_PIN(78, G, 1),
    __STM32_PIN(79, E, 7),
    __STM32_PIN(80, E, 8),
    __STM32_PIN(81, E, 9),
    __STM32_PIN_DEFAULT,    /* VSS */
    __STM32_PIN_DEFAULT,    /* VDD */
    __STM32_PIN(84, E, 10),
    __STM32_PIN(85, E, 11),
    __STM32_PIN(86, E, 12),
    __STM32_PIN(87, E, 13),
    __STM32_PIN(88, E, 14),
    __STM32_PIN(89, E, 15),
    __STM32_PIN(90, B, 10),
    __STM32_PIN(91, B, 11),
    __STM32_PIN_DEFAULT,    /* VCAP_1 */
    __STM32_PIN_DEFAULT,    /* VSS */
    __STM32_PIN_DEFAULT,    /* VDD */
    __STM32_PIN(95, J, 5),
    __STM32_PIN(96, H, 6),
    __STM32_PIN(97, H, 7),
    __STM32_PIN(98, H, 8),
    __STM32_PIN(99, H, 9),
    __STM32_PIN(100, H, 10),
    __STM32_PIN(101, H, 11),
    __STM32_PIN(102, H, 12),
    __STM32_PIN_DEFAULT,    /* VDD */
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
    __STM32_PIN_DEFAULT,    /* VSS */
    __STM32_PIN_DEFAULT,    /* VDD */
    __STM32_PIN(116, D, 14),
    __STM32_PIN(117, D, 15),
    __STM32_PIN(118, J, 6),
    __STM32_PIN(119, J, 7),
    __STM32_PIN(120, J, 8),
    __STM32_PIN(121, J, 9),
    __STM32_PIN(122, J, 10),
    __STM32_PIN(123, J, 11),
    __STM32_PIN_DEFAULT,    /* VDD */
    __STM32_PIN_DEFAULT,    /* VSS */
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
    __STM32_PIN_DEFAULT,    /* VSS */
    __STM32_PIN_DEFAULT,    /* VDD */
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
    __STM32_PIN_DEFAULT,    /* VCAP_2 */
    __STM32_PIN_DEFAULT,    /* VSS */
    __STM32_PIN_DEFAULT,    /* VDD */
    __STM32_PIN(151, H, 13),
    __STM32_PIN(152, H, 14),
    __STM32_PIN(153, H, 15),
    __STM32_PIN(154, I, 0),
    __STM32_PIN(155, I, 1),
    __STM32_PIN(156, I, 2),
    __STM32_PIN(157, I, 3),
    __STM32_PIN_DEFAULT,    /* VDD */
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
    __STM32_PIN_DEFAULT,    /* VSS */
    __STM32_PIN_DEFAULT,    /* VDD */
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
    __STM32_PIN_DEFAULT,    /* VSS */
    __STM32_PIN_DEFAULT,    /* VDD */
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
    __STM32_PIN_DEFAULT,    /* BOOT0 */
    __STM32_PIN(198, B, 8),
    __STM32_PIN(199, B, 9),
    __STM32_PIN(200, E, 0),
    __STM32_PIN(201, E, 1),
    __STM32_PIN_DEFAULT,    /* VSS */
    __STM32_PIN_DEFAULT,    /* PDR_ON */
    __STM32_PIN_DEFAULT,    /* VDD */
    __STM32_PIN(205, I, 4),
    __STM32_PIN(206, I, 5),
    __STM32_PIN(207, I, 6),
    __STM32_PIN(208, I, 7)
#endif
};
#endif /* STM32F4 */

#if defined(STM32F0) || defined(STM32F2) || defined(STM32F3) || defined(STM32F7)
#error need user defined other pin map!
#endif /* other pin map */

static const struct pin_irq_map pin_irq_map[] = 
{
    {GPIO_PIN_0,  EXTI0_IRQn    },
    {GPIO_PIN_1,  EXTI1_IRQn    },
    {GPIO_PIN_2,  EXTI2_IRQn    },
    {GPIO_PIN_3,  EXTI3_IRQn    },
    {GPIO_PIN_4,  EXTI4_IRQn    },
    {GPIO_PIN_5,  EXTI9_5_IRQn  },
    {GPIO_PIN_6,  EXTI9_5_IRQn  },
    {GPIO_PIN_7,  EXTI9_5_IRQn  },
    {GPIO_PIN_8,  EXTI9_5_IRQn  },
    {GPIO_PIN_9,  EXTI9_5_IRQn  },
    {GPIO_PIN_10, EXTI15_10_IRQn},
    {GPIO_PIN_11, EXTI15_10_IRQn},
    {GPIO_PIN_12, EXTI15_10_IRQn},
    {GPIO_PIN_13, EXTI15_10_IRQn},
    {GPIO_PIN_14, EXTI15_10_IRQn},
    {GPIO_PIN_15, EXTI15_10_IRQn}
};

static struct rt_pin_irq_hdr pin_irq_hdr_tab[] = 
{
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL},
    {-1, 0, RT_NULL, RT_NULL}
};


static const struct pin_map *get_pin_map(uint16_t pin_index);
static const struct pin_irq_map *get_pin_irq_map(uint32_t pinbit);
static rt_int32_t bit2bitno(rt_uint32_t bit);
static void pin_irq_hdr(int irqno);

/* STM32外部中断服务函数入口 */
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
    pin_irq_hdr(bit2bitno(GPIO_Pin));
}

/* 通过pin编号获取对应pin_map结构体 */
static const struct pin_map *get_pin_map(uint16_t pin_index)
{
    const struct pin_map *p = RT_NULL;
    
    if(pin_index < GET_ARRAY_LENGTH(pins))
    {
        p = &pins[pin_index];
        if(p->index == (-1))
        {
            p = RT_NULL;
        }
    }
    
    return p;
}

/* 通过pin获取对应pin_irq_map结构体 */
static const struct pin_irq_map *get_pin_irq_map(uint32_t pin_bit)
{
    rt_int32_t map_index = bit2bitno(pin_bit);
    
    if(map_index < 0 || map_index >= GET_ARRAY_LENGTH(pin_irq_map))
    {
        return RT_NULL;
    }
    
    return &pin_irq_map[map_index];
}

/* 通过pin获取pin编号 */
static rt_int32_t bit2bitno(rt_uint32_t bit)
{
    rt_uint32_t index;
    
    for(index = 0; index < 32; index++)
    {
        if((0x01 << index) == bit)
        {
            return index;
        }
    }
    
    return (-1);
}

static void pin_irq_hdr(int irqno)
{
    if(pin_irq_hdr_tab[irqno].hdr)
    {
        pin_irq_hdr_tab[irqno].hdr(pin_irq_hdr_tab[irqno].args);
    }
}

/* write pin level */
static void stm32_pin_write(rt_device_t dev, rt_base_t pin, rt_base_t value)
{
    const struct pin_map *p = RT_NULL;
    
    p =  get_pin_map(pin);
    if(p == RT_NULL)
    {
        return;
    }
    
    HAL_GPIO_WritePin(p->port, p->pin, (GPIO_PinState)value);
}

/* read pin level */
static int stm32_pin_read(rt_device_t dev, rt_base_t pin)
{
    const struct pin_map *p = RT_NULL;
    
    p = get_pin_map(pin);
    if(p == RT_NULL)
    {
        return (-RT_ENOSYS);
    }
    
    return HAL_GPIO_ReadPin(p->port, p->pin);
}

/* config pin mode */
static void stm32_pin_mode(rt_device_t dev, rt_base_t pin, rt_base_t mode)
{
    const struct pin_map *p = RT_NULL;
    GPIO_InitTypeDef  gpio;
    
    p =  get_pin_map(pin);
    if(p == RT_NULL)
    {
        return;
    }
    
    /* ENABLE CLK ENABLE */
    __RCC_GPIO_CLK_ENABLE(p->clk);
    
    /* Configure GPIO */
    gpio.Pin   = p->pin;
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
    
    HAL_GPIO_Init(p->port, &gpio);
}

rt_err_t stm32_pin_attach_irq(struct rt_device *device, rt_int32_t pin,
    rt_uint32_t mode, void (*hdr)(void *args), void *args)
{
    const struct pin_map *p;
    rt_base_t level;
    rt_int32_t irq_index = -1;
    
    p = get_pin_map(pin);
    if(p == RT_NULL)
    {
        return RT_ENOSYS;
    }
    
    irq_index = bit2bitno(p->pin);
    if(irq_index < 0 || irq_index >= GET_ARRAY_LENGTH(pin_irq_map))
    {
        return RT_ENOSYS;
    }
    
    level = rt_hw_interrupt_disable();
    if (pin_irq_hdr_tab[irq_index].pin  == pin  &&
        pin_irq_hdr_tab[irq_index].hdr  == hdr  &&
        pin_irq_hdr_tab[irq_index].mode == mode &&
        pin_irq_hdr_tab[irq_index].args == args)
    {
        rt_hw_interrupt_enable(level);
        return RT_EOK;
    }
    
    if(pin_irq_hdr_tab[irq_index].pin != -1)
    {
        rt_hw_interrupt_enable(level);
        return RT_EBUSY;
    }
    
    pin_irq_hdr_tab[irq_index].pin  = pin;
    pin_irq_hdr_tab[irq_index].hdr  = hdr;
    pin_irq_hdr_tab[irq_index].mode = mode;
    pin_irq_hdr_tab[irq_index].args = args;
    rt_hw_interrupt_enable(level);

    return RT_EOK;
}

rt_err_t stm32_pin_dettach_irq(struct rt_device *device, rt_int32_t pin)
{
    const struct pin_map *p;
    rt_base_t level;
    rt_int32_t irq_index = -1;

    p = get_pin_map(pin);
    if(p == RT_NULL)
    {
        return RT_ENOSYS;
    }
    
    irq_index = bit2bitno(p->pin);
    if(irq_index < 0 || irq_index >= GET_ARRAY_LENGTH(pin_irq_map))
    {
        return RT_ENOSYS;
    }

    level = rt_hw_interrupt_disable();
    if (pin_irq_hdr_tab[irq_index].pin == -1)
    {
        rt_hw_interrupt_enable(level);
        return RT_EOK;
    }
    
    pin_irq_hdr_tab[irq_index].pin  = -1;
    pin_irq_hdr_tab[irq_index].hdr  = RT_NULL;
    pin_irq_hdr_tab[irq_index].mode = 0;
    pin_irq_hdr_tab[irq_index].args = RT_NULL;
    rt_hw_interrupt_enable(level);

    return RT_EOK;
}

rt_err_t stm32_pin_irq_enable(struct rt_device *device, rt_base_t pin,
    rt_uint32_t enabled)
{
    const struct pin_map *p;
    const struct pin_irq_map *irq_map;
    rt_base_t level;
    rt_int32_t irq_index = -1;
    GPIO_InitTypeDef gpio;

    p = get_pin_map(pin);
    if(p == RT_NULL)
    {
        return RT_ENOSYS;
    }
    
    if(enabled == PIN_IRQ_ENABLE)
    {
        irq_index = bit2bitno(p->pin);
        if(irq_index < 0 || irq_index >= GET_ARRAY_LENGTH(pin_irq_map))
        {
            return RT_ENOSYS;
        }
        
        level = rt_hw_interrupt_disable();
        if (pin_irq_hdr_tab[irq_index].pin == (-1))
        {
            rt_hw_interrupt_enable(level);
            return RT_ENOSYS;
        }
        
        irq_map = &pin_irq_map[irq_index];
        
        __RCC_GPIO_CLK_ENABLE(p->clk);
        
        gpio.Pin   = p->pin;
        gpio.Speed = GPIO_SPEED_FREQ_HIGH;
        
        switch(pin_irq_hdr_tab[irq_index].mode)
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
        
        HAL_GPIO_Init(p->port, &gpio);
        
        HAL_NVIC_SetPriority(irq_map->irqno, 5, 0);
        HAL_NVIC_EnableIRQ(irq_map->irqno);
        
        rt_hw_interrupt_enable(level);
    }
    else if(enabled == PIN_IRQ_DISABLE)
    {
        irq_map = get_pin_irq_map(p->pin);
        if (irq_map == RT_NULL)
        {
            return RT_ENOSYS;
        }
        HAL_NVIC_DisableIRQ(irq_map->irqno);
    }
    else
    {
        return RT_ENOSYS;
    }

    return RT_EOK;
}

const static struct rt_pin_ops stm32_pin_ops = 
{
    stm32_pin_mode,
    stm32_pin_write,
    stm32_pin_read,
    stm32_pin_attach_irq,
    stm32_pin_dettach_irq,
    stm32_pin_irq_enable
};

int rt_hw_pin_init(void)
{
    return rt_device_pin_register("pin", &stm32_pin_ops, RT_NULL);
}
INIT_BOARD_EXPORT(rt_hw_pin_init);

#endif /* RT_USING_PIN */
