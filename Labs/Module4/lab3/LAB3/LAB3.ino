/*
 * Lab 3
 * Taylor Okel
 * 9/25/2018
 * 
 * Write a program that 
 *  collects data from the quadrature encoder,
 *  translate the data to the flow rate,
 *  communicates the rate and direction to the host pc, and
 *  displays overflow/backflow errors via LED
 */

// Serial Rate
const int BAUD = 2000000;

// Update Rate to PC
const int UPDATE_RATE = 1000; //milliseconds

// Conversions
const int OZ_P_GAL = 128;

// Pin Definitions
const int A_PIN = 2; // D2 - encoder bit 0
const int B_PIN = 4; // D4 - encoder bit 1
const int OF_LED_PIN = 0; //D0 - overflow led indicator
const int BF_LED_PIN = 14; //D14 - backflow led indicator

// Flow rate conversions
const int NUM_PULSES = 24; // number of positions on the device 
const float OUNCE_PER_PULSE = 0.0416667f; // product dispensed per pulse, hardcoded due to perceived precision errors
                                          // the displayed rate is in *gallons* to one decimal point; the given precision is more than accurate 

// struct that holds the state of the encoder
typedef struct {
  bool a;
  bool b;
} state;

/*** ISR Variables ***/
// These need to be volatile
volatile state currentState;  // Holds the current state; need to remember this for flow direction
volatile int numTicks = 0;    // Number of pulses recieved since last communication with host PC
                              //  Positive is CW flow, Negative is CCW flow
                              //  Used for flow rate calculations
/*********************/

// flow rate calculations
double flowRate = 0;            //oz
const double maxFlowRate = 1.5; //oz (arbitrary)
double totalDispersed =0; // gallons

// timekeeping
unsigned long elapsedTime = 0; //us, time since last update
unsigned long lastUpdated = 0; //us, last time updated

/*** Logic diagrams ***/
// Transition CW (AB):
// 11 -> 01 -> 00 (detent) -> 10 -> 11
// If a changed, clockwise = (b is off, a turned on) OR (b is on, a turned off)
// If b changed, clockwise = (a is off, b turned off) OR (a is on, b turned on)
/**********************/

/*
 * Performs onetime setup; Serial, digital pins, ISR attachment, var init
 */
void setup() {
  // put your setup code here, to run once:
  Serial.begin(BAUD);

  delay(2000);
  
  Serial.println("OK");
  
  delay(1000);

  pinMode(A_PIN, INPUT);
  pinMode(B_PIN, INPUT);

  pinMode(OF_LED_PIN, OUTPUT);
  pinMode(BF_LED_PIN, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(A_PIN), a_change, CHANGE);
  attachInterrupt(digitalPinToInterrupt(B_PIN), b_change, CHANGE);
  
  currentState.a = digitalRead(A_PIN);
  currentState.b = digitalRead(B_PIN);
 
  flowRate = 0;
}

/*
 * Runs continuously, repeats on UPDATE_RATE milliseconds
 * Calculates flowRate base on number of ticks, tick to ounce conversion rate, and elapsed time. 
 * Calculates total gallons dispersed from the flowrate and ounce to gallon conversion rate
 * Emits LEDs if rates are out of spec (overflow, backflow)
 * 
 * The ticks and direction are updated in the ISRs, see below.
 */
void loop() {
  // Collect the time elapsed
  elapsedTime = micros()-lastUpdated;
  lastUpdated = micros();

  // Perform flowrate calculations
  // Refer to ISRs; positive flow rate is CW
  flowRate = ((numTicks*OUNCE_PER_PULSE)/elapsedTime)*1E6; //ounces per second

  // Communicate flow rate, to 6 decimal places
  Serial.print("Flow Rate (oz/second):\t");Serial.println(flowRate,6);

  // accumulate total gallons and display
  totalDispersed += flowRate/OZ_P_GAL; //gallons per second, every second
  Serial.print("Total (gallons):\t");Serial.println(totalDispersed,2);

  // Overflow Indicator
  if(flowRate > maxFlowRate){
    digitalWrite(OF_LED_PIN, HIGH);
  } else {
    digitalWrite(OF_LED_PIN, LOW);
  }

  // Backflow Indicator
  if(flowRate < 0){
     digitalWrite(BF_LED_PIN, HIGH);
  } else {
    digitalWrite(BF_LED_PIN, LOW);
  }

  // Reset ticks for accurate flowrate calculations
  numTicks=0;

  // Wait for next update
  delay(UPDATE_RATE);
}

/*** ISRs ***/
// These are attached to the a/b pins of the encoder (pulled up through 10K resistors)

/*
 * Interrupt on a change; [inc]/[dec]rement the ticks
 *  incrememnt the ticks on CW,
 *  decrement on CCW
 * therefore, CW flowrate is positive.
 */
void a_change(){
  noInterrupts(); // turn off interrupts 
  // get the current state
  currentState.a = digitalRead(A_PIN);

  // the direction is based on the state pattern ~ see logic diagram
  //   A goes high - B must be low for CW
  //   A goes low  - B must be high for CW
  if (( currentState.a && !currentState.b) ||
      (!currentState.a &&  currentState.b ))
  {
    numTicks++;
  } else {
    numTicks--;
  }
  interrupts(); // restore interrupts
}

void b_change(){
  noInterrupts();
  currentState.b = digitalRead(B_PIN);

  // the direction is based on the state pattern ~ see logic diagram
  //   B goes high - A must be high for CW
  //   B goes low  - A must be low for CW
  if ((  currentState.a &&  currentState.b) ||
       (!currentState.a && !currentState.b ))
  {
    numTicks++;
  } else {
    numTicks--;
  }
  interrupts();
}
