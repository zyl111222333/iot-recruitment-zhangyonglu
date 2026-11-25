#include "stm32f10x.h"
#include "LED.h"

void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    
    GPIO_InitStructure.GPIO_Pin = LED_BLUE_PIN;
    GPIO_Init(LED_BLUE_PORT, &GPIO_InitStructure);

    
    GPIO_InitStructure.GPIO_Pin = LED_RED_PIN;
    GPIO_Init(LED_RED_PORT, &GPIO_InitStructure);

    
    GPIO_InitStructure.GPIO_Pin = LED_PC13_PIN;
    GPIO_Init(LED_PC13_PORT, &GPIO_InitStructure);

    
    LED_Blue_Off();
    LED_Red_Off();
    GPIO_ResetBits(LED_PC13_PORT, LED_PC13_PIN);//PC13常亮
}

void LED_Blue_On(void)
{
    GPIO_ResetBits(LED_BLUE_PORT, LED_BLUE_PIN);
}

void LED_Blue_Off(void)
{
    GPIO_SetBits(LED_BLUE_PORT, LED_BLUE_PIN);
}

void LED_Blue_Toggle(void)
{
    if (GPIO_ReadOutputDataBit(LED_BLUE_PORT, LED_BLUE_PIN) == 0)
    {
        LED_Blue_Off();
    }
    else
    {
        LED_Blue_On();
    }
}

void LED_Red_On(void)
{
    GPIO_ResetBits(LED_RED_PORT, LED_RED_PIN);
}

void LED_Red_Off(void)
{
    GPIO_SetBits(LED_RED_PORT, LED_RED_PIN);
}

void LED_Red_Toggle(void)
{
    if (GPIO_ReadOutputDataBit(LED_RED_PORT, LED_RED_PIN) == 0)
    {
        LED_Red_Off();
    }
    else
    {
        LED_Red_On();
    }
}