#include "SH1106.h"
SH1106  display(0x3c, 4, 5);



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_24);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("  Display \n");
  display.clear();
//  display.drawString(0, 0, "display");
  display.display();
//  delay(1000);                  // waits for a second
     
}
