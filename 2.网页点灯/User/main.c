#include "stm32f10x.h"
#include "usart.h"
#include "esp8266.h"
#include "delay.h"
#include "dht11.h"
#include <string.h>
#include <stdio.h>

extern char esp_rx_buffer[1024];
extern u16 esp_rx_cnt;

uint8_t g_temp = 0;
uint8_t g_humi = 0;

char web_html[512]; 
char at_cmd[32];

int main(void) {
    delay_init(); 
    USART1_Config();
    DHT11_Init(); 
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_SetBits(GPIOC, GPIO_Pin_13);
    
    ESP8266_Init();
    
    while(1) {
   
        uint8_t t, h;
        if (DHT11_Read_Data(&t, &h) == 0) {
            g_temp = t;
            g_humi = h;
        }
        delay_ms(2000); 
    }
}

void USART1_IRQHandler(void) {
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        char res = USART_ReceiveData(USART1);
        
        if (esp_rx_cnt < 1023) {
            esp_rx_buffer[esp_rx_cnt++] = res;
            esp_rx_buffer[esp_rx_cnt] = '\0';
        }
        
        
        u8 link_id = 0; 
        char *ipd_ptr = strstr(esp_rx_buffer, "+IPD,");
        if (ipd_ptr != NULL) {
            link_id = ipd_ptr[5] - '0'; 
        }

        //收到 LED=1 (开灯)
        if (strstr(esp_rx_buffer, "LED=1") != NULL) {
            GPIO_ResetBits(GPIOC, GPIO_Pin_13); 
            
            char *html = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n"
                         "<!DOCTYPE html><html><head><meta charset='utf-8'><meta name='viewport' content='width=device-width,initial-scale=1'><link rel='icon' href='data:,'><style>body{background:#121212;color:#fff;text-align:center;padding-top:50px}h1{color:#4CAF50}button{background:#4CAF50;color:#fff;border:none;padding:15px 40px;font-size:18px;border-radius:10px;}</style></head><body><h1>LED IS ON</h1><a href='/?LED=0'><button>TURN OFF</button></a></body></html>";
            
            char cmd[32];
            sprintf(cmd, "AT+CIPSEND=%d,%d\r\n", link_id, strlen(html));
            ESP8266_SendString(cmd); 
            delay_ms(50); 
            ESP8266_SendString(html); 
            
            delay_ms(100);
        
            sprintf(cmd, "AT+CIPCLOSE=%d\r\n", link_id);
            ESP8266_SendString(cmd);
            
            delay_ms(50);
            ESP_ClearBuffer();
        }
        //收到 LED=0 (关灯)
        else if (strstr(esp_rx_buffer, "LED=0") != NULL) {
            GPIO_SetBits(GPIOC, GPIO_Pin_13); 
            
            char *html = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n"
                         "<!DOCTYPE html><html><head><meta charset='utf-8'><meta name='viewport' content='width=device-width,initial-scale=1'><link rel='icon' href='data:,'><style>body{background:#121212;color:#fff;text-align:center;padding-top:50px}h1{color:#F44336}button{background:#F44336;color:#fff;border:none;padding:15px 40px;font-size:18px;border-radius:10px;}</style></head><body><h1>LED IS OFF</h1><a href='/?LED=1'><button>TURN ON</button></a></body></html>";
            
            char cmd[32];
            sprintf(cmd, "AT+CIPSEND=%d,%d\r\n", link_id, strlen(html));
            ESP8266_SendString(cmd); 
            delay_ms(50); 
            ESP8266_SendString(html); 
            
            delay_ms(100);
            sprintf(cmd, "AT+CIPCLOSE=%d\r\n", link_id);
            ESP8266_SendString(cmd);
            
            delay_ms(50);
            ESP_ClearBuffer();
        }
        //默认首页
        else if (strstr(esp_rx_buffer, "GET / HTTP") != NULL) {
            char *html = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n"
                         "<!DOCTYPE html><html><head><meta charset='utf-8'><meta name='viewport' content='width=device-width,initial-scale=1'><link rel='icon' href='data:,'><style>body{background:#121212;color:#fff;text-align:center;padding-top:50px}h1{color:#2196F3}button{background:#2196F3;color:#fff;border:none;padding:15px 40px;font-size:18px;border-radius:10px;}</style></head><body><h1>ESP8266 WEB</h1><a href='/?LED=1'><button>TURN ON</button></a></body></html>";
            
            char cmd[32];
            sprintf(cmd, "AT+CIPSEND=%d,%d\r\n", link_id, strlen(html));
            ESP8266_SendString(cmd); 
            delay_ms(50); 
            ESP8266_SendString(html); 
            
            delay_ms(100);
            sprintf(cmd, "AT+CIPCLOSE=%d\r\n", link_id);
            ESP8266_SendString(cmd);
            
            delay_ms(50);
            ESP_ClearBuffer();
        }
        
        if (esp_rx_cnt >= 1022) {
            ESP_ClearBuffer();
        }
    }
}






