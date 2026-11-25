#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

const char* ssid = "Xiaomi_88E5";
const char* password = "1234567890";

const char* mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;
const char* mqtt_user = "";                 
const char* mqtt_pass = "";

const char* topic_telemetry = "devices/esp8266_01/telemetry";
const char* topic_command   = "devices/esp8266_01/command";

#define LED_PIN D4
#define DHTPIN  D2
#define DHTTYPE DHT11

#define HEAP_LOG_MS 10000UL // 每 10 秒打印一次内存信息
#define DHT_READ_MS 5000UL  // 每 5 秒读取一次温湿度


DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient mqttClient(espClient);

bool ledState = false;

void setup_wifi();
void reconnect();
void callback(char* topic, byte* payload, unsigned int length);
void readAndReportDHT();
static void appliedLed();

static void appliedLed() {
  digitalWrite(LED_PIN, ledState ? LOW : HIGH);
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("正在连接 WiFi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi 已连接");
  Serial.print("IP 地址: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("收到消息 [");
  Serial.print(topic);
  Serial.print("] ");
  
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  if (String(topic) == topic_command) {
    if (message == "on") {
      ledState = true;
      appliedLed();
      Serial.println("[APP] -> LED ON");
    } else if (message == "off") {
      ledState = false;
      appliedLed();
      Serial.println("[APP] -> LED OFF");
    } else if (message == "toggle") {
      ledState = !ledState;
      appliedLed();
      Serial.printf("[APP] -> LED %s\n", ledState ? "ON" : "OFF");
    }
  }
}

void reconnect() {
  while (!mqttClient.connected()) {
    Serial.print("尝试连接 MQTT 服务器...");
    
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);

    if (mqttClient.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
      Serial.println("已连接");
      
      mqttClient.subscribe(topic_command);
      Serial.print("已订阅主题: ");
      Serial.println(topic_command);
      
      mqttClient.publish(topic_command, ledState ? "on" : "off");
      
    } else {
      Serial.print("失败, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" 5 秒后重试");
      delay(5000);
    }
  }
}

void readAndReportDHT() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  
  if (!isnan(t) && !isnan(h)) {
    // 构造 JSON 格式的 payload
    // 形如：{"temp":25.5,"humi":60.0}
    String payload = "{\"temp\":" + String(t, 1) + ",\"humi\":" + String(h, 1) + "}";
    
    mqttClient.publish(topic_telemetry, payload.c_str());
    
    Serial.printf("[DHT11] 上报数据: %s\n", payload.c_str());
  } else {
    Serial.println("[DHT11] 读取失败!");
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("=== SmartHome_MQTT_ESP8266 启动 ===");
  Serial.printf("启动时空闲堆: %u bytes\n", ESP.getFreeHeap());

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH); 

  dht.begin();

  setup_wifi();

  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(callback); 
}

void loop() {
  // 1. 确保 WiFi 和 MQTT 连接正常
  if (WiFi.status() != WL_CONNECTED) {
    setup_wifi();
  }
  if (!mqttClient.connected()) {
    reconnect();
  }
  // 必须不断调用 loop 以保持连接并接收消息
  mqttClient.loop();

  // 2. 定时任务：读取 DHT11 并发布
  static uint32_t lastDhtMs = 0;
  if (millis() - lastDhtMs >= DHT_READ_MS) {
    lastDhtMs = millis();
    readAndReportDHT();
  }

  // 3. 定时任务：打印内存信息
  static uint32_t lastHeapMs = 0;
  if (millis() - lastHeapMs >= HEAP_LOG_MS) {
    lastHeapMs = millis();
    Serial.printf("[HEAP] up=%lus free=%u, maxBlock=%u, frag=%u%%\n",
                  (unsigned long)(millis() / 1000UL),
                  (unsigned int)ESP.getFreeHeap(),
                  (unsigned int)ESP.getMaxFreeBlockSize(),
                  (unsigned int)ESP.getHeapFragmentation());
  }
}