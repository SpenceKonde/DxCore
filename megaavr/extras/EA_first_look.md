# EA-series first look
Based only on the header files (and what is already well known).

DAC: Headers show n that there will be a high and a low output range selectable, or automatic mode. Whatever does that mean?

PORTMUX: USART2 is in USARTROUTEB it looks like.

TCA1 gets a number of extra 5-6-7 pin mappings, so you can get PWM out of it on parts with less than 48 pins. It'll also be killer as a utility timer, because of how it can interact with CCL and EVSYS.

EVSYS is getting major changes in the two areas where not all channels were the same:
Each PORTx looks like it's getting an 8 bit register to control event generation from generators 0 and 1 of that port. The group masks are defined as 0x07 and 0x70. I'm gonna take a wild guess and say we put the number of the pin within the port into this bitfield. In return, all event channels can be tuned to any port's generators. Oh - here, my guess confirmed.

The mass port configuration that the DX had is here.

INLVL is here despite the absence of MVIO

Same reset and UPDI pin alternate functions as DD.

RTC gets a similar treatment to port in terms of event generation from the PIT, it has an 8 bit register with two 4-bit bitfields to control the two event outputs, which will toggle oncce per 2^(groupcode) prescaled RTC clocks. This is less powerful than what we have now, but less burden of remembering which event channels can do what frequencies.

CLKCTRL - We have autotune.
We appear to have 8 bits dedicated to the tuning value
We also have a 16/20 MHz fuse (boooo!)
Sounds like the tuning registers, when not autotuning, will be like the 2-series (step right up ladies and gentlemenm place your bets - whose internal oscillator can be tuned higher without crashing?)

NVMCTRL - clearly some things are expoected to be different since it supports RWW supposedly
We are back to paged writes, which is of course sad.

Commands list is:
```c
typedef enum NVMCTRL_CMD_enum
{
    NVMCTRL_CMD_NOCMD_gc = (0x00<<0),  /* No Command */
    NVMCTRL_CMD_NOOP_gc = (0x01<<0),  /* No Operation */
    NVMCTRL_CMD_FLPW_gc = (0x04<<0),  /* Flash Page Write */
    NVMCTRL_CMD_FLPERW_gc = (0x05<<0),  /* Flash Page Erase and Write */
    NVMCTRL_CMD_FLPER_gc = (0x08<<0),  /* Flash Page Erase */
    NVMCTRL_CMD_FLMPER2_gc = (0x09<<0),  /* Flash 2-page erase enable */
    NVMCTRL_CMD_FLMPER4_gc = (0x0A<<0),  /* Flash 4-page erase enable */
    NVMCTRL_CMD_FLMPER8_gc = (0x0B<<0),  /* Flash 8-page erase enable */
    NVMCTRL_CMD_FLMPER16_gc = (0x0C<<0),  /* Flash 16-page erase enable */
    NVMCTRL_CMD_FLMPER32_gc = (0x0D<<0),  /* Flash 32-page erase enable */
    NVMCTRL_CMD_FLPBCLR_gc = (0x0F<<0),  /* Flash Page Buffer Clear */
    NVMCTRL_CMD_EEPW_gc = (0x14<<0),  /* EEPROM Page Write */
    NVMCTRL_CMD_EEPERW_gc = (0x15<<0),  /* EEPROM Page Erase and Write */
    NVMCTRL_CMD_EEPER_gc = (0x17<<0),  /* EEPROM Page Erase */
    NVMCTRL_CMD_EEPBCLR_gc = (0x1F<<0),  /* EEPROM Page Buffer Clear */ <- new?
    NVMCTRL_CMD_CHER_gc = (0x20<<0),  /* Chip Erase Command (UPDI only) */
    NVMCTRL_CMD_EECHER_gc = (0x30<<0)  /* EEPROM Erase Command (UPDI only) */
} NVMCTRL_CMD_t;
```


Page size of 64k parts is 128b.

Not unexpectedly, for RWW we have a few new vectors:
```c
#define NVMCTRL_FLREADY_vect_num  33
#define NVMCTRL_FLREADY_vect      _VECTOR(33)  /*  */
#define NVMCTRL_NVMREADY_vect_num  33
#define NVMCTRL_NVMREADY_vect      _VECTOR(33)  /*  */
```

What I *wasn't* expecting was anything like this:
```c
/* NVMCTRL.EECALB  bit masks and bit positions */
#define NVMCTRL_EEPUMPLVL_gm  0x0F  /* EEPROM Pump Level During Programming group mask. */
#define NVMCTRL_EEPUMPLVL_gp  0  /* EEPROM Pump Level During Programming group position. */
#define NVMCTRL_EEPUMPLVL_0_bm  (1<<0)  /* EEPROM Pump Level During Programming bit 0 mask. */
#define NVMCTRL_EEPUMPLVL_0_bp  0  /* EEPROM Pump Level During Programming bit 0 position. */
#define NVMCTRL_EEPUMPLVL_1_bm  (1<<1)  /* EEPROM Pump Level During Programming bit 1 mask. */
#define NVMCTRL_EEPUMPLVL_1_bp  1  /* EEPROM Pump Level During Programming bit 1 position. */
#define NVMCTRL_EEPUMPLVL_2_bm  (1<<2)  /* EEPROM Pump Level During Programming bit 2 mask. */
#define NVMCTRL_EEPUMPLVL_2_bp  2  /* EEPROM Pump Level During Programming bit 2 position. */
#define NVMCTRL_EEPUMPLVL_3_bm  (1<<3)  /* EEPROM Pump Level During Programming bit 3 mask. */
#define NVMCTRL_EEPUMPLVL_3_bp  3  /* EEPROM Pump Level During Programming bit 3 position. */
#define NVMCTRL_EEPROGTIME_gm  0x70  /* EEPROM Programming Time group mask. */
#define NVMCTRL_EEPROGTIME_gp  4  /* EEPROM Programming Time group position. */
#define NVMCTRL_EEPROGTIME_0_bm  (1<<4)  /* EEPROM Programming Time bit 0 mask. */
#define NVMCTRL_EEPROGTIME_0_bp  4  /* EEPROM Programming Time bit 0 position. */
#define NVMCTRL_EEPROGTIME_1_bm  (1<<5)  /* EEPROM Programming Time bit 1 mask. */
#define NVMCTRL_EEPROGTIME_1_bp  5  /* EEPROM Programming Time bit 1 position. */
#define NVMCTRL_EEPROGTIME_2_bm  (1<<6)  /* EEPROM Programming Time bit 2 mask. */
#define NVMCTRL_EEPROGTIME_2_bp  6  /* EEPROM Programming Time bit 2 position. */

/* NVMCTRL.EECALC  bit masks and bit positions */
#define NVMCTRL_EESABIAS_gm  0x07  /* EEPROM Sense Amplifier Bias Calibration group mask. */
#define NVMCTRL_EESABIAS_gp  0  /* EEPROM Sense Amplifier Bias Calibration group position. */
#define NVMCTRL_EESABIAS_0_bm  (1<<0)  /* EEPROM Sense Amplifier Bias Calibration bit 0 mask. */
#define NVMCTRL_EESABIAS_0_bp  0  /* EEPROM Sense Amplifier Bias Calibration bit 0 position. */
#define NVMCTRL_EESABIAS_1_bm  (1<<1)  /* EEPROM Sense Amplifier Bias Calibration bit 1 mask. */
#define NVMCTRL_EESABIAS_1_bp  1  /* EEPROM Sense Amplifier Bias Calibration bit 1 position. */
#define NVMCTRL_EESABIAS_2_bm  (1<<2)  /* EEPROM Sense Amplifier Bias Calibration bit 2 mask. */
#define NVMCTRL_EESABIAS_2_bp  2  /* EEPROM Sense Amplifier Bias Calibration bit 2 position. */
#define NVMCTRL_EEGATESEL_bm  0x08  /* EEPROM Cell Gate Select bit mask. */
#define NVMCTRL_EEGATESEL_bp  3  /* EEPROM Cell Gate Select bit position. */
#define NVMCTRL_EEAUXCAL_gm  0xC0  /* EEPROM AUX Calibration group mask. */
#define NVMCTRL_EEAUXCAL_gp  6  /* EEPROM AUX Calibration group position. */
#define NVMCTRL_EEAUXCAL_0_bm  (1<<6)  /* EEPROM AUX Calibration bit 0 mask. */
#define NVMCTRL_EEAUXCAL_0_bp  6  /* EEPROM AUX Calibration bit 0 position. */
#define NVMCTRL_EEAUXCAL_1_bm  (1<<7)  /* EEPROM AUX Calibration bit 1 mask. */
#define NVMCTRL_EEAUXCAL_1_bp  7  /* EEPROM AUX Calibration bit 1 position. */
```
Probably will be gone from headers before release, but this is a damned sight more interesting than the BIST that was in early 2-series headers...

Especially if these registers are writable at release (or even visible at all) - I think its's most likely that they'll be made to load from
SIGROW, and/or hidden entirely, and these are just there for development and testing.

Did I miss anything?
