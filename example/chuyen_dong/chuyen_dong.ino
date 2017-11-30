

int InputPin = A0;   //using digital pin10 as input
bool Pin_Status = LOW;
bool PIR_State = LOW; //LOW = no motion, HIGH = motion

void setup()
{
  pinMode(InputPin, INPUT);  //input declaration
  Serial.begin(115200);
}
void loop()
{
  Pin_Status = analogRead(InputPin);
  Serial.println(Pin_Status);
  delay(2000);
}
