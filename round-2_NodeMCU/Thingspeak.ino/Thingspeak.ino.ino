#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>  
#include <DHT.h>

// ================== 配置区 ==================
const char* ssid = "Xiaomi_88E5";
const char* password = "1234567890";

String apiKey = "3QHJEFLOI7UW850U"; 

#define DHTPIN D2
#define DHTTYPE DHT11
#define LED_PIN D4
// ==========================================

DHT dht(DHTPIN, DHTTYPE);
ESP8266WebServer server(80);

bool ledState = false; 
unsigned long lastDhtRead = 0; 
unsigned long lastPrint = 0; 
unsigned long lastCloudUpdate = 0; // 记录上次上云的时间

float currentTemp = 0.0; 
float currentHumi = 0.0; 

void handleRoot() {
  // ... 网页代码不变 ...
  float t = currentTemp;
  float h = currentHumi;
  String html = "<!DOCTYPE html><html><head><meta charset='utf-8'>";
  html += "<meta http-equiv='refresh' content='5'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>ESP8266 智能家居</title>";
  html += "<style>body{background:#121212;color:#fff;text-align:center;padding-top:50px;font-family:sans-serif}.card{background:#1e1e1e;padding:40px;border-radius:20px;display:inline-block;box-shadow:0 4px 15px rgba(0,0,0,0.5)}h1{color:#4CAF50;margin-bottom:30px}.data{font-size:22px;color:#FFC107;margin:15px 0}button{background:#2196F3;color:#fff;border:none;padding:15px 40px;font-size:18px;border-radius:10px;margin-top:20px;cursor:pointer}</style></head><body>";
  html += "<div class='card'><h1>智能环境监控</h1>";
  html += "<div class='data'>温度: " + String(t) + " °C</div>";
  html += "<div class='data'>湿度: " + String(h) + " %</div>";
  html += "<div class='data'>LED状态: <span style='color:" + String(ledState ? "#4CAF50" : "#F44336") + "'>" + String(ledState ? "开启" : "关闭") + "</span></div>";
  html += "<a href='/toggle'><button>切换LED</button></a></div></body></html>";
  server.send(200, "text/html", html);
}

void handleToggle() {
  ledState = !ledState;
  digitalWrite(LED_PIN, ledState ? LOW : HIGH); 
  server.sendHeader("Location", "/");
  server.send(303);
}

// 负责发数据给云端的函数
void sendToCloud(float t, float h) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;
    
    // 拼接 ThingSpeak 的 API 地址
    String url = "http://api.thingspeak.com/update?api_key=" + apiKey;
    url += "&field1=" + String(t);
    url += "&field2=" + String(h);
    
    http.begin(client, url);
    int httpCode = http.GET(); // 发送 GET 请求
    
    if (httpCode > 0) {
      Serial.printf("[云端] 上传成功, HTTP状态码: %d\n", httpCode);
    } else {
      Serial.printf("[云端] 上传失败, 错误: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH); 
  dht.begin();
  
  Serial.print("正在连接 WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi 连接成功！");
  Serial.print("局域网控制IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/toggle", handleToggle);
  server.begin();
}

void loop() {
  server.handleClient(); 

  unsigned long currentMillis = millis();

  if (currentMillis - lastDhtRead >= 2000) {
    lastDhtRead = currentMillis; 
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    if (!isnan(t) && !isnan(h)) {
      currentTemp = t;
      currentHumi = h;
    }
  }

  if (currentMillis - lastPrint >= 5000) {
    lastPrint = currentMillis;
    Serial.printf("本地打卡 -> 温度: %.1f °C, 湿度: %.1f %%\r\n", currentTemp, currentHumi);
  }

  if (currentMillis - lastCloudUpdate >= 20000) {
    lastCloudUpdate = currentMillis;
    sendToCloud(currentTemp, currentHumi);
  }
}
