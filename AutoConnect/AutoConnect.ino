#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

const char* default_mqtt_server = "server.tobernguyen.com";
const char* default_mqtt_port = "1883";
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());

  Serial.println(myWiFiManager->getConfigPortalSSID());
}
void setup() {
    Serial.begin(115200);

 
    WiFiManager wifiManager;
    wifiManager.setAPCallback(configModeCallback);

//    wifiManager.autoConnect("THAO_ESP");// để đây ko add them số vào được
//    wifiManager.setAPCallback(configModeCallback);

    WiFiManagerParameter custom_mqtt_server("server", "mqtt server", default_mqtt_server, 40);
    WiFiManagerParameter custom_mqtt_port("port", "mqtt port", default_mqtt_port, 6);
    WiFiManagerParameter custom_text("Ahihi");
    wifiManager.addParameter(&custom_mqtt_server); 
    wifiManager.addParameter(&custom_mqtt_port);
    wifiManager.addParameter(&custom_text); 

    
//    wifiManager.resetSettings();//  xóa cmt
    if (!wifiManager.autoConnect("THAO_ESP")) {
      Serial.println("failed to connect, we should reset as see if it connects");
      delay(3000);
      ESP.reset();
      delay(5000);
    } 
    Serial.println("connected...yeey :)");
}

void loop() {
    // put your main code here, to run repeatedly:
    
}
