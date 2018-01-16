/*
 * @File:   drv_sdram.h 
 * @Author: liuguang 
 * @Date:   2017-10-02 03:20:05 
 * 
 * @LICENSE: 
 * The code owner is liuguang. Other personnel may 
 * not be used or modified.
 * 
 * Change Logs: 
 * Date           Author       Notes 
 * 2017-10-02     liuguang     The first version. 
 * 2018-01-15     liuguang     Update f469 MICRON MT48LC4M32B2B5-6A. 
 */ 

#ifndef __DRV_SDRAM_H_
#define __DRV_SDRAM_H_

#include <rthw.h>
#include <rtthread.h>

#define SDRAM_SIZE                ((uint32_t)0x01000000)
#define SDRAM_BANK_ADDR           ((uint32_t)0xC0000000) 
#define SDRAM_BANK_ADDR_END       (SDRAM_BANK_ADDR + SDRAM_SIZE) 

#define SDRAM_REFRESH_COUNT       ((uint32_t)0x0569)

#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200)

int rt_hw_sdram_init(void);

#endif
