#include <Servo.h>

#define SERVO_PIN 2  // 02, 00, 04, 05, 10, 12, 13, 14, 15 các chân 12- 15 không ổn định động cơ bị giật khi quay

Servo myservo1;
int pos = 0;
void setup() {
  myservo1.attach(SERVO_PIN);
  Serial.begin(115200);

}
void loop() {
  for ( pos = 0; pos <= 180; pos += 5) {
    myservo1.write(pos);
    Serial.println("Pos= ");
    Serial.print(pos);
    delay(100);
  }
  for ( pos = 180; pos >= 0; pos -= 5) {
    myservo1.write(pos);
    Serial.println("Pos= ");
    Serial.print(pos);
    delay(100);
  }
  delay(500);
}
