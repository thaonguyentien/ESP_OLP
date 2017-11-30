#include <ESP8266WiFi.h>  
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <Servo.h>
#include <Wire.h>
#include <BH1750.h>

#include "SH1106.h"
SH1106  display(0x3c, 4, 5);

BH1750 lightMeter;
const char* default_mqtt_server = "192.168.1.105";
const char* default_mqtt_port = "1883";

char mqtt_server[255];
char mqtt_port[6];

const char* topic_pub="bkcloud/data";
const char* topic_sub="bkcloud/60:01:94:34:AB:AC/action";
const char* topic_new="bkcloud/newDevice";
char* macAdd="";
unsigned long previousMillis1 = 0;
unsigned long previousMillis2 = 0; 
const long intervalSendData = 5000;
const long intervalDisplayData = 10000;  
uint16_t light;
boolean isRegister=false;
WiFiClient espClient;

PubSubClient client(espClient);


char dataMessage[500];
int movePin = 2;   //using digital pin10 as input
int ledPin = 12;

#define SERVO_PIN 0
Servo myservo1;

#define DHTPIN 4   // Sử dụng pin 4 để đọc dữ liệu từ cảm biến
#define DHTTYPE DHT22// Loại cảm biến ở đây là DHT11
DHT dht(DHTPIN, DHTTYPE);


bool Pin_Status = LOW;
bool PIR_State = LOW; //LOW = no motion, HIGH = motion

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
  Serial.println("Message coming");
  if(strcmp(topic,topic_sub)==0){
      payload[length] = '\0';
      char payload_string[200];
      strncpy(payload_string, (char*)payload,sizeof(payload_string));
      Serial.println(payload_string);
      StaticJsonBuffer<200> jsonBuffer;

      JsonObject& root = jsonBuffer.parseObject(payload_string);
      
      const char* type = root["type"];
      
      const char* action = root["action"];

      if(strcmp(type,"register")==0){
        const char* status1 = root["status"];
        if(strcmp(status1,"OK")==0){
          isRegister=true;
          Serial.println("Register");
        }
      }
      
      if(strcmp(type,"servoAction")==0){
        Serial.println(type);
        if(strcmp(action,"ON")==0){
          Serial.println(action);
          servoRun();
        }
      }
      
      if(strcmp(type,"ledAction")==0){
        Serial.println("led");
        Serial.println(action);
        if(strcmp(action,"ON")==0){
          digitalWrite(ledPin,HIGH);
        }else{
          digitalWrite(ledPin,LOW);
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
      client.publish(topic_pub,"1");
      client.subscribe(topic_sub);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void servoRun(){
  Serial.println("runServo");
  int pos=0;
  for ( pos = 90; pos <= 180; pos += 5) {
    myservo1.write(pos);
    myservo1.write(0);
    delay(100);
  }
  for ( pos = 180; pos >= 90; pos -= 5) {
    myservo1.write(pos);
    delay(100);
  }
//   myservo1.write(90);
}

void setup() {
    Serial.begin(115200);
    display.init();
    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_24);
    
    WiFiManager wifiManager;
    WiFiManagerParameter custom_mqtt_server("server", "mqtt server", default_mqtt_server, 40);
    WiFiManagerParameter custom_mqtt_port("port", "mqtt port", default_mqtt_port, 6);
    wifiManager.addParameter(&custom_mqtt_server); 
    wifiManager.addParameter(&custom_mqtt_port);
    
    
    Serial.println(WiFi.macAddress());
    pinMode(movePin, INPUT);  //input declaration
    pinMode(ledPin, OUTPUT);
    myservo1.attach(SERVO_PIN);
    wifiManager.resetSettings();//  xóa cmt 
    if (!wifiManager.autoConnect("THAO_ESP")) {
      Serial.println("failed to connect, we should reset as see if it connects");
      delay(3000);
      ESP.reset();
      delay(5000);
    } 
    dht.begin();
    lightMeter.begin();
    Serial.println("connected...yeey :)");
    strcpy(mqtt_server, custom_mqtt_server.getValue());
    strcpy(mqtt_port, custom_mqtt_port.getValue());
    Serial.println(mqtt_server);
    Serial.println(mqtt_port);
    client.setServer(mqtt_server, atoi(mqtt_port));
//    client.setServer(default_mqtt_server, 1883);
    client.setCallback(callback);
}


void loop() {
  
   if (!client.connected()) {
      reconnect();
  }
  
  client.loop();
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis1 >= intervalSendData) {
    previousMillis1 = currentMillis;
    if(isRegister==true){
      Serial.println("sendRegister");
      sendData();
    }
    
    
  }    
  if (currentMillis - previousMillis2 >= intervalDisplayData) {
    previousMillis2 = currentMillis;
    if(isRegister==false){
      Serial.println("sendRegister");
      sendRegister();
    }
    displayData();
    
  }
  int isMove = digitalRead(movePin);
  if(isMove==1){
      sendMove();
      delay(4000);
   }
}

void sendRegister(){
  StaticJsonBuffer<400> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["macAddr"] = WiFi.macAddress();
  root["type"] = "ESP8266";

  JsonArray& sensors = root.createNestedArray("sensors");

  StaticJsonBuffer<100> jsonBuffer1;
  JsonObject& temperature = jsonBuffer1.createObject();
  temperature["name"] = "DHT11-t";
  temperature["unit"] = "C";

  
  StaticJsonBuffer<100> jsonBuffer2;
  JsonObject& humidity = jsonBuffer2.createObject();
  humidity["name"] = "DHT11-h";
  humidity["unit"] = "%";

  StaticJsonBuffer<100> jsonBuffer3;
  JsonObject& light = jsonBuffer2.createObject();
  light["name"] = "BH1750";
  light["unit"] = "Lux";

  sensors.add(temperature);
  sensors.add(humidity);
  sensors.add(light);
  
  char newMessage[200];
  root.printTo(newMessage, sizeof(newMessage));// Chuyen tu json sang String de pub
  Serial.println(newMessage);
  client.publish(topic_new,newMessage);
}

void displayData(){
    display.clear();
    float temperature = getTemperature();
    float humidity = getHumidity();
    display.drawString(0, 0, (String)temperature);
    display.drawString(0, 40, (String)humidity);
    display.display();
}

void sendMove(){
  Serial.println("sendMove");
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["macAddr"] = WiFi.macAddress();
  root["type"] = "motion";
  char dataMessage[200];
  root.printTo(dataMessage, sizeof(dataMessage));// Chuyen tu json sang String de pub
  client.publish(topic_pub,dataMessage);
}

void sendData(){
  StaticJsonBuffer<500> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["macAddr"] = WiFi.macAddress();
  root["type"] = "data";
  JsonArray& sensorsData = root.createNestedArray("sensorsData");
  
  StaticJsonBuffer<100> jsonBuffer1;
  JsonObject& temperature = jsonBuffer1.createObject();
  temperature["name"] = "DHT11-t";
//  if(isnan(getTemperature())){
//    temperature["value"]="nan";
//  }else{
//    temperature["value"] = getTemperature();
//  }
  temperature["value"] = random(200, 500) / 10.0;
  temperature["unit"] = "C";

  StaticJsonBuffer<100> jsonBuffer2;
  JsonObject& humidity = jsonBuffer2.createObject();
  humidity["name"] = "DHT11-h";
//  if(isnan(getHumidity())){
//    humidity["value"]="nan";
//  }else{
//    humidity["value"] = getHumidity();
//  }
  humidity["value"] = random(200, 500) / 10.0;
  humidity["unit"] = "%";

  StaticJsonBuffer<100> jsonBuffer3;
  JsonObject& light = jsonBuffer3.createObject();
  light["name"] = "BH1750";
  light["value"] = random(200, 500) / 10.0;
  
  light["unit"] = "Lux";

  sensorsData.add(temperature);
  sensorsData.add(humidity);
  sensorsData.add(light);

  char dataMessage[500];
  root.printTo(dataMessage, sizeof(dataMessage));// Chuyen tu json sang String de pub
  client.publish(topic_pub,dataMessage);
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

String mac2String(byte ar[])
{
  String s;
  for (byte i = 0; i < 6; ++i)
  {
    char buf[3];
//    sprintf(buf, "%2X", cliMacAddr[i]);
    s += buf;
    if (i < 5) s += ':';
  }
  return s;
}

