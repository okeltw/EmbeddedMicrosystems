const int ledPin = 0;
uint8_t val = 2; // 00000001

void setup() {
  pinMode(ledPin, OUTPUT);

}

void loop() {
  Serial.println(val);
  //digitalWrite(ledPin, HIGH);
  analogWrite(ledPin, val);
  val = val==0 ? 2 : val << 1; 
  
  delay(500);
}
