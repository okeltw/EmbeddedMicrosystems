#include <libmaple/libmaple.h>
#include "RTCController.h"

#define PUMP_PIN 0
#define VENT_PIN 1

voidFuncPtr nextState;

// Call the current state's handler
void handler() { nextState(); }

// Prototypes, defined below
void pumpOn();
void ventOn();
void pumpOff();
void ventOff();

void stateMachineInit();

// LED and state transitions
void pumpOn()
{
  digitalWrite(PUMP_PIN, HIGH);
  uint32_t t = rtcGetCount();
  rtcSetAlarm(t+5);
  nextState = ventOn;
}
void ventOn()
{
  digitalWrite(VENT_PIN, HIGH);
  uint32_t t = rtcGetCount();
  rtcSetAlarm(t+5);
  nextState = pumpOff;
}
void pumpOff()
{
  digitalWrite(PUMP_PIN, LOW);
  uint32_t t = rtcGetCount();
  rtcSetAlarm(t+5);
  nextState = ventOff;
}
void ventOff()
{
  digitalWrite(VENT_PIN, LOW);
  uint32_t currentTime = rtcGetCount();
  uint32_t seconds = currentTime % 60;
  uint32_t alarmTime = (currentTime-seconds) + 60;
  rtcSetAlarm(alarmTime);
  nextState = pumpOn;
}

void stateMachineInit()
{
  digitalWrite(PUMP_PIN, LOW);
  digitalWrite(VENT_PIN, LOW);
  nextState = pumpOn; 
}
