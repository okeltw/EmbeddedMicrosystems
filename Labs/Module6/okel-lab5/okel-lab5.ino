#include <Arduino.h>
// Controls the LED states
//  ~ Contains PUMP/VENT_PIN macros
//  ~ contains "handler" pointer
#include "StateMachine.h"
#include "RTCController.h"

// comment out for non-debug run
// Currently just places the state machine at 50s to save some time verifying the pattern
#define DEBUG

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
    if (rtcGetCount() >= timestamp+5) 
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

}

#ifdef DEBUG
void printTime()
{
  noInterrupts();
  Serial.println(rtcGetCount());
  interrupts();
}
#endif
