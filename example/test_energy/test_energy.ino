int pin1=2;
void setup(){
  
  pinMode(pin1,INPUT);
}



void loop(){
  float sample1=0;
  float voltage=0;
  for(int i=0;i<150;++i){
  sample1=sample1+analogRead(pin1); //read the voltage from the divider circuit
  delay (2);
  }
  sample1=sample1/150;
  voltage=4.669*2*sample1/1000;
}

