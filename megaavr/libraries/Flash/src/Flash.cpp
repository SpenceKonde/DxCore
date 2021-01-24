#include <Arduino.h>
#include <avr/pgmspace.h>
#include "Flash.h"

typedef union {
    uint16_t word;
    uint8_t bytes[2];
} flword16_t;


/* My go-to NVMCTRL.CTRLA write function - check status only at start
 * always set to 0 first - and then if asked to set it elsewhere, then
 * do so. Undecided about whether to have these functions clear it.
 * It's not risky to leave them that way, but could annoy people with
 * other routines to write to EEPROM. On the other hand, depending on
 * how I end up implementing bootloaderless flash writes, they might
 * have no choice in adapting to our way of doing things if they want
 * to write to EEPROM (or USERROW).
 */
void do_nvmctrl(uint8_t command) {
  while (NVMCTRL.STATUS & 0x03); //wait if busy, though this is unlikely
  _PROTECTED_WRITE_SPM(NVMCTRL.CTRLA,NVMCTRL_CMD_NONE_gc);
  _PROTECTED_WRITE_SPM(NVMCTRL.CTRLA,command);
  return;
}


uint8_t FlashClass::checkWritable() {
  #ifndef USING_OPTIBOOT
    if (pgm_read_word_near(0x0004)!=0x95Ff) {
      // Non-bootloader-dependent versions are planned to
      // replace the (basically useless for Arduino) NMI_vect
      // instead of Optiboots .version-4, plus as the target
      // for the SPM Z+ RET location. I think NVM_EE_vect
      // goes too, because we need to do the same sort of thing
      // to maintain access to EEPROM if we were to do what
      // I think we will need to, which is to set a sliver of
      // the flash as "APPCODE" and the rest as the APPDATA.
      // Since APPDATA can't write to EEPROM, we'd replace
      // the EERPOM vector (which nobodty in Arduino-land
      // ever uses - it's sometimes used for other dirty ends
      // on classic AVRs too (ex: VirtualBoot for Optiboot.)
      return FLASHWRITE_NOBOOT;
    } else {
      // Good to go - except it's not yet implemented.
      return FLASHWRITE_NYI;
    }
  #else
    if (FUSE.BOOTSIZE != 0x01) {
      // Should we support BIGBOOT?
      // I vote "NO" because I know I have never made that work
      // but it would be far less work than getting a BIGBOOT
      // bootloader together and offering features the normakl
      // version didn't.
      return FLASHWRITE_NOBOOT;
    }
    if (FUSE.BOOTSIZE > 1) {
      return FLASHWRITE_UNRECOGNIZED;
    }
    // Optiboot 9.1 without SPM Z+ app callin support
    // was shipped with 1.2.x and earlier of DxCore
    uint16_t optiversion=pgm_read_word_near(0x01fe);;
    if (optiversion == 0x0901) {
      return FLASHWRITE_OLD;
    }
    // Version of the bootloader that doesn't support this
    // but also not the one I adapted for this project.
    if (optiversion != 0x1901) {
      return FLASHWRITE_UNRECOGNIZED;
    }
    // After making sure we're "on"
    optiversion = pgm_read_word_near(0x01fa);;
    if (optiversion==0) {
      return FLASHWRITE_DISABLED;
    }
    if (optiversion == 0x95f8) {
      // That's the  SPM instruction
      // or at least the first byte of it.
    return FLASHWRITE_OK;
    }
    /*
    else if (optiversion == 0x95e8) {
      // *probably* not a realistic situation to handle.
      // There's no reason not to use SPM Z+ instead of SPM
      // but I nearly released with plain SPM!
      return FLASHWRITE_NO_Z_INC;
    }
    */
    return FLASHWRITE_UNRECOGNIZED;
  #endif
}

/* All significant docs only written out once where first apply
 * In general, I am much more careful than normal about trying to catch
 * inappropriate arguments and recklessness than usual, in the interest
 * of trying to help clever fools not write code that can brick itself
 * in corner cases. It totally still can, but it's more careful than
 * usual Spence-code.
 */

uint8_t FlashClass::erasePage(const uint32_t address, const uint8_t size) {
  if (FUSE.BOOTSIZE != 0x01) {
    return FLASHWRITE_NOBOOT;
  }
  uint8_t command;
  uint16_t minaddress = 0x200; //512 (bootloader section).
  switch (size) {
    case 1:
      command=NVMCTRL_CMD_FLPER_gc;
      break;
    case 2:
      command=NVMCTRL_CMD_FLMPER2_gc;
      minaddress<<=1;
      break;
    case 4:
      command=NVMCTRL_CMD_FLMPER4_gc;
      minaddress<<=2;
      break;
    case 8:
      command=NVMCTRL_CMD_FLMPER8_gc;
      minaddress<<=3;
      break;
    case 16:
      command=NVMCTRL_CMD_FLMPER16_gc;
      minaddress<<=4;
      break;
    case 32:
      command=NVMCTRL_CMD_FLMPER32_gc;
      minaddress<<=5;
      break;
    default:
      return FLASHWRITE_BADSIZE;
  }
  if (address > (PROGMEM_SIZE-1) || address < minaddress) {
    return FLASHWRITE_BADADDR;
  }
  #if (PROGMEM_SIZE > 0x10000)
    if (address > 0xFFFF) {
      RAMPZ = 1;
    } else {
      RAMPZ = 0;
    }
  #endif
  do_nvmctrl(command);
  uint16_t zaddress=address;  // temp variable; SPM Z+ changes it!
  /* Long story short, if you are passing things in that may be chanced by
   * the ASM, declare it as an output... If you have a variable that must
   * get in there, it appears to me that you must copy it to another variable
   * and shove the other variable into the assembly. However - the insidious
   * thing is that depending on how it distributes variables between the
   * registersa, if when you finally get to the asm, your constraints are
   * highly, ah, constraining, you could well end up in the situation I was
   * in: All the assembly blocks passed the address to it and assumed it would
   * come out unscathed... and... sometimes it did. Sometimes It didn't.
   * So don't change anything not passed in as an output, kids!
   * Remember, assembly has no guardrails other than the constraints.
   * It won't let you pass a const as an output for example. But it won't
   * won't keep your assembly from writing to it anyway!
   */
  __asm__ __volatile__ ("call 0x1FA" : "+z" (zaddress));
  #if (PROGMEM_SIZE > 0x10000)
    RAMPZ = 0; //just begging for trouble not resetting that.
  #endif
  uint8_t status = NVMCTRL.STATUS & 0x70;
  if (status != 0) {
    NVMCTRL.STATUS = 0;
    return (FLASHWRITE_FAIL | (status >> 4)); // uhoh, NVMCTRL says we did something wrong...
  }
  if ((NVMCTRL.ADDR&0xFFFFFF) != (address + 1)) {
    // When we tell the NVM controller to write or erase, it will
    // set the NVMCTRL.ADDR register to the address. So if that's not set to the address
    // we expected, but there's no error in status... it's as if SPM never happened...
    // which, if someone ever got this, is likely the case. Maybe the bootloader wasn't
    // actually checked before trying to use it?
    return FLASHWRITE_SPM_NOT_USED;
  }
  return FLASHWRITE_OK;
}

uint8_t FlashClass::writeWord(const uint32_t address, const uint16_t data) {
  if (FUSE.BOOTSIZE != 0x01) {
    return FLASHWRITE_NOBOOT;
  }
  if (address > (PROGMEM_SIZE - 2) || address < 512) {
    return FLASHWRITE_BADADDR;
  }

    if (address & 0x01) {
      return FLASHWRITE_ALIGN;
    }

  #if (PROGMEM_SIZE > 0x10000)
    if (address > 0xFFFF) {
      RAMPZ = 1;
    } else {
      RAMPZ = 0;
    }
  #endif
  do_nvmctrl(NVMCTRL_CMD_FLWR_gc);
  uint16_t zaddress=address;
  __asm__ __volatile__(
            "mov  r0,%A[dat]"                     "\n\t"
            "mov  r1,%B[dat]"                     "\n\t"
            "call 0x1FA"                          "\n\t"
            "clr  r1"                             "\n\t"
            : [flptr]   "+z"   (zaddress)
            : [dat]      "r"   (data)
          );
  #if (PROGMEM_SIZE > 0x10000)
    RAMPZ = 0; //just begging for trouble not resetting that.
  #endif
  uint8_t status = NVMCTRL.STATUS & 0x70;
  if (status != 0) {
    NVMCTRL.STATUS = 0;
    return (FLASHWRITE_FAIL | (status >> 4)); // uhoh, NVMCTRL says we did something wrong...
  }
  if ((NVMCTRL.ADDR&0xFFFFFF) != (address + 1)) {
    return FLASHWRITE_SPM_NOT_USED;
  }
  return FLASHWRITE_OK;
}



uint8_t FlashClass::writeByte(const uint32_t address, const uint8_t data) {
  if (FUSE.BOOTSIZE != 0x01) {
    return FLASHWRITE_NOBOOT;
  }
  if (address > (PROGMEM_SIZE - 2) || address < 512) {
    return FLASHWRITE_BADADDR;
  }
  #if (PROGMEM_SIZE > 0x10000)
    if (address > 0xFFFF) {
      RAMPZ = 1;
    } else {
      RAMPZ = 0;
    }
  #endif
  uint16_t dataword;
  // We need to write a word, so we have to make one - one byte of it
  // will be 0xFF, so it can't stomp on whatever is already there
  // (only erase can turn a 0 back to 1).
  // Check if address is odd (high byte) or even (low byte)
  // and put our byte into the appropriate half of the word
  if (address & 0x01) {

    dataword = 0x00FF | ((uint16_t)data << 8);
  } else {
    dataword = 0xFF00 | data;
  }
  // > "The Flash is word-accessed for code space write operations, so
  // > the least significant bit (bit 0) in the Address Pointer is ignored"
  // No, it most definitely is not ignored! It will definitely do unaligned writes
  // unless they cross the bage boundary, in which case the byte that would
  // be in the last byte of the prior page ends up on the second byte if the new page...
  uint16_t zaddress=address&0xFFFE; //truncate and force low bit to 0...
  do_nvmctrl(NVMCTRL_CMD_FLWR_gc);
  __asm__ __volatile__(
            "mov  r0,%A[dat]"                     "\n\t"
            "mov  r1,%B[dat]"                     "\n\t"
            "call 0x1FA"                          "\n\t"
            "clr  r1"                             "\n\t"
            : [flptr]   "+z"   (zaddress)
            : [dat]      "r"   (dataword)
          );
  #if (PROGMEM_SIZE > 0x10000)
    RAMPZ = 0; //just begging for trouble not resetting that.
  #endif
  uint8_t status = NVMCTRL.STATUS & 0x70;
  if (status != 0) {
    NVMCTRL.STATUS = 0;
    return (FLASHWRITE_FAIL | (status >> 4)); // uhoh, NVMCTRL says we did something wrong...
  }
  if ((NVMCTRL.ADDR&0xFFFFFF) != (address | 1)) {
    return FLASHWRITE_SPM_NOT_USED;
  }
  return FLASHWRITE_OK;
}

uint8_t FlashClass::writeWords(const uint32_t address, const uint16_t* data, uint16_t length) {
  if (length == 0) {
    return FLASHWRITE_0LENGTH;
  }
  if (FUSE.BOOTSIZE != 0x01) {
    return FLASHWRITE_NOBOOT;
  }
  if (address > (PROGMEM_SIZE-2) || address < 512) {
    return FLASHWRITE_BADADDR;
  }
  if (address & 0x01) {
    return FLASHWRITE_ALIGN;
  }

  // Length must not run off the end of the flash. Also, an attempt tp write
  // more flash than would fit in the entire program memory is an integer
  // underflow, or some other malfunction serious enough to conclude that
  // the application is not capable of writing meaningful data at this time
  // much less ensuring that the destination flash is appropriate and has
  // been erased.
  if (address+(2*length) > PROGMEM_SIZE  || length >= RAMSIZE ){
    return FLASHWRITE_TOOBIG;
  }
  #if (PROGMEM_SIZE > 0x10000)
    if (address > 0xFFFF) {
      RAMPZ = 1;
    } else {
      RAMPZ = 0;
    }
  #endif
  do_nvmctrl(NVMCTRL_CMD_FLWR_gc);
  // If the bootloader had an SPM instead of SPM Z+, then we would need
  // that adiw - AND we coouldn't write across the 64k barrier - but
  // since I didn't release the bad version, I think the world is probably
  // safe from that!
  // like address, length is copied to declength so we can accept a const,
  // and use the original value to confirm that SPM occurred - while the
  // copy is used count down the number of words remaining to write.
  uint16_t zaddress=address;
  uint16_t declength=length;
  __asm__ __volatile__  (
          "head_%=:"                              "\n\t"
            "ld   r0, %a[ptr]+"                   "\n\t"
            "ld   r1, %a[ptr]+"                   "\n\t"
            "call 0x1FA"                          "\n\t"
        /*  "adiw r30,  2"                        "\n\t" */
            "sbiw %[len], 1"                      "\n\t"
            "brne head_%="                        "\n\t"
            "clr  r1"                             "\n\t"
            : [ptr]     "+e"  ((uint16_t)data),
              [flptr]   "+z"  (zaddress),
              [len]     "+w"  (declength)
          );
  #if (PROGMEM_SIZE > 0x10000)
    RAMPZ = 0; //just begging for trouble not resetting that.
  #endif
  uint8_t status = NVMCTRL.STATUS & 0x70;
  if (status != 0) {
    NVMCTRL.STATUS = 0;
    return (FLASHWRITE_FAIL | (status >> 4)); // uhoh, NVMCTRL says we did something wrong...
  }
  if ((NVMCTRL.ADDR&0xFFFFFF) != (address + (length*2) - 1)) {
    return FLASHWRITE_SPM_NOT_USED;
  }
  return FLASHWRITE_OK;
}

uint8_t FlashClass::writeBytes(const uint32_t address, const uint8_t* data, uint16_t length) {
  uint32_t tAddress=address;
  uint8_t status;
  if(address & 0x01) {
    status=writeByte(tAddress++,*(data));
    if (status) return status;
    length--;
  }
  if(length >1) {
    status=writeWords(tAddress,(uint16_t*)data,(length>>1));
    if (status) return status;
  }
  // there may be one more byte...
  if (length&1) {
    data+=(length & 0xFFFE); // what we wrote with the word above...
    status = writeByte(tAddress+length-2,*data);
  }
  return status;
}



uint8_t FlashClass::readByte(const uint32_t address){
  #if PROGMEM_SIZE > 0x10000
    return pgm_read_byte_far(address);
  #else
    return pgm_read_byte_near((uint16_t)address);
  #endif
}

uint16_t FlashClass::readWord(const uint32_t address) {
  #if PROGMEM_SIZE > 0x10000
    return pgm_read_word_far(address);
  #else
    return pgm_read_word_near((uint16_t)address);
  #endif
}

uint8_t* FlashClass::mappedPointer(const uint32_t address) {
  if (address > PROGMEM_SIZE) return (uint8_t*) NULL;
  // If location is outside bounds of flash, return null pointer
  #if PROGMEM_SIZE == 0x10000
    if ( (address > 0x8000) == !!(NVMCTRL.CTRLB & NVMCTRL_FLMAP0_bm)) {
      return (uint8_t *)(0x8000|((uint16_t)address));
    }
    else {
      return (uint8_t*) NULL;
    }
  #elif PROGMEM_SIZE == 0x20000
    uint8_t section=address>>15;
    // this will be 0~3 - corresponding to the number of the flash section
    // we return a pointer if it's in the mapped flash, otherwise,
    if ( section == ((NVMCTRL.CTRLB & NVMCTRL_FLMAP_gm) >> 4)) {
      return (uint8_t *)(0x8000|((uint16_t)address));
    }
    else {
      return (uint8_t*) NULL;
    }
  #else
    // all of the flash is mapped
    return (uint8_t*)(0x8000 | (uint16_t)address);
  #endif
}

uint32_t FlashClass::flashAddress(uint8_t* mappedPtr) {
  if (((uint16_t)mappedPtr) < 0x8000) {
    return 0; //is not a pointer to mapped flash!
  }
  uint32_t address= (uint16_t)mappedPtr;
  #if PROGMEM_SIZE == 0x10000
    if (!(NVMCTRL.CTRLB & NVMCTRL_FLMAP0_bm)) {
      address -=0x8000;
    }
    return address;
  #elif PROGMEM_SIZE == 0x20000
    uint8_t flmap = ((NVMCTRL.CTRLB & NVMCTRL_FLMAP_gm) >> 4);
    if (flmap & 0x02) {
      // Section 2 or 3 is mapped
      address +=0x10000;
    }
    if (!(flmap & 1)){
      // section 0 or 2 is mapped
      address -=0x8000;
    }
    return address;
  #else
    // The whole flash is mapped and this is super easy:
    return (address & 0x7FFF);
  #endif
}

FlashClass Flash;
