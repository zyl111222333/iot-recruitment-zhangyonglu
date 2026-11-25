#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// ================== 配置区 ==================
const char* ssid = "Xiaomi_88E5";     
const char* password = "1234567890";  

// 引脚定义 (三个外接 LED)
#define LED_1 D1  // 单灯 & 流水灯第 1 个
#define LED_2 D2  // 流水灯第 2 个
#define LED_3 D5  // 流水灯第 3 个

ESP8266WebServer server(80);

bool singleLedState = false; 
bool flowLedState = false;   
int flowStep = 0;          

unsigned long lastFlowTime = 0;

String getHTML() {
  String html = "<!DOCTYPE html><html><head><meta charset='utf-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>NodeMCU 智能控制</title>";
  html += "<style>";
  html += "body{background:#121212;color:#fff;text-align:center;padding-top:50px;font-family:sans-serif}";
  html += ".card{background:#1e1e1e;padding:40px;border-radius:20px;display:inline-block;box-shadow:0 4px 15px rgba(0,0,0,0.5)}";
  html += "h1{color:#4CAF50;margin-bottom:30px}";
  html += "h2{color:#FFC107;font-size:20px;margin-top:30px}";
  html += "a{text-decoration:none}";
  html += "button{color:#fff;border:none;padding:15px 40px;font-size:18px;border-radius:10px;margin:10px;cursor:pointer}";
  html += ".btn-on{background:#F44336}";  
  html += ".btn-off{background:#4CAF50}"; 
  html += ".btn-disabled{background:#555;cursor:not-allowed}"; 
  html += "</style></head><body>";
  
  html += "<div class='card'>";
  html += "<h1>NodeMCU 控制台</h1>";
  
  
  html += "<h2>单灯控制 (D1)</h2>";
  if (flowLedState) {
    
    html += "<button class='btn-disabled' disabled>流水灯运行中</button>";
  } else {
    if (singleLedState) {
      html += "<a href='/single?state=0'><button class='btn-on'>关闭单灯</button></a>";
    } else {
      html += "<a href='/single?state=1'><button class='btn-off'>开启单灯</button></a>";
    }
  }

  
  html += "<h2>流水灯模式 (D1->D2->D5)</h2>";
  if (flowLedState) {
    html += "<a href='/flow?state=0'><button class='btn-on'>停止流水灯</button></a>";
  } else {
    html += "<a href='/flow?state=1'><button class='btn-off'>开启流水灯</button></a>";
  }

  html += "</div></body></html>";
  return html;
}

void handleRoot() {
  server.send(200, "text/html", getHTML());
}


void handleSingle() {
  if (server.hasArg("state") && !flowLedState) { 
    singleLedState = (server.arg("state") == "1");
  
    digitalWrite(LED_1, singleLedState ? HIGH : LOW);
  }
  server.sendHeader("Location", "/");
  server.send(303);
}


void handleFlow() {
  if (server.hasArg("state")) {
    flowLedState = (server.arg("state") == "1");
    if (flowLedState) {
      flowStep = 0; 
    } else {
      
      digitalWrite(LED_1, singleLedState ? HIGH : LOW);
      digitalWrite(LED_2, LOW);
      digitalWrite(LED_3, LOW);
    }
  }
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);
  
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);
  
  digitalWrite(LED_1, LOW);
  digitalWrite(LED_2, LOW);
  digitalWrite(LED_3, LOW);

  WiFi.begin(ssid, password);
  Serial.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected! IP: ");
  Serial.println(WiFi.localIP());


  server.on("/", handleRoot);
  server.on("/single", handleSingle);
  server.on("/flow", handleFlow);
  server.begin();
}

// ================== 核心：非阻塞多任务 ==================
void loop() {
  // 任务1：疯狂处理网页请求（非阻塞）
  server.handleClient();

  // 任务2：非阻塞处理流水灯逻辑
  if (flowLedState) {
    // 每 300 毫秒切换一次状态
    if (millis() - lastFlowTime >= 300) {
      lastFlowTime = millis();
      
      
      digitalWrite(LED_1, LOW);
      digitalWrite(LED_2, LOW);
      digitalWrite(LED_3, LOW);

     
      if (flowStep == 0) {
        digitalWrite(LED_1, HIGH);
        flowStep = 1;
      } else if (flowStep == 1) {
        digitalWrite(LED_2, HIGH);
        flowStep = 2;
      } else if (flowStep == 2) {
        digitalWrite(LED_3, HIGH);
        flowStep = 0;
      }
    }
  }
}