#include <ESP8266WiFi.h>

void setup()
{

  Serial.begin(115200);
  int a = WiFi.scanNetworks();
  Serial.printf(" Number of networks: %d \n",a);
  for(int i=0;i<a;++i){
    Serial.printf("SSID: %s\n", WiFi.SSID(i).c_str());
  }
 
}

void loop()
{
}
