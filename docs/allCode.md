# AP wifi

```cpp
#include <ESP8266WiFi.h>

IPAddress local_IP(192,168,4,22);
IPAddress gateway(192,168,4,9);
IPAddress subnet(255,255,255,0);

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.print("Setting soft-AP configuration ... ");
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");

  Serial.print("Setting soft-AP ... ");
  Serial.println(WiFi.softAP("HPCC_ESP_1","hpcc_iot") ? "Ready" : "Failed!");

  Serial.print("Soft-AP IP address = ");
  Serial.println(WiFi.softAPIP());

}

void loop() {}
```

## Auto Connect

```cpp
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

const char* default_mqtt_server = "server.tobernguyen.com";
const char* default_mqtt_port = "1883";
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());

  Serial.println(myWiFiManager->getConfigPortalSSID());
}
void setup() {
    Serial.begin(115200);

 
    WiFiManager wifiManager;
    wifiManager.setAPCallback(configModeCallback);

//    wifiManager.autoConnect("THAO_ESP");// để đây ko add them số vào được
//    wifiManager.setAPCallback(configModeCallback);

    WiFiManagerParameter custom_mqtt_server("server", "mqtt server", default_mqtt_server, 40);
    WiFiManagerParameter custom_mqtt_port("port", "mqtt port", default_mqtt_port, 6);
    WiFiManagerParameter custom_text("Ahihi");
    wifiManager.addParameter(&custom_mqtt_server); 
    wifiManager.addParameter(&custom_mqtt_port);
    wifiManager.addParameter(&custom_text); 

    
//    wifiManager.resetSettings();//  xóa cmt
    if (!wifiManager.autoConnect("THAO_ESP")) {
      Serial.println("failed to connect, we should reset as see if it connects");
      delay(3000);
      ESP.reset();
      delay(5000);
    } 
    Serial.println("connected...yeey :)");
}

void loop() {
    // put your main code here, to run repeatedly:
    
}

```

## Basic OTA

```
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

const char* ssid = "HPCC-IOT";
const char* password = "hpcc_iot_icse";

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  ArduinoOTA.handle();
}
```

##  07_11

```cpp
#include "SH1106.h"
SH1106  display(0x3c, 4, 5);
#include <ESP8266WiFi.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

//DHT Settings
#define DHTPIN 16
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);


byte willQoS = 2;
const char* willTopic = "willTopic";
const char* willMessage = "My Will Message";
boolean willRetain = false;

int led1=2;
int led2=10;



// Wi-Fi Settings
const char* ssid = "HPCC-IOT"; // your wireless network name (SSID)
const char* password = "hpcc_iot_icse"; // your Wi-Fi network password

//MQTT Settings
const char* default_mqtt_server="192.168.0.11";
const char* default_mqtt_port="1883";
const char* topic_pub="icse/sensor";
const char* topic_sub="icse/action";


float temp,humi;
char data[80];
char temperatureCString[10];
char humiString[10];
unsigned long timeLastCheck = 0;
unsigned long intervalCheck = 5000;

StaticJsonBuffer<300> JSONbuffer;
JsonObject& JSONencoder = JSONbuffer.createObject();

WiFiClient espClient;

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());

  Serial.println(myWiFiManager->getConfigPortalSSID());
}

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
  if(strcmp(topic,topic_sub)==0){
     payload[length] = '\0';
     char inData[80];
     Serial.print("payload: ");
     char payload_string[100];
     strncpy(payload_string, (char*)payload,sizeof(payload_string));
     Serial.print(payload_string);
     Serial.print("\n");
     StaticJsonBuffer<200> JSONbuffer_1;
    
      JsonObject& payload_json = JSONbuffer_1.parseObject(payload_string);
      String led1_status=payload_json["led1"];
      String led2_status=payload_json["led2"];
      
     if(led2_status == "ON"){
       digitalWrite(led2,HIGH);
     }else{
      digitalWrite(led2,LOW);
     }
     
     if(led1_status == "ON"){
       digitalWrite(led1,HIGH);
     }else{
      digitalWrite(led1,LOW);
     }
     
     
     Serial.println();
  }
  
}

PubSubClient client(espClient);

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str()),"","",willTopic,willQoS,willRetain,willMessage) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(willTopic, "hello world");
      // ... and resubscribe
      client.subscribe(topic_sub);
      char* message = "Retained";
      int length = strlen(message);
      boolean retained = true;
      client.publish("myTopic",(byte*)message,length,retained);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(led1,OUTPUT);
  pinMode(led2,OUTPUT);
  dht.begin();
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_24);
  Serial.printf("Connecting to %s \n", ssid);
//  WiFi.begin(ssid,password);
  
//  while(WiFi.status() != WL_CONNECTED){
//    delay(500);
//    Serial.print(".");
//    
//  }
  
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

  Serial.print("\n CONNECTED SUCCESS\n ");
  Serial.println(WiFi.localIP());
  client.setServer(default_mqtt_server, 1883);
  client.setCallback(callback);
}


void send_data(){
  
  temp=dht.readTemperature();
  humi=dht.readHumidity();
  
  if(isnan(temp) || isnan(humi)){
    Serial.println("Failed to read from DHT sensor!");
    client.publish("icse/sensor","Failed");
  }else{
    JSONencoder["tem"] = temp;
    JSONencoder["humi"] = humi;
    char JSONmessageBuffer[100];
    JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
    Serial.print(JSONmessageBuffer);
    Serial.printf("\n");
    client.publish("icse/sensor",JSONmessageBuffer);
    
    return;
  }
  return;
  
}

void loop() {
    if (!client.connected()) {
      reconnect();
    }
    unsigned long timeNow = millis();
    if ( timeLastCheck == 0 || timeNow-timeLastCheck > intervalCheck ){ 
      timeLastCheck=timeNow;
      display.clear();
      dtostrf(temp, 2, 2, temperatureCString);
      dtostrf(humi, 2, 2, humiString);
      display.drawString(0, 0, "T  = "+ String(temperatureCString));
      display.drawString(0, 30,"RH ="  + String(humiString));
      display.display();
      send_data();
      
    }
   client.loop();
  
//  delay(1000);
  
  
}

```

## Anh sang

```cpp
int val = 0;         // variable to store the read value
void setup() {
  Serial.begin(115200);


}

void loop() {
  val = analogRead(A0);   // variable to read the value input
  Serial.print(val);


}

```

## AP Wifi

```cpp
#include <ESP8266WiFi.h>

IPAddress local_IP(192,168,4,22);
IPAddress gateway(192,168,4,9);
IPAddress subnet(255,255,255,0);

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.print("Setting soft-AP configuration ... ");
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");

  Serial.print("Setting soft-AP ... ");
  Serial.println(WiFi.softAP("HPCC_ESP_1","hpcc_iot") ? "Ready" : "Failed!");

  Serial.print("Soft-AP IP address = ");
  Serial.println(WiFi.softAPIP());

  
}

void loop() {}

```

## Blynk

```cpp
#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char pin10=D10;
// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "92ce2ccc1d7641588abe4d80d59f645e";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "NTT_TNN_1";
char pass[] = "thao0983451175";

int status;
void setup()
{ 
  // Debug console
  Serial.begin(9600);
  pinMode(pin10,INPUT);
  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 8442);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8442);
}

void loop()
{
  Blynk.run();
  status=digitalRead(pin10);
  Serial.print("status");
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
}

```

## Chuyen dong 


```cpp
int movePin = 02;   //using digital pin10 as input
bool Pin_Status = LOW;
bool PIR_State = LOW; //LOW = no motion, HIGH = motion

void setup()
{
  pinMode(InputPin, INPUT);  //input declaration
  Serial.begin(115200);
}
void loop()
{
  movePin = digitalRead(InputPin);
  Serial.println(Pin_Status);
  delay(2000);
}

```

## DeepSleep

```cpp
/**
 * An example showing how to put ESP8266 into Deep-sleep mode
 */
 
void setup() {
  Serial.begin(115200);
  Serial.setTimeout(2000);

  // Wait for serial to initialize.
//  while(!Serial) { }
  
  Serial.println("I'm awake.");

  Serial.println("Going into deep sleep for 20 seconds");
  ESP.deepSleep(20e6); // 20e6 is 20 microseconds
}

void loop() {
}

```

## DHT11

```cpp
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#include "SH1106.h"

//SH1106 display(0x3c,4,5);

//Config mqttt
const char* default_mqtt_server="192.168.1.100";
const char* default_mqtt_port="1883";
const char* topic="icse/sensor";
char msg[50];

char mqtt_server[255];
char mqtt_port[6];


//Config DHT
#define DHTPIN 16

#define DHTTYPE DHT11

DHT dht(DHTPIN,DHTTYPE);

// Wifi setting
const char* ssid="HPCC-IOT";
const char* password="hpcc_iot";
             
unsigned long preTimer = 0;
const long interval = 5000;
float temp,humi;
char data[80];

StaticJsonBuffer<300> JSONbuffer;
JsonObject& JSONencoder = JSONbuffer.createObject();

WiFiClient espClient;

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
  char inData[80];

  
 Serial.print("payload: ");
 for(int i =26; i<length; i++){
  // Serial.print((char)payload[i]);
   inData[(i - 26)] = (char)payload[i];
 }
 Serial.println();
 
// JsonObject& root = jsonBuffer.parseObject(inData);  


 // Pull the value from key "VALUE" from the JSON message {"value": 1 , "someOtherValue" : 200}
// int val = root["value"];

// DO SOMETHING WITH THE DATA THAT CAME IN!
// Serial.println(val); 
  

}

PubSubClient client(espClient);


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
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
  Serial.begin(115200);
  client.setServer(default_mqtt_server, 1883);
  dht.begin();
  Serial.print("Connectting");
//  WiFi.begin(ssid,password);
//  while(WiFi.status() != WL_CONNECTED){
//    Serial.print(".");
//    delay(100);
//  }
  Serial.print("\n CONNECTED SUCCESS\n ");
  
}



void send_data(){
  int a6=analogRead(04);
  int d6=digitalRead(04);
  Serial.print("a6: ");
  Serial.print(a6);
  Serial.print(" d6: ");
  Serial.print(d6);
  Serial.print("\n");
  temp=dht.readTemperature();
  humi=dht.readHumidity();
  
  if(isnan(temp) || isnan(humi)){
    Serial.println("Failed to read from DHT sensor!");
    String payload = "{ \"temperature\":"+String(temp) +",\"humidity\":" + String(humi) + " }";
    payload.toCharArray(data, (payload.length() + 1));
    JSONencoder["temperature"] = temp;
    JSONencoder["humidity"] = humi;
    char JSONmessageBuffer[100];
    JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
    Serial.print(JSONmessageBuffer);
    Serial.printf("\n");
    client.publish("icse/sensor",JSONmessageBuffer);
    client.loop();
    return;
  }else{
    String payload = "{ \"temperature\":"+String(temp) +",\"humidity\":" + String(humi) + " }";
    payload.toCharArray(data, (payload.length() + 1));
    JSONencoder["temperature"] = temp;
    JSONencoder["humidity"] = humi;
    char JSONmessageBuffer[100];
    JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
    Serial.print(JSONmessageBuffer);
    Serial.printf("\n");
    client.publish("icse/sensor",JSONmessageBuffer);
    client.loop();
  }
  return;
}


void loop(){
  delay(2000);
  
//  if (!client.connected()) {
//    reconnect();
//  }
  send_data();
//  display.clear();
//  display.drawString(0,0,"T= "+String(temp));
//  display.display();
  
}

```

## Display

```c
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
  display.drawString(0, 0, "display");
  display.display();
  delay(1000);                  // waits for a second
     
}

```

## DS18B20

```
 #include "SH1106.h"
#include <OneWire.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DallasTemperature.h>

SH1106  display(0x3c, 4, 5);

OneWire ds(2);
DallasTemperature DS18B20(&ds);

//Config mqttt
const char* default_mqtt_server="192.168.1.11";
const int default_mqtt_port= 1883;
const char* topic_pub="icse/sensor";
const char* topic_sub="icse/led";
char temperatureCString[10];
float temp;
String led_status="OFF";
int led=3;
// Wifi setting
const char* ssid="HPCC-IOT";
const char* password="hpcc_iot_icse";

StaticJsonBuffer<300> JSONbuffer;
JsonObject& JSONencoder = JSONbuffer.createObject();

WiFiClient espClient;

void callback(char* topic, byte* payload, unsigned int length) {
  if(strcmp(topic,topic_sub)==0){
    payload[length] = '\0';
      Serial.print((char*)payload);
     char inData[80];
     Serial.print("payload: ");
     char payload_string[100];
     strncpy(payload_string, (char*)payload,sizeof(payload_string));
     Serial.print(payload_string);
     Serial.print("\n");
     StaticJsonBuffer<200> JSONbuffer_1;
    
      JsonObject& payload_json = JSONbuffer_1.parseObject(payload_string);
      String status1 = payload_string
      Serial.print("\n");
    
     Serial.print(status1);
     pinMode(3,OUTPUT);
    //  Serial.print(led_status);
      if(status1 == "ON") {
        batLed();
      }else{
        tatLed();
        Serial.print("Off");
      }
     Serial.println();
  }
 
 
}

PubSubClient client(espClient);

void tatLed(){
   digitalWrite(led,LOW);
}

void batLed(){
     digitalWrite(led,HIGH);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.publish("outTopic", "hello world");
      client.subscribe(topic_sub);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}



float getTemperature(){
  float temperature;
  do{
    DS18B20.requestTemperatures();
    temperature = DS18B20.getTempCByIndex(0);
    delay(200);
    return temperature;
  }while(temperature == 85.0);
}

void setup(){
  Serial.begin(115200);
  client.setServer(default_mqtt_server, default_mqtt_port);
    Serial.print("Connectting");
  WiFi.begin(ssid,password);
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(100);
  }
  Serial.print("\n CONNECTED SUCCESS\n ");
  
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_24);
  DS18B20.begin();
  pinMode(2, INPUT_PULLUP);
  client.setCallback(callback);
}


void send_data(){
  
  temp=getTemperature();
  
  if(isnan(temp)){
    Serial.println("Failed to read from DHT sensor!");
    return;
  }else{

    JSONencoder["temperature"] = temp;
    char JSONmessageBuffer[100];
    JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
//    Serial.print(JSONmessageBuffer);
//    Serial.printf("\n");
    client.publish(topic_pub,JSONmessageBuffer);
  }
  return;
}

void display_oled(){
  display.clear();
//  Serial.print(temp);
//  Serial.print("\n");
  dtostrf(temp, 2, 2, temperatureCString);
  Serial.print("\n");
  display.drawString(0, 0, temperatureCString);
  display.display();
}
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  
  delay(1000);
  send_data();
  display_oled();
  client.loop();
  
}

```

## Exam1
```

#include "SH1106.h"
SH1106  display(0x3c, 4, 5);
#include <ESP8266WiFi.h>  
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

#include <Wire.h>
#include <BH1750.h>

BH1750 lightMeter;
const char* default_mqtt_server = "192.168.1.100";
const char* default_mqtt_port = "1883";

int ledStatus=0;
int LedDo=12;
int LedXanh=13;
int LedVang=15;
float temp,humi;
boolean motion;
uint16_t light;
unsigned long previousMillis = 0; 
const long interval = 1000; 

WiFiClient espClient;

PubSubClient client(espClient);


char dataMessage[500];
int InputPin = 2;   //using digital pin10 as input
#define DHTPIN 4     // what digital pin we're connected to
int ledPin = 16;
const char* icseData = "icse/data";
const char* icseNew = "icse/newDevice"; 
int isRegister=false;
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);

bool Pin_Status = LOW;
bool PIR_State = LOW; //LOW = no motion, HIGH = motion

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
  
  if(strcmp(topic,"icse/60:01:94:34:AC:E3/action")==0){
      payload[length] = '\0';
      char payload_string[200];
      strncpy(payload_string, (char*)payload,sizeof(payload_string));
      Serial.println(payload_string);
      StaticJsonBuffer<200> jsonBuffer;
      JsonObject& root = jsonBuffer.parseObject(payload_string);
      String type=root["type"];
      Serial.println(type);
      if(strcmp(root["type"],"register")==0){
        Serial.println("register111");
        if(strcmp(root["type"],"OK")){
          isRegister=true;
        }
      }else {
        if(strcmp(root["greenLed"],"ON")==0){
          digitalWrite(LedXanh,HIGH);
          Serial.println("greenLed ON");
        }else{
          digitalWrite(LedXanh,LOW);
        }
          Serial.print("checkMove");

        if(strcmp(root["redLed"],"ON")==0){
          digitalWrite(LedDo,HIGH);
          Serial.println("redLed ON");
        }else{
          digitalWrite(LedDo,LOW);
        }

        if(strcmp(root["yellowLed"],"ON")==0){
          digitalWrite(LedVang,HIGH);
          Serial.println("yellowLed ON");
        }else{
          digitalWrite(LedVang,LOW);
        }
      }

  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      char* message = "Retained";
      int length = strlen(message);
      boolean retained = true;
      client.publish("icse/60:01:94:34:AC:E3/action","1");
      client.subscribe("icse/60:01:94:34:AC:E3/action");
      client.subscribe("icse/test");
      client.publish("testTopic",(byte*)message,length,retained);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
    Serial.begin(115200);
    pinMode(LedDo,OUTPUT);
    pinMode(LedXanh,OUTPUT);
    pinMode(LedVang,OUTPUT);
    dht.begin();
    Wire.begin();
    display.init();
    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_24);
    lightMeter.begin();
    WiFiManager wifiManager;
    WiFiManagerParameter custom_mqtt_server("server", "mqtt server", default_mqtt_server, 40);
    WiFiManagerParameter custom_mqtt_port("port", "mqtt port", default_mqtt_port, 6);
    wifiManager.addParameter(&custom_mqtt_server); 
    wifiManager.addParameter(&custom_mqtt_port);
    Serial.println(WiFi.macAddress());
    pinMode(InputPin, INPUT);  //input declaration
    pinMode(ledPin, OUTPUT);
//    wifiManager.resetSettings();//  xóa cmt 
    if (!wifiManager.autoConnect("THAO_ESP")) {
      Serial.println("failed to connect, we should reset as see if it connects");
      delay(3000);
      ESP.reset();
      delay(5000);
    } 
    Serial.println("connected...yeey :)");
    client.setServer(default_mqtt_server, 1883);
    client.setCallback(callback);
}


void loop() {
   if (!client.connected()) {
      reconnect();
      
  }
  client.loop();
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    sendData();
    sendRegister();
  }

}

void sendRegister(){client.loop();

  if(isRegister==false){
      StaticJsonBuffer<500> jsonBuffer;

      JsonObject& jsonNew = jsonBuffer.createObject();
      jsonNew["macAddr"]=WiFi.macAddress();
      jsonNew["type"]= "ESP8266";
      JsonArray& sensors = jsonNew.createNestedArray("sensors");
      
      StaticJsonBuffer<100> jsonDeviceBuffer;
      JsonObject& device1 = jsonDeviceBuffer.createObject();
      device1["name"]="DHT11-t";
      device1["unit"]="C";
      sensors.add(device1);
    
      StaticJsonBuffer<100> jsonTempBuffer;
      JsonObject& jsonTemp = jsonTempBuffer.createObject();
      jsonTemp["unit"]="%";
      jsonTemp["name"]="DHT11-h";
      sensors.add(jsonTemp);
    
      StaticJsonBuffer<100> jsonHumiBuffer;
      JsonObject& jsonHumi = jsonHumiBuffer.createObject();
      jsonHumi["unit"]="";
      jsonHumi["name"]="HC-SR501";
      sensors.add(jsonHumi);
    
      StaticJsonBuffer<100> jsonLightBuffer;
      JsonObject& jsonLight = jsonLightBuffer.createObject();
      jsonLight["unit"]="lux";
      jsonLight["name"]="BH1750";
      sensors.add(jsonLight);
      
      jsonNew.printTo(dataMessage, sizeof(dataMessage));
    //  Serial.println(dataMessage);
      client.publish(icseNew, dataMessage);
      
//      Serial.println(" Register");
//      Serial.println(icseNew);
//      Serial.println(dataMessage);
  }
}
void sendData(){
    
  StaticJsonBuffer<500> jsonBuffer;

  JsonObject& jsonData = jsonBuffer.createObject();
  jsonData["macAddr"]=WiFi.macAddress();
  JsonArray& sensorsData = jsonData.createNestedArray("sensorsData");
  
  StaticJsonBuffer<100> jsonMotionBuffer;
  JsonObject& jsonMotion = jsonMotionBuffer.createObject();
  jsonMotion["value"]=checkMove();
  jsonMotion["name"]="HC-SR501_01";
  sensorsData.add(jsonMotion);

  StaticJsonBuffer<100> jsonTempBuffer;
  JsonObject& jsonTemp = jsonTempBuffer.createObject();
  if(isnan(getTemperature())){
    jsonTemp["value"]="nan";
  }else{
    jsonTemp["value"]=getTemperature();
  }
  
  jsonTemp["name"]="DHT11-t";
  sensorsData.add(jsonTemp);

  StaticJsonBuffer<100> jsonHumiBuffer;
  JsonObject& jsonHumi = jsonHumiBuffer.createObject();
  if(isnan(getHumidity())){
    jsonHumi["value"]="nan";
  }else{
    jsonHumi["value"]=getHumidity();
  }
  jsonHumi["name"]="DHT11-h";
  sensorsData.add(jsonHumi);

  StaticJsonBuffer<100> jsonLightBuffer;
  JsonObject& jsonLight = jsonLightBuffer.createObject();
  jsonLight["value"]=getLight();
  jsonLight["name"]="BH1750_01";
  sensorsData.add(jsonLight);
  
  jsonData.printTo(dataMessage, sizeof(dataMessage));
  
  client.publish(icseData, dataMessage);
//  
//  Serial.println(icseData);
//  Serial.println(dataMessage);
}

boolean checkMove(){
  return digitalRead(InputPin);
}

float getLight(){
  return lightMeter.readLightLevel() ;
}

float getTemperature(){
//  Serial.println(dht.readTemperature());
  return dht.readTemperature();
}

float getHumidity(){
  return dht.readHumidity();
}

```
## MAC

```cpp
#include <ESP8266WiFi.h>
 
void setup(){
 
   Serial.begin(115200);
   delay(500);
 
   Serial.println();
   Serial.print("MAC: ");
   Serial.println(WiFi.macAddress());
 
}
 
void loop(){}

```

## MQTT

```
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "SH1106.h"
SH1106  display(0x3c, 4, 5);
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
OneWire  ds(2);  // on pin D4 (a 4.7K resistor is necessary)
DallasTemperature DS18B20(&ds);
// bạn thay đổi thông tin bên dưới cho phù hợp
const char* ssid =         "HPCC_IOT";
const char* password =    "hpcc_iot";
const char* mqtt_server = "192.168.1.100";   /// lấy ip bằng lệnh ifconfig, ví dụ 192.168.1.105
const uint16_t mqtt_port = 1883;
const byte ledPin = D0;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;


//get nhiệt độ từ cảm biến
float getTemperature() {
  float tempC;
  float tempF;
  do {
    DS18B20.requestTemperatures();
    tempC = DS18B20.getTempCByIndex(0);
    return tempC;
    delay(100);
  } while (tempC == 85.0 || tempC == (-127.0));
}

void setup() {
  pinMode(ledPin, OUTPUT);     // Khởi tạo LED
  Serial.begin(115200);
  setup_wifi();
  DS18B20.begin();
  pinMode(2, INPUT_PULLUP);
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_24);
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void setup_wifi() {

  delay(10);
  // Kết nối wifi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Bật LED nếu nhận được lệnh là 1
  if ((char)payload[0] == '1') {
     Serial.println("ON");
    digitalWrite(ledPin, LOW);   // LED có sẵn trên NodeMCU bật ở mức 0(LOW)
  } 
  // Tắt LED nếu nhận được lệnh là 0
 if ((char)payload[0] == '0') {
    Serial.println("OFF");
    digitalWrite(ledPin, HIGH);  // LED tắt ở mức 1(HIGH)
  }

}

void reconnect() {
  // Đợi tới khi kết nối
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Khi kết nối thành công sẽ gửi chuỗi helloworld lên topic event
      client.publish("Nhiet Do", "hello world");
      // ... sau đó sub lại thông tin
      client.subscribe("Nhiet Do1");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  
  client.loop();
 // Thực hiện 2s gửi dữ liệu helloworld lên broker 1 lần
  long now = millis();
  float temp=getTemperature();
  String tempInt= String(temp);
  char str_temp[6];
  dtostrf(temp, 4, 2, str_temp);
  display.clear();
  display.drawString(0, 0,"T= " +String(temp));
  display.display();
  Serial.println(tempInt);
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf (msg, 75, "nhiet do:# %s C", str_temp);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("Nhiet Do", msg);
  }
}

```

## Mua

```cpp
#include <Wire.h>

// IoT Wifi Uno-----Cam bien mua
//      ADC---------------A0
//      3.3V--------------VCC
//      GND---------------GND

int nRainIn = 0;          // khai bao chan ADC
int nRainVal;             // bien de doc gia tri ADC
const int timeInterval=1000;
int previousMillis=0;
void setup() {
  Serial.begin(115200);
  pinMode(2, INPUT_PULLUP);
}

void loop()
{
  currentMillis = millis();
  if(currentMillis-previousMillis>=timeInterval){
    previousMillis=currentMillis;
    nRainVal = analogRead(nRainIn);
    Serial.print("\t Moisture Level: ");
    Serial.println(nRainVal);
    
    if(nRainVal < 320)
    {
      Serial.println(":STOP");
    }
    else
    {
      Serial.println( ":Xuong");
    }
  }
  
 

}

```

## Nhiet do do am 2 led 

```
#include <ESP8266WiFi.h>
#include <DHT.h>
#include "SH1106.h"
SH1106  display(0x3c, 4, 5);

//Wifi Settings
const char* ssid = "NTT_TNN_1";
const char* password = "thao0983451175";

//DHT Settings
#define DHTPIN 16
#define DHTTYPE DHT22 
DHT dht(DHTPIN, DHTTYPE);

float temp,humi;

unsigned long timeLastCheck = 0;
unsigned long intervalCheck = 1000;

char temperatureString[10];

void setup(void)
{
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  dht.begin();
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_24);
  Serial.printf("Connecting to %s \n", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected!");

}

void loop() {
    unsigned long timeNow = millis();
    if ( timeLastCheck == 0 || timeNow-timeLastCheck > intervalCheck ){
      timeLastCheck=timeNow;
      temp=dht.readTemperature();
      humi=dht.readHumidity(); 
      Serial.println(humi);
      display.clear();
      dtostrf(temp, 2, 2, temperatureString);
      display.drawString(0, 0, temperatureString);
      display.display();
      
    }
}

```

## Scan

```cpp
#include <ESP8266WiFi.h>

void setup()
{

  Serial.begin(115200);
  int a = WiFi.scanNetworks();
  Serial.printf(" Number of networks: %d \n",a);
  for(int i=0;i<a;++i){
    Serial.printf("SSID: %s\n", WiFi.SSID(i).c_str());
  }
 
}

void loop()
{
}

```

## Servo

```cpp
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

```