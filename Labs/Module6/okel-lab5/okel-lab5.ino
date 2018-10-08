#include <Arduino.h>
// Controls the LED states
//  ~ Contains PUMP/VENT_PIN macros
//  ~ contains "handler" pointer
#include "StateMachine.h"
#include "RTCController.h"

void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println("Begin Revision 0.0.4");
  
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(VENT_PIN, OUTPUT);

  stateMachineInit();

  rtcInit();
  rtcSetPrescaler(0x7FFF);

  rtcSetCount(50);
  
  uint32_t currentTime = rtcGetCount();
  uint32_t seconds = currentTime % 60;
  uint32_t alarmTime = (currentTime-seconds) + 60;

  Serial.print("Current: ");Serial.println(currentTime);
  Serial.print("Alarm: ");Serial.println(alarmTime);

  
  rtcSetAlarm(alarmTime);
  rtcAttachAlarmInt(handler);
  //rtcAttachSecondInt(printTime);
}

void loop() {
}

//void ding()
//{
//  noInterrupts();
//  Serial.println("DING");
//  interrupts();
//}

void printTime()
{
  noInterrupts();
  Serial.println(rtcGetCount());
  interrupts();
}

//void printTime(uint32_t t)
//{
//  uint32_t seconds = t % 60;
//  t /= 60;
//  uint32_t minutes = t % 60;
//
//  Serial.print(minutes); Serial.print(':');
//  Serial.println(seconds);
//}
