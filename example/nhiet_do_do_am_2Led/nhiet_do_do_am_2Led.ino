#include <ESP8266WiFi.h>
#include <DHT.h>
#include "SH1106.h"
SH1106  display(0x3c, 4, 5);

//Wifi Settings
const char* ssid = "NTT_TNN_1";
const char* password = "thao0983451175";

//DHT Settings
#define DHTPIN 16
#define DHTTYPE DHT22 
DHT dht(DHTPIN, DHTTYPE);

float temp,humi;

unsigned long timeLastCheck = 0;
unsigned long intervalCheck = 1000;

char temperatureString[10];

void setup(void)
{
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  dht.begin();
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_24);
  Serial.printf("Connecting to %s \n", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected!");

}

void loop() {
    unsigned long timeNow = millis();
    if ( timeLastCheck == 0 || timeNow-timeLastCheck > intervalCheck ){
      timeLastCheck=timeNow;
      temp=dht.readTemperature();
      humi=dht.readHumidity(); 
      Serial.println(humi);
      display.clear();
      dtostrf(temp, 2, 2, temperatureString);
      display.drawString(0, 0, temperatureString);
      display.display();
      
    }
}

