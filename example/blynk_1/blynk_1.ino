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

