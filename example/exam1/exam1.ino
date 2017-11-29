j #include "SH1106.h"
SH1106  display(0x3c, 4, 5);
#include <ESP8266WiFi.h>  
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

#include <Wire.h>
#include <BH1750.h>

BH1750 lightMeter;
const char* default_mqtt_server = "192.168.1.100";
const char* default_mqtt_port = "1883";

int ledStatus=0;
int LedDo=12;
int LedXanh=13;
int LedVang=15;
float temp,humi;
boolean motion;
uint16_t light;
unsigned long previousMillis = 0; 
const long interval = 1000; 

WiFiClient espClient;

PubSubClient client(espClient);


char dataMessage[500];
int InputPin = 2;   //using digital pin10 as input
#define DHTPIN 4     // what digital pin we're connected to
int ledPin = 16;
const char* icseData = "icse/data";
const char* icseNew = "icse/newDevice"; 
int isRegister=false;
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);

bool Pin_Status = LOW;
bool PIR_State = LOW; //LOW = no motion, HIGH = motion

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
  
  if(strcmp(topic,"icse/60:01:94:34:AC:E3/action")==0){
      payload[length] = '\0';
      char payload_string[200];
      strncpy(payload_string, (char*)payload,sizeof(payload_string));
      Serial.println(payload_string);
      StaticJsonBuffer<200> jsonBuffer;
      JsonObject& root = jsonBuffer.parseObject(payload_string);
      String type=root["type"];
      Serial.println(type);
      if(strcmp(root["type"],"register")==0){
        Serial.println("register111");
        if(strcmp(root["type"],"OK")){
          isRegister=true;
        }
      }else {
        if(strcmp(root["greenLed"],"ON")==0){
          digitalWrite(LedXanh,HIGH);
          Serial.println("greenLed ON");
        }else{
          digitalWrite(LedXanh,LOW);
        }
          Serial.print("checkMove");

        if(strcmp(root["redLed"],"ON")==0){
          digitalWrite(LedDo,HIGH);
          Serial.println("redLed ON");
        }else{
          digitalWrite(LedDo,LOW);
        }

        if(strcmp(root["yellowLed"],"ON")==0){
          digitalWrite(LedVang,HIGH);
          Serial.println("yellowLed ON");
        }else{
          digitalWrite(LedVang,LOW);
        }
      }

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
    pinMode(LedDo,OUTPUT);
    pinMode(LedXanh,OUTPUT);
    pinMode(LedVang,OUTPUT);
    dht.begin();
    Wire.begin();
    display.init();
    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_24);
    lightMeter.begin();
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
    sendRegister();
  }

}

void sendRegister(){

  if(isRegister==false){
      StaticJsonBuffer<500> jsonBuffer;

      JsonObject& jsonNew = jsonBuffer.createObject();
      jsonNew["macAddr"]=WiFi.macAddress();
      jsonNew["type"]= "ESP8266";
      JsonArray& sensors = jsonNew.createNestedArray("sensors");
      
      StaticJsonBuffer<100> jsonDeviceBuffer;
      JsonObject& device1 = jsonDeviceBuffer.createObject();
      device1["name"]="DHT11-t";
      device1["unit"]="C";
      sensors.add(device1);
    
      StaticJsonBuffer<100> jsonTempBuffer;
      JsonObject& jsonTemp = jsonTempBuffer.createObject();
      jsonTemp["unit"]="%";
      jsonTemp["name"]="DHT11-h";
      sensors.add(jsonTemp);
    
      StaticJsonBuffer<100> jsonHumiBuffer;
      JsonObject& jsonHumi = jsonHumiBuffer.createObject();
      jsonHumi["unit"]="";
      jsonHumi["name"]="HC-SR501";
      sensors.add(jsonHumi);
    
      StaticJsonBuffer<100> jsonLightBuffer;
      JsonObject& jsonLight = jsonLightBuffer.createObject();
      jsonLight["unit"]="lux";
      jsonLight["name"]="BH1750";
      sensors.add(jsonLight);
      
      jsonNew.printTo(dataMessage, sizeof(dataMessage));
    //  Serial.println(dataMessage);
      client.publish(icseNew, dataMessage);
      
//      Serial.println(" Register");
//      Serial.println(icseNew);
//      Serial.println(dataMessage);
  }
}
void sendData(){
    
  StaticJsonBuffer<500> jsonBuffer;

  JsonObject& jsonData = jsonBuffer.createObject();
  jsonData["macAddr"]=WiFi.macAddress();
  JsonArray& sensorsData = jsonData.createNestedArray("sensorsData");
  
  StaticJsonBuffer<100> jsonMotionBuffer;
  JsonObject& jsonMotion = jsonMotionBuffer.createObject();
  jsonMotion["value"]=checkMove();
  jsonMotion["name"]="HC-SR501_01";
  sensorsData.add(jsonMotion);

  StaticJsonBuffer<100> jsonTempBuffer;
  JsonObject& jsonTemp = jsonTempBuffer.createObject();
  if(isnan(getTemperature())){
    jsonTemp["value"]="nan";
  }else{
    jsonTemp["value"]=getTemperature();
  }
  
  jsonTemp["name"]="DHT11-t";
  sensorsData.add(jsonTemp);

  StaticJsonBuffer<100> jsonHumiBuffer;
  JsonObject& jsonHumi = jsonHumiBuffer.createObject();
  if(isnan(getHumidity())){
    jsonHumi["value"]="nan";
  }else{
    jsonHumi["value"]=getHumidity();
  }
  jsonHumi["name"]="DHT11-h";
  sensorsData.add(jsonHumi);

  StaticJsonBuffer<100> jsonLightBuffer;
  JsonObject& jsonLight = jsonLightBuffer.createObject();
  jsonLight["value"]=getLight();
  jsonLight["name"]="BH1750_01";
  sensorsData.add(jsonLight);
  
  jsonData.printTo(dataMessage, sizeof(dataMessage));
  
  client.publish(icseData, dataMessage);
//  
//  Serial.println(icseData);
//  Serial.println(dataMessage);
}

boolean checkMove(){
  return digitalRead(InputPin);
}

float getLight(){
  return lightMeter.readLightLevel() ;
}

float getTemperature(){
//  Serial.println(dht.readTemperature());
  return dht.readTemperature();
}

float getHumidity(){
  return dht.readHumidity();
}
