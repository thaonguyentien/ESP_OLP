int val = 0;         // variable to store the read value
void setup() {
  Serial.begin(115200);


}

void loop() {
  val = analogRead(A0);   // variable to read the value input
  Serial.print(val);


}
