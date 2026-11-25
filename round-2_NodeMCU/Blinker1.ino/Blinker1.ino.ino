/* ====================================================================
 * SmartHome_Finalino v6_core302_DHT11
 * 功能：Blinker + DHT11 温湿度上报 + 远程控灯 + 内存监控
 * ==================================================================== */

#define BLINKER_WIFI
#include <Blinker.h>
#include <DHT.h> 

// ================== 配置区 ==================
char auth[] = "1fa6e58213d7";
char ssid[] = "Xiaomi_88E5";
char pswd[] = "1234567890";

// 引脚定义
#define LED_PIN D4
#define DHTPIN  D2      //  DHT11 接在 D2
#define DHTTYPE DHT11   //  传感器型号

#define HEAP_LOG_MS 10000UL // 每 10 秒打印一次内存信息
#define DHT_READ_MS 5000UL  // 每 5 秒读取一次温湿度（千万别低于2秒）

// ==========================================

DHT dht(DHTPIN, DHTTYPE); // 实例化传感器
BlinkerButton Button1("btn-led");
bool ledState      = false;
bool stateChanged  = false;

BlinkerNumber NumberTemp("temp");
BlinkerNumber NumberHumi("humi");

// 全局变量存储温湿度
float currentTemp = 0.0;
float currentHumi = 0.0;

// 辅助函数：应用 LED 状态
static void appliedLed() {
  digitalWrite(LED_PIN, ledState ? LOW : HIGH);
}

// 辅助函数：读取并上报温湿度
void readAndReportDHT() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  
  // 如果读取成功，更新全局变量并发送给 App
  if (!isnan(t) && !isnan(h)) {
    currentTemp = t;
    currentHumi = h;
    NumberTemp.print(currentTemp);
    NumberHumi.print(currentHumi);
    
    // 可选：在串口打印，方便调试
    Serial.printf("[DHT11] Temp: %.1f C, Humi: %.1f %%\n", currentTemp, currentHumi);
  }
}

// 按键回调函数
void button1_callback(const String & state) {
  if (state == "on") {
    ledState = true;
  } else if (state == "off") {
    ledState = false;
  } else {
    ledState = !ledState; // tap/press 翻转
  }
  
  appliedLed();
  Button1.print(ledState ? "on" : "off");
  stateChanged = true;
  
  // 按键时顺便上报一次数据，让 App 界面立刻刷新
  readAndReportDHT(); 
}

// 心跳函数（每隔一段时间自动执行）
void heartbeat() {
  Button1.print(ledState ? "on" : "off");
  //心跳时也顺带发一次温湿度，防止 App 数据过期
  NumberTemp.print(currentTemp);
  NumberHumi.print(currentHumi);
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("=== SmartHome_Finalino v6 启动 ===");
  Serial.print("复位原因: ");
  Serial.println(ESP.getResetReason());
  Serial.printf("启动时空闲堆: %u bytes\n", ESP.getFreeHeap());

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  dht.begin();

  WiFi.persistent(false);
  WiFi.setAutoReconnect(true);

  Blinker.begin(auth, ssid, pswd);
  Button1.attach(button1_callback);
  Blinker.attachHeartbeat(heartbeat);
}

void loop() {
  Blinker.run();

  // 状态变化打印
  if (stateChanged) {
    stateChanged = false;
    Serial.println(ledState ? "[APP] -> ON" : "[APP] -> OFF");
  }

  //任务：非阻塞定时读取 DHT11
  static uint32_t lastDhtMs = 0;
  if (millis() - lastDhtMs >= DHT_READ_MS) {
    lastDhtMs = millis();
    readAndReportDHT();
  }

  // 任务：定时打印内存状况
  static uint32_t lastMs = 0;
  if (millis() - lastMs >= HEAP_LOG_MS) {
    lastMs = millis();
    Serial.printf("[HEAP] up=%lus free=%u, maxBlock=%u, frag=%u%%\n",
                  (unsigned long)(millis() / 1000UL),
                  (unsigned int)ESP.getFreeHeap(),
                  (unsigned int)ESP.getMaxFreeBlockSize(),
                  (unsigned int)ESP.getHeapFragmentation());
  }
}
