#include <Wire.h>

// IoT Wifi Uno-----Cam bien mua
//      ADC---------------A0
//      3.3V--------------VCC
//      GND---------------GND

int nRainIn = 0;          // khai bao chan ADC
int nRainVal;             // bien de doc gia tri ADC
const int timeInterval=1000;
int previousMillis=0;
void setup() {
  Serial.begin(115200);
  pinMode(2, INPUT_PULLUP);
}

void loop()
{
  currentMillis = millis();
  if(currentMillis-previousMillis>=timeInterval){
    previousMillis=currentMillis;
    nRainVal = analogRead(nRainIn);
    Serial.print("\t Moisture Level: ");
    Serial.println(nRainVal);
    
    if(nRainVal < 320)
    {
      Serial.println(":STOP");
    }
    else
    {
      Serial.println( ":Xuong");
    }
  }
  
 

}
