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
 
#include "drv_spi.h"

#include <board.h>
#include <finsh.h>

// #define DEBUG

#ifdef DEBUG
#define DEBUG_PRINTF(...)   rt_kprintf(__VA_ARGS__)
#else
#define DEBUG_PRINTF(...)   
#endif

/* private rt-thread spi ops function */
static rt_err_t configure(struct rt_spi_device* device, struct rt_spi_configuration* configuration);
static rt_uint32_t xfer(struct rt_spi_device* device, struct rt_spi_message* message);

static struct rt_spi_ops stm32_spi_ops =
{
    configure,
    xfer
};

static rt_err_t configure(struct rt_spi_device* device,
                          struct rt_spi_configuration* configuration)
{
	struct rt_spi_bus *spi_bus;
	struct stm32f1_spi *f1_spi;
	
	RT_ASSERT(device != RT_NULL);
	RT_ASSERT(configuration != RT_NULL);
	
	spi_bus = (struct rt_spi_bus *)(device->bus);
	f1_spi  = (struct stm32f1_spi *)(spi_bus->parent.user_data);
	
	SPI_HandleTypeDef *SpiHandle = &f1_spi->spi_handle;
	
	/* data_width */
	if(configuration->data_width <= 8)
	{
		SpiHandle->Init.DataSize = SPI_DATASIZE_8BIT;
	}
	else if(configuration->data_width <= 16)
	{
		SpiHandle->Init.DataSize = SPI_DATASIZE_16BIT;
	}
	else
    {
        return RT_EIO;
    }
	
	/* baudrate */
	{
		uint32_t SPI_APB_CLOCK;
		uint32_t max_hz;
		
		max_hz = configuration->max_hz;
		
		/* ½µµÍflashµÄÆµÂÊ */
		max_hz = 50 * 1000 * 1000UL; /* 10Mbit/s */
		
		DEBUG_PRINTF("max_hz = %ld\n", max_hz);
		
		DEBUG_PRINTF("sys   freq: %d\n", HAL_RCC_GetSysClockFreq());
		DEBUG_PRINTF("pclk2 freq: %d\n", HAL_RCC_GetPCLK2Freq());
		
		SPI_APB_CLOCK = HAL_RCC_GetPCLK2Freq();
		
		if(max_hz >= SPI_APB_CLOCK/4)
		{
			SpiHandle->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
		}
		else if(max_hz >= SPI_APB_CLOCK/8)
		{
			SpiHandle->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
		}
		else if(max_hz >= SPI_APB_CLOCK/16)
		{
			SpiHandle->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
		}
		else if(max_hz >= SPI_APB_CLOCK/32)
		{
			SpiHandle->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
		}
		else if(max_hz >= SPI_APB_CLOCK/64)
		{
			SpiHandle->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
		}
        else if(max_hz >= SPI_APB_CLOCK/128)
        {
            SpiHandle->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
        }
        else /* min prescaler 256 */
        {
            SpiHandle->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
        }
	} /* baudrate */
	
	/* CPOL */
	if(configuration->mode & RT_SPI_CPOL)
	{
		SpiHandle->Init.CLKPolarity = SPI_POLARITY_HIGH;
	}
	else
	{
		SpiHandle->Init.CLKPolarity = SPI_POLARITY_LOW;
	}
	
	/* CPHA */
	if(configuration->mode & RT_SPI_CPHA)
	{
		SpiHandle->Init.CLKPhase = SPI_PHASE_2EDGE;
	}
	else
	{
		SpiHandle->Init.CLKPhase = SPI_PHASE_1EDGE;
	}
	
	/* MSB or LSB */
	if(configuration->mode & RT_SPI_MSB)
	{
		SpiHandle->Init.FirstBit = SPI_FIRSTBIT_MSB;
	}
	else
	{
		SpiHandle->Init.FirstBit = SPI_FIRSTBIT_LSB;
	}
	
	SpiHandle->Init.Mode           = SPI_MODE_MASTER;
	SpiHandle->Init.Direction      = SPI_DIRECTION_2LINES;
	SpiHandle->Init.NSS            = SPI_NSS_SOFT;
	SpiHandle->Init.TIMode         = SPI_TIMODE_DISABLE;
	SpiHandle->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	SpiHandle->Init.CRCPolynomial  = 10;
	
	/* init SPI */
	if(HAL_SPI_Init(SpiHandle) != HAL_OK)
	{
		return RT_ERROR;
	}
	
	/* Enable SPI_MASTER */
	__HAL_SPI_ENABLE(SpiHandle);
	
	DEBUG_PRINTF("spi configuration\n");
	
    return RT_EOK;
};

static rt_uint32_t xfer(struct rt_spi_device* device, struct rt_spi_message* message)
{
	struct rt_spi_bus *stm32_spi_bus = (struct rt_spi_bus *)(device->bus);
	struct stm32f1_spi *f1_spi = (struct stm32f1_spi *)(stm32_spi_bus->parent.user_data);
	struct rt_spi_configuration *config = &device->config;
	SPI_TypeDef *SPI = f1_spi->spi_handle.Instance;
	struct stm32_spi_cs *stm32_spi_cs = device->parent.user_data;
	rt_uint32_t size = message->length;
	
	RT_ASSERT(device  != NULL);
	RT_ASSERT(message != NULL);
	
	/* take CS */
	if(message->cs_take)
	{
		HAL_GPIO_WritePin(stm32_spi_cs->GPIOx, stm32_spi_cs->GPIO_Pin, GPIO_PIN_RESET);
	}
	
	if(config->data_width <= 8)
	{
		const rt_uint8_t *send_ptr = message->send_buf;
		rt_uint8_t *recv_ptr       = message->recv_buf;
		
		while(size--)
		{
			rt_uint8_t data = 0xFF;
			
			if(send_ptr != RT_NULL)
			{
				data = *send_ptr++;
			}
			
			while ((SPI->SR & SPI_FLAG_TXE) == RESET);	
			SPI->DR = data;
			
			while ((SPI->SR & SPI_FLAG_RXNE) == RESET);
			data = SPI->DR;
			
			if(recv_ptr != RT_NULL)
			{
				*recv_ptr++ = data;
			}
		}
	}
	else if(config->data_width <= 16)
	{
		const rt_uint16_t *send_ptr = message->send_buf;
		rt_uint8_t *recv_ptr        = message->recv_buf;
		
		while(size--)
		{
			rt_uint16_t data = 0xFFFF;
			
			if(send_ptr != RT_NULL)
			{
				data = *send_ptr++;
			}
			
			while((SPI->SR & SPI_FLAG_TXE) == RESET);
			SPI->DR = data;
			
			while((SPI->SR & SPI_FLAG_RXNE) == RESET);
			data = SPI->DR;
			
			if(recv_ptr != RT_NULL)
			{
				*recv_ptr++ = data;
			}
		}	
	}
	
	/* release CS */	
	if(message->cs_release)
	{
		HAL_GPIO_WritePin(stm32_spi_cs->GPIOx, stm32_spi_cs->GPIO_Pin, GPIO_PIN_SET);
	}
	
    return message->length;
};

#ifdef RT_USING_SPI1
static struct rt_spi_bus spi1_bus;
static struct stm32f1_spi stm32f1_spi1 = 
{
    {SPI1}
};
#endif

/*  */
rt_err_t stm32_spi_bus_register(SPI_TypeDef * SPI, const char * spi_bus_name)
{
    struct stm32f1_spi * p_spi_bus;
    struct rt_spi_bus *  spi_bus;
    
    RT_ASSERT(SPI != RT_NULL);
    RT_ASSERT(spi_bus_name != RT_NULL);
	
	if(SPI != SPI1)
	{
		return RT_ENOSYS;
	}
        
#ifdef RT_USING_SPI1
    if(SPI == SPI1)
    {			
		__HAL_RCC_SPI1_CLK_ENABLE();
	
        spi_bus = &spi1_bus;
		p_spi_bus = &stm32f1_spi1;
    }
#endif
	
	spi_bus->parent.user_data = p_spi_bus;
	
    return rt_spi_bus_register(spi_bus, spi_bus_name, &stm32_spi_ops);
}
