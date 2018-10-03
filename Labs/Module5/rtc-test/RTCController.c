#include "RTCController.h"

#define WAIT_FOR_RTOFF   while( (*RTC_CTRL & RTOFF) != RTOFF ) { ;/*noop*/ }

// Configure the RTC registers
void rtcSetup()
{
  *RCC_APB1ENR  |= (RCC_BKPEN | RCC_PWREN);
  *PWR_CR       |= PWR_DBP;
  *RCC_BDCR     |= RCC_LSEON;

  // Poll for LSERDY
  while((*RCC_BDCR & RCC_LSERDY) != RCC_LSERDY ){;} // noop

  *RCC_BDCR |=  RCC_RTCSEL1;  //1  LSI Select
  *RCC_BDCR &= ~RCC_RTCSEL0;  //0
  *RCC_BDCR |=  RCC_RTCEN;

  // Poll for Clock Synchonization
  while(( *RTC_CTRL & RSF ) != RSF) {;}
}

// Enters config mode to write the config to the address
void rtcWriteConfig(int16_t conf, int16_t *address)
{
    
    // Poll RTOFF
    WAIT_FOR_RTOFF;

    // Set CNF bit to enter conf mode
    *RTC_CTRL |= CNF;
    
    // Write to the register 
    address = conf;
    
    // Set CNF bit to enter conf mode
    *RTC_CTRL &= ~CNF;
    
    // Poll RTOFF
    WAIT_FOR_RTOFF;
}

// Enters conf mode to write the conf to the address
void rtcWriteConfigs(RtcConfig_t conf)
{
    
    // Poll RTOFF
    WAIT_FOR_RTOFF;

    // Set CNF bit to enter conf mode
    *RTC_CTRL |= CNF;
    
    // Write to the registers
    *RTC_INT = conf.intMask;
    *RTC_CTRL = conf.ctrlMask | CNF; // preserve the CNF bit
    *RTC_PRLL = conf.prescaleLoad & 0x0000FFFF; // Mask out the lower bits
    *RTC_PRLH = conf.prescaleLoad >> 4; // Shift out the upper bits
    // This register is read only.
//    *RTC_DIVL = conf.prescaleDiv & 0x0000FFFF; // Mask out the lower bits
//    *RTC_DIVH = conf.prescaleDiv >> 4; // Shift out the upper bits
    *RTC_CNTL = conf.counterVal & 0x0000FFFF; // Mask out the lower bits
    *RTC_CNTH = conf.counterVal >> 4; // Shift out the upper bits
    *RTC_ALRL = conf.alarmVal & 0x0000FFFF; // Mask out the lower bits
    *RTC_ALRH = conf.alarmVal >> 4; // Shift out the upper bits
    
    // Clear the CNF bit to write
    *RTC_CTRL &= ~CNF;
    
    // Poll RTOFF
    WAIT_FOR_RTOFF;
}

RtcConfig_t rtcReadConfig()
{
  RtcConfig_t conf;
	
	conf.intMask 		= (int16_t)(*(int16_t*)RTC_INT);
	conf.ctrlMask 	= (int16_t)(*(int16_t*)RTC_CTRL);
	conf.prescaleLoad = (int32_t)(*(int32_t*)RTC_PRLH << 4) | (int16_t)(*(int16_t*)RTC_PRLL);
	conf.prescaleDiv 	= (int32_t)(*(int32_t*)RTC_DIVH << 4) | (int16_t)(*(int16_t*)RTC_DIVL);
	conf.counterVal	= (int32_t)(*(int32_t*)RTC_CNTH << 4) | (int16_t)(*(int16_t*)RTC_CNTL);
	conf.alarmVal		= (int32_t)(*(int32_t*)RTC_ALRH << 4) | (int16_t)(*(int16_t*)RTC_ALRL);
	
	return conf;
}
