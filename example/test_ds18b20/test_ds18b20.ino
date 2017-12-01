#include <OneWire.h>
#include <DallasTemperature.h>

OneWire ds(2); // Chân cắm output trên cảm biến có thể là 1 trong các chân sau: 00, 02, 04, 05,  10, 12, 13, 14
DallasTemperature DS18B20(&ds);

void setup(){
  Serial.begin(115200);
  DS18B20.begin();
  pinMode(2, INPUT_PULLUP);
}

//Hàm đọc nhiệt độ
float getTemperature(){
  float temperature;
  do{
    DS18B20.requestTemperatures();
    temperature = DS18B20.getTempCByIndex(0);
    delay(200);
    return temperature;
  }while(temperature == 85.0);
}

void loop(){
  float temp=getTemperature(); // gọi hàm để lấy nhiệt đố gán vào biến temp
  Serial.println(temp);
}
