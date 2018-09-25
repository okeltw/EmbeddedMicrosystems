#define BAUD 9600
#define A_PIN 2
#define B_PIN 4


void setup() {
  // put your setup code here, to run once:
  Serial.begin(BAUD);

  delay(500);
  Serial.println("OK");

  pinMode(A_PIN, INPUT_PULLUP);
  pinMode(B_PIN, INPUT_PULLUP);

  
}

void loop() {
  Serial.print(digitalRead(A_PIN));
  Serial.println(digitalRead(B_PIN));

  delay(100);

}
