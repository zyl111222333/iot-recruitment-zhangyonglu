#include "stm32f10x.h"
#include "Delay.h"
#include "LED.h"
#include "Key.h"

int main(void)
{
    uint8_t KeyNum;

    LED_Init();
    Key_Init();

    while (1)
    {
        KeyNum = Key_GetNum();

        if (KeyNum == 1)
        {
            LED_Blue_Toggle();
        }
        else if (KeyNum == 2)
        {
            LED_Red_Toggle();
        }
    }
}