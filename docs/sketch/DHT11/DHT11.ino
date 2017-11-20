#include <DHT.h>

#define DHTPIN 4   // Sử dụng pin 16 để đọc dữ liệu từ cảm biến
#define DHTTYPE DHT11 // Loại cảm biến ở đây là DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup(){
  Serial.begin(115200);
  dht.begin();// Trong hàm setup() để khởi tạo.
}

void loop(){
  float  temp=dht.readTemperature(); // Đọc nhiệt độ từ cảm biến
  float humi=dht.readHumidity();  // Đọc độ ẩm từ cảm biến
  Serial.println(temp);
  Serial.println(humi);
  delay(200);
}

