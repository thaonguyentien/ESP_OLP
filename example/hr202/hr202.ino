int analogPin = 0; 

void setup()
{
 Serial.begin(115200);          //  setup serial
}

void loop()
{
 Serial.print("Analog: ");
 Serial.println(getValue());
 delay(1000);
}

float getValue() {
  float val = analogRead(analogPin);
 // need some calca to HR% 
 return val;
}
