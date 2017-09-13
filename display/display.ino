#include "SH1106.h"
SH1106  display(0x3c, 4, 5);
#include <OneWire.h>
#include <DallasTemperature.h>

int ledPin = 16; 

// OneWire DS18S20, DS18B20, DS1822 Temperature Example

OneWire  ds(2);  // on pin D4 (a 4.7K resistor is necessary)
DallasTemperature DS18B20(&ds);

String temperatureCString;
String temperatureFString;
int i=0;
void getTemperature() {
  float tempC;
  float tempF;
  if(i%2==0)
  {
    digitalWrite(ledPin, HIGH);// sets the LED on
    temperatureCString="Hello World";
    i++;
  }
  else{
    digitalWrite(ledPin, LOW); // sets the LED off 
    temperatureCString="Goodbye";
    i++;
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_24);
  DS18B20.begin();
  pinMode(2, INPUT_PULLUP);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("  Reading \n");
  getTemperature();
  display.clear();
  display.drawString(0, 0, temperatureCString);
  Serial.print(temperatureCString);
  display.display();
  delay(1000);                  // waits for a second
     
}
