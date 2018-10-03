/*
   Real Time Clock Controller
     Routines to contol the RTC on the STM32F103 chip
     Tested on the Olimexino development board

*/



#ifndef _RTC_CTRL_H
#define _RTC_CTRL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/*** Register Macros ***/
// First line in each section is the offset from base, the remaining are mask definitions
// BASE is the base address from which these registers are offset
// Nomenclature:
//      rw  - read/write
//      r   - read only
//      rc_w0 - read, clear by writing 0 (1 has no effect)
//      rc_w1 - read, clear by writing 1 (0 has no effect)
#define BASE  0x40002800
#define END   0x40002BFF

// Ctrl High - Interrupt Masking
#define RTC_INT     (int16_t*)(BASE+0x00)
#define OWIE        0x4     // Overflow Interrupt Enable
#define ALRIE       0x2     // Alarm Interrupt Enable
#define SECIE       0x1     // Second Interrupt Enable
#define INT_RST     0x0000  // Reset value

// Ctrl Low - Status bits
// Any flag remains pending until the appropriate RTC_CR request bit is reset by software
#define RTC_CTRL    (int16_t*)(BASE+0x04)
#define RTOFF       0x20    // (r) Status of last wrtie op
#define CNF         0x10    // (rw) Configuration flag - set to write registers
#define RSF         0x8     // (rc_w0) Registers Synchronized
#define OWF         0x4     // (rc_w0) Overflow Flag
#define ALRF        0x2     // (rc_w0) Alarm Flag
#define SECF        0x1     // (rc_w0) Second Flag
#define CTRL_RST    0x0020

// RTC prescaler load - write only
// Used to define the counter clock frequency:
//      f_TR_CLK = f_RTCCLK/(PRL[19:0]+1)
// Zero value not recommended.
// If f_RTCCLK=32.768 KHz, write 0x7FFF for 1 second signal period
#define RTC_PRLH    (int16_t*)(BASE+0x08) // High word, bits 15:4 RESERVED, forced to 0.
#define RTC_PRLL    (int16_t*)(BASE+0x0C)
#define PRL_SECOND  0x7FFF      // Prescalar value to get 1 second period @ clk 32.768KHz
#define PRL_RST     0x0000

// RTC prescalar divider - read only
#define RTC_DIVH    (int16_t*)(BASE+0x10) // bits 15:4 reserved
#define RTC_DIVL    (int16_t*)(BASE+0x14)
#define DIV_RST     0x0000

// RTC Counter Registers
// Read/Write; Write allowed if RTOFF==1 (config mode)
#define RTC_CNTH    (int16_t*)(BASE+0x18)
#define RTC_CNTL    (int16_t*)(BASE+0x1C)
#define CNT_RST     0x0000

// RTC Alarm Registers
// Interrupt when counter equals this value
// Write Only; Write allowed if RTOFF==1 (config mode)
#define RTC_ALRH    (int16_t*)(BASE+0x20)
#define RTC_ALRL    (int16_t*)(BASE+0x24)
#define ALR_RST     0xFFFF

/*** Setup addresses and values ***/
// All the stuff relating to setting up APB1
#define RCC           (int32_t*)0x40021000
#define RCC_APB1ENR   (int32_t*)(RCC + 0x1C)
#define RCC_PWREN     (int32_t)0x10000000 // bit 28
#define RCC_BKPEN     (int32_t)0x08000000 // bit 27

#define RCC_BDCR      (int32_t*)(RCC + 0x20)
#define RCC_RTCEN     (int32_t)0x8000     // bit 15
#define RCC_RTCSEL1   (int32_t)0x200      // bit 9
#define RCC_RTCSEL0   (int32_t)0x100      // bit 8
#define RCC_LSEBYP    (int32_t)0x4        // bit 2
#define RCC_LSERDY    (int32_t)0x2        // bit 1
#define RCC_LSEON     (int32_t)0x1        // bit 0

#define RCC_CSR       (int32_t*)(RCC + 0x24)
#define RCC_LSIRDY    (int32_t)0x2
#define RCC_LSION     (int32_t)0x1

#define PWR           (int32_t*)0x40007000
#define PWR_CR        (int32_t*)(PWR + 0x00)
#define PWR_DBP       (int32_t)0x100

typedef struct RtcConfig_t
{
  int16_t intMask;
  int16_t ctrlMask;
  int32_t prescaleLoad;
  int32_t prescaleDiv;
  int32_t counterVal;
  int32_t alarmVal;
} RtcConfig_t;

// Performs setup - enables APB1 bus and clock sources
//void rtcSetup();

// Enters config mode to write the config to the address
void rtcWriteConfig(int16_t config, int16_t *address); // One config
void rtcWriteConfigs(RtcConfig_t config); // All configs (setup)

RtcConfig_t rtcReadConfig();

#ifdef __cplusplus
}
#endif

#endif //_RTC_CTRL_H
