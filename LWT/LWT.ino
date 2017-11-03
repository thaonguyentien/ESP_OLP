#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiManager.h>

byte willQoS = 2;
const char* willTopic = "willTopic";
const char* willMessage = "My Will Message";
boolean willRetain = false;

// Wi-Fi Settings
const char* ssid = "HPCC-IOT"; // your wireless network name (SSID)
const char* password = "hpcc_iot_icse"; // your Wi-Fi network password

//MQTT Settings
const char* default_mqtt_server="192.168.1.17";
const char* default_mqtt_port="1883";
const char* topic_pub="icse/sensor";
const char* topic_sub="icse/action";


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
    if (client.connect(clientId.c_str()),"","",willTopic,willQoS,willRetain,willMessage) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(willTopic, "hello world");
      // ... and resubscribe
      client.subscribe(topic_sub);
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
  Serial.begin(115200);
  
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


void loop() {
    if (!client.connected()) {
      reconnect();
    }
   client.publish("myTopic","hihi");
   client.loop();
  
  delay(1000);
  
  
}

