#include <Arduino.h>
// Controls the LED states
//  ~ Contains PUMP/VENT_PIN macros
//  ~ contains "handler" pointer
#include "StateMachine.h"
#include "RTCController.h"

void setup() {
  Serial.begin(9600);
  
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(VENT_PIN, OUTPUT);

  // Turn 
  stateMachineInit();

  rtcInit();
  rtcSetPrescaler(0x7FFF);
  rtcAttachSecondInt(handler);

  delay(1000);
  Serial.println("Begin Revision 0.0.3.a");
}

void loop() {
}
