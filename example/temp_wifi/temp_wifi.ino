#include "SH1106.h"
SH1106  display(0x3c, 4, 5);
#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
// Wi-Fi Settings
const char* ssid = "NTT_TNN_1"; // your wireless network name (SSID)
const char* password = "thao0983451175"; // your Wi-Fi network password

WiFiClient client;

const int postingInterval = 2 * 1000; // post data every 2 seconds

OneWire  ds(2);  // on pin D4 (a 4.7K resistor is necessary)
DallasTemperature DS18B20(&ds); 
IPAddress server(192, 168, 0, 5); // BBB

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
        Serial.begin(115200);
        Serial.print("Connecting");
        WiFi.begin(ssid, password);
        while (WiFi.status() != WL_CONNECTED) {
                Serial.print(".");
                delay(100);
        }
        Serial.print("\r\nWiFi connected to ");
        Serial.println(ssid);
        display.init();
        display.flipScreenVertically();
        display.setFont(ArialMT_Plain_24);
        DS18B20.begin();
        pinMode(2, INPUT_PULLUP);
}

void loop() {
        // wait and then post again
        delay(postingInterval);

        float temp = getTemperature();
        if (isnan(temp)) {
                Serial.println("Failed to read from DHT sensor!");
                return;
        }
        Serial.print(temp);
        display.clear();
        display.drawString(0, 0,"T= " +String(temp));
        display.display();
        if (client.connect(server, 8086)) {
                // Construct API request body
                String body = " temperature value=" + String(temp, 1);
                Serial.println(body);
                client.print("POST /write?db=temperature1 HTTP/1.1\n");
                client.print("Host: 192.168.1.100\n");
                client.println("User-Agent: Arduino/1.6");
                client.print("Connection: close\n");
                client.print("Content-Type: application/x-www-form-urlencoded\n");
                client.print("Content-Length: ");
                client.print(body.length());
                client.print("\n\n");
                client.print(body);
                client.print("\n\n");
                Serial.printf("Success");
        }else{
          Serial.println("Cannot connected");
        }
        client.stop();
}

