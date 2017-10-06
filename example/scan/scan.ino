#include <ESP8266WiFi.h>

void setup()
{

  Serial.begin(115200);
  int a = WiFi.scanNetworks();
  Serial.printf(" Number of networks: %d \n",a);
  Serial.printf("SSID: %s\n", WiFi.SSID(0).c_str());
  Serial.printf("SSID: %s\n", WiFi.SSID(1).c_str());
  Serial.println(WiFi.isHidden(0));
  Serial.println(WiFi.isHidden(1));
}

void loop()
{
}
