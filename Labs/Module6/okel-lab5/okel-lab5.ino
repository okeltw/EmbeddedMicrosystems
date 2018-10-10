#include <Arduino.h>
// Controls the LED states
//  ~ Contains PUMP/VENT_PIN macros
//  ~ contains "handler" pointer
#include "StateMachine.h"
#include "RTCController.h"

// comment out for non-debug run
// Currently just places the state machine at 50s to save some time verifying the pattern
#define DEBUG

// Length of long press, in seconds
#define LONG_PRESS 2

// Serial to uint
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

void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println("Begin Revision 0.0.4");
  
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(VENT_PIN, OUTPUT);
  pinMode(DIAG_LED, OUTPUT);
  pinMode(DIAG_BUT, INPUT);

  rtcInit();
  rtcSetPrescaler(0x7FFF);

#ifdef DEBUG
  rtcSetCount(50);
  //rtcAttachSecondInt(printTime);
#endif

  stateMachineInit();
}

void loop() {
  // Check for diagnostic entry - poll is sufficient, as alarm is a slow interrupt
  uint32_t timestamp = rtcGetCount();

  // While the button is held down
  while(digitalRead(DIAG_BUT) == HIGH)
  {
    // check if it is held down for 5 seconds
    if (rtcGetCount() >= timestamp+LONG_PRESS) 
    {
      if(!diagnosticMode)
      {
        digitalWrite(DIAG_LED, HIGH);
        enterDiagnostics();
        break;
      }
      else
      {
        digitalWrite(DIAG_LED, LOW);
        exitDiagnostics();
        break;
      }
    }
  }

  // if in diagnostic mode, allow for time updates
  if (diagnosticMode)
  {
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
    if(newTime) rtcSetCount(newTime);
  }

}

#ifdef DEBUG
void printTime()
{
  noInterrupts();
  Serial.println(rtcGetCount());
  interrupts();
}
#endif
