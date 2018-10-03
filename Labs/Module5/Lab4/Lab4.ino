#include <RTClock.h>

#include <TimeLib.h>

// Use https://www.epochconverter.com/ to help set time
// Current time since epoch: 1538522542 (outdated...obviously)

RTClock rtc;

uint32 tt;

#define INT_PIN 0
#define OUTCLK_PIN 1

typedef struct Time_t {
  uint32 day;
  uint32 month;
  uint32 year;
  uint32 hour;
  uint32 minute;
  uint32 second;
} Time_t;

Time_t Time; 


void convertTime(uint32 tt)
{
  Time.year = year(tt);
  Time.month = month(tt);
  Time.day = day(tt);
  Time.hour = hour(tt);
  Time.minute = minute(tt);
  Time.second = second(tt);
}

uint8_t smToUint(char data)
{
  switch (data)
  {
    case '0':
      return 0;
    case '1':
      return 1;
    case '2':
      return 2;
    case '3':
      return 3;
    case '4':
      return 4;
    case '5':
      return 5;
    case '6':
      return 6;
    case '7':
      return 7;
    case '8':
      return 8;
    case '9':
      return 9;
    default:
      return -1;  
  }
}

void startClk()
{
  noInterrupts();
  bool clkSignal = false;
  for(int cnt=0; cnt<1024;cnt++)
  {
    if (cnt % 32 == 0) clkSignal = !clkSignal;
    digitalWrite(OUTCLK_PIN, clkSignal);
  }
  interrupts();
}

void printTime()
{
  tt = rtc.now();
  convertTime(tt);
  Serial.print(Time.month);
  Serial.print('/');
  Serial.print(Time.day);  
  Serial.print('/');
  Serial.print(Time.year);
  Serial.print(' ');
  Serial.print(Time.hour);
  Serial.print(':');
  Serial.print(Time.minute);
  Serial.print(':');
  Serial.println(Time.second);
}

void setup() {
  pinMode(INT_PIN, INPUT);
  pinMode(OUTCLK_PIN, OUTPUT);
  
  digitalWrite(OUTCLK_PIN, LOW);
  
  Serial.begin(9600);
  delay(1000);

  Serial.println("Set time by sending serial packet with number of seconds since epoch");

  rtc.attachSecondsInterrupt(printTime);
  attachInterrupt(digitalPinToInterrupt(INT_PIN), startClk, FALLING);

  rtc.setTime(now());
}

void loop() {
  char data = 0;
  uint32 newTime = 0;
  if(Serial.available() > 0) {
      data = Serial.read();
      Serial.print("GOT: ");Serial.println(data, DEC);
      while(data != 10)
      {
        newTime = newTime*10 + smToUint(data);
        data = Serial.read();
      }
  }

  if(newTime) rtc_set_count(newTime);
  
}
