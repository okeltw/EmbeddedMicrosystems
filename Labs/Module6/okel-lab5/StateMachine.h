#include <libmaple/libmaple.h>
#include "RTCController.h"

#define PUMP_PIN 0
#define VENT_PIN 1

#define DIAG_LED 8
#define DIAG_BUT 9

// The length of time to debounce the button
#define DEBOUNCE 200 // ms

voidFuncPtr nextState;
bool diagnosticMode;
static uint32_t last_interrupt_time = -200; // so the first interrupt will always be caught
static uint32_t interrupt_time = 0;


// Call the current state's handler
// debounce button
void handler() { 
  interrupt_time = millis();
  if(interrupt_time - last_interrupt_time > 200)  
  {
    nextState(); 
  }
  last_interrupt_time = interrupt_time;
}

// Prototypes, defined below
void pumpOn();
void ventOn();
void pumpOff();
void ventOff();
void d_pumpOn();
void d_ventOn();
void d_pumpOff();
void d_ventOff();

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
  diagnosticMode = false;

  detachInterrupt(DIAG_BUT);

  uint32_t currentTime = rtcGetCount();
  uint32_t seconds = currentTime % 60;
  uint32_t alarmTime = (currentTime-seconds) + 60;
  rtcSetAlarm(alarmTime);
  rtcAttachAlarmInt(handler);
  
  digitalWrite(PUMP_PIN, LOW);
  digitalWrite(VENT_PIN, LOW);
  nextState = pumpOn; 
}

// Diagnostic functions
void enterDiagnostics()
{
  digitalWrite(PUMP_PIN, LOW);
  digitalWrite(VENT_PIN, LOW);
  
  diagnosticMode = true;

  rtcDetachAlarmInt();
  attachInterrupt(DIAG_BUT, handler, RISING); 
    
  
  nextState = d_pumpOn;
}

void exitDiagnostics()
{
//  diagnosticMode = false; - covered in stateMachineInit
  stateMachineInit();
}

void d_pumpOn()
{
  digitalWrite(PUMP_PIN, HIGH);
  nextState = ventOn;
}
void d_ventOn()
{
  digitalWrite(VENT_PIN, HIGH);
  nextState = pumpOff;
}
void d_pumpOff()
{
  digitalWrite(PUMP_PIN, LOW);
  nextState = ventOff;
}
void d_ventOff()
{
  digitalWrite(VENT_PIN, LOW);
  nextState = pumpOn;
}
