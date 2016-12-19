#include "stm32f4_discovery.h"
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "uart.h"
#include "mp3dec.h"

#include <stdio.h>

#define BAUD (uint32_t)1000000ull

const uint16_t LEDS = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
const uint16_t LED[4] = {GPIO_Pin_12, GPIO_Pin_13, GPIO_Pin_14, GPIO_Pin_15};

uint32_t counter = 0;

extern int uart_data_filled;
extern unsigned char* uart_data;

unsigned char buffer1[UART_DATA_SIZE];
unsigned char buffer2[UART_DATA_SIZE];

MP3FrameInfo mp3FrameInfo;
HMP3Decoder hMP3Decoder;

void EXTI0_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line0) != RESET)
  {
    /* Clear the EXTI line 0 pending bit */
    EXTI_ClearITPendingBit(EXTI_Line0);

    GPIO_ResetBits(GPIOD, LEDS);

    STM_EVAL_LEDOff(LEDS);

    counter = 0;

    USART_puts(USART1, "Button Pressed! Hello World!\r\n"); // just send a message to indicate that it works

    char numstr[16];
    sprintf(numstr, "%d\r\n", SystemCoreClock);
    USART_puts(USART1, numstr);
  }
}


void init();
void loop();

void delay();

int main() {
    init();

    do {
        loop();
    } while (1);
}

void init() {
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    GPIO_InitTypeDef gpio;
    GPIO_StructInit(&gpio);
    gpio.GPIO_Mode = GPIO_Mode_OUT;
    gpio.GPIO_Pin = LEDS;
    GPIO_Init(GPIOD, &gpio);

    GPIO_SetBits(GPIOD, LEDS);

    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable GPIOA clock */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    /* Enable SYSCFG clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    /* Configure PA0 pin as input floating */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Connect EXTI Line0 to PA0 pin */
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);

    /* Configure EXTI Line0 */
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Enable and set EXTI Line0 Interrupt to the lowest priority */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    init_USART1(BAUD); // initialize USART1 @ 9600 baud

    hMP3Decoder = MP3InitDecoder();

    uart_data = buffer1;
}

void loop() {
    // ++counter;
    //
    // GPIO_ResetBits(GPIOD, LEDS);
    // GPIO_SetBits(GPIOD, LED[counter % 4]);
    //
    // delay(1000);

    static short samples[10000];

    // __disable_irq();
    if (uart_data_filled == UART_DATA_SIZE)
    {
        unsigned char *data = uart_data;
        int offset = MP3FindSyncWord(data, UART_DATA_SIZE);
        data += offset;
        int bytes = UART_DATA_SIZE - offset;
        // USART_puts(USART1, "START");
        // __disable_irq();
        int err = MP3Decode(hMP3Decoder, &data, &bytes, samples, 0);
        // __enable_irq();
        // USART_puts(USART1, "FINISH");

        if (err) {
            USART_puts(USART1, "ERROR");
            /* error occurred */
            switch (err) {
            case ERR_MP3_INDATA_UNDERFLOW:
                // outOfData = 1;
                break;
            case ERR_MP3_MAINDATA_UNDERFLOW:
                /* do nothing - next call to decode will provide more mainData */
                break;
            case ERR_MP3_FREE_BITRATE_SYNC:
            default:
                // outOfData = 1;
                break;
            }
        } else {
            USART_puts(USART1, "NO ERROR");
            /* no error */
            MP3GetLastFrameInfo(hMP3Decoder, &mp3FrameInfo);
            USART_sendn(USART1, (uint8_t*)samples, mp3FrameInfo.outputSamps * sizeof(short));
        }
    }
    // __enable_irq();
}

void delay(uint32_t ms) {
    ms *= 3360;
    while(ms--) {
        __NOP();
    }
}
