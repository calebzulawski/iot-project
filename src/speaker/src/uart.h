#ifndef _UART_PROJECT_H_
#define _UART_PROJECT_H_

#define UART_DATA_SIZE 1000

void init_USART1(uint32_t baudrate);
void USART_sendn(USART_TypeDef* USARTx, volatile unsigned char *d, int l);
void USART_puts(USART_TypeDef* USARTx, volatile char *s);
void USART1_IRQHandler(void);

#endif
