#include "RTCController.h"

// Enters config mode to write the config to the address
void rtcWriteConfig(uint16 config, uint16_ptr *address)
{
    
    // Poll RTOFF
    while( (*RTC_CRL & RTOFF) == RTOFF ) { ;/*noop*/ }

    // Set CNF bit to enter config mode
    *RTC_CRL |= CNF;
    
    // Write to the register 
    address = config;
    
    // Set CNF bit to enter config mode
    *RTC_CRL &= ~CNF;
    
    // Poll RTOFF
    while( (*RTC_CRL & RTOFF) == RTOFF ) { ;/*noop*/ }
}

// Enters config mode to write the config to the address
void rtcWriteConfig(RtcConfig_t config)
{
    
    // Poll RTOFF
    while( (*RTC_CRL & RTOFF) == RTOFF ) { ;/*noop*/ }

    // Set CNF bit to enter config mode
    *RTC_CRL |= CNF;
    
    // Write to the registers
    *RTC_INT = config.intMask;
    *RTC_CTRL = config.ctrlMask | CNF; // preserve the CNF bit
    *RTC_PRLL = config.prescaleLoad & 0x0000FFFF; // Mask out the lower bits
    *RTC_PRLH = config.prescaleLoad >> 4; // Shift out the upper bits
    *RTC_DIVL = config.prescaleDiv & 0x0000FFFF; // Mask out the lower bits
    *RTC_DIVH = config.prescaleDiv >> 4; // Shift out the upper bits
    *RTC_CNTL = config.counterVal & 0x0000FFFF; // Mask out the lower bits
    *RTC_CNTH = config.counterVal >> 4; // Shift out the upper bits
    *RTC_ALRL = config.alarmVal & 0x0000FFFF; // Mask out the lower bits
    *RTC_ALRH = config.alarmVal >> 4; // Shift out the upper bits
    
    // Clear the CNF bit to write
    *RTC_CRL &= ~CNF;
    
    // Poll RTOFF
    while( (*RTC_CRL & RTOFF) == RTOFF ) { ;/*noop*/ }
}

RtcConfig_t rtcReadConfig()
{
    RtcConfig_t config;
	
	config.intMask 		= (uint16)*RTC_INT;
	config.ctrlMask 	= (uint16)*RTC_CTRL;
	config.prescaleLoad = (uint32)(*RTC_PRLH << 4) | (uint16)*RTC_PRLL;
	config.prescaleDiv 	= (uint32)(*RTC_DIVH << 4) | (uint16)*RTC_DIVL;
	config.counterVal	= (uint32)(*RTC_CNTH << 4) | (uint16)*RTC_CNTL;
	config.alarmVal		= (uint32)(*RTC_ALRH << 4) | (uint16)*RTC_ALRL;
	
	return config;
}