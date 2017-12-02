#include <Wire.h>
#include <BH1750.h>

BH1750 lightMeter;
uint16_t light;

void setup(){
  lightMeter.begin();
  Serial.begin(115200);
}

float getLight(){
  return lightMeter.readLightLevel() ;
}

void loop(){
  Serial.print(getLight());
  Serial.println("Lux");
  delay(2000);
}

