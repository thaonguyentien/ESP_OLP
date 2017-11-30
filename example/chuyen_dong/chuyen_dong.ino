
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
