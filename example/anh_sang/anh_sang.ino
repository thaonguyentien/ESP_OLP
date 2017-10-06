int cambien = 10;// khai báo chân digital 10 cho cảm biến

int Led = 15;//kháo báo chân digital 8 cho đèn LED

void setup (){

pinMode(Led,OUTPUT);//pinMode xuất tín hiệu đầu ra cho led
pinMode(Led,INPUT);//pinMode xuất tín hiệu đầu ra cho led


pinMode(cambien,INPUT);//pinMode nhận tín hiệu đầu vào cho cảm biê

}

void loop (){

int value = digitalRead(cambien);//lưu giá trị cảm biến vào biến value
int value1 = digitalRead(Led);//lưu giá trị cảm biến vào biến value

Serial.print(value1);

digitalWrite(Led,value);//xuất giá trị ra đèn LED
delay(100);
}
