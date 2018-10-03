#include "RTCController.h"

void setup() {
  Serial.begin(9600);

  delay(1000);

  Serial.println("OK");

  rtcSetup();

  Serial.println("RTC Set Up");
  
//  // put your setup code here, to run once:
//  RtcConfig_t conf = {
//    .intMask = SECIE, // Interrupt on seconds
//    .ctrlMask = CTRL_RST, // Default
//    .prescaleLoad = PRL_SECOND, // 1 second
//    .prescaleDiv = DIV_RST, // Don't Care
//    .counterVal = CNT_RST, // Don't Care
//    .alarmVal = ALR_RST // Don't Care
//  };

  int16_t intConf = *RTC_INT | SECIE;

  Serial.println(intConf, HEX);
  Serial.println((long)RTC_INT, HEX);

  rtcWriteConfig(intConf, RTC_INT);

  Serial.println("GO");
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(*RTC_CTRL, HEX);
  delay(100);
}

// Configure the RTC registers
void rtcSetup()
{ 
  Serial.println(*RCC_CSR, HEX);
  *RCC_APB1ENR  |= (RCC_BKPEN | RCC_PWREN);
  *PWR_CR       |= PWR_DBP;
  *RCC_BDCR     |= RCC_LSEON;

  Serial.println(*RCC_BDCR, HEX);
  // Poll for LSERDY
  while((*RCC_BDCR & RCC_LSERDY) != RCC_LSERDY ){;} // noop
  
  Serial.println('c');
  *RCC_BDCR &= ~RCC_RTCSEL1;  //1  LSI Select
  *RCC_BDCR |=  RCC_RTCSEL0;  //0
  *RCC_BDCR |=  RCC_RTCEN;

  Serial.println('d');
  // Poll for Clock Synchonization
  while(( *RTC_CTRL & RSF ) != RSF) {;}
}
