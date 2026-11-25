#ifndef __ESP8266_H
#define __ESP8266_H

#include "stm32f10x.h"

// 修改为你的手机热点和密码
#define WIFI_SSID "Xiaomi_88E5"
#define WIFI_PASS "1234567890"

void ESP8266_Init(void);
void ESP8266_SendString(char *str);
void ESP_ClearBuffer(void);
uint8_t ESP8266_SendAT(char *cmd, char *expect_res, uint16_t timeout);

#endif
