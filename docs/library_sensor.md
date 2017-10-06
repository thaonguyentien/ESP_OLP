# Docs các thư viện sử dụng để đọc và điều khiển cảm biến

## 1. DHT11(Cảm biến nhiệt độ và độ ẩm)

Giá trị trả về dạng số thực.
```
#include <DHT.h>

#define DHTPIN 16   // Sử dụng pin 16 để đọc dữ liệu từ cảm biến
#define DHTTYPE DHT11 // Loại cảm biến ở đây là DHT11
DHT dht(DHTPIN, DHTTYPE);

dht.begin();// Trong hàm setup() để khởi tạo.

temp=dht.readTemperature(); // Đọc nhiệt độ từ cảm biến
humi=dht.readHumidity();  // Đọc độ ẩm từ cảm biến
```

## 2. DS18B20(Cảm biến nhiệt độ)

## 3. ArduinoJson.h

### 3.1 Decoding/parser

Dùng để nhận một string có dạng giống json sau đó decode/parser thành json. Thường dùng khi nhận dữ liệu đến sau đó chuyển về json để tách các trường dữ liệu.

VD:

```
char json[] = "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";

StaticJsonBuffer<200> jsonBuffer;

JsonObject& root = jsonBuffer.parseObject(json);

const char* sensor = root["sensor"];
long time          = root["time"];
double latitude    = root["data"][0];
double longitude   = root["data"][1];
```

### 3.2 Encoding

```
Dùng để tạo một json.

StaticJsonBuffer<200> jsonBuffer

JsonObject& root = jsonBuffer.createObject();
root["sensor"] = "gps";
root["time"] = 1351824120;

JsonArray& data = root.createNestedArray("data");
data.add(48.756080);
data.add(2.302038);


root.printTo(Serial);
// This prints:
// {"sensor":"gps","time":1351824120,"data":[48.756080,2.302038]}
```

## 4. OLED

## 5. Cảm biến ánh sáng

Sử dụng digitalRead để đọc dữ liệu trả về trên con cảm biến ánh sáng. Giá trị trả về sẽ là 0 hoặc 1.
