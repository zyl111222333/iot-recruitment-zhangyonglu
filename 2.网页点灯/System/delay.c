#include "delay.h"

void delay_init(void) {
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); // 9MHz
}

void delay_ms(uint16_t ms) {
    uint32_t temp;
    SysTick->LOAD = (uint32_t)ms * 9000;
    SysTick->VAL = 0x00;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    do { temp = SysTick->CTRL; } while ((temp & 0x01) && !(temp & (1 << 16)));
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

void delay_us(uint32_t us) {
    uint32_t temp;
    SysTick->LOAD = us * 9; // 9MHz，1us就是9个tick
    SysTick->VAL = 0x00;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    do { temp = SysTick->CTRL; } while ((temp & 0x01) && !(temp & (1 << 16)));
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}
