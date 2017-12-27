#ifndef _drv_ds1302_H
#define _drv_ds1302_H
#include "stm32f1xx_hal.h"

#include "rtthread.h"
#include "stm32f1xx.h"
#include <time.h>

#ifdef RT_USING_FINSH
#include "finsh.h"
#endif

// Write Register Address
#define DS1302_SEC_W        0x80
#define DS1302_SEC_R        0x81
#define DS1302_MIN_W        0x82
#define DS1302_MIN_R        0x83
#define DS1302_HOUR_W       0x84
#define DS1302_HOUR_R       0x85
#define DS1302_DATE_W       0x86
#define DS1302_DATE_R       0x87
#define DS1302_MONTH_W      0x88
#define DS1302_MONTH_R      0x89
#define DS1302_DAY_W        0x8A
#define DS1302_DAY_R        0x8B
#define DS1302_YEAR_W       0x8C
#define DS1302_YEAR_R       0x8D
#define DS1302_CONTROL_W    0x8E
#define DS1302_CONTROL_R    0x8F
#define DS1302_CHARGER_W    0x90
#define DS1302_CHARGER_R    0x91
#define DS1302_CLKBURST_W   0xBE
#define DS1302_CLKBURST_R   0xBF
#define DS1302_RAMBURST_W   0xFE
#define DS1302_RAMBURST_R   0xFF

#define RAMSIZE             0x31    // Ram Size in bytes
#define DS1302_RAMSTART     0xc0    // First Address

#define HEX2BCD(v)          ((v) % 10 + (v) / 10 * 16)
#define BCD2HEX(v)          ((v) % 16 + (v) / 16 * 10)

/* GPIO Pins */
#define DS1302_GPIO         GPIOC
#define DS1302_SCLK         GPIO_PIN_13
#define DS1302_SDA          GPIO_PIN_14
#define DS1302_RST          GPIO_PIN_15 

#endif
