#include "SH1106.h"
SH1106  display(0x3c, 4, 5);
#include <ESP8266WiFi.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

//DHT Settings
#define DHTPIN 16
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);


int  led1=2;
int led2=10;



// Wi-Fi Settings
const char* ssid = "NTT_TNN_1"; // your wireless network name (SSID)
const char* password = "thao0983451175"; // your Wi-Fi network password

//MQTT Settings
const char* default_mqtt_server="192.168.0.11";
const char* default_mqtt_port="1883";
const char* topic_pub="icse/sensor";
const char* topic_sub="icse/action";


float temp,humi;
char data[80];
char temperatureCString[10];
char humiString[10];
unsigned long timeLastCheck = 0;
unsigned long intervalCheck = 5000;

StaticJsonBuffer<300> JSONbuffer;
JsonObject& JSONencoder = JSONbuffer.createObject();

WiFiClient espClient;

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());

  Serial.println(myWiFiManager->getConfigPortalSSID());
}

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
  if(strcmp(topic,topic_sub)==0){
     payload[length] = '\0';
     char inData[80];
     Serial.print("payload: ");
     char payload_string[100];
     strncpy(payload_string, (char*)payload,sizeof(payload_string));
     Serial.print(payload_string);
     Serial.print("\n");
     StaticJsonBuffer<200> JSONbuffer_1;
    
      JsonObject& payload_json = JSONbuffer_1.parseObject(payload_string);
      String led1_status=payload_json["led1"];
      String led2_status=payload_json["led2"];
      
     if(led2_status == "ON"){
       digitalWrite(led2,HIGH);
     }else{
      digitalWrite(led2,LOW);
     }
     
     if(led1_status == "ON"){
       digitalWrite(led1,HIGH);
     }else{
      digitalWrite(led1,LOW);
     }
     
     
     Serial.println();
  }
  
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
      client.subscribe(topic_sub);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(led1,OUTPUT);
  pinMode(led2,OUTPUT);
  dht.begin();
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_24);
  Serial.printf("Connecting to %s \n", ssid);
//  WiFi.begin(ssid,password);
  
//  while(WiFi.status() != WL_CONNECTED){
//    delay(500);
//    Serial.print(".");
//    
//  }
  
    WiFiManager wifiManager;
    wifiManager.setAPCallback(configModeCallback);

    WiFiManagerParameter custom_mqtt_server("server", "mqtt server", default_mqtt_server, 40);
    WiFiManagerParameter custom_mqtt_port("port", "mqtt port", default_mqtt_port, 6);
    WiFiManagerParameter custom_text("Ahihi");
    wifiManager.addParameter(&custom_mqtt_server);
    wifiManager.addParameter(&custom_mqtt_port);
    wifiManager.addParameter(&custom_text);
        if (!wifiManager.autoConnect("THAO_ESP")) {
      Serial.println("failed to connect, we should reset as see if it connects");
      delay(3000);
      ESP.reset();
      delay(5000);
    }
    Serial.println("connected...yeey");
    
  
  Serial.print("\n CONNECTED SUCCESS\n ");
  Serial.println(WiFi.localIP());
  client.setServer(default_mqtt_server, 1883);
  client.setCallback(callback);
}


void send_data(){
  
  temp=dht.readTemperature();
  humi=dht.readHumidity();
  
  if(isnan(temp) || isnan(humi)){
    Serial.println("Failed to read from DHT sensor!");
    
  }else{
    JSONencoder["tem"] = temp;
    JSONencoder["humi"] = humi;
    char JSONmessageBuffer[100];
    JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
    Serial.print(JSONmessageBuffer);
    Serial.printf("\n");
    client.publish("icse/sensor",JSONmessageBuffer);
    
    return;
  }
  return;
  
}

void loop() {
    if (!client.connected()) {
      reconnect();
    }
    unsigned long timeNow = millis();
    if ( timeLastCheck == 0 || timeNow-timeLastCheck > intervalCheck ){ 
      timeLastCheck=timeNow;
      display.clear();
      dtostrf(temp, 2, 2, temperatureCString);
      dtostrf(humi, 2, 2, humiString);
      display.drawString(0, 0, "T  = "+ String(temperatureCString));
      display.drawString(0, 30,"RH ="  + String(humiString));
      display.display();
      send_data();
      client.loop();
    }
  
  
//  delay(1000);
  
  
}

