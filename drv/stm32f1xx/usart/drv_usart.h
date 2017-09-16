#ifndef __DRV_USART_H__
#define __DRV_USART_H__

#define UART_ENABLE_IRQ(n)            NVIC_EnableIRQ((n))
#define UART_DISABLE_IRQ(n)           NVIC_DisableIRQ((n))

int stm32_hw_usart_init(void);

#endif
