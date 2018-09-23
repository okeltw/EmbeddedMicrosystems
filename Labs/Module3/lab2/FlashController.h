/*
   Flash Controller

   API for reading/writing to the STM32F103RBT6 flash memory

   Flash device characteristics can be found in the PM0075 Programming Manual, available off of STM's website.

   Features:
      128 KBytes Flash memory
      ICP and IAP methods - this controller will use IAP (obviously)
      128 x 1KB pages
      FPEC - read/writes managaged by controller
      Main Memory
        Starting address  0x0800 0000
        Ending address    0x0801 FFFF

   Revisions
      9/20    Initial
*/

// *** Addresses ***
// Basic
uintptr_t* const flashBegin = (uintptr_t*)0x08000000;
uintptr_t* const blEnd      = (uintptr_t*)0x08005000;
uintptr_t* const flashHalf  = (uintptr_t*)0x08010000;
uintptr_t* const flashEnd   = (uintptr_t*)0x0801FFFE;

// Memory Interfaces
//   All Adjacent and listed in order...
uintptr_t* const FLASH_ACR  = (uintptr_t*)0x40022000; // Access Control Register
uintptr_t* const FLASH_KEYR = (uintptr_t*)0x40022004; // FPEC key register
uintptr_t* const FLASH_OPTKEYR = (uintptr_t*)0x40022008;  // Option byte key register
uintptr_t* const FLASH_SR   = (uintptr_t*)0x4002200C; // Flash status register
uintptr_t* const FLASH_CR   = (uintptr_t*)0x40022010; // Flash control register
uintptr_t* const FLASH_AR   = (uintptr_t*)0x40022014; // Flash Address register
//uintptr_t* const RESERVED = (uintptr_t*)0x40022018; Reserved, don't use.
uintptr_t* const FLASH_OBR  = (uintptr_t*)0x4002201C; // Option byte register
uintptr_t* const FLASH_WRPR = (uintptr_t*)0x40022020; // Write protection register

/*** Masks for control bits ***/
// Status
#define FLASH_CR_LOCK   (*FLASH_CR & 0x80)
#define FLASH_CR_PER    (0x2)
#define FLASH_CR_STRT   (0x40)
#define FLASH_CR_PG     (0x1)
#define FLASH_CR_EOPIE  (0x1000)
#define FLASH_CR_ERRIE  (0x400)

#define FLASH_SR_BSY    (*FLASH_SR & 0x0)
#define FLASH_SR_EOP    (0x20)

#define ERASE_PATTERN   (0xFFFFFFFF)

// Key Values - I don't know if these are used, uncomment if so
//const uint RDPRT  = 0x00A5;
const uint KEY1   = 0x45670123;
const uint KEY2   = 0xCDEF89AB;

/*** Prototypes ***/
bool setupFlash();
bool writeFlash(uint16, uint16*);
bool erasePage(uintptr_t*);
uint16 readFlash(uint16*);
bool addrChk(uintptr_t*);
bool unlockFlash();

bool setupFlash() {
  // Turn off interrupts, ensure the program bit isn't set
  *FLASH_CR &= (~FLASH_CR_EOPIE | ~FLASH_CR_ERRIE | ~FLASH_CR_PG );

  return true;
}

/*
   The main Flash memory programming sequence in standard mode is as follows:
      ● Check that no main Flash memory operation is ongoing by
              checking the BSY bit in the FLASH_SR register.
      ● Set the PG bit in the FLASH_CR register.
      ● Perform the data write (half-word) at the desired address.
      ● Wait for the BSY bit to be reset.
      ● Read the programmed value and verify.
*/
bool writeFlash(uint16 data, uint16* addr) {
  // Address check will be performed in read/erase.
  // Unlock check performed in read/erase

  // Perform erase, if necessary
  if (readFlash(addr) != (uint16)0xFFFF)
  {
    if (!erasePage((uintptr_t*)addr))
    {
      Serial.println("FlashController ERROR: failed to erase page for writing");
      return false;
    }
  }

  // Write addr to FAR
  *FLASH_AR = (uintptr_t)addr;

  // Set program bit
  *FLASH_CR |= FLASH_CR_PG;

  // Wait for operation to finish
  while (FLASH_SR_BSY) {
    /*NOOP*/
  }

  // Write to address
  *addr = data;

  // Wait for operation to finish
  while (FLASH_SR_BSY) {
    /*NOOP*/
  }

  // Clear program bit
  *FLASH_CR &= ~FLASH_CR_PG;

  // Return true if the address matches the data pattern
  return (*addr == data);
}

bool erasePage(uintptr_t* addr) {
  //   if(!addrChk(addr)){
  //      Serial.println("FlashController ERROR: failed address check");
  //      return false;
  //   }


  // Check lock
  if (!unlockFlash()) {
    Serial.println("FlashController ERROR: failed to unlock flash for erase");
    return false;
  }

  while (FLASH_SR_BSY) {
    /*NOOP*/
  }
  // Set FLASH_CR_PER
  *FLASH_CR |= FLASH_CR_PER;
  
  while (FLASH_SR_BSY) {
    /*NOOP*/
  }
  // Write addr to FAR
  *FLASH_AR = (uint32)addr;

  while (FLASH_SR_BSY) {
    /*NOOP*/
  }
  // Set FLASH_CR_STRT
  *FLASH_CR |= FLASH_CR_STRT;

  // Wait for previous operation to complete
  while (FLASH_SR_BSY) {
    /*NOOP*/
  }

  // Page erase clear
  *FLASH_CR &= ~FLASH_CR_PER;

  // Wait for previous operation to complete
  while (FLASH_SR_BSY) {
    /*NOOP*/
  }
  *FLASH_CR &= ~FLASH_CR_STRT;

  while (FLASH_SR_BSY) {
    /*NOOP*/
  }
  // Check erasure - value should be 0xFFFFFFF;
  return (*addr == ERASE_PATTERN);
}

uint16 readFlash(uint16* addr) {
  // Note: while reading the bootloader is OK, this general check will prevent it.
  //    No real reason to check it anyways...
  //  if(!addrChk((uintptr_t*)addr)){
  //     Serial.println("FlashController ERROR: failed address check");
  //     return 0; // No real value we can return to indicate failure here...
  //  }

  // Lock check not necessary

  // Wait for previous operation to complete
  while (FLASH_SR_BSY) {
    /*NOOP*/
  }

  // read operation
  return *addr;
}

// TODO: Locking function, if needed

bool unlockFlash() {
  // Check if the FLASH_CR_LOCK is set - bit 7
  // If it is, no unlock needed.
  while (FLASH_SR_BSY) {
    /*NOOP*/
  }
  if (!FLASH_CR_LOCK) {
    return true;
  }

  Serial.print("");
  // Else, perform the unlock sequence
  while (FLASH_SR_BSY) {
    /*NOOP*/
  }
  *FLASH_KEYR = KEY1;

  Serial.print("");
  while (FLASH_SR_BSY) {
    /*NOOP*/
  }
  *FLASH_KEYR = KEY2;

  Serial.print("");
  while (FLASH_SR_BSY) {
    /*NOOP*/
  }
  // Now return lock status. Should be clear.
  return !FLASH_CR_LOCK;
}

bool addrChk(uintptr_t* addr) {
  // Some error checking
  // might consider wrapping with a "not defined NO_ERR_CHK" macro
  // for speedup/size considerations
  if (addr < flashBegin || addr > flashEnd) {
    Serial.println("FlashController ERROR: out of bounds address");
    return false;
  }
  else if (addr < blEnd) {
    // Protect the bootloader.
    // Consider defining an "UNSAFE_FLASH_WRITE" macro that can
    //   be defined to conditionally compile this check.
    Serial.println("FlashController ERROR: attempt to overwrite bootloader");
    return false;
  }

  return true;
}
