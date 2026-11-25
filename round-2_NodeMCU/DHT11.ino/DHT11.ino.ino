#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>

// ================== 配置区 ==================
const char* ssid = "Xiaomi_88E5";
const char* password = "1234567890";

#define DHTPIN D2         
#define DHTTYPE DHT11      
#define LED_PIN D4         
// ==========================================

DHT dht(DHTPIN, DHTTYPE);
ESP8266WebServer server(80);

bool ledState = false; 
unsigned long lastDhtRead = 0; // 记录上一次读取DHT11的时间戳
float currentTemp = 0.0;       // 后台存着的温度
float currentHumi = 0.0;       // 后台存着的湿度

void handleRoot() {

  float t = currentTemp;
  float h = currentHumi;

  String html = "<!DOCTYPE html><html><head><meta charset='utf-8'>";
  html += "<meta http-equiv='refresh' content='5'>"; // 每5秒自动刷新网页
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>ESP8266 智能家居</title>";
  html += "<style>";
  html += "body{background:#121212;color:#fff;text-align:center;padding-top:50px;font-family:sans-serif}";
  html += ".card{background:#1e1e1e;padding:40px;border-radius:20px;display:inline-block;box-shadow:0 4px 15px rgba(0,0,0,0.5)}";
  html += "h1{color:#4CAF50;margin-bottom:30px}";
  html += ".data{font-size:22px;color:#FFC107;margin:15px 0}";
  html += "button{background:#2196F3;color:#fff;border:none;padding:15px 40px;font-size:18px;border-radius:10px;margin-top:20px;cursor:pointer}";
  html += "</style></head><body>";
  
  html += "<div class='card'>";
  html += "<h1>智能环境监控</h1>";
  html += "<div class='data'>温度: " + String(t) + " °C</div>";
  html += "<div class='data'>湿度: " + String(h) + " %</div>";
  html += "<div class='data'>LED状态: <span style='color:" + String(ledState ? "#4CAF50" : "#F44336") + "'>" + String(ledState ? "开启" : "关闭") + "</span></div>";
  html += "<a href='/toggle'><button>切换LED</button></a>";
  html += "</div></body></html>";
  
  server.send(200, "text/html", html);
}

// 处理翻转LED请求
void handleToggle() {
  ledState = !ledState;
  digitalWrite(LED_PIN, ledState ? LOW : HIGH); 
  
  server.sendHeader("Location", "/");
  server.send(303);
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
  Serial.print("请用手机浏览器访问这个IP: ");
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
}