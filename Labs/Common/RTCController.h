/*
 * Real Time Clock Controller
 *   Routines to contol the RTC on the STM32F103 chip
 *   Tested on the Olimexino development board
 *
 */
 
/*** Register Macros ***/
// First line in each section is the offset from base, the remaining are mask definitions
// BASE is the base address from which these registers are offset
// Nomenclature:
//      rw  - read/write
//      r   - read only
//      rc_w0 - read, clear by writing 0 (1 has no effect)
//      rc_w1 - read, clear by writing 1 (0 has no effect)
#define BASE        0x40002800  
#define END         0x40002BFF

// Ctrl High - Interrupt Masking
#define RTC_INT     (BASE+0x00)
#define OWIE        0x4     // Overflow Interrupt Enable
#define ALRIE       0x2     // Alarm Interrupt Enable
#define SECIE       0x1     // Second Interrupt Enable
#define INT_RST     0x0000  // Reset value
 
// Ctrl Low - Status bits
// Any flag remains pending until the appropriate RTC_CR request bit is reset by software
#define RTC_CTRL    (BASE+0x04)
#define RTOFF       0x20    // (r) Status of last wrtie op
#define CNF         0x10    // (rw) Configuration flag - set to write registers
#define RSF         0x8     // (rc_w0) Registers Syncronized
#define OWF         0x4     // (rc_w0) Overflow Flag
#define ALRF        0x2     // (rc_w0) Alarm Flag
#define SECF        0x1     // (rc_w0) Second Flag
#define CTRL_RST    0x0020  //

// RTC prescaler load - write only
// Used to define the counter clock frequency:
//      f_TR_CLK = f_RTCCLK/(PRL[19:0]+1)
// Zero value not recommended.
// If f_RTCCLK=32.768 KHz, write 0x7FFF for 1 second signal period
#define RTC_PRLH    (BASE+0x08) // High word, bits 15:4 RESERVED, forced to 0.
#define RTC_PRLL    (BASE+0x0C)
#define PRL_SECOND  0x7FFF      // Prescalar value to get 1 second period @ clk 32.768KHz
#define PRL_RST     0x0000  

// RTC prescalar divider - read only
#define RTC_DIVH    (BASE+0x10) // bits 15:4 reserved
#define RTC_DIVL    (BASE+0x14)
#define DIV_RST     0x0000

// RTC Counter Registers
// Read/Write; Write allowed if RTOFF==1 (config mode)
#define RTC_CNTH    (BASE+0x18)
#define RTC_CNTL    (BASE+0x1C)
#define CNT_RST     0x0000

// RTC Alarm Registers
// Interrupt when counter equals this value
// Write Only; Write allowed if RTOFF==1 (config mode)
#define RTC_ALRH    (BASE+0x20)
#define RTC_ALRL    (BASE+0x24)
#define ALR_RST     0xFFFF

typedef struct RtcConfig_t {|
    uint16 intMask;
    uint16 ctrlMask;
    uint32 prescaleLoad;
    uint32 presacleDiv;
    uint32 counterVal;
    uint32 alarmVal;
}

// Enters config mode to write the config to the address
void rtcWriteConfig(uint16 config, uint16_ptr address); // One config
void rtcWriteConfig(RtcConfig_t config); // All configs (setup)
