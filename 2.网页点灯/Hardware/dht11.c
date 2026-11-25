#include "dht11.h"
#include "delay.h"

// 改变引脚方向
static void DHT11_SetPinOut(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = DHT11_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DHT11_GPIO_PORT, &GPIO_InitStructure);
}

static void DHT11_SetPinIn(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = DHT11_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // 上拉输入
    GPIO_Init(DHT11_GPIO_PORT, &GPIO_InitStructure);
}

void DHT11_Init(void) {
    RCC_APB2PeriphClockCmd(DHT11_RCC_PORT, ENABLE);
    DHT11_SetPinOut();
    GPIO_SetBits(DHT11_GPIO_PORT, DHT11_GPIO_PIN); // 空闲状态拉高
}

// 返回值：0-成功，1-响应超时，2-校验失败，3-读数据超时
uint8_t DHT11_Read_Data(uint8_t *temp, uint8_t *humi) {
    uint8_t buf[5];
    uint8_t i, j;
    uint32_t timeout = 0; // 新增超时计数器
    
    // 1. 主机发送开始信号
    DHT11_SetPinOut();
    GPIO_ResetBits(DHT11_GPIO_PORT, DHT11_GPIO_PIN);
    delay_ms(20);
    GPIO_SetBits(DHT11_GPIO_PORT, DHT11_GPIO_PIN);
    delay_us(30);
    
    // 2. 等待 DHT11 响应
    DHT11_SetPinIn();
    if (GPIO_ReadInputDataBit(DHT11_GPIO_PORT, DHT11_GPIO_PIN) == 0) {
        // ⚠️ 加入 timeout 限制，防止死循环
        timeout = 0;
        while (GPIO_ReadInputDataBit(DHT11_GPIO_PORT, DHT11_GPIO_PIN) == 0) {
            if(++timeout > 5000) return 1; // 卡死退出
        }
        timeout = 0;
        while (GPIO_ReadInputDataBit(DHT11_GPIO_PORT, DHT11_GPIO_PIN) == 1) {
            if(++timeout > 5000) return 1; // 卡死退出
        }
    } else {
        return 1; // 响应超时
    }
    
    // 3. 读取40位数据 (5个字节)
    for (i = 0; i < 5; i++) {
        buf[i] = 0;
        for (j = 0; j < 8; j++) {
            timeout = 0;
            while (GPIO_ReadInputDataBit(DHT11_GPIO_PORT, DHT11_GPIO_PIN) == 0) {
                if(++timeout > 5000) return 3; // 卡死退出
            }
            
            delay_us(40); 
            
            if (GPIO_ReadInputDataBit(DHT11_GPIO_PORT, DHT11_GPIO_PIN) == 1) {
                buf[i] |= (1 << (7 - j));
                timeout = 0;
                while (GPIO_ReadInputDataBit(DHT11_GPIO_PORT, DHT11_GPIO_PIN) == 1) {
                    if(++timeout > 5000) return 3; // 卡死退出
                }
            }
        }
    }
    
    // 4. 校验数据
    if (buf[4] == (buf[0] + buf[1] + buf[2] + buf[3])) {
        *humi = buf[0];
        *temp = buf[2];
        return 0; // 成功
    }
    return 2; // 校验失败
}
