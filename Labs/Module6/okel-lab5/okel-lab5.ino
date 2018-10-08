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
  
  uint32_t currentTime = rtcGetCount();
  uint32_t seconds = currentTime % 60;
  uint32_t alarmTime = (currentTime-seconds) + 60;

  delay(1000);
  Serial.println("Current Time:");
  Serial.println(currentTime);
  Serial.println("Alarm:");
  Serial.println(alarmTime);

  rtcSetAlarm(alarmTime);

  rtcAttachAlarmInt(ding);
  rtcAttachSecondInt(_printTime);
    
  Serial.println("Begin Revision 0.0.3.a");
}

void loop() {
}

void ding()
{
  noInterrupts();
  Serial.println("DING");
  interrupts();
}

void _printTime()
{
  noInterrupts();
  Serial.println(rtcGetCount());
  interrupts();
}

void printTime(uint32_t t)
{
  uint32_t seconds = t % 60;
  t /= 60;
  uint32_t minutes = t % 60;

  Serial.print(minutes); Serial.print(':');
  Serial.println(seconds);
}
