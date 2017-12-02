
int movePin = 02;   //using digital pin10 as input
bool Pin_Status = LOW;

void setup()
{
  pinMode(movePin, INPUT);  //input declaration
  Serial.begin(115200);
}
void loop()
{
  Pin_Status = digitalRead(movePin);
  Serial.println(Pin_Status);
  delay(2000);
}
