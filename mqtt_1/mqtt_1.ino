#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "SH1106.h"
SH1106  display(0x3c, 4, 5);
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
OneWire  ds(2);  // on pin D4 (a 4.7K resistor is necessary)
DallasTemperature DS18B20(&ds);
// bạn thay đổi thông tin bên dưới cho phù hợp
const char* ssid =         "HPCC-IOT";
const char* password =    "hpcc_iot";
const char* mqtt_server = "192.168.1.100";   /// lấy ip bằng lệnh ifconfig, ví dụ 192.168.1.105
const uint16_t mqtt_port = 1883;
const byte ledPin = D0;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;


//get nhiệt độ từ cảm biến
float getTemperature() {
  float tempC;
  float tempF;
  do {
    DS18B20.requestTemperatures();
    tempC = DS18B20.getTempCByIndex(0);
    return tempC;
    delay(100);
  } while (tempC == 85.0 || tempC == (-127.0));
}

void setup() {
  pinMode(ledPin, OUTPUT);     // Khởi tạo LED
  Serial.begin(115200);
  setup_wifi();
  DS18B20.begin();
  pinMode(2, INPUT_PULLUP);
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_24);
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void setup_wifi() {

  delay(10);
  // Kết nối wifi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Bật LED nếu nhận được lệnh là 1
  if ((char)payload[0] == '1') {
     Serial.println("ON");
    digitalWrite(ledPin, LOW);   // LED có sẵn trên NodeMCU bật ở mức 0(LOW)
  } 
  // Tắt LED nếu nhận được lệnh là 0
 if ((char)payload[0] == '0') {
    Serial.println("OFF");
    digitalWrite(ledPin, HIGH);  // LED tắt ở mức 1(HIGH)
  }

}

void reconnect() {
  // Đợi tới khi kết nối
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Khi kết nối thành công sẽ gửi chuỗi helloworld lên topic event
      client.publish("Nhiet Do", "hello world");
      // ... sau đó sub lại thông tin
      client.subscribe("Nhiet Do");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  
  client.loop();
 // Thực hiện 2s gửi dữ liệu helloworld lên broker 1 lần
  long now = millis();
  float temp=getTemperature();
  String tempInt= String(temp);
  char str_temp[6];
  dtostrf(temp, 4, 2, str_temp);
  display.clear();
  display.drawString(0, 0,"T= " +String(temp));
  display.display();
  Serial.println(tempInt);
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf (msg, 75, "nhiet do:# %s C", str_temp);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("Nhiet Do", msg);
  }
}
