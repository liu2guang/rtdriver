/*
 * File      : dev_spi_ss1306.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2015, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author            Notes
 * 2018-04-17     liu2guang         the first version
 */

#ifndef __DRV_SPI_SS1306_H_
#define __DRV_SPI_SS1306_H_

#include <rtthread.h>
#include <rtdevice.h>

int rt_hw_spi_ss1306_init(const char *spidev, const char *spibus); 

void ss1306_disp_on(void); 
void ss1306_disp_off(void); 
void ss1306_clear(void); 
void ss1306_set_pos(uint8_t x, uint8_t y); 
void ss1306_scroll(void); 
void ss1306_show_ch06x08(uint8_t x, uint8_t y, uint8_t ch); 
void ss1306_show_ch08x16(uint8_t x, uint8_t y, uint8_t ch); 
void ss1306_show_string(uint8_t x, uint8_t y, uint8_t *string); 
void ss1306_show_chinese(uint8_t x, uint8_t y, uint8_t no); 
void ss1306_show_bmp(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, const uint8_t *data); 

#endif
