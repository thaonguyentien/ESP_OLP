# Docs các thư viện sử dụng để đọc và điều khiển cảm biến

## 1. DHT11(Cảm biến nhiệt độ và độ ẩm)

Giá trị trả về dạng số thực.

```sh

#include <DHT.h>

#define DHTPIN 16   // Sử dụng pin 16 để đọc dữ liệu từ cảm biến
#define DHTTYPE DHT11 // Loại cảm biến ở đây là DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup(){
    dht.begin();// Trong hàm setup() để khởi tạo.
}

temp=dht.readTemperature(); // Đọc nhiệt độ từ cảm biến
humi=dht.readHumidity();  // Đọc độ ẩm từ cảm biến
```

### Note: Chú ý lúc install library chọn bản 1.2.3 để cài đặt ko sẽ bị lỗi `Adafruit_Sensor.h: No such file or directory`


## 2. DS18B20(Cảm biến nhiệt độ)

```cpp

#include <OneWire.h>
#include <DallasTemperature.h>

OneWire ds(2);
DallasTemperature DS18B20(&ds);

void setup(){
    DS18B20.begin();
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

float   temp=getTemperature(); // gọi hàm để lấy nhiệt đố gán vào biến temp

```

## 3. ArduinoJson.h

### 3.1 Decoding/parser

Dùng để nhận một string có dạng giống json sau đó decode/parser thành json. Thường dùng khi nhận dữ liệu đến sau đó chuyển về json để tách các trường dữ liệu.

VD:

```sh
char json[] = "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";

StaticJsonBuffer<200> jsonBuffer;

JsonObject& root = jsonBuffer.parseObject(json);

const char* sensor = root["sensor"];
long time          = root["time"];
double latitude    = root["data"][0];
double longitude   = root["data"][1];
```

### 3.2 Encoding

```sh
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

(Download bằng tay repo về sau đó cho vào cùng thư mục với file ino.)

```cpp
#include "SH1106.h"

SH1106  display(0x3c, 4, 5);

void setup(){
    display.init();
    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_24);
}

void loop(){
    display.clear();
    splay.drawString(0, 0, temperatureCString);
    display.display();
}
```

### Note: Chú ý copy các file thư viện vào folder mã nguồn.

## 5. Cảm biến ánh sáng

Sử dụng digitalRead để đọc dữ liệu trả về trên con cảm biến ánh sáng. Giá trị trả về sẽ là 0 hoặc 1.

## 6. Kết nối wifi

```cpp
#include <ESP8266WiFi.h>

const char* ssid = "NTT_TNN_1";
const char* password = "thao0983451175";

void setup(){
    WiFi.begin(ssid, password);
  WiFi.config(staticIP, gateway, subnet);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.print("Connected, IP address: ");
}
```

## 7. PubSubClient

Thư viện dùng để connect tới mqtt server

```cpp
#include <PubSubClient.h>
#include <ESP8266WiFi.h>

//MQTT Settings
const char* default_mqtt_server="10.0.0.221";
const char* default_mqtt_port="1883";
const char* topic_pub="icse/sensor";

WiFiClient espClient; // Khai báo wifi

// Hàm callback được goi để xử lý dữ liệu trả về từ mqtt server
void callback(char* topic, byte* payload, unsigned int length) {
  if(strcmp(topic,topic_sub)==0){
     payload[length] = '\0';  // Cắt bỏ dữ liệu thừa 
     char inData[80];
     char payload_string[100];
     strncpy(payload_string, (char*)payload,sizeof(payload_string)); // chuyển về dàng char  
}
PubSubClient client(espClient); // Gắn PubSub vào wifi


// Connected to mqtt server
void reconnect() {

  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");

      client.subscribe(topic_sub);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
 void setup(){
     client.setServer(default_mqtt_server, 1883);
     client.setCallback(callback);

}

loop(){
    // Kiem tra viec connect toi mqtt server neu ko chua connect se connect
    if (!client.connected()) {
      reconnect();
    }
    // Publish noi dung theo topic icse/sensor
    client.publish("icse/sensor",JSONmessageBuffer);
}

```

## 8. Smart Wifi

Sử dụng web server để cho phép nhận vào wifi và mqtt server

```cpp
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

const char* default_mqtt_server="10.0.0.221";
const char* default_mqtt_port="1883";

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());

  Serial.println(myWiFiManager->getConfigPortalSSID());
}

    WiFiManager wifiManager;
    wifiManager.setAPCallback(configModeCallback);

    WiFiManagerParameter custom_mqtt_server("server", "mqtt server", default_mqtt_server, 40);
    WiFiManagerParameter custom_mqtt_port("port", "mqtt port", default_mqtt_port, 6);
    WiFiManagerParameter custom_text("Ahihi");
    wifiManager.addParameter(&custom_mqtt_server);
    wifiManager.addParameter(&custom_mqtt_port);
    wifiManager.addParameter(&custom_text);
        if (!wifiManager.autoConnect("THAO_ESP")) {
      Serial.println("failed to connect, we should reset as see if it connects");
      delay(3000);
      ESP.reset();
      delay(5000);
    }
    Serial.println("connected...yeey");
    
```

## 9. Phát Wifi

Sử dụng để phát wifi

```cpp
IPAddress local_IP(192,168,4,22);
IPAddress gateway(192,168,4,9);
IPAddress subnet(255,255,255,0);

Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");

Serial.print("Setting soft-AP ... ");
Serial.println(WiFi.softAP("HPCC_ESP_1","hpcc_iot") ? "Ready" : "Failed!");

Serial.print("Soft-AP IP address = ");
Serial.println(WiFi.softAPIP());

```