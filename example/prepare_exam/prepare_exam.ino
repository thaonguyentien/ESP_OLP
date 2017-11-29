#include <ESP8266WiFi.h>  
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <DHT.h>

#include <Wire.h>
#include <BH1750.h>

BH1750 lightMeter;
const char* default_mqtt_server = "192.168.1.100";
const char* default_mqtt_port = "1883";

unsigned long previousMillis = 0; 
const long interval = 5000; 
uint16_t light;

WiFiClient espClient;

PubSubClient client(espClient);


char dataMessage[500];
int InputPin = 2;   //using digital pin10 as input
#define DHTPIN 16   // Sử dụng pin 16 để đọc dữ liệu từ cảm biến
#define DHTTYPE DHT11 // Loại cảm biến ở đây là DHT11
DHT dht(DHTPIN, DHTTYPE);
int ledPin = 16;
const char* icseData = "icse/data";
const char* icseNew = "icse/newDevice"; 
int isRegister=false;

bool Pin_Status = LOW;
bool PIR_State = LOW; //LOW = no motion, HIGH = motion

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
  
  if(strcmp(topic,"icse/60:01:94:34:AC:E3/action")==0){
      payload[length] = '\0';
      char payload_string[200];
      strncpy(payload_string, (char*)payload,sizeof(payload_string));
      Serial.println(payload_string);
      

  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      char* message = "Retained";
      int length = strlen(message);
      boolean retained = true;
      client.publish("icse/60:01:94:34:AC:E3/action","1");
      client.subscribe("icse/60:01:94:34:AC:E3/action");
      client.subscribe("icse/test");
      client.publish("testTopic",(byte*)message,length,retained);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
    Serial.begin(115200);
    WiFiManager wifiManager;
    WiFiManagerParameter custom_mqtt_server("server", "mqtt server", default_mqtt_server, 40);
    WiFiManagerParameter custom_mqtt_port("port", "mqtt port", default_mqtt_port, 6);
    wifiManager.addParameter(&custom_mqtt_server); 
    wifiManager.addParameter(&custom_mqtt_port);
    Serial.println(WiFi.macAddress());
    pinMode(InputPin, INPUT);  //input declaration
    pinMode(ledPin, OUTPUT);
//    wifiManager.resetSettings();//  xóa cmt 
    if (!wifiManager.autoConnect("THAO_ESP")) {
      Serial.println("failed to connect, we should reset as see if it connects");
      delay(3000);
      ESP.reset();
      delay(5000);
    } 
    dht.begin();
    lightMeter.begin();
    Serial.println("connected...yeey :)");
    client.setServer(default_mqtt_server, 1883);
    client.setCallback(callback);
}


void loop() {
   if (!client.connected()) {
      reconnect();
      
  }
  client.loop();
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    sendData();
  }

}

void sendData(){
  StaticJsonBuffer<500> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["macAddr"] = WiFi.macAddress();
  root["type"] = "data";
  JsonArray& sensorsData = root.createNestedArray("sensorsData");
  
  StaticJsonBuffer<100> jsonBuffer1;
  JsonObject& temperature = jsonBuffer1.createObject();
  root["name"] = "DHT11-t";
  root["value"] = getTemperature();

  StaticJsonBuffer<100> jsonBuffer2;
  JsonObject& humidity = jsonBuffer2.createObject();
  root["name"] = "DHT11-h";
  root["value"] = getHumidity();

  StaticJsonBuffer<100> jsonBuffer3;
  JsonObject& light = jsonBuffer3.createObject();
  root["name"] = "BH1750";
  root["value"] = getLight();

  sensorsData.add(temperature);
  sensorsData.add(humidity);
  sensorsData.add(light);
  
}

float getTemperature(){
  return dht.readTemperature(); // Đọc nhiệt độ từ cảm biến
}

float getHumidity(){
  return dht.readHumidity(); // Đọc nhiệt độ từ cảm biến
}

float getLight(){
  return lightMeter.readLightLevel() ;
}

