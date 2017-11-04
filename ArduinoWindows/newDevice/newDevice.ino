#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <EEPROM.h>

const char* newDeviceTopic = "newDeviceTopic";
byte value;
//const char* newDevicelMessage = WiFi.macAddress();

int flag = EEPROM.read(0);
// Wi-Fi Settings
const char* ssid = "ICSE"; // your wireless network name (SSID)
const char* password = "hpcc@)!%"; // your Wi-Fi network password

//MQTT Settings
const char* default_mqtt_server="broker.hivemq.com";
const char* default_mqtt_port="1883";


void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
  Serial.print("sub");
  if(strcmp(topic,"newDevice")==0){
     EEPROM.write(0, 1);
     EEPROM.commit();
     Serial.print(flag);
  }
}
WiFiClient espClient;

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
      // ... and resubscribe
      client.subscribe("newDevice");
      char* message = "Retained";
      int length = strlen(message);
      boolean retained = true;
      
      client.publish("myTopic",(byte*)message,length,retained);
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
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.begin(ssid,password);
  EEPROM.begin(512);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  client.setServer(default_mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!client.connected()) {
      reconnect();
  }

  flag = EEPROM.read(0);
  Serial.print(flag);
  if(flag==0){
    client.publish(newDeviceTopic, "123");
    Serial.println("123");
  }
  
  client.loop();
  delay(2000);
}
