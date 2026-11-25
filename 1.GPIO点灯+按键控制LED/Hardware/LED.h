#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"
#define LED_BLUE_PIN    GPIO_Pin_5
#define LED_BLUE_PORT   GPIOB
#define LED_RED_PIN     GPIO_Pin_1
#define LED_RED_PORT    GPIOB
#define LED_PC13_PIN    GPIO_Pin_13
#define LED_PC13_PORT   GPIOC


void LED_Init(void);
void LED_Blue_On(void);
void LED_Blue_Off(void);
void LED_Blue_Toggle(void);
void LED_Red_On(void);
void LED_Red_Off(void);
void LED_Red_Toggle(void);

#endif