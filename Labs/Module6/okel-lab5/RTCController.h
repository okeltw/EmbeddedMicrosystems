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


// Maple Libraries
#include <libmaple/libmaple.h>  // libmaple_types, stm32, and util headers
#include <libmaple/rcc.h>       // reset and clock control support
#include <libmaple/nvic.h>      // nested vector interrupt support
#include <libmaple/bitband.h>   // bit-banding support
#include <libmaple/pwr.h>       // power control support
#include <libmaple/bkp.h>       // backup register support
#include <libmaple/exti.h>      // external interrupt support
#include <stdint.h>

/*** Register Macros ***/
#define RTC_CRH_OWIE_BIT  2
#define RTC_CRH_ALRIE_BIT 1
#define RTC_CRH_SECIE_BIT 0

#define RTC_CRH_OWIE    BIT(RTC_CRH_OWIE_BIT)
#define RTC_CRH_ALRIE   BIT(RTC_CRH_ALRIE_BIT)
#define RTC_CRH_SECIE   BIT(RTC_CRH_SECIE_BIT)

/* Control register low (CRL) */
#define RTC_CRL_RTOFF_BIT 5
#define RTC_CRL_CNF_BIT   4
#define RTC_CRL_RSF_BIT   3
#define RTC_CRL_OWF_BIT   2
#define RTC_CRL_ALRF_BIT  1
#define RTC_CRL_SECF_BIT  0

// Use util.h BIT to pull out the bit mask
#define RTC_CRL_RTOFF BIT(RTC_CRL_RTOFF_BIT)
#define RTC_CRL_CNF   BIT(RTC_CRL_CNF_BIT)
#define RTC_CRL_RSF   BIT(RTC_CRL_RSF_BIT)
#define RTC_CRL_OWF   BIT(RTC_CRL_OWF_BIT)
#define RTC_CRL_ALRF  BIT(RTC_CRL_ALRF_BIT)
#define RTC_CRL_SECF  BIT(RTC_CRL_SECF_BIT)

#define PRL_SECOND 0x7FFF

// Leverage LibMaple's approach to the register map
// Note: __IO is in util.h, and is basically a volatile macro
typedef struct rtc_reg_map {
  __IO uint32_t CRH;  // Control High
  __IO uint32_t CRL;  // Control Low
  __IO uint32_t PRLH; // Prescaler High
  __IO uint32_t PRLL; // Prescaler Low
  __IO uint32_t DIVH; // Divider High
  __IO uint32_t DIVL; // Divider Low
  __IO uint32_t CNTH; // Count High
  __IO uint32_t CNTL; // Count Low
  __IO uint32_t ALRH; // Alarm High
  __IO uint32_t ALRL; // Alarm Low
} rtc_reg_map;

#define RTC_BASE  ((struct rtc_reg_map*)0x40002800)

// Device contains mem map and interrupt handler pointer
typedef struct rtc_dev {
  rtc_reg_map *regs;
  voidFuncPtr secondHandler;
  voidFuncPtr alarmHandler;
} rtc_dev;

extern rtc_dev *RTC; // set this in the .c file

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

// Prototype for longer (>2 line) functions
// aka any not-inline function...
void rtcInit();
void rtcAttachSecondInt(voidFuncPtr handlerFunc); // voidFuncPtr is a void* type from LibMaple
uint32_t rtcGetCount();
void rtcSetCount(uint32_t countVal);
void rtcSetPrescaler(uint32_t prescaleVal);
void rtcSetAlarm(uint32_t t);
void rtcAttachAlarmInt(voidFuncPtr handlerFunc);
void rtcDetachAlarmInt();

// simple inline functions

/*
   Polls RTOFF until cleared
*/
static inline void rtcWaitFinished()
{
  while (*bb_perip(&(RTC->regs)->CRL, RTC_CRL_RTOFF_BIT) == 0);
}

/*
   Clears the synchronization bit
*/
static inline void rtcClearSync()
{
  rtcWaitFinished();
  *bb_perip(&(RTC->regs)->CRL, RTC_CRL_RSF_BIT) = 0;
}

/*
   Polls for synchronization clear
*/
static inline void rtcWaitSync()
{
  while (*bb_perip(&(RTC->regs)->CRL, RTC_CRL_RSF_BIT) == 0);
}

/*
   Sets the config bit to enter config mode
*/
static inline void rtcEnterConfig()
{
  rtcWaitFinished();
  *bb_perip(&(RTC->regs)->CRL, RTC_CRL_CNF_BIT) = 1;
}

/*
   Clear the config bit to exit config mode
*/
static inline void rtcExitConfig()
{
  rtcWaitFinished();
  *bb_perip(&(RTC->regs)->CRL, RTC_CRL_CNF_BIT) = 0;
}

static inline void rtcEnableSecondInterrupt()
{
  rtcWaitFinished();
  *bb_perip(&(RTC->regs)->CRH, RTC_CRH_SECIE_BIT) = 1;
}

static inline void rtcEnableAlarmInterrupt()
{
  rtcWaitFinished();
  *bb_perip(&(RTC->regs)->CRH, RTC_CRH_ALRIE_BIT) = 1;
}

static inline void rtcDisableAlarmInterrupt()
{
  rtcWaitFinished();
  *bb_perip(&(RTC->regs)->CRL, RTC_CRL_ALRF_BIT) = 0;
  *bb_perip(&(RTC->regs)->CRH, RTC_CRH_ALRIE_BIT) = 0;
}

/*
   Combines the common wait ops needed to
   ensure sync and write finish
*/
static inline void rtcWaitSyncAndFinish()
{
  // Make sure we are still synchronized
  rtcClearSync();
  rtcWaitSync();

  // Wait for any config ops to finish
  rtcWaitFinished();
}

#ifdef __cplusplus
}
#endif

#endif //_RTC_CTRL_H
