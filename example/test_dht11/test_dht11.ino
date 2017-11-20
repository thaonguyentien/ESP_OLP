#include <DHT.h>
#include <Wire.h>
#define DHTPIN 16     // what digital pin we're connected to
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  dht.begin();
//    Wire.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  float test= dht.readTemperature();
   Serial.println(dht.readTemperature());
   Serial.println(test);
    Serial.println("t");
    delay(2000);
}
