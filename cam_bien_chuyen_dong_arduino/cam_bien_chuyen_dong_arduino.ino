#include <SoftwareSerial.h>
#include <ArduinoJson.h>
SoftwareSerial mySerial(10, 11); // RX, TX

int ledPin = 13;        // chọn chân 13 báo hiệu LED
int inputPin = 2;       // chọn ngõ tín hiệu vào cho cảm biến chuyển động
int val = 0;

void setup()
{
pinMode(ledPin, OUTPUT);
pinMode(inputPin, INPUT);
Serial.begin(115200);
mySerial.begin(115200);
}


StaticJsonBuffer<200> jsonBuffer;
JsonObject& json_buffer = jsonBuffer.createObject();
char buffer_motion[200];

void loop()
{
val = digitalRead(inputPin);    // đọc giá trị đầu vào.
//      if (val == HIGH)                // nếu giá trị ở mức cao.(1)
//      {
//        digitalWrite(ledPin, HIGH); // LED On
//        //playTone(300, 160);         // thời gian chuông kêu
//        delay(3000);
//        Serial.println("Motion detected! Send to server!");
//        mySerial.write(val);
//      }
//      else
//      {
//        digitalWrite(ledPin, LOW);
//        //playTone(0, 0);
//        delay(3000);
//        Serial.println("Motion ended!");
//        mySerial.write(val);
//      }
if(mySerial.available() > 0){
    String led = mySerial.readString();
    Serial.println(led);
}
json_buffer["montion"] = val;  
json_buffer.printTo(buffer_motion, sizeof(buffer_motion));
Serial.println(buffer_motion);
//client.publish(mqtt_topic_pub, buffer_motion);
mySerial.write(val);
delay(3000);


}



