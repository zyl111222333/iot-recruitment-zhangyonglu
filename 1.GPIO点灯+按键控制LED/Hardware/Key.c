#include "stm32f10x.h"                 
#include "Delay.h"

void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;				
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;		
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;		
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);	
}

/**
  * @brief 获取按键值
  * @param  无
  * @retval KeyNum,要读取的按键值
  */
uint8_t Key_GetNum(void)
{
	uint8_t KeyNum = 0;						
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11) == 0)
	{
		Delay_ms(20);						
		while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11) == 0);
		Delay_ms(20);							
		KeyNum = 1;								
	}
	
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_12) == 0)
	{
		Delay_ms(20);							
		while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_12) == 0);
		Delay_ms(20);							
		KeyNum = 2;								
	}

	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0) == 0)
	{
		Delay_ms(20);						
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0) == 0);
		Delay_ms(20);							
		KeyNum = 3;							
	}


	return KeyNum;
}
