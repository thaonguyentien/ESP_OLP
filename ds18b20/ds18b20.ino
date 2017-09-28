 #include "SH1106.h"
#include <OneWire.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DallasTemperature.h>

SH1106  display(0x3c, 4, 5);

OneWire ds(2);
DallasTemperature DS18B20(&ds);

//Config mqttt
const char* default_mqtt_server="192.168.1.11";
const int default_mqtt_port= 1883;
const char* topic_pub="icse/sensor";
const char* topic_sub="icse/led";
char temperatureCString[10];
float temp;
String led_status="OFF";
int led=3;
// Wifi setting
const char* ssid="HPCC-IOT";
const char* password="hpcc_iot_icse";

StaticJsonBuffer<300> JSONbuffer;
JsonObject& JSONencoder = JSONbuffer.createObject();

WiFiClient espClient;

void callback(char* topic, byte* payload, unsigned int length) {
  if(strcmp(topic,topic_sub)==0){
    payload[length] = '\0';
      Serial.print((char*)payload);
     char inData[80];
     Serial.print("payload: ");
     char payload_string[100];
     strncpy(payload_string, (char*)payload,sizeof(payload_string));
     Serial.print(payload_string);
     Serial.print("\n");
     StaticJsonBuffer<200> JSONbuffer_1;
    
      JsonObject& payload_json = JSONbuffer_1.parseObject(payload_string);
      String status1 = payload_string
      Serial.print("\n");
    
     Serial.print(status1);
     pinMode(3,OUTPUT);
    //  Serial.print(led_status);
      if(status1 == "ON") {
        batLed();
      }else{
        tatLed();
        Serial.print("Off");
      }
     Serial.println();
  }
 
 
}

PubSubClient client(espClient);

void tatLed(){
   digitalWrite(led,LOW);
}

void batLed(){
     digitalWrite(led,HIGH);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.publish("outTopic", "hello world");
      client.subscribe(topic_sub);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}



float getTemperature(){
  float temperature;
  do{
    DS18B20.requestTemperatures();
    temperature = DS18B20.getTempCByIndex(0);
    delay(200);
    return temperature;
  }while(temperature == 85.0);
}

void setup(){
  Serial.begin(115200);
  client.setServer(default_mqtt_server, default_mqtt_port);
    Serial.print("Connectting");
  WiFi.begin(ssid,password);
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(100);
  }
  Serial.print("\n CONNECTED SUCCESS\n ");
  
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_24);
  DS18B20.begin();
  pinMode(2, INPUT_PULLUP);
  client.setCallback(callback);
}


void send_data(){
  
  temp=getTemperature();
  
  if(isnan(temp)){
    Serial.println("Failed to read from DHT sensor!");
    return;
  }else{

    JSONencoder["temperature"] = temp;
    char JSONmessageBuffer[100];
    JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
//    Serial.print(JSONmessageBuffer);
//    Serial.printf("\n");
    client.publish(topic_pub,JSONmessageBuffer);
  }
  return;
}

void display_oled(){
  display.clear();
//  Serial.print(temp);
//  Serial.print("\n");
  dtostrf(temp, 2, 2, temperatureCString);
  Serial.print("\n");
  display.drawString(0, 0, temperatureCString);
  display.display();
}
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  
  delay(1000);
  send_data();
  display_oled();
  client.loop();
  
}

