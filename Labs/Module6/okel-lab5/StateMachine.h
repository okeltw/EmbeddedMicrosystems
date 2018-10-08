#include <libmaple/libmaple.h>

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
  nextState = ventOn;
}
void ventOn()
{
  digitalWrite(VENT_PIN, HIGH);
  nextState = pumpOff;
}
void pumpOff()
{
  digitalWrite(PUMP_PIN, LOW);
  nextState = ventOff;
}
void ventOff()
{
  digitalWrite(VENT_PIN, LOW);
  nextState = pumpOn;
}

void stateMachineInit()
{
  pumpOff(); // Turn off Pump
  ventOff(); // Turn off Vent; set handler to pumpOn
}
