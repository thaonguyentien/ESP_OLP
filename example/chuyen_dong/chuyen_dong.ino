#include "SSD1306.h"

int InputPin = 2;   //using digital pin10 as input
int ledPin = 16;
bool Pin_Status = LOW;
bool PIR_State = LOW; //LOW = no motion, HIGH = motion

SSD1306  display(0x3c, 4, 5);

void setup()
{
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);
  display.drawString(15, 20, "IotMaker.VN");
  display.display();
  delay(1000);

  pinMode(InputPin, INPUT);  //input declaration
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
}
void loop()
{
  Pin_Status = digitalRead(InputPin);
  if (Pin_Status == HIGH)
  {
    if (PIR_State == LOW)
    {
      display.clear();
      display.setFont(ArialMT_Plain_16);
      display.drawString(30, 20, "Motion");
      display.display();
      digitalWrite(ledPin, LOW);
      PIR_State = HIGH;
    }
  }
  else
  {
    if (PIR_State == HIGH)
    {
      display.clear();
      display.setFont(ArialMT_Plain_16);
      display.drawString(20, 20, "Not Motion");
      display.display();
      digitalWrite(ledPin, HIGH);
      PIR_State = LOW;
    }
  }
}
