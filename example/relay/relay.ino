//Luu y ko cam relay trong luc nap code, chi cam sau khi nap xong

int pin=2;
void setup() {
  // put your setup code here, to run once:
  pinMode(pin,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(2000);
  digitalWrite(pin,HIGH);
  delay(2000);
  digitalWrite(pin,LOW);
}
