#include <Servo.h>
#include "SSD1306.h"
#include "Wire.h"
#define SERVO_PIN 4
SSD1306  display(0x3c, 4, 5);
// SH1106 display(0x3c, D3, D5);

Servo myservo1;
int pos = 0;
void setup() {
  myservo1.attach(SERVO_PIN);
  Serial.begin(9600);
  Serial.println();

  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.drawString(35, 15, "servo motor"); //write text in OLED when it's start.
  display.drawString(35, 35, "_IoTmaker.vn_");
  display.display();

}
void loop() {
  display.clear();
  for ( pos = 0; pos <= 180; pos += 5) {
    display.drawString(35, 15, "Rotation:" + String(pos));
    display.drawString(35, 35, "_IoTmaker.vn_");
    display.display();
    myservo1.write(pos);
    Serial.print("Pos= ");
    Serial.print(pos);
    Serial.println();
    delay(100);
    display.clear();
  }
  for ( pos = 180; pos >= 0; pos -= 5) {
    display.drawString(35, 15, "Rotation:" + String(pos));
    display.drawString(35, 35, "_IoTmaker.vn_");
    display.display();
    myservo1.write(pos);
    Serial.print("Pos= ");
    Serial.print(pos);
    Serial.println();
    delay(100);
    display.clear();
  }
  delay(500);
}
