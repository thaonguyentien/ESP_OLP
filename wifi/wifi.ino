#include <ESP8266WiFi.h>

const char* ssid = "NTT_TNN_1";
const char* password = "thao0983451175";

IPAddress staticIP(192,168,1,22);
IPAddress gateway(192,168,1,9);
IPAddress subnet(255,255,255,0);

void setup(void)
{
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.printf("Connecting to %s \n", ssid);
  WiFi.begin(ssid, password);
  WiFi.config(staticIP, gateway, subnet);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
  Serial.printf("Connected, mac address: %s\n", WiFi.macAddress().c_str());
  Serial.print("Subnet mask: ");
  Serial.println(WiFi.subnetMask());
  Serial.printf("Gataway IP: %s\n", WiFi.gatewayIP().toString().c_str());
  Serial.print("DNS #1, #2 IP: ");  
  WiFi.dnsIP().printTo(Serial);
  Serial.print(", ");
  WiFi.dnsIP(1).printTo(Serial);
  Serial.println();
  Serial.printf("Default hostname: %s\n", WiFi.hostname().c_str());
  WiFi.hostname("HPCC_arduino_1");
  Serial.printf("New hostname: %s\n", WiFi.hostname().c_str());
  Serial.printf("Cuong Do Tin Hieu:");
  Serial.printf("RSSI: %d dBm\n", WiFi.RSSI());

}

void loop() {}

