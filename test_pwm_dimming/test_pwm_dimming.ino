void setup() {
  // put your setup code here, to run once:
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  for (float i=255; i>1; i=i/1.0001) {
    analogWrite(9, i);
    analogWrite(10, i);
    analogWrite(11, i);
    //delay(50);
  }
  for (float i=1; i<255; i=i*1.0001) {
    analogWrite(9, int(i));
    analogWrite(10, int(i));
    analogWrite(11, int(i));
    //delay(50);
  }
}
