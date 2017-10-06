#include "SH1106.h"
SH1106  display(0x3c, 4, 5);
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>  
WiFiClient client;

// OneWire DS18S20, DS18B20, DS1822 Temperature Example

OneWire  ds(2);  // on pin D4 (a 4.7K resistor is necessary)
DallasTemperature DS18B20(&ds);
// Wi-Fi Settings
const char* ssid = "HPCC-IOT"; // your wireless network name (SSID)
const char* password = "hpcc_iot"; // your Wi-Fi network password
const int postingInterval = 2 * 1000; // post data every 2 seconds
const char* server = "http://192.168.0.5:8086";
byte host[] = {192, 168,1 ,100};
int port = 8086;
WiFiUDP udp;


float getTemperature() {
  float tempC;
  float tempF;
  do {
    DS18B20.requestTemperatures();
    tempC = DS18B20.getTempCByIndex(0);
    return tempC;
    delay(100);
  } while (tempC == 85.0 || tempC == (-127.0));
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.print("Connecting");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
          Serial.print(".");
          delay(100);
  }
  Serial.println("\r\nWiFi connected");
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_24);
  DS18B20.begin();
  pinMode(2, INPUT_PULLUP);
}

void loop() {
   String line, temperature;

  delay(postingInterval);
  float temp=getTemperature();
//  Serial.print("  Reading \n");
  display.clear();
  display.drawString(0, 0,"T= " +String(temp));
  display.display();
  
//  temperature = String(getTemperature(), 2);
  line = String("temperature value=" + String(temp));
  Serial.println(line);
  // send the packet
  Serial.println("Sending UDP packet...");
  udp.beginPacket(host, port);
  udp.print(line);
  udp.endPacket();
}

