#include "RTCController.h"

static rtc_dev rtc = {
  .regs = RTC_BASE,
  .handler = NULL
};
rtc_dev *RTC = &rtc;

void rtcInit()
{
  // bkp setup
  bkp_init(); // enable backup registers for clock
  bkp_enable_writes(); // enable writes to the backup registers

  // RCC setup ~ choose clock
  // Had troube with the clock selection - turning the bits off seems to help?
  RCC_BASE->BDCR &= ~RCC_BDCR_RTCSEL;

  // Use the LSE clock
  rcc_start_lse();
  RCC_BASE->BDCR |= RCC_BDCR_RTCSEL_LSE;

  // Enable RTC
  bb_peri_set_bit(&RCC_BASE->BDCR, RCC_BDCR_RTCEN_BIT, 1);

  // Confirm that the rtc is ready to rock
  rtcWaitSyncAndFinish();
}

void rtcAttachSecondInt(voidFuncPtr handlerFunc)
{
  RTC->handler = handlerFunc;
  rtcEnableSecondInterrupt(handlerFunc);
  nvic_irq_enable(NVIC_RTC); // Set the Nested Vector Interrupt Control to the RTC
}

uint32_t rtcGetCount()
{
  rtcWaitSyncAndFinish();

  // grab and return the counter values
  return (RTC->regs->CNTH << 16) | RTC->regs->CNTL;
}

void rtcSetCount(uint32_t countVal)
{
  rtcWaitSyncAndFinish();

  // Enter config mode
  rtcEnterConfig();

  // Set the registers
  RTC->regs->CNTH = (countVal >> 16);
  RTC->regs->CNTL = countVal;

  // Exit Config Mode
  rtcExitConfig();

  // Wait for the write to finish
  rtcWaitFinished();
}
void rtcSetPrescaler(uint32_t prescaleVal)
{
  rtcWaitSyncAndFinish();

  // Enter config mode
  rtcEnterConfig();

  // Set the registers
  RTC->regs->PRLH = (prescaleVal >> 16);
  RTC->regs->PRLL = prescaleVal;

  // Exit Config Mode
  rtcExitConfig();

  // Wait for the write to finish
  rtcWaitFinished();
}

// dispatch interrupt handler - assume only seconds, as
void __irq_rtc(void)
{
  void (*__handler)(void) = RTC->handler;
  if (__handler)
  {
    __handler();
  }

  RTC->regs->CRL &= ~RTC_CRL_SECF;
}

//#define WAIT_FOR_RTOFF   while( (*RTC_CTRL & RTOFF) != RTOFF ) { ;/*noop*/ }
