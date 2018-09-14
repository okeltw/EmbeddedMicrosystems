
int bytesalloc = 0;

void setup() {
  // put your setup code here, to run once:
    kernelopts(bytesalloc);
    Serial.setup(9600);
    delay(1000);
    Serial.print(bytesalloc);
}

void loop() {
  // put your main code here, to run repeatedly:
    
}
