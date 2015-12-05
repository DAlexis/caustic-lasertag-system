/*
 * boot-uart.c
 *
 *  Created on: 6 дек. 2015 г.
 *      Author: alexey
 */

#include "bootloader/boot-uart.h"

/*
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

//Функция предназначена для формирования небольшой задержки
void Delay(void) {
  volatile uint32_t i;
  for (i=0; i != 0x70000; i++);
}

//Функция отправляющая байт в UART
void send_to_uart(uint8_t data) {
  while(!(USART1->SR & USART_SR_TC)); //Ждем пока бит TC в регистре SR станет 1
  USART1->DR=data; //Отсылаем байт через UART
}

int initUart1(void) {
  GPIO_InitTypeDef PORTA_init_struct;
  // Включаем тактирование порта А и USART1
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
  // Настраиваем ногу TxD (PA9) как выход push-pull c альтернативной функцией
  PORTA_init_struct.GPIO_Pin = GPIO_Pin_9;
  PORTA_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
  PORTA_init_struct.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &PORTA_init_struct);
  //Настраиваем UART
  USART1->BRR=0x9c4; //BaudRate 9600
  USART1->CR1 |= USART_CR1_UE; //Разрешаем работу USART1
  USART1->CR1 |= USART_CR1_TE; //Включаем передатчик
  //Все остальные параметры (стоп биты, чётность,кол-во байт данных) уже настроены
  //как надо, (во все эти биты по умолчанию записаны нули), таким образом мы имеем
  // скорость 9600 1 стоп бит, 8 бит данных, без проверки чётности
  while(1) {
    //Выдаем в UART слово Hello
    send_to_uart('H');
    send_to_uart('e');
    send_to_uart('l');
    send_to_uart('l');
    send_to_uart('o');
    send_to_uart(' ');
    send_to_uart(':');
    send_to_uart(')');
    send_to_uart('\n');
    Delay(); //небольшая задержка
  }
}*/
