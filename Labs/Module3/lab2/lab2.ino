/*
   ToDos
    Flash - how to read/write pages
    DLOGADDR isn't printing
    test
    lab report
*/

#include <cstdint>        //uintptr_t
#include <avr/pgmspace.h>
#include "TestSpeedup.h"  //test-related macros
#include "FlashController.h"

// Decide for speed or granularity
// If debug (printing of addresses) is off, there is no noticeable difference.
// Most of the slowdown is in the print statement...
// FULL_TEST: increment SRAM pointer by 1
// BALANCED_TEST: increment SRAM pointer by 4 (default if not defined)
// FAST_TEST: increament SRAM pointer by 1000
#define FULL_TEST
// Various debug macros, e.g. DLOGADDR
// If not defined (commented), suggested to define FULL_TEST above
//#define _DEBUG

uintptr_t* const sramBegin  = (uintptr_t*)0x20000000;
uintptr_t* const sramEnd    = (uintptr_t*)((uintptr_t)sramBegin + 0x4E20);

const uintptr_t ProgramSize = 0x0400; // based on upload printout

//******* RAM graphic ********
// | .data | .bss | heap | free memory | stack |
//                ^ a    ^ b           ^ c
extern unsigned int __bss_end;    // points to the top of .bss, bottom of heap     (a)
extern unsigned int __heap_start; // points to top of heap, bottom of free memory  (b)
extern void* __brkval;             // points to top of free memory, bottom of stack (c)
// Use these to figure free memory address range

struct memPtrs_t {
  uintptr_t* bot;
  uintptr_t* top;
};

void setup() {

  Serial.begin(9600);
  delay(1000);
  Serial.println("OK");

  if (!checkFlash()) {
    Serial.println("Flash test FAIL");
  } else {
    Serial.println("Flash test PASS");
    Serial.print("Confirmed at least ");Serial.print((uintptr_t)flashEnd-(uintptr_t)flashHalf, DEC); Serial.println("Bytes of Flash memory.");
  }

  if (!checkSRAM()) {
    Serial.println("SRAM test FAIL");
  } else {
    Serial.println("SRAM test PASS");
    Serial.print("Confirmed at least ");Serial.print((uintptr_t)sramEnd-(uintptr_t)sramBegin, DEC); Serial.println("Bytes of SRAM.");
  }

  Serial.println("Done.");
}



bool checkFlash() {

  uint16 pattern      = 0xAAAA;
  uint16 antipattern  = ~pattern;
  bool testPass = true;

  Serial.println("Flash Test: Begin");

  //setupFlash();

  // TODO: replace flashHalf with the actual ptr
  //  This is a good debug step
  for (uint16* ptr = (uint16*)flashHalf; ptr < (uint16*)flashEnd-1; ptr++) {
#ifdef _DEBUG
    Serial.println((uintptr_t)ptr, HEX);
#endif

    if (!writeFlash(pattern, (uint16*)ptr) ||
        (readFlash((uint16*)ptr) != pattern))
    {
      Serial.print("Flash Test FAILED pattern test at address ");
      Serial.println((uintptr_t)ptr, HEX);
      testPass = false;
    }

    if (!writeFlash(antipattern, (uint16*)ptr) ||
        (readFlash((uint16*)ptr) != antipattern))
    {
      Serial.print("Flash Test FAILED antipattern test at address ");
      Serial.println((uintptr_t)ptr, HEX);
      testPass = false;
    }

  }

  return testPass;
}

bool checkSRAM() {
  uint8_t zeroPattern = 0x00000000;
  uint8_t onePattern  = 0xFFFFFFFF;
  uint8_t beefPattern = 0xDEADBEEF;
  bool testPass = true;

  Serial.println("SRAM Test: Begin");

  uintptr_t* ptr;

  // Some variables to restore SRAM
  uintptr_t memBkup;
  uintptr_t memBkup2;

  for (ptr = sramBegin; ptr < sramEnd; INC(ptr)) {
#ifdef _DEBUG
    Serial.println((uintptr_t)ptr, HEX);
#endif

    memBkup = *ptr;
    memBkup2 = *ptr;

    *ptr = onePattern;
    if(*ptr != onePattern){
      Serial.print("SRAM \'ones\' Test FAILED at address ");
      Serial.println((uintptr_t)ptr, HEX);
      testPass = false;
    }
    *ptr = beefPattern;
    if(*ptr != beefPattern){
      Serial.print("SRAM \'ones\' Test FAILED at address ");
      Serial.println((uintptr_t)ptr, HEX);
      testPass = false;
    }
    *ptr = zeroPattern;
    if(*ptr != zeroPattern){
      Serial.print("SRAM \'ones\' Test FAILED at address ");
      Serial.println((uintptr_t)ptr, HEX);
      testPass = false;
    }

    *ptr = memBkup | memBkup2;
  }

  return testPass;
}

size_t printAddr(uintptr_t* addr) {
  return Serial.println((uintptr_t)addr, HEX);
}

void loop() {
  /*noop*/
}
