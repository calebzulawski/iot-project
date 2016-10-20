#ifndef _UART_PROJECT_H_
#define _UART_PROJECT_H_

void init_USART1(uint32_t baudrate);
void USART_puts(USART_TypeDef* USARTx, volatile char *s);
void USART1_IRQHandler(void);

#endif
