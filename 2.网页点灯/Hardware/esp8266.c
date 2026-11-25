#include "esp8266.h"
#include "usart.h"
#include "delay.h"
#include <string.h>
#include <stdio.h>

char esp_rx_buffer[1024];
u16 esp_rx_cnt = 0;

// 清理缓冲区
void ESP_ClearBuffer(void) {
    memset(esp_rx_buffer, 0, 1024);
    esp_rx_cnt = 0;
}

// 发送字符串（自带 \r\n）
void ESP8266_SendString(char *str) {
    while (*str) {
        USART_SendData(USART1, *str++);
        while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    }
}

// 发送 AT 指令并等待应答
uint8_t ESP8266_SendAT(char *cmd, char *expect_res, uint16_t timeout) {
    ESP_ClearBuffer();
    ESP8266_SendString(cmd);
    ESP8266_SendString("\r\n");
    
    while (timeout--) {
        if (strstr(esp_rx_buffer, expect_res) != NULL) {
            return 1; // 成功
        }
        delay_ms(10);
    }
    return 0; // 失败
}

// 初始化连网
void ESP8266_Init(void) {
	  delay_ms(2000);
    char cmd[128];
    // 1. 测试
    ESP8266_SendAT("AT", "OK", 500);
    // 2. 设模式
    ESP8266_SendAT("AT+CWMODE=1", "OK", 500);
    // 3. 连 WiFi
    sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"", WIFI_SSID, WIFI_PASS);
    ESP8266_SendAT(cmd, "OK", 15000); // 连 WiFi 给足 15 秒
    // 4. 开多连接
    ESP8266_SendAT("AT+CIPMUX=1", "OK", 500);
    // 5. 建服务器
    ESP8266_SendAT("AT+CIPSERVER=1,8080", "OK", 500);
	  ESP8266_SendAT("AT+CIFSR", "OK", 1000); 
	
}


