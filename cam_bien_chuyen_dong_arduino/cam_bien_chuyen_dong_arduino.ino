#include <SoftwareSerial.h>
#include <ArduinoJson.h>
SoftwareSerial mySerial(10, 11); // RX, TX

int ledXanh = 3;        // chọn chân 13 báo hiệu LED
int ledDo = 4; 
int ledVang = 5; 

int inputPin = 2;       // chọn ngõ tín hiệu vào cho cảm biến chuyển động
int val = 0;

void setup(){
  pinMode(ledXanh, OUTPUT);
  pinMode(ledDo, OUTPUT);
  pinMode(ledVang , OUTPUT);
  pinMode(inputPin, INPUT);
  Serial.begin(115200);
  mySerial.begin(115200);
}


StaticJsonBuffer<200> jsonBuffer;
JsonObject& json_buffer = jsonBuffer.createObject();
char buffer_motion[200];

void loop(){
  val = digitalRead(inputPin);    // đọc giá trị đầu vào.
  
  if(mySerial.available() > 0){
      StaticJsonBuffer<200> jsonBufferLed;
      String ledString = mySerial.readString();
      JsonObject& ledJson = jsonBufferLed.parseObject(ledString);
      Serial.println("thao");
      if(strcmp(ledJson["xanh"],"ON")==0){
        digitalWrite(ledXanh,HIGH);
        Serial.println("Xanh On");
      }else{
        digitalWrite(ledXanh,LOW);
        Serial.println("Xanh OFF");
      }
      if(strcmp(ledJson["do"],"ON")==0){
        digitalWrite(ledDo,HIGH);
      }else{
        digitalWrite(ledDo,LOW);
      }
      if(strcmp(ledJson["vang"],"ON")==0){
        digitalWrite(ledVang,HIGH);
      }else{
        digitalWrite(ledVang,LOW);
      }
  }
  json_buffer["montion"] = val;  
  json_buffer.printTo(buffer_motion, sizeof(buffer_motion));
  Serial.println(buffer_motion);
  //client.publish(mqtt_topic_pub, buffer_motion);
  mySerial.write(val);
  delay(3000);


}



