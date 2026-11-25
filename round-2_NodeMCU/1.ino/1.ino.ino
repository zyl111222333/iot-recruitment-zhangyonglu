#include <Wire.h>
#include <DHT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define DHTPIN      D4      
#define DHTTYPE     DHT11
#define OLED_SCL    D1
#define OLED_SDA    D2
#define INA_PIN     D5     
#define INB_PIN     D6      
#define RELAY_PIN   D7     

#define TEMP_ON     30.0    
#define TEMP_OFF    28.0    

DHT dht(DHTPIN, DHTTYPE); 
Adafruit_SSD1306 oled(128, 64, &Wire, -1);

bool fanRunning = false;    

void fanOn() {
  if (fanRunning) return;
  digitalWrite(RELAY_PIN, LOW);   
  delay(80);                     
  digitalWrite(INA_PIN, HIGH);
  digitalWrite(INB_PIN, LOW);
  fanRunning = true;
  Serial.println(">> 风扇已开启（继电器吸合）");
}

void fanOff() {
  if (!fanRunning) return;
  digitalWrite(INA_PIN, LOW);
  digitalWrite(INB_PIN, LOW);
  delay(80);                      
  digitalWrite(RELAY_PIN, HIGH);  
  fanRunning = false;
  Serial.println(">> 风扇已关闭（继电器断开）");
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n===== 系统启动 =====");

  pinMode(INA_PIN, OUTPUT);
  pinMode(INB_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(INA_PIN, LOW);
  digitalWrite(INB_PIN, LOW);
  digitalWrite(RELAY_PIN, HIGH); 

  dht.begin();

  Wire.begin(OLED_SDA, OLED_SCL);
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED 初始化失败！");
    while (true) { delay(1000); }
  }
  oled.clearDisplay();
  oled.setTextColor(WHITE);
  oled.setCursor(0, 0);
  oled.println("ESP8266 启动中...");
  oled.display();
  delay(1000);
}

void loop() {
  static unsigned long lastRead = 0;
  if (millis() - lastRead >= 2000) {   
    lastRead = millis();

    float temp = dht.readTemperature();
    float hum  = dht.readHumidity();

    if (isnan(temp) || isnan(hum)) {
      Serial.println("DHT11 读取失败！");
      return;
    }

    Serial.printf("温度: %.1f℃  湿度: %.1f%%\n", temp, hum);

  
    if (!fanRunning && temp >= TEMP_ON) {
      fanOn();
    } else if (fanRunning && temp <= TEMP_OFF) {
      fanOff();
    }

    
    oled.clearDisplay();
    oled.setTextSize(1);
    oled.setCursor(0, 0);
    oled.printf("T: %.1f C\n", temp);
    oled.printf("H: %.1f %%\n", hum);
    oled.printf("Fan: %s\n", fanRunning ? "ON" : "OFF");
    oled.printf("Relay: %s\n", fanRunning ? "ON" : "OFF");
    oled.display();
  }
}

