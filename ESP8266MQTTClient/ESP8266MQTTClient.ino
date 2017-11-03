#include <ESP8266MQTTClient.h>
#include <ESP8266WiFi.h>
MQTTClient mqtt;

// Wi-Fi Settings
const char* ssid = "HPCC-IOT"; // your wireless network name (SSID)
const char* password = "hpcc_iot_icse"; // your Wi-Fi network password


void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //topic, data, data is continuing
  mqtt.onData([](String topic, String data, bool cont) {
    Serial.printf("Data received, topic: %s, data: %s\r\n", topic.c_str(), data.c_str());
    mqtt.unSubscribe("/qos0");
  });

  mqtt.onSubscribe([](int sub_id) {
    Serial.printf("Subscribe topic id: %d ok\r\n", sub_id);
    mqtt.publish("hpcc_iot", "qos0", 0, 0);
  });
  mqtt.onConnect([]() {
    Serial.printf("MQTT: Connected\r\n");
    Serial.printf("Subscribe id: %d\r\n", mqtt.subscribe("hpcc_iot", 0));
//    mqtt.subscribe("/qos1", 1);
//    mqtt.subscribe("/qos2", 2);
  });

//  mqtt.begin("mqtt://test.mosquitto.org:1883");
  mqtt.begin("mqtt://test.mosquitto.org:1883", {.lwtTopic = "thao_lwt", .lwtMsg = "thao111", .lwtQos = 0, .lwtRetain = 0});
//  mqtt.begin("mqtt://user:pass@mosquito.org:1883");
//  mqtt.begin("mqtt://user:pass@mosquito.org:1883#clientId");

}

void loop() {
  delay(1000);
  
  mqtt.handle();
  
}

