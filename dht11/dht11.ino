#include <DHT.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#include "SH1106.h"

//SH1106 display(0x3c,4,5);

//Config mqttt
const char* default_mqtt_server="192.168.1.100";
const char* default_mqtt_port="1883";
const char* topic="icse/sensor";
char msg[50];

char mqtt_server[255];
char mqtt_port[6];


//Config DHT
#define DHTPIN 16

#define DHTTYPE DHT11

DHT dht(DHTPIN,DHTTYPE);

// Wifi setting
const char* ssid="HPCC-IOT";
const char* password="hpcc_iot";
             
unsigned long preTimer = 0;
const long interval = 5000;
float temp,humi;
char data[80];

StaticJsonBuffer<300> JSONbuffer;
JsonObject& JSONencoder = JSONbuffer.createObject();

WiFiClient espClient;

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
  char inData[80];

  
 Serial.print("payload: ");
 for(int i =26; i<length; i++){
  // Serial.print((char)payload[i]);
   inData[(i - 26)] = (char)payload[i];
 }
 Serial.println();
 
// JsonObject& root = jsonBuffer.parseObject(inData);  


 // Pull the value from key "VALUE" from the JSON message {"value": 1 , "someOtherValue" : 200}
// int val = root["value"];

// DO SOMETHING WITH THE DATA THAT CAME IN!
// Serial.println(val); 
  

}

PubSubClient client(espClient);


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void setup(){
  Serial.begin(115200);
  client.setServer(default_mqtt_server, 1883);
  dht.begin();
  Serial.print("Connectting");
//  WiFi.begin(ssid,password);
//  while(WiFi.status() != WL_CONNECTED){
//    Serial.print(".");
//    delay(100);
//  }
  Serial.print("\n CONNECTED SUCCESS\n ");
  
}



void send_data(){
  int a6=analogRead(04);
  int d6=digitalRead(04);
  Serial.print("a6: ");
  Serial.print(a6);
  Serial.print(" d6: ");
  Serial.print(d6);
  Serial.print("\n");
  temp=dht.readTemperature();
  humi=dht.readHumidity();
  
  if(isnan(temp) || isnan(humi)){
    Serial.println("Failed to read from DHT sensor!");
    String payload = "{ \"temperature\":"+String(temp) +",\"humidity\":" + String(humi) + " }";
    payload.toCharArray(data, (payload.length() + 1));
    JSONencoder["temperature"] = temp;
    JSONencoder["humidity"] = humi;
    char JSONmessageBuffer[100];
    JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
    Serial.print(JSONmessageBuffer);
    Serial.printf("\n");
    client.publish("icse/sensor",JSONmessageBuffer);
    client.loop();
    return;
  }else{
    String payload = "{ \"temperature\":"+String(temp) +",\"humidity\":" + String(humi) + " }";
    payload.toCharArray(data, (payload.length() + 1));
    JSONencoder["temperature"] = temp;
    JSONencoder["humidity"] = humi;
    char JSONmessageBuffer[100];
    JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
    Serial.print(JSONmessageBuffer);
    Serial.printf("\n");
    client.publish("icse/sensor",JSONmessageBuffer);
    client.loop();
  }
  return;
}


void loop(){
  delay(2000);
  
//  if (!client.connected()) {
//    reconnect();
//  }
  send_data();
//  display.clear();
//  display.drawString(0,0,"T= "+String(temp));
//  display.display();
  
}

