# 嵌入式工作室IoT方向招新任务总结
## 项目简介
- 本项目包含我在2026年嵌入式工作室IoT方向招新中的完整代码与总结。项目经历了从底层STM32驱动ESP8266模块，到进阶NodeMCU局域网网页控制，再到云端MQTT远程通信的完整技术演进。
- `round-1_STM32`: 基于 STM32F103 的底层网络通信与本地控制
  * `1.GPIO点灯+按键控制LED`: STM32 基础 GPIO 输入输出与 OLED 驱动
  * `2.网页点灯`: 通过 USART 串口发送 AT 指令驱动 ESP-01，建立 TCP 服务器，并解析 HTTP 请求实现网页控灯
-`round-2_NodeMCU`: 基于 ESP8266 的进阶物联网应用
  * 抛弃繁琐的 AT 透传，使用 Arduino 框架独立开发
  * 实现多任务非阻塞架构，网页毫秒级响应单灯与流水灯（D1/D2/D5）
  * 接入 DHT11 温湿度传感器，并通过 MQTT 协议（点灯科技）实现公网远程控制与数据同步
## 核心技术栈
- 硬件：STM32F103C8T6, NodeMCU(ESP-12E), ESP-01, DHT11, SSD1306 OLED
- 工具：Keil MDK, Arduino IDE, XCOM, NetAssist, GitHub
- 通信协议：UART (AT指令), TCP/IP, HTTP, MQTT
- 架构思想：非阻塞轮询（millis 时间戳）、状态机、前后端分离

## 作者信息
- 电子科技大学信息与软件工程学院
- 张永禄
  
