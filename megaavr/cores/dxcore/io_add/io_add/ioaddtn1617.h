
/*****************************************************************************
 * I/O Header Addendums for AVRxt processors (C) 2021 Spence Konde based on
 * the Atmel/Microchip header files from ATPACK versions:
 * ATtiny 1.9.337
 * AVR Dx 1.7.85
 * These headers define every group code (the constants presented as enums)
 * with the _gc replaced with _GC, as a preprocessor define.
 * This makes it possible to use them in #if and #ifdef preprocessor tests.
 *
 * Licensed under the same terms as original file as permitted and required
 * by that license.
 *
 * The copyright notice enclosed with those datasheets follows below
 *****************************************************************************
 *
 * Copyright (C) 2020 Atmel Corporation, a wholly owned subsidiary of
 * Microchip Technology Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ****************************************************************************/

 #ifndef IO_ADDXXX_H
 #define IO_ADDXXX_H


/*------------------------------------------------------------------------
AC - Analog Comparator
------------------------------------------------------------------------*/

/* Hysteresis Mode select */
#define AC_HYSMODE_OFF_GC (0x00<<1)  /* No hysteresis */
#define AC_HYSMODE_10mV_GC (0x01<<1)  /* 10mV hysteresis */
#define AC_HYSMODE_25mV_GC (0x02<<1)  /* 25mV hysteresis */
#define AC_HYSMODE_50mV_GC (0x03<<1)  /* 50mV hysteresis */

/* Interrupt Mode select */
#define AC_INTMODE_BOTHEDGE_GC (0x00<<4)  /* Any Edge */
#define AC_INTMODE_NEGEDGE_GC (0x02<<4)  /* Negative Edge */
#define AC_INTMODE_POSEDGE_GC (0x03<<4)  /* Positive Edge */

/* Low Power Mode select */
#define AC_LPMODE_DIS_GC (0x00<<3)  /* Low power mode disabled */
#define AC_LPMODE_EN_GC (0x01<<3)  /* Low power mode enabled */

/* Negative Input MUX Selection select */
#define AC_MUXNEG_PIN0_GC (0x00<<0)  /* Negative Pin 0 */
#define AC_MUXNEG_PIN1_GC (0x01<<0)  /* Negative Pin 1 */
#define AC_MUXNEG_VREF_GC (0x02<<0)  /* Voltage Reference */
#define AC_MUXNEG_DAC_GC (0x03<<0)  /* DAC output */

/* Positive Input MUX Selection select */
#define AC_MUXPOS_PIN0_GC (0x00<<3)  /* Positive Pin 0 */
#define AC_MUXPOS_PIN1_GC (0x01<<3)  /* Positive Pin 1 */
#define AC_MUXPOS_PIN2_GC (0x02<<3)  /* Positive Pin 2 */
#define AC_MUXPOS_PIN3_GC (0x03<<3)  /* Positive Pin 3 */

/*------------------------------------------------------------------------
ADC - Analog to Digital Converter
------------------------------------------------------------------------*/

/* Automatic Sampling Delay Variation select */
#define ADC_ASDV_ASVOFF_GC (0x00<<4)  /* The Automatic Sampling Delay Variation is disabled */
#define ADC_ASDV_ASVON_GC (0x01<<4)  /* The Automatic Sampling Delay Variation is enabled */

/* Duty Cycle select */
#define ADC_DUTYCYC_DUTY50_GC (0x00<<0)  /* 50% Duty cycle */
#define ADC_DUTYCYC_DUTY25_GC (0x01<<0)  /* 25% Duty cycle */

/* Initial Delay Selection select */
#define ADC_INITDLY_DLY0_GC (0x00<<5)  /* Delay 0 CLK_ADC cycles */
#define ADC_INITDLY_DLY16_GC (0x01<<5)  /* Delay 16 CLK_ADC cycles */
#define ADC_INITDLY_DLY32_GC (0x02<<5)  /* Delay 32 CLK_ADC cycles */
#define ADC_INITDLY_DLY64_GC (0x03<<5)  /* Delay 64 CLK_ADC cycles */
#define ADC_INITDLY_DLY128_GC (0x04<<5)  /* Delay 128 CLK_ADC cycles */
#define ADC_INITDLY_DLY256_GC (0x05<<5)  /* Delay 256 CLK_ADC cycles */

/* Analog Channel Selection Bits select */
#define ADC_MUXPOS_AIN0_GC (0x00<<0)  /* ADC input pin 0 */
#define ADC_MUXPOS_AIN1_GC (0x01<<0)  /* ADC input pin 1 */
#define ADC_MUXPOS_AIN2_GC (0x02<<0)  /* ADC input pin 2 */
#define ADC_MUXPOS_AIN3_GC (0x03<<0)  /* ADC input pin 3 */
#define ADC_MUXPOS_AIN4_GC (0x04<<0)  /* ADC input pin 4 */
#define ADC_MUXPOS_AIN5_GC (0x05<<0)  /* ADC input pin 5 */
#define ADC_MUXPOS_AIN6_GC (0x06<<0)  /* ADC input pin 6 */
#define ADC_MUXPOS_AIN7_GC (0x07<<0)  /* ADC input pin 7 */
#define ADC_MUXPOS_AIN8_GC (0x08<<0)  /* ADC input pin 8 */
#define ADC_MUXPOS_AIN9_GC (0x09<<0)  /* ADC input pin 9 */
#define ADC_MUXPOS_AIN10_GC (0x0A<<0)  /* ADC input pin 10 */
#define ADC_MUXPOS_AIN11_GC (0x0B<<0)  /* ADC input pin 11 */
#define ADC_MUXPOS_PTC_GC (0x1B<<0)  /* PTC/DAC2 */
#define ADC_MUXPOS_DAC0_GC (0x1C<<0)  /* DAC0/DAC0 */
#define ADC_MUXPOS_INTREF_GC (0x1D<<0)  /* Internal Ref */
#define ADC_MUXPOS_TEMPSENSE_GC (0x1E<<0)  /* Temp sensor/DAC1 */
#define ADC_MUXPOS_GND_GC (0x1F<<0)  /* GND */

/* Clock Pre-scaler select */
#define ADC_PRESC_DIV2_GC (0x00<<0)  /* CLK_PER divided by 2 */
#define ADC_PRESC_DIV4_GC (0x01<<0)  /* CLK_PER divided by 4 */
#define ADC_PRESC_DIV8_GC (0x02<<0)  /* CLK_PER divided by 8 */
#define ADC_PRESC_DIV16_GC (0x03<<0)  /* CLK_PER divided by 16 */
#define ADC_PRESC_DIV32_GC (0x04<<0)  /* CLK_PER divided by 32 */
#define ADC_PRESC_DIV64_GC (0x05<<0)  /* CLK_PER divided by 64 */
#define ADC_PRESC_DIV128_GC (0x06<<0)  /* CLK_PER divided by 128 */
#define ADC_PRESC_DIV256_GC (0x07<<0)  /* CLK_PER divided by 256 */

/* Reference Selection select */
#define ADC_REFSEL_INTREF_GC (0x00<<4)  /* Internal reference */
#define ADC_REFSEL_VDDREF_GC (0x01<<4)  /* VDD */
#define ADC_REFSEL_VREFA_GC (0x02<<4)  /* External reference */

/* ADC Resolution select */
#define ADC_RESSEL_10BIT_GC (0x00<<2)  /* 10-bit mode */
#define ADC_RESSEL_8BIT_GC (0x01<<2)  /* 8-bit mode */

/* Accumulation Samples select */
#define ADC_SAMPNUM_ACC1_GC (0x00<<0)  /* 1 ADC sample */
#define ADC_SAMPNUM_ACC2_GC (0x01<<0)  /* Accumulate 2 samples */
#define ADC_SAMPNUM_ACC4_GC (0x02<<0)  /* Accumulate 4 samples */
#define ADC_SAMPNUM_ACC8_GC (0x03<<0)  /* Accumulate 8 samples */
#define ADC_SAMPNUM_ACC16_GC (0x04<<0)  /* Accumulate 16 samples */
#define ADC_SAMPNUM_ACC32_GC (0x05<<0)  /* Accumulate 32 samples */
#define ADC_SAMPNUM_ACC64_GC (0x06<<0)  /* Accumulate 64 samples */

/* Window Comparator Mode select */
#define ADC_WINCM_NONE_GC (0x00<<0)  /* No Window Comparison */
#define ADC_WINCM_BELOW_GC (0x01<<0)  /* Below Window */
#define ADC_WINCM_ABOVE_GC (0x02<<0)  /* Above Window */
#define ADC_WINCM_INSIDE_GC (0x03<<0)  /* Inside Window */
#define ADC_WINCM_OUTSIDE_GC (0x04<<0)  /* Outside Window */

/*------------------------------------------------------------------------
BOD - Bod interface
------------------------------------------------------------------------*/

/* Operation in active mode select */
#define BOD_ACTIVE_DIS_GC (0x00<<2)  /* Disabled */
#define BOD_ACTIVE_ENABLED_GC (0x01<<2)  /* Enabled */
#define BOD_ACTIVE_SAMPLED_GC (0x02<<2)  /* Sampled */
#define BOD_ACTIVE_ENWAKE_GC (0x03<<2)  /* Enabled with wake-up halted until BOD is ready */

/* Bod level select */
#define BOD_LVL_BODLEVEL0_GC (0x00<<0)  /* 1.8 V */
#define BOD_LVL_BODLEVEL2_GC (0x02<<0)  /* 2.6 V */
#define BOD_LVL_BODLEVEL7_GC (0x07<<0)  /* 4.2 V */

/* Sample frequency select */
#define BOD_SAMPFREQ_1KHZ_GC (0x00<<4)  /* 1kHz sampling frequency */
#define BOD_SAMPFREQ_125HZ_GC (0x01<<4)  /* 125Hz sampling frequency */

/* Operation in sleep mode select */
#define BOD_SLEEP_DIS_GC (0x00<<0)  /* Disabled */
#define BOD_SLEEP_ENABLED_GC (0x01<<0)  /* Enabled */
#define BOD_SLEEP_SAMPLED_GC (0x02<<0)  /* Sampled */

/* Configuration select */
#define BOD_VLMCFG_BELOW_GC (0x00<<1)  /* Interrupt when supply goes below VLM level */
#define BOD_VLMCFG_ABOVE_GC (0x01<<1)  /* Interrupt when supply goes above VLM level */
#define BOD_VLMCFG_CROSS_GC (0x02<<1)  /* Interrupt when supply crosses VLM level */

/* voltage level monitor level select */
#define BOD_VLMLVL_5ABOVE_GC (0x00<<0)  /* VLM threshold 5% above BOD level */
#define BOD_VLMLVL_15ABOVE_GC (0x01<<0)  /* VLM threshold 15% above BOD level */
#define BOD_VLMLVL_25ABOVE_GC (0x02<<0)  /* VLM threshold 25% above BOD level */

/*------------------------------------------------------------------------
CCL - Configurable Custom Logic
------------------------------------------------------------------------*/

/* Edge Detection Enable select */
#define CCL_EDGEDET_DIS_GC (0x00<<7)  /* Edge detector is disabled */
#define CCL_EDGEDET_EN_GC (0x01<<7)  /* Edge detector is enabled */

/* Filter Selection select */
#define CCL_FILTSEL_DISABLE_GC (0x00<<4)  /* Filter disabled */
#define CCL_FILTSEL_SYNCH_GC (0x01<<4)  /* Synchronizer enabled */
#define CCL_FILTSEL_FILTER_GC (0x02<<4)  /* Filter enabled */

/* LUT Input 0 Source Selection select */
#define CCL_INSEL0_MASK_GC (0x00<<0)  /* Masked input */
#define CCL_INSEL0_FEEDBACK_GC (0x01<<0)  /* Feedback input source */
#define CCL_INSEL0_LINK_GC (0x02<<0)  /* Linked LUT input source */
#define CCL_INSEL0_EVENT0_GC (0x03<<0)  /* Event input source 0 */
#define CCL_INSEL0_EVENT1_GC (0x04<<0)  /* Event input source 1 */
#define CCL_INSEL0_IO_GC (0x05<<0)  /* IO pin LUTn-IN0 input source */
#define CCL_INSEL0_AC0_GC (0x06<<0)  /* AC0 OUT input source */
#define CCL_INSEL0_TCB0_GC (0x07<<0)  /* TCB0 WO input source */
#define CCL_INSEL0_TCA0_GC (0x08<<0)  /* TCA0 WO0 input source */
#define CCL_INSEL0_TCD0_GC (0x09<<0)  /* TCD0 WOA input source */
#define CCL_INSEL0_USART0_GC (0x0A<<0)  /* USART0 XCK input source */
#define CCL_INSEL0_SPI0_GC (0x0B<<0)  /* SPI0 SCK source */
#define CCL_INSEL0_AC1_GC (0x0C<<0)  /* AC1 OUT input source */
#define CCL_INSEL0_TCB1_GC (0x0D<<0)  /* TCB1 WO input source */
#define CCL_INSEL0_AC2_GC (0x0E<<0)  /* AC2 OUT input source */

/* LUT Input 1 Source Selection select */
#define CCL_INSEL1_MASK_GC (0x00<<4)  /* Masked input */
#define CCL_INSEL1_FEEDBACK_GC (0x01<<4)  /* Feedback input source */
#define CCL_INSEL1_LINK_GC (0x02<<4)  /* Linked LUT input source */
#define CCL_INSEL1_EVENT0_GC (0x03<<4)  /* Event input source 0 */
#define CCL_INSEL1_EVENT1_GC (0x04<<4)  /* Event input source 1 */
#define CCL_INSEL1_IO_GC (0x05<<4)  /* IO pin LUTn-N1 input source */
#define CCL_INSEL1_AC0_GC (0x06<<4)  /* AC0 OUT input source */
#define CCL_INSEL1_TCB0_GC (0x07<<4)  /* TCB0 WO input source */
#define CCL_INSEL1_TCA0_GC (0x08<<4)  /* TCA0 WO1 input source */
#define CCL_INSEL1_TCD0_GC (0x09<<4)  /* TCD0 WOB input source */
#define CCL_INSEL1_USART0_GC (0x0A<<4)  /* USART0 TXD input source */
#define CCL_INSEL1_SPI0_GC (0x0B<<4)  /* SPI0 MOSI input source */
#define CCL_INSEL1_AC1_GC (0x0C<<4)  /* AC1 OUT input source */
#define CCL_INSEL1_TCB1_GC (0x0D<<4)  /* TCB1WO input source */
#define CCL_INSEL1_AC2_GC (0x0E<<4)  /* AC2 OUT input source */

/* LUT Input 2 Source Selection select */
#define CCL_INSEL2_MASK_GC (0x00<<0)  /* Masked input */
#define CCL_INSEL2_FEEDBACK_GC (0x01<<0)  /* Feedback input source */
#define CCL_INSEL2_LINK_GC (0x02<<0)  /* Linked LUT input source */
#define CCL_INSEL2_EVENT0_GC (0x03<<0)  /* Event input source 0 */
#define CCL_INSEL2_EVENT1_GC (0x04<<0)  /* Event input source 1 */
#define CCL_INSEL2_IO_GC (0x05<<0)  /* IO pin LUTn-IN2 input source */
#define CCL_INSEL2_AC0_GC (0x06<<0)  /* AC0 OUT input source */
#define CCL_INSEL2_TCB0_GC (0x07<<0)  /* TCB0 WO input source */
#define CCL_INSEL2_TCA0_GC (0x08<<0)  /* TCA0 WO2 input source */
#define CCL_INSEL2_TCD0_GC (0x09<<0)  /* TCD0 WOA input source */
#define CCL_INSEL2_SPI0_GC (0x0B<<0)  /* SPI0 MISO source */
#define CCL_INSEL2_AC1_GC (0x0C<<0)  /* AC1 OUT input source */
#define CCL_INSEL2_TCB1_GC (0x0D<<0)  /* TCB1 WO input source */
#define CCL_INSEL2_AC2_GC (0x0E<<0)  /* AC2 OUT input source */

/* Sequential Selection select */
#define CCL_SEQSEL_DISABLE_GC (0x00<<0)  /* Sequential logic disabled */
#define CCL_SEQSEL_DFF_GC (0x01<<0)  /* D FlipFlop */
#define CCL_SEQSEL_JK_GC (0x02<<0)  /* JK FlipFlop */
#define CCL_SEQSEL_LATCH_GC (0x03<<0)  /* D Latch */
#define CCL_SEQSEL_RS_GC (0x04<<0)  /* RS Latch */

/*------------------------------------------------------------------------
CLKCTRL - Clock controller
------------------------------------------------------------------------*/

/* clock select select */
#define CLKCTRL_CLKSEL_OSC20M_GC (0x00<<0)  /* 20MHz internal oscillator */
#define CLKCTRL_CLKSEL_OSCULP32K_GC (0x01<<0)  /* 32KHz internal Ultra Low Power oscillator */
#define CLKCTRL_CLKSEL_XOSC32K_GC (0x02<<0)  /* 32.768kHz external crystal oscillator */
#define CLKCTRL_CLKSEL_EXTCLK_GC (0x03<<0)  /* External clock */

/* Crystal startup time select */
#define CLKCTRL_CSUT_1K_GC (0x00<<4)  /* 1K cycles */
#define CLKCTRL_CSUT_16K_GC (0x01<<4)  /* 16K cycles */
#define CLKCTRL_CSUT_32K_GC (0x02<<4)  /* 32K cycles */
#define CLKCTRL_CSUT_64K_GC (0x03<<4)  /* 64K cycles */

/* Prescaler division select */
#define CLKCTRL_PDIV_2X_GC (0x00<<1)  /* 2X */
#define CLKCTRL_PDIV_4X_GC (0x01<<1)  /* 4X */
#define CLKCTRL_PDIV_8X_GC (0x02<<1)  /* 8X */
#define CLKCTRL_PDIV_16X_GC (0x03<<1)  /* 16X */
#define CLKCTRL_PDIV_32X_GC (0x04<<1)  /* 32X */
#define CLKCTRL_PDIV_64X_GC (0x05<<1)  /* 64X */
#define CLKCTRL_PDIV_6X_GC (0x08<<1)  /* 6X */
#define CLKCTRL_PDIV_10X_GC (0x09<<1)  /* 10X */
#define CLKCTRL_PDIV_12X_GC (0x0A<<1)  /* 12X */
#define CLKCTRL_PDIV_24X_GC (0x0B<<1)  /* 24X */
#define CLKCTRL_PDIV_48X_GC (0x0C<<1)  /* 48X */

/*------------------------------------------------------------------------
CPU - CPU
------------------------------------------------------------------------*/

/* CCP signature select */
#define CCP_SPM_GC (0x9D<<0)  /* SPM Instruction Protection */
#define CCP_IOREG_GC (0xD8<<0)  /* IO Register Protection */

/*------------------------------------------------------------------------
CPUINT - Interrupt Controller
------------------------------------------------------------------------*/


/*------------------------------------------------------------------------
CRCSCAN - CRCSCAN
------------------------------------------------------------------------*/

/* CRC Flash Access Mode select */
#define CRCSCAN_MODE_PRIORITY_GC (0x00<<4)  /* Priority to flash */
#define CRCSCAN_MODE_RESERVED_GC (0x01<<4)  /* Reserved */
#define CRCSCAN_MODE_BACKGROUND_GC (0x02<<4)  /* Lowest priority to flash */
#define CRCSCAN_MODE_CONTINUOUS_GC (0x03<<4)  /* Continuous checks in background */

/* CRC Source select */
#define CRCSCAN_SRC_FLASH_GC (0x00<<0)  /* CRC on entire flash */
#define CRCSCAN_SRC_APPLICATION_GC (0x01<<0)  /* CRC on boot and appl section of flash */
#define CRCSCAN_SRC_BOOT_GC (0x02<<0)  /* CRC on boot section of flash */

/*------------------------------------------------------------------------
DAC - Digital to Analog Converter
------------------------------------------------------------------------*/


/*------------------------------------------------------------------------
EVSYS - Event System
------------------------------------------------------------------------*/

/* Asynchronous Channel 0 Generator Selection select */
#define EVSYS_ASYNCCH0_OFF_GC (0x00<<0)  /* Off */
#define EVSYS_ASYNCCH0_CCL_LUT0_GC (0x01<<0)  /* Configurable Custom Logic LUT0 */
#define EVSYS_ASYNCCH0_CCL_LUT1_GC (0x02<<0)  /* Configurable Custom Logic LUT1 */
#define EVSYS_ASYNCCH0_AC0_OUT_GC (0x03<<0)  /* Analog Comparator 0 out */
#define EVSYS_ASYNCCH0_TCD0_CMPBCLR_GC (0x04<<0)  /* Timer/Counter D0 compare B clear */
#define EVSYS_ASYNCCH0_TCD0_CMPASET_GC (0x05<<0)  /* Timer/Counter D0 compare A set */
#define EVSYS_ASYNCCH0_TCD0_CMPBSET_GC (0x06<<0)  /* Timer/Counter D0 compare B set */
#define EVSYS_ASYNCCH0_TCD0_PROGEV_GC (0x07<<0)  /* Timer/Counter D0 program event */
#define EVSYS_ASYNCCH0_RTC_OVF_GC (0x08<<0)  /* Real Time Counter overflow */
#define EVSYS_ASYNCCH0_RTC_CMP_GC (0x09<<0)  /* Real Time Counter compare */
#define EVSYS_ASYNCCH0_PORTA_PIN0_GC (0x0A<<0)  /* Asynchronous Event from Pin PA0 */
#define EVSYS_ASYNCCH0_PORTA_PIN1_GC (0x0B<<0)  /* Asynchronous Event from Pin PA1 */
#define EVSYS_ASYNCCH0_PORTA_PIN2_GC (0x0C<<0)  /* Asynchronous Event from Pin PA2 */
#define EVSYS_ASYNCCH0_PORTA_PIN3_GC (0x0D<<0)  /* Asynchronous Event from Pin PA3 */
#define EVSYS_ASYNCCH0_PORTA_PIN4_GC (0x0E<<0)  /* Asynchronous Event from Pin PA4 */
#define EVSYS_ASYNCCH0_PORTA_PIN5_GC (0x0F<<0)  /* Asynchronous Event from Pin PA5 */
#define EVSYS_ASYNCCH0_PORTA_PIN6_GC (0x10<<0)  /* Asynchronous Event from Pin PA6 */
#define EVSYS_ASYNCCH0_PORTA_PIN7_GC (0x11<<0)  /* Asynchronous Event from Pin PA7 */
#define EVSYS_ASYNCCH0_UPDI_GC (0x12<<0)  /* Unified Program and debug interface */
#define EVSYS_ASYNCCH0_AC1_OUT_GC (0x13<<0)  /* Analog Comparator 1 out */
#define EVSYS_ASYNCCH0_AC2_OUT_GC (0x14<<0)  /* Analog Comparator 2 out */

/* Asynchronous Channel 1 Generator Selection select */
#define EVSYS_ASYNCCH1_OFF_GC (0x00<<0)  /* Off */
#define EVSYS_ASYNCCH1_CCL_LUT0_GC (0x01<<0)  /* Configurable custom logic LUT0 */
#define EVSYS_ASYNCCH1_CCL_LUT1_GC (0x02<<0)  /* Configurable custom logic LUT1 */
#define EVSYS_ASYNCCH1_AC0_OUT_GC (0x03<<0)  /* Analog Comparator 0 out */
#define EVSYS_ASYNCCH1_TCD0_CMPBCLR_GC (0x04<<0)  /* Timer/Counter D0 compare B clear */
#define EVSYS_ASYNCCH1_TCD0_CMPASET_GC (0x05<<0)  /* Timer/Counter D0 compare A set */
#define EVSYS_ASYNCCH1_TCD0_CMPBSET_GC (0x06<<0)  /* Timer/Counter D0 compare B set */
#define EVSYS_ASYNCCH1_TCD0_PROGEV_GC (0x07<<0)  /* Timer/Counter D0 program event */
#define EVSYS_ASYNCCH1_RTC_OVF_GC (0x08<<0)  /* Real Time Counter overflow */
#define EVSYS_ASYNCCH1_RTC_CMP_GC (0x09<<0)  /* Real Time Counter compare */
#define EVSYS_ASYNCCH1_PORTB_PIN0_GC (0x0A<<0)  /* Asynchronous Event from Pin PB0 */
#define EVSYS_ASYNCCH1_PORTB_PIN1_GC (0x0B<<0)  /* Asynchronous Event from Pin PB1 */
#define EVSYS_ASYNCCH1_PORTB_PIN2_GC (0x0C<<0)  /* Asynchronous Event from Pin PB2 */
#define EVSYS_ASYNCCH1_PORTB_PIN3_GC (0x0D<<0)  /* Asynchronous Event from Pin PB3 */
#define EVSYS_ASYNCCH1_PORTB_PIN4_GC (0x0E<<0)  /* Asynchronous Event from Pin PB4 */
#define EVSYS_ASYNCCH1_PORTB_PIN5_GC (0x0F<<0)  /* Asynchronous Event from Pin PB5 */
#define EVSYS_ASYNCCH1_PORTB_PIN6_GC (0x10<<0)  /* Asynchronous Event from Pin PB6 */
#define EVSYS_ASYNCCH1_PORTB_PIN7_GC (0x11<<0)  /* Asynchronous Event from Pin PB7 */
#define EVSYS_ASYNCCH1_AC1_OUT_GC (0x12<<0)  /* Analog Comparator 1 out */
#define EVSYS_ASYNCCH1_AC2_OUT_GC (0x13<<0)  /* Analog Comparator 2 out */

/* Asynchronous Channel 2 Generator Selection select */
#define EVSYS_ASYNCCH2_OFF_GC (0x00<<0)  /* Off */
#define EVSYS_ASYNCCH2_CCL_LUT0_GC (0x01<<0)  /* Configurable Custom Logic LUT0 */
#define EVSYS_ASYNCCH2_CCL_LUT1_GC (0x02<<0)  /* Configurable Custom Logic LUT1 */
#define EVSYS_ASYNCCH2_AC0_OUT_GC (0x03<<0)  /* Analog Comparator 0 out */
#define EVSYS_ASYNCCH2_TCD0_CMPBCLR_GC (0x04<<0)  /* Timer/Counter D0 compare B clear */
#define EVSYS_ASYNCCH2_TCD0_CMPASET_GC (0x05<<0)  /* Timer/Counter D0 compare A set */
#define EVSYS_ASYNCCH2_TCD0_CMPBSET_GC (0x06<<0)  /* Timer/Counter D0 compare B set */
#define EVSYS_ASYNCCH2_TCD0_PROGEV_GC (0x07<<0)  /* Timer/Counter D0 program event */
#define EVSYS_ASYNCCH2_RTC_OVF_GC (0x08<<0)  /* Real Time Counter overflow */
#define EVSYS_ASYNCCH2_RTC_CMP_GC (0x09<<0)  /* Real Time Counter compare */
#define EVSYS_ASYNCCH2_PORTC_PIN0_GC (0x0A<<0)  /* Asynchronous Event from Pin PC0 */
#define EVSYS_ASYNCCH2_PORTC_PIN1_GC (0x0B<<0)  /* Asynchronous Event from Pin PC1 */
#define EVSYS_ASYNCCH2_PORTC_PIN2_GC (0x0C<<0)  /* Asynchronous Event from Pin PC2 */
#define EVSYS_ASYNCCH2_PORTC_PIN3_GC (0x0D<<0)  /* Asynchronous Event from Pin PC3 */
#define EVSYS_ASYNCCH2_PORTC_PIN4_GC (0x0E<<0)  /* Asynchronous Event from Pin PC4 */
#define EVSYS_ASYNCCH2_PORTC_PIN5_GC (0x0F<<0)  /* Asynchronous Event from Pin PC5 */
#define EVSYS_ASYNCCH2_AC1_OUT_GC (0x10<<0)  /* Analog Comparator 1 out */
#define EVSYS_ASYNCCH2_AC2_OUT_GC (0x11<<0)  /* Analog Comparator 2 out */

/* Asynchronous Channel 3 Generator Selection select */
#define EVSYS_ASYNCCH3_OFF_GC (0x00<<0)  /* Off */
#define EVSYS_ASYNCCH3_CCL_LUT0_GC (0x01<<0)  /* Configurable custom logic LUT0 */
#define EVSYS_ASYNCCH3_CCL_LUT1_GC (0x02<<0)  /* Configurable custom logic LUT1 */
#define EVSYS_ASYNCCH3_AC0_OUT_GC (0x03<<0)  /* Analog Comparator 0 out */
#define EVSYS_ASYNCCH3_TCD0_CMPBCLR_GC (0x04<<0)  /* Timer/Counter type D compare B clear */
#define EVSYS_ASYNCCH3_TCD0_CMPASET_GC (0x05<<0)  /* Timer/Counter type D compare A set */
#define EVSYS_ASYNCCH3_TCD0_CMPBSET_GC (0x06<<0)  /* Timer/Counter type D compare B set */
#define EVSYS_ASYNCCH3_TCD0_PROGEV_GC (0x07<<0)  /* Timer/Counter type D program event */
#define EVSYS_ASYNCCH3_RTC_OVF_GC (0x08<<0)  /* Real Time Counter overflow */
#define EVSYS_ASYNCCH3_RTC_CMP_GC (0x09<<0)  /* Real Time Counter compare */
#define EVSYS_ASYNCCH3_PIT_DIV8192_GC (0x0A<<0)  /* Periodic Interrupt CLK_RTC div 8192 */
#define EVSYS_ASYNCCH3_PIT_DIV4096_GC (0x0B<<0)  /* Periodic Interrupt CLK_RTC div 4096 */
#define EVSYS_ASYNCCH3_PIT_DIV2048_GC (0x0C<<0)  /* Periodic Interrupt CLK_RTC div 2048 */
#define EVSYS_ASYNCCH3_PIT_DIV1024_GC (0x0D<<0)  /* Periodic Interrupt CLK_RTC div 1024 */
#define EVSYS_ASYNCCH3_PIT_DIV512_GC (0x0E<<0)  /* Periodic Interrupt CLK_RTC div 512 */
#define EVSYS_ASYNCCH3_PIT_DIV256_GC (0x0F<<0)  /* Periodic Interrupt CLK_RTC div 256 */
#define EVSYS_ASYNCCH3_PIT_DIV128_GC (0x10<<0)  /* Periodic Interrupt CLK_RTC div 128 */
#define EVSYS_ASYNCCH3_PIT_DIV64_GC (0x11<<0)  /* Periodic Interrupt CLK_RTC div 64 */
#define EVSYS_ASYNCCH3_AC1_OUT_GC (0x12<<0)  /* Analog Comparator 1 out */
#define EVSYS_ASYNCCH3_AC2_OUT_GC (0x13<<0)  /* Analog Comparator 2 out */

/* Asynchronous User Ch 0 Input Selection - TCB0 select */
#define EVSYS_ASYNCUSER0_OFF_GC (0x00<<0)  /* Off */
#define EVSYS_ASYNCUSER0_SYNCCH0_GC (0x01<<0)  /* Synchronous Event Channel 0 */
#define EVSYS_ASYNCUSER0_SYNCCH1_GC (0x02<<0)  /* Synchronous Event Channel 1 */
#define EVSYS_ASYNCUSER0_ASYNCCH0_GC (0x03<<0)  /* Asynchronous Event Channel 0 */
#define EVSYS_ASYNCUSER0_ASYNCCH1_GC (0x04<<0)  /* Asynchronous Event Channel 1 */
#define EVSYS_ASYNCUSER0_ASYNCCH2_GC (0x05<<0)  /* Asynchronous Event Channel 2 */
#define EVSYS_ASYNCUSER0_ASYNCCH3_GC (0x06<<0)  /* Asynchronous Event Channel 3 */

/* Asynchronous User Ch 1 Input Selection - ADC0 select */
#define EVSYS_ASYNCUSER1_OFF_GC (0x00<<0)  /* Off */
#define EVSYS_ASYNCUSER1_SYNCCH0_GC (0x01<<0)  /* Synchronous Event Channel 0 */
#define EVSYS_ASYNCUSER1_SYNCCH1_GC (0x02<<0)  /* Synchronous Event Channel 1 */
#define EVSYS_ASYNCUSER1_ASYNCCH0_GC (0x03<<0)  /* Asynchronous Event Channel 0 */
#define EVSYS_ASYNCUSER1_ASYNCCH1_GC (0x04<<0)  /* Asynchronous Event Channel 1 */
#define EVSYS_ASYNCUSER1_ASYNCCH2_GC (0x05<<0)  /* Asynchronous Event Channel 2 */
#define EVSYS_ASYNCUSER1_ASYNCCH3_GC (0x06<<0)  /* Asynchronous Event Channel 3 */

/* Asynchronous User Ch 10 Input Selection - Event Out 2 select */
#define EVSYS_ASYNCUSER10_OFF_GC (0x00<<0)  /* Off */
#define EVSYS_ASYNCUSER10_SYNCCH0_GC (0x01<<0)  /* Synchronous Event Channel 0 */
#define EVSYS_ASYNCUSER10_SYNCCH1_GC (0x02<<0)  /* Synchronous Event Channel 1 */
#define EVSYS_ASYNCUSER10_ASYNCCH0_GC (0x03<<0)  /* Asynchronous Event Channel 0 */
#define EVSYS_ASYNCUSER10_ASYNCCH1_GC (0x04<<0)  /* Asynchronous Event Channel 1 */
#define EVSYS_ASYNCUSER10_ASYNCCH2_GC (0x05<<0)  /* Asynchronous Event Channel 2 */
#define EVSYS_ASYNCUSER10_ASYNCCH3_GC (0x06<<0)  /* Asynchronous Event Channel 3 */

/* Asynchronous User Ch 11 Input Selection - TCB1 select */
#define EVSYS_ASYNCUSER11_OFF_GC (0x00<<0)  /* Off */
#define EVSYS_ASYNCUSER11_SYNCCH0_GC (0x01<<0)  /* Synchronous Event Channel 0 */
#define EVSYS_ASYNCUSER11_SYNCCH1_GC (0x02<<0)  /* Synchronous Event Channel 1 */
#define EVSYS_ASYNCUSER11_ASYNCCH0_GC (0x03<<0)  /* Asynchronous Event Channel 0 */
#define EVSYS_ASYNCUSER11_ASYNCCH1_GC (0x04<<0)  /* Asynchronous Event Channel 1 */
#define EVSYS_ASYNCUSER11_ASYNCCH2_GC (0x05<<0)  /* Asynchronous Event Channel 2 */
#define EVSYS_ASYNCUSER11_ASYNCCH3_GC (0x06<<0)  /* Asynchronous Event Channel 3 */

/* Asynchronous User Ch 12 Input Selection - ADC0 select */
#define EVSYS_ASYNCUSER12_OFF_GC (0x00<<0)  /* Off */
#define EVSYS_ASYNCUSER12_SYNCCH0_GC (0x01<<0)  /* Synchronous Event Channel 0 */
#define EVSYS_ASYNCUSER12_SYNCCH1_GC (0x02<<0)  /* Synchronous Event Channel 1 */
#define EVSYS_ASYNCUSER12_ASYNCCH0_GC (0x03<<0)  /* Asynchronous Event Channel 0 */
#define EVSYS_ASYNCUSER12_ASYNCCH1_GC (0x04<<0)  /* Asynchronous Event Channel 1 */
#define EVSYS_ASYNCUSER12_ASYNCCH2_GC (0x05<<0)  /* Asynchronous Event Channel 2 */
#define EVSYS_ASYNCUSER12_ASYNCCH3_GC (0x06<<0)  /* Asynchronous Event Channel 3 */

/* Asynchronous User Ch 2 Input Selection - CCL LUT0 Event 0 select */
#define EVSYS_ASYNCUSER2_OFF_GC (0x00<<0)  /* Off */
#define EVSYS_ASYNCUSER2_SYNCCH0_GC (0x01<<0)  /* Synchronous Event Channel 0 */
#define EVSYS_ASYNCUSER2_SYNCCH1_GC (0x02<<0)  /* Synchronous Event Channel 1 */
#define EVSYS_ASYNCUSER2_ASYNCCH0_GC (0x03<<0)  /* Asynchronous Event Channel 0 */
#define EVSYS_ASYNCUSER2_ASYNCCH1_GC (0x04<<0)  /* Asynchronous Event Channel 1 */
#define EVSYS_ASYNCUSER2_ASYNCCH2_GC (0x05<<0)  /* Asynchronous Event Channel 2 */
#define EVSYS_ASYNCUSER2_ASYNCCH3_GC (0x06<<0)  /* Asynchronous Event Channel 3 */

/* Asynchronous User Ch 3 Input Selection - CCL LUT1 Event 0 select */
#define EVSYS_ASYNCUSER3_OFF_GC (0x00<<0)  /* Off */
#define EVSYS_ASYNCUSER3_SYNCCH0_GC (0x01<<0)  /* Synchronous Event Channel 0 */
#define EVSYS_ASYNCUSER3_SYNCCH1_GC (0x02<<0)  /* Synchronous Event Channel 1 */
#define EVSYS_ASYNCUSER3_ASYNCCH0_GC (0x03<<0)  /* Asynchronous Event Channel 0 */
#define EVSYS_ASYNCUSER3_ASYNCCH1_GC (0x04<<0)  /* Asynchronous Event Channel 1 */
#define EVSYS_ASYNCUSER3_ASYNCCH2_GC (0x05<<0)  /* Asynchronous Event Channel 2 */
#define EVSYS_ASYNCUSER3_ASYNCCH3_GC (0x06<<0)  /* Asynchronous Event Channel 3 */

/* Asynchronous User Ch 4 Input Selection - CCL LUT0 Event 1 select */
#define EVSYS_ASYNCUSER4_OFF_GC (0x00<<0)  /* Off */
#define EVSYS_ASYNCUSER4_SYNCCH0_GC (0x01<<0)  /* Synchronous Event Channel 0 */
#define EVSYS_ASYNCUSER4_SYNCCH1_GC (0x02<<0)  /* Synchronous Event Channel 1 */
#define EVSYS_ASYNCUSER4_ASYNCCH0_GC (0x03<<0)  /* Asynchronous Event Channel 0 */
#define EVSYS_ASYNCUSER4_ASYNCCH1_GC (0x04<<0)  /* Asynchronous Event Channel 1 */
#define EVSYS_ASYNCUSER4_ASYNCCH2_GC (0x05<<0)  /* Asynchronous Event Channel 2 */
#define EVSYS_ASYNCUSER4_ASYNCCH3_GC (0x06<<0)  /* Asynchronous Event Channel 3 */

/* Asynchronous User Ch 5 Input Selection - CCL LUT1 Event 1 select */
#define EVSYS_ASYNCUSER5_OFF_GC (0x00<<0)  /* Off */
#define EVSYS_ASYNCUSER5_SYNCCH0_GC (0x01<<0)  /* Synchronous Event Channel 0 */
#define EVSYS_ASYNCUSER5_SYNCCH1_GC (0x02<<0)  /* Synchronous Event Channel 1 */
#define EVSYS_ASYNCUSER5_ASYNCCH0_GC (0x03<<0)  /* Asynchronous Event Channel 0 */
#define EVSYS_ASYNCUSER5_ASYNCCH1_GC (0x04<<0)  /* Asynchronous Event Channel 1 */
#define EVSYS_ASYNCUSER5_ASYNCCH2_GC (0x05<<0)  /* Asynchronous Event Channel 2 */
#define EVSYS_ASYNCUSER5_ASYNCCH3_GC (0x06<<0)  /* Asynchronous Event Channel 3 */

/* Asynchronous User Ch 6 Input Selection - TCD0 Event 0 select */
#define EVSYS_ASYNCUSER6_OFF_GC (0x00<<0)  /* Off */
#define EVSYS_ASYNCUSER6_SYNCCH0_GC (0x01<<0)  /* Synchronous Event Channel 0 */
#define EVSYS_ASYNCUSER6_SYNCCH1_GC (0x02<<0)  /* Synchronous Event Channel 1 */
#define EVSYS_ASYNCUSER6_ASYNCCH0_GC (0x03<<0)  /* Asynchronous Event Channel 0 */
#define EVSYS_ASYNCUSER6_ASYNCCH1_GC (0x04<<0)  /* Asynchronous Event Channel 1 */
#define EVSYS_ASYNCUSER6_ASYNCCH2_GC (0x05<<0)  /* Asynchronous Event Channel 2 */
#define EVSYS_ASYNCUSER6_ASYNCCH3_GC (0x06<<0)  /* Asynchronous Event Channel 3 */

/* Asynchronous User Ch 7 Input Selection - TCD0 Event 1 select */
#define EVSYS_ASYNCUSER7_OFF_GC (0x00<<0)  /* Off */
#define EVSYS_ASYNCUSER7_SYNCCH0_GC (0x01<<0)  /* Synchronous Event Channel 0 */
#define EVSYS_ASYNCUSER7_SYNCCH1_GC (0x02<<0)  /* Synchronous Event Channel 1 */
#define EVSYS_ASYNCUSER7_ASYNCCH0_GC (0x03<<0)  /* Asynchronous Event Channel 0 */
#define EVSYS_ASYNCUSER7_ASYNCCH1_GC (0x04<<0)  /* Asynchronous Event Channel 1 */
#define EVSYS_ASYNCUSER7_ASYNCCH2_GC (0x05<<0)  /* Asynchronous Event Channel 2 */
#define EVSYS_ASYNCUSER7_ASYNCCH3_GC (0x06<<0)  /* Asynchronous Event Channel 3 */

/* Asynchronous User Ch 8 Input Selection - Event Out 0 select */
#define EVSYS_ASYNCUSER8_OFF_GC (0x00<<0)  /* Off */
#define EVSYS_ASYNCUSER8_SYNCCH0_GC (0x01<<0)  /* Synchronous Event Channel 0 */
#define EVSYS_ASYNCUSER8_SYNCCH1_GC (0x02<<0)  /* Synchronous Event Channel 1 */
#define EVSYS_ASYNCUSER8_ASYNCCH0_GC (0x03<<0)  /* Asynchronous Event Channel 0 */
#define EVSYS_ASYNCUSER8_ASYNCCH1_GC (0x04<<0)  /* Asynchronous Event Channel 1 */
#define EVSYS_ASYNCUSER8_ASYNCCH2_GC (0x05<<0)  /* Asynchronous Event Channel 2 */
#define EVSYS_ASYNCUSER8_ASYNCCH3_GC (0x06<<0)  /* Asynchronous Event Channel 3 */

/* Asynchronous User Ch 9 Input Selection - Event Out 1 select */
#define EVSYS_ASYNCUSER9_OFF_GC (0x00<<0)  /* Off */
#define EVSYS_ASYNCUSER9_SYNCCH0_GC (0x01<<0)  /* Synchronous Event Channel 0 */
#define EVSYS_ASYNCUSER9_SYNCCH1_GC (0x02<<0)  /* Synchronous Event Channel 1 */
#define EVSYS_ASYNCUSER9_ASYNCCH0_GC (0x03<<0)  /* Asynchronous Event Channel 0 */
#define EVSYS_ASYNCUSER9_ASYNCCH1_GC (0x04<<0)  /* Asynchronous Event Channel 1 */
#define EVSYS_ASYNCUSER9_ASYNCCH2_GC (0x05<<0)  /* Asynchronous Event Channel 2 */
#define EVSYS_ASYNCUSER9_ASYNCCH3_GC (0x06<<0)  /* Asynchronous Event Channel 3 */

/* Synchronous Channel 0 Generator Selection select */
#define EVSYS_SYNCCH0_OFF_GC (0x00<<0)  /* Off */
#define EVSYS_SYNCCH0_TCB0_GC (0x01<<0)  /* Timer/Counter B0 */
#define EVSYS_SYNCCH0_TCA0_OVF_LUNF_GC (0x02<<0)  /* Timer/Counter A0 overflow */
#define EVSYS_SYNCCH0_TCA0_HUNF_GC (0x03<<0)  /* Timer/Counter A0 underflow high byte (split mode) */
#define EVSYS_SYNCCH0_TCA0_CMP0_GC (0x04<<0)  /* Timer/Counter A0 compare 0 */
#define EVSYS_SYNCCH0_TCA0_CMP1_GC (0x05<<0)  /* Timer/Counter A0 compare 1 */
#define EVSYS_SYNCCH0_TCA0_CMP2_GC (0x06<<0)  /* Timer/Counter A0 compare 2 */
#define EVSYS_SYNCCH0_PORTC_PIN0_GC (0x07<<0)  /* Synchronous Event from Pin PC0 */
#define EVSYS_SYNCCH0_PORTC_PIN1_GC (0x08<<0)  /* Synchronous Event from Pin PC1 */
#define EVSYS_SYNCCH0_PORTC_PIN2_GC (0x09<<0)  /* Synchronous Event from Pin PC2 */
#define EVSYS_SYNCCH0_PORTC_PIN3_GC (0x0A<<0)  /* Synchronous Event from Pin PC3 */
#define EVSYS_SYNCCH0_PORTC_PIN4_GC (0x0B<<0)  /* Synchronous Event from Pin PC4 */
#define EVSYS_SYNCCH0_PORTC_PIN5_GC (0x0C<<0)  /* Synchronous Event from Pin PC5 */
#define EVSYS_SYNCCH0_PORTA_PIN0_GC (0x0D<<0)  /* Synchronous Event from Pin PA0 */
#define EVSYS_SYNCCH0_PORTA_PIN1_GC (0x0E<<0)  /* Synchronous Event from Pin PA1 */
#define EVSYS_SYNCCH0_PORTA_PIN2_GC (0x0F<<0)  /* Synchronous Event from Pin PA2 */
#define EVSYS_SYNCCH0_PORTA_PIN3_GC (0x10<<0)  /* Synchronous Event from Pin PA3 */
#define EVSYS_SYNCCH0_PORTA_PIN4_GC (0x11<<0)  /* Synchronous Event from Pin PA4 */
#define EVSYS_SYNCCH0_PORTA_PIN5_GC (0x12<<0)  /* Synchronous Event from Pin PA5 */
#define EVSYS_SYNCCH0_PORTA_PIN6_GC (0x13<<0)  /* Synchronous Event from Pin PA6 */
#define EVSYS_SYNCCH0_PORTA_PIN7_GC (0x14<<0)  /* Synchronous Event from Pin PA7 */
#define EVSYS_SYNCCH0_TCB1_GC (0x15<<0)  /* Timer/Counter B1 */

/* Synchronous Channel 1 Generator Selection select */
#define EVSYS_SYNCCH1_OFF_GC (0x00<<0)  /* Off */
#define EVSYS_SYNCCH1_TCB0_GC (0x01<<0)  /* Timer/Counter B0 */
#define EVSYS_SYNCCH1_TCA0_OVF_LUNF_GC (0x02<<0)  /* Timer/Counter A0 overflow */
#define EVSYS_SYNCCH1_TCA0_HUNF_GC (0x03<<0)  /* Timer/Counter A0 underflow high byte (split mode) */
#define EVSYS_SYNCCH1_TCA0_CMP0_GC (0x04<<0)  /* Timer/Counter A0 compare 0 */
#define EVSYS_SYNCCH1_TCA0_CMP1_GC (0x05<<0)  /* Timer/Counter A0 compare 1 */
#define EVSYS_SYNCCH1_TCA0_CMP2_GC (0x06<<0)  /* Timer/Counter A0 compare 2 */
#define EVSYS_SYNCCH1_PORTB_PIN0_GC (0x08<<0)  /* Synchronous Event from Pin PB0 */
#define EVSYS_SYNCCH1_PORTB_PIN1_GC (0x09<<0)  /* Synchronous Event from Pin PB1 */
#define EVSYS_SYNCCH1_PORTB_PIN2_GC (0x0A<<0)  /* Synchronous Event from Pin PB2 */
#define EVSYS_SYNCCH1_PORTB_PIN3_GC (0x0B<<0)  /* Synchronous Event from Pin PB3 */
#define EVSYS_SYNCCH1_PORTB_PIN4_GC (0x0C<<0)  /* Synchronous Event from Pin PB4 */
#define EVSYS_SYNCCH1_PORTB_PIN5_GC (0x0D<<0)  /* Synchronous Event from Pin PB5 */
#define EVSYS_SYNCCH1_PORTB_PIN6_GC (0x0E<<0)  /* Synchronous Event from Pin PB6 */
#define EVSYS_SYNCCH1_PORTB_PIN7_GC (0x0F<<0)  /* Synchronous Event from Pin PB7 */
#define EVSYS_SYNCCH1_TCB1_GC (0x10<<0)  /* Timer/Counter B1 */

/* Synchronous User Ch 0 - TCA0 select */
#define EVSYS_SYNCUSER0_OFF_GC (0x00<<0)  /* Off */
#define EVSYS_SYNCUSER0_SYNCCH0_GC (0x01<<0)  /* Synchronous Event Channel 0 */
#define EVSYS_SYNCUSER0_SYNCCH1_GC (0x02<<0)  /* Synchronous Event Channel 1 */

/* Synchronous User Ch 1 - USART0 select */
#define EVSYS_SYNCUSER1_OFF_GC (0x00<<0)  /* Off */
#define EVSYS_SYNCUSER1_SYNCCH0_GC (0x01<<0)  /* Synchronous Event Channel 0 */
#define EVSYS_SYNCUSER1_SYNCCH1_GC (0x02<<0)  /* Synchronous Event Channel 1 */

/*------------------------------------------------------------------------
FUSE - Fuses
------------------------------------------------------------------------*/

/* BOD Operation in Active Mode select */
#define ACTIVE_DIS_GC (0x00<<2)  /* Disabled */
#define ACTIVE_ENABLED_GC (0x01<<2)  /* Enabled */
#define ACTIVE_SAMPLED_GC (0x02<<2)  /* Sampled */
#define ACTIVE_ENWAKE_GC (0x03<<2)  /* Enabled with wake-up halted until BOD is ready */

/* CRC Source select */
#define CRCSRC_FLASH_GC (0x00<<6)  /* The CRC is performed on the entire Flash (boot, application code and application data section). */
#define CRCSRC_BOOT_GC (0x01<<6)  /* The CRC is performed on the boot section of Flash */
#define CRCSRC_BOOTAPP_GC (0x02<<6)  /* The CRC is performed on the boot and application code section of Flash */
#define CRCSRC_NOCRC_GC (0x03<<6)  /* Disable CRC. */

/* Frequency Select select */
#define FREQSEL_16MHZ_GC (0x01<<0)  /* 16 MHz */
#define FREQSEL_20MHZ_GC (0x02<<0)  /* 20 MHz */

/* BOD Level select */
#define LVL_BODLEVEL0_GC (0x00<<5)  /* 1.8 V */
#define LVL_BODLEVEL2_GC (0x02<<5)  /* 2.6 V */
#define LVL_BODLEVEL7_GC (0x07<<5)  /* 4.2 V */

/* Watchdog Timeout Period select */
#define PERIOD_OFF_GC (0x00<<0)  /* Off */
#define PERIOD_8CLK_GC (0x01<<0)  /* 8 cycles (8ms) */
#define PERIOD_16CLK_GC (0x02<<0)  /* 16 cycles (16ms) */
#define PERIOD_32CLK_GC (0x03<<0)  /* 32 cycles (32ms) */
#define PERIOD_64CLK_GC (0x04<<0)  /* 64 cycles (64ms) */
#define PERIOD_128CLK_GC (0x05<<0)  /* 128 cycles (0.128s) */
#define PERIOD_256CLK_GC (0x06<<0)  /* 256 cycles (0.256s) */
#define PERIOD_512CLK_GC (0x07<<0)  /* 512 cycles (0.512s) */
#define PERIOD_1KCLK_GC (0x08<<0)  /* 1K cycles (1.0s) */
#define PERIOD_2KCLK_GC (0x09<<0)  /* 2K cycles (2.0s) */
#define PERIOD_4KCLK_GC (0x0A<<0)  /* 4K cycles (4.1s) */
#define PERIOD_8KCLK_GC (0x0B<<0)  /* 8K cycles (8.2s) */

/* Reset Pin Configuration select */
#define RSTPINCFG_GPIO_GC (0x00<<2)  /* GPIO mode */
#define RSTPINCFG_UPDI_GC (0x01<<2)  /* UPDI mode */
#define RSTPINCFG_RST_GC (0x02<<2)  /* Reset mode */

/* BOD Sample Frequency select */
#define SAMPFREQ_1KHZ_GC (0x00<<4)  /* 1kHz sampling frequency */
#define SAMPFREQ_125HZ_GC (0x01<<4)  /* 125Hz sampling frequency */

/* BOD Operation in Sleep Mode select */
#define SLEEP_DIS_GC (0x00<<0)  /* Disabled */
#define SLEEP_ENABLED_GC (0x01<<0)  /* Enabled */
#define SLEEP_SAMPLED_GC (0x02<<0)  /* Sampled */

/* Startup Time select */
#define SUT_0MS_GC (0x00<<0)  /* 0 ms */
#define SUT_1MS_GC (0x01<<0)  /* 1 ms */
#define SUT_2MS_GC (0x02<<0)  /* 2 ms */
#define SUT_4MS_GC (0x03<<0)  /* 4 ms */
#define SUT_8MS_GC (0x04<<0)  /* 8 ms */
#define SUT_16MS_GC (0x05<<0)  /* 16 ms */
#define SUT_32MS_GC (0x06<<0)  /* 32 ms */
#define SUT_64MS_GC (0x07<<0)  /* 64 ms */

/* Watchdog Window Timeout Period select */
#define WINDOW_OFF_GC (0x00<<4)  /* Off */
#define WINDOW_8CLK_GC (0x01<<4)  /* 8 cycles (8ms) */
#define WINDOW_16CLK_GC (0x02<<4)  /* 16 cycles (16ms) */
#define WINDOW_32CLK_GC (0x03<<4)  /* 32 cycles (32ms) */
#define WINDOW_64CLK_GC (0x04<<4)  /* 64 cycles (64ms) */
#define WINDOW_128CLK_GC (0x05<<4)  /* 128 cycles (0.128s) */
#define WINDOW_256CLK_GC (0x06<<4)  /* 256 cycles (0.256s) */
#define WINDOW_512CLK_GC (0x07<<4)  /* 512 cycles (0.512s) */
#define WINDOW_1KCLK_GC (0x08<<4)  /* 1K cycles (1.0s) */
#define WINDOW_2KCLK_GC (0x09<<4)  /* 2K cycles (2.0s) */
#define WINDOW_4KCLK_GC (0x0A<<4)  /* 4K cycles (4.1s) */
#define WINDOW_8KCLK_GC (0x0B<<4)  /* 8K cycles (8.2s) */

/*------------------------------------------------------------------------
LOCKBIT - Lockbit
------------------------------------------------------------------------*/

/* Lock Bits select */
#define LB_RWLOCK_GC (0x3A<<0)  /* Read and write lock */
#define LB_NOLOCK_GC (0xC5<<0)  /* No locks */

/*------------------------------------------------------------------------
NVMCTRL - Non-volatile Memory Controller
------------------------------------------------------------------------*/

/* Command select */
#define NVMCTRL_CMD_NONE_GC (0x00<<0)  /* No Command */
#define NVMCTRL_CMD_PAGEWRITE_GC (0x01<<0)  /* Write page */
#define NVMCTRL_CMD_PAGEERASE_GC (0x02<<0)  /* Erase page */
#define NVMCTRL_CMD_PAGEERASEWRITE_GC (0x03<<0)  /* Erase and write page */
#define NVMCTRL_CMD_PAGEBUFCLR_GC (0x04<<0)  /* Page buffer clear */
#define NVMCTRL_CMD_CHIPERASE_GC (0x05<<0)  /* Chip erase */
#define NVMCTRL_CMD_EEERASE_GC (0x06<<0)  /* EEPROM erase */
#define NVMCTRL_CMD_FUSEWRITE_GC (0x07<<0)  /* Write fuse (PDI only) */

/*------------------------------------------------------------------------
PORT - I/O Ports
------------------------------------------------------------------------*/

/* Input/Sense Configuration select */
#define PORT_ISC_INTDISABLE_GC (0x00<<0)  /* Interrupt disabled but input buffer enabled */
#define PORT_ISC_BOTHEDGES_GC (0x01<<0)  /* Sense Both Edges */
#define PORT_ISC_RISING_GC (0x02<<0)  /* Sense Rising Edge */
#define PORT_ISC_FALLING_GC (0x03<<0)  /* Sense Falling Edge */
#define PORT_ISC_INPUT_DISABLE_GC (0x04<<0)  /* Digital Input Buffer disabled */
#define PORT_ISC_LEVEL_GC (0x05<<0)  /* Sense low Level */

/*------------------------------------------------------------------------
PORTMUX - Port Multiplexer
------------------------------------------------------------------------*/

/* Configurable Custom Logic LUT0 select */
#define PORTMUX_LUT0_DEFAULT_GC (0x00<<4)  /* Default pin */
#define PORTMUX_LUT0_ALTERNATE_GC (0x01<<4)  /* Alternate pin */

/* Configurable Custom Logic LUT1 select */
#define PORTMUX_LUT1_DEFAULT_GC (0x00<<5)  /* Default pin */
#define PORTMUX_LUT1_ALTERNATE_GC (0x01<<5)  /* Alternate pin */

/* Port Multiplexer SPI0 select */
#define PORTMUX_SPI0_DEFAULT_GC (0x00<<2)  /* Default pins */
#define PORTMUX_SPI0_ALTERNATE_GC (0x01<<2)  /* Alternate pins */

/* Port Multiplexer TCA0 Output 0 select */
#define PORTMUX_TCA00_DEFAULT_GC (0x00<<0)  /* Default pin */
#define PORTMUX_TCA00_ALTERNATE_GC (0x01<<0)  /* Alternate pin */

/* Port Multiplexer TCA0 Output 1 select */
#define PORTMUX_TCA01_DEFAULT_GC (0x00<<1)  /* Default pin */
#define PORTMUX_TCA01_ALTERNATE_GC (0x01<<1)  /* Alternate pin */

/* Port Multiplexer TCA0 Output 2 select */
#define PORTMUX_TCA02_DEFAULT_GC (0x00<<2)  /* Default pin */
#define PORTMUX_TCA02_ALTERNATE_GC (0x01<<2)  /* Alternate pin */

/* Port Multiplexer TCA0 Output 3 select */
#define PORTMUX_TCA03_DEFAULT_GC (0x00<<3)  /* Default pin */
#define PORTMUX_TCA03_ALTERNATE_GC (0x01<<3)  /* Alternate pin */

/* Port Multiplexer TCA0 Output 4 select */
#define PORTMUX_TCA04_DEFAULT_GC (0x00<<4)  /* Default pin */
#define PORTMUX_TCA04_ALTERNATE_GC (0x01<<4)  /* Alternate pin */

/* Port Multiplexer TCA0 Output 5 select */
#define PORTMUX_TCA05_DEFAULT_GC (0x00<<5)  /* Default pin */
#define PORTMUX_TCA05_ALTERNATE_GC (0x01<<5)  /* Alternate pin */

/* Port Multiplexer TCB0 select */
#define PORTMUX_TCB0_DEFAULT_GC (0x00<<0)  /* Default pin */
#define PORTMUX_TCB0_ALTERNATE_GC (0x01<<0)  /* Alternate pin */

/* Port Multiplexer TCB1 select */
#define PORTMUX_TCB1_DEFAULT_GC (0x00<<1)  /* Default pin */
#define PORTMUX_TCB1_ALTERNATE_GC (0x01<<1)  /* Alternate pin */

/* Port Multiplexer TWI0 select */
#define PORTMUX_TWI0_DEFAULT_GC (0x00<<4)  /* Default pins */
#define PORTMUX_TWI0_ALTERNATE_GC (0x01<<4)  /* Alternate pins */

/* Port Multiplexer USART0 select */
#define PORTMUX_USART0_DEFAULT_GC (0x00<<0)  /* Default pins */
#define PORTMUX_USART0_ALTERNATE_GC (0x01<<0)  /* Alternate pins */

/*------------------------------------------------------------------------
RSTCTRL - Reset controller
------------------------------------------------------------------------*/


/*------------------------------------------------------------------------
RTC - Real-Time Counter
------------------------------------------------------------------------*/

/* Clock Select select */
#define RTC_CLKSEL_INT32K_GC (0x00<<0)  /* Internal 32kHz OSC */
#define RTC_CLKSEL_INT1K_GC (0x01<<0)  /* Internal 1kHz OSC */
#define RTC_CLKSEL_TOSC32K_GC (0x02<<0)  /* 32KHz Crystal OSC */
#define RTC_CLKSEL_EXTCLK_GC (0x03<<0)  /* External Clock */

/* Period select */
#define RTC_PERIOD_OFF_GC (0x00<<3)  /* Off */
#define RTC_PERIOD_CYC4_GC (0x01<<3)  /* RTC Clock Cycles 4 */
#define RTC_PERIOD_CYC8_GC (0x02<<3)  /* RTC Clock Cycles 8 */
#define RTC_PERIOD_CYC16_GC (0x03<<3)  /* RTC Clock Cycles 16 */
#define RTC_PERIOD_CYC32_GC (0x04<<3)  /* RTC Clock Cycles 32 */
#define RTC_PERIOD_CYC64_GC (0x05<<3)  /* RTC Clock Cycles 64 */
#define RTC_PERIOD_CYC128_GC (0x06<<3)  /* RTC Clock Cycles 128 */
#define RTC_PERIOD_CYC256_GC (0x07<<3)  /* RTC Clock Cycles 256 */
#define RTC_PERIOD_CYC512_GC (0x08<<3)  /* RTC Clock Cycles 512 */
#define RTC_PERIOD_CYC1024_GC (0x09<<3)  /* RTC Clock Cycles 1024 */
#define RTC_PERIOD_CYC2048_GC (0x0A<<3)  /* RTC Clock Cycles 2048 */
#define RTC_PERIOD_CYC4096_GC (0x0B<<3)  /* RTC Clock Cycles 4096 */
#define RTC_PERIOD_CYC8192_GC (0x0C<<3)  /* RTC Clock Cycles 8192 */
#define RTC_PERIOD_CYC16384_GC (0x0D<<3)  /* RTC Clock Cycles 16384 */
#define RTC_PERIOD_CYC32768_GC (0x0E<<3)  /* RTC Clock Cycles 32768 */

/* Prescaling Factor select */
#define RTC_PRESCALER_DIV1_GC (0x00<<3)  /* RTC Clock / 1 */
#define RTC_PRESCALER_DIV2_GC (0x01<<3)  /* RTC Clock / 2 */
#define RTC_PRESCALER_DIV4_GC (0x02<<3)  /* RTC Clock / 4 */
#define RTC_PRESCALER_DIV8_GC (0x03<<3)  /* RTC Clock / 8 */
#define RTC_PRESCALER_DIV16_GC (0x04<<3)  /* RTC Clock / 16 */
#define RTC_PRESCALER_DIV32_GC (0x05<<3)  /* RTC Clock / 32 */
#define RTC_PRESCALER_DIV64_GC (0x06<<3)  /* RTC Clock / 64 */
#define RTC_PRESCALER_DIV128_GC (0x07<<3)  /* RTC Clock / 128 */
#define RTC_PRESCALER_DIV256_GC (0x08<<3)  /* RTC Clock / 256 */
#define RTC_PRESCALER_DIV512_GC (0x09<<3)  /* RTC Clock / 512 */
#define RTC_PRESCALER_DIV1024_GC (0x0A<<3)  /* RTC Clock / 1024 */
#define RTC_PRESCALER_DIV2048_GC (0x0B<<3)  /* RTC Clock / 2048 */
#define RTC_PRESCALER_DIV4096_GC (0x0C<<3)  /* RTC Clock / 4096 */
#define RTC_PRESCALER_DIV8192_GC (0x0D<<3)  /* RTC Clock / 8192 */
#define RTC_PRESCALER_DIV16384_GC (0x0E<<3)  /* RTC Clock / 16384 */
#define RTC_PRESCALER_DIV32768_GC (0x0F<<3)  /* RTC Clock / 32768 */

/*------------------------------------------------------------------------
SIGROW - Signature row
------------------------------------------------------------------------*/


/*------------------------------------------------------------------------
SLPCTRL - Sleep Controller
------------------------------------------------------------------------*/

/* Sleep mode select */
#define SLPCTRL_SMODE_IDLE_GC (0x00<<1)  /* Idle mode */
#define SLPCTRL_SMODE_STDBY_GC (0x01<<1)  /* Standby Mode */
#define SLPCTRL_SMODE_PDOWN_GC (0x02<<1)  /* Power-down Mode */

/*------------------------------------------------------------------------
SPI - Serial Peripheral Interface
------------------------------------------------------------------------*/

/* SPI Mode select */
#define SPI_MODE_0_GC (0x00<<0)  /* SPI Mode 0 */
#define SPI_MODE_1_GC (0x01<<0)  /* SPI Mode 1 */
#define SPI_MODE_2_GC (0x02<<0)  /* SPI Mode 2 */
#define SPI_MODE_3_GC (0x03<<0)  /* SPI Mode 3 */

/* Prescaler select */
#define SPI_PRESC_DIV4_GC (0x00<<1)  /* System Clock / 4 */
#define SPI_PRESC_DIV16_GC (0x01<<1)  /* System Clock / 16 */
#define SPI_PRESC_DIV64_GC (0x02<<1)  /* System Clock / 64 */
#define SPI_PRESC_DIV128_GC (0x03<<1)  /* System Clock / 128 */

/*------------------------------------------------------------------------
SYSCFG - System Configuration Registers
------------------------------------------------------------------------*/


/*------------------------------------------------------------------------
TCA - 16-bit Timer/Counter Type A
------------------------------------------------------------------------*/

/* Clock Selection select */
#define TCA_SINGLE_CLKSEL_DIV1_GC (0x00<<1)  /* System Clock */
#define TCA_SINGLE_CLKSEL_DIV2_GC (0x01<<1)  /* System Clock / 2 */
#define TCA_SINGLE_CLKSEL_DIV4_GC (0x02<<1)  /* System Clock / 4 */
#define TCA_SINGLE_CLKSEL_DIV8_GC (0x03<<1)  /* System Clock / 8 */
#define TCA_SINGLE_CLKSEL_DIV16_GC (0x04<<1)  /* System Clock / 16 */
#define TCA_SINGLE_CLKSEL_DIV64_GC (0x05<<1)  /* System Clock / 64 */
#define TCA_SINGLE_CLKSEL_DIV256_GC (0x06<<1)  /* System Clock / 256 */
#define TCA_SINGLE_CLKSEL_DIV1024_GC (0x07<<1)  /* System Clock / 1024 */

/* Command select */
#define TCA_SINGLE_CMD_NONE_GC (0x00<<2)  /* No Command */
#define TCA_SINGLE_CMD_UPDATE_GC (0x01<<2)  /* Force Update */
#define TCA_SINGLE_CMD_RESTART_GC (0x02<<2)  /* Force Restart */
#define TCA_SINGLE_CMD_RESET_GC (0x03<<2)  /* Force Hard Reset */

/* Direction select */
#define TCA_SINGLE_DIR_UP_GC (0x00<<0)  /* Count up */
#define TCA_SINGLE_DIR_DOWN_GC (0x01<<0)  /* Count down */

/* Event Action select */
#define TCA_SINGLE_EVACT_POSEDGE_GC (0x00<<1)  /* Count on positive edge event */
#define TCA_SINGLE_EVACT_ANYEDGE_GC (0x01<<1)  /* Count on any edge event */
#define TCA_SINGLE_EVACT_HIGHLVL_GC (0x02<<1)  /* Count on prescaled clock while event line is 1. */
#define TCA_SINGLE_EVACT_UPDOWN_GC (0x03<<1)  /* Count on prescaled clock. Event controls count direction. Up-count when event line is 0, down-count when event line is 1. */

/* Waveform generation mode select */
#define TCA_SINGLE_WGMODE_NORMAL_GC (0x00<<0)  /* Normal Mode */
#define TCA_SINGLE_WGMODE_FRQ_GC (0x01<<0)  /* Frequency Generation Mode */
#define TCA_SINGLE_WGMODE_SINGLESLOPE_GC (0x03<<0)  /* Single Slope PWM */
#define TCA_SINGLE_WGMODE_DSTOP_GC (0x05<<0)  /* Dual Slope PWM, overflow on TOP */
#define TCA_SINGLE_WGMODE_DSBOTH_GC (0x06<<0)  /* Dual Slope PWM, overflow on TOP and BOTTOM */
#define TCA_SINGLE_WGMODE_DSBOTTOM_GC (0x07<<0)  /* Dual Slope PWM, overflow on BOTTOM */

/* Clock Selection select */
#define TCA_SPLIT_CLKSEL_DIV1_GC (0x00<<1)  /* System Clock */
#define TCA_SPLIT_CLKSEL_DIV2_GC (0x01<<1)  /* System Clock / 2 */
#define TCA_SPLIT_CLKSEL_DIV4_GC (0x02<<1)  /* System Clock / 4 */
#define TCA_SPLIT_CLKSEL_DIV8_GC (0x03<<1)  /* System Clock / 8 */
#define TCA_SPLIT_CLKSEL_DIV16_GC (0x04<<1)  /* System Clock / 16 */
#define TCA_SPLIT_CLKSEL_DIV64_GC (0x05<<1)  /* System Clock / 64 */
#define TCA_SPLIT_CLKSEL_DIV256_GC (0x06<<1)  /* System Clock / 256 */
#define TCA_SPLIT_CLKSEL_DIV1024_GC (0x07<<1)  /* System Clock / 1024 */

/* Command select */
#define TCA_SPLIT_CMD_NONE_GC (0x00<<2)  /* No Command */
#define TCA_SPLIT_CMD_UPDATE_GC (0x01<<2)  /* Force Update */
#define TCA_SPLIT_CMD_RESTART_GC (0x02<<2)  /* Force Restart */
#define TCA_SPLIT_CMD_RESET_GC (0x03<<2)  /* Force Hard Reset */

/*------------------------------------------------------------------------
TCB - 16-bit Timer Type B
------------------------------------------------------------------------*/

/* Clock Select select */
#define TCB_CLKSEL_CLKDIV1_GC (0x00<<1)  /* CLK_PER (No Prescaling) */
#define TCB_CLKSEL_CLKDIV2_GC (0x01<<1)  /* CLK_PER/2 (From Prescaler) */
#define TCB_CLKSEL_CLKTCA_GC (0x02<<1)  /* Use Clock from TCA */

/* Timer Mode select */
#define TCB_CNTMODE_INT_GC (0x00<<0)  /* Periodic Interrupt */
#define TCB_CNTMODE_TIMEOUT_GC (0x01<<0)  /* Periodic Timeout */
#define TCB_CNTMODE_CAPT_GC (0x02<<0)  /* Input Capture Event */
#define TCB_CNTMODE_FRQ_GC (0x03<<0)  /* Input Capture Frequency measurement */
#define TCB_CNTMODE_PW_GC (0x04<<0)  /* Input Capture Pulse-Width measurement */
#define TCB_CNTMODE_FRQPW_GC (0x05<<0)  /* Input Capture Frequency and Pulse-Width measurement */
#define TCB_CNTMODE_SINGLE_GC (0x06<<0)  /* Single Shot */
#define TCB_CNTMODE_PWM8_GC (0x07<<0)  /* 8-bit PWM */

/*------------------------------------------------------------------------
TCD - Timer Counter D
------------------------------------------------------------------------*/

/* event action select */
#define TCD_ACTION_FAULT_GC (0x00<<2)  /* Event trigger a fault */
#define TCD_ACTION_CAPTURE_GC (0x01<<2)  /* Event trigger a fault and capture */

/* event config select */
#define TCD_CFG_NEITHER_GC (0x00<<6)  /* Neither Filter nor Asynchronous Event is enabled */
#define TCD_CFG_FILTER_GC (0x01<<6)  /* Input Capture Noise Cancellation Filter enabled */
#define TCD_CFG_ASYNC_GC (0x02<<6)  /* Asynchronous Event output qualification enabled */

/* clock select select */
#define TCD_CLKSEL_20MHZ_GC (0x00<<5)  /* 20 MHz oscillator */
#define TCD_CLKSEL_EXTCLK_GC (0x02<<5)  /* External clock */
#define TCD_CLKSEL_SYSCLK_GC (0x03<<5)  /* System clock */

/* Compare C output select select */
#define TCD_CMPCSEL_PWMA_GC (0x00<<6)  /* PWM A output */
#define TCD_CMPCSEL_PWMB_GC (0x01<<6)  /* PWM B output */

/* Compare D output select select */
#define TCD_CMPDSEL_PWMA_GC (0x00<<7)  /* PWM A output */
#define TCD_CMPDSEL_PWMB_GC (0x01<<7)  /* PWM B output */

/* counter prescaler select */
#define TCD_CNTPRES_DIV1_GC (0x00<<3)  /* Sync clock divided by 1 */
#define TCD_CNTPRES_DIV4_GC (0x01<<3)  /* Sync clock divided by 4 */
#define TCD_CNTPRES_DIV32_GC (0x02<<3)  /* Sync clock divided by 32 */

/* dither select select */
#define TCD_DITHERSEL_ONTIMEB_GC (0x00<<0)  /* On-time ramp B */
#define TCD_DITHERSEL_ONTIMEAB_GC (0x01<<0)  /* On-time ramp A and B */
#define TCD_DITHERSEL_DEADTIMEB_GC (0x02<<0)  /* Dead-time rampB */
#define TCD_DITHERSEL_DEADTIMEAB_GC (0x03<<0)  /* Dead-time ramp A and B */

/* Delay prescaler select */
#define TCD_DLYPRESC_DIV1_GC (0x00<<4)  /* No prescaling */
#define TCD_DLYPRESC_DIV2_GC (0x01<<4)  /* Prescale with 2 */
#define TCD_DLYPRESC_DIV4_GC (0x02<<4)  /* Prescale with 4 */
#define TCD_DLYPRESC_DIV8_GC (0x03<<4)  /* Prescale with 8 */

/* Delay select select */
#define TCD_DLYSEL_OFF_GC (0x00<<0)  /* No delay */
#define TCD_DLYSEL_INBLANK_GC (0x01<<0)  /* Input blanking enabled */
#define TCD_DLYSEL_EVENT_GC (0x02<<0)  /* Event delay enabled */

/* Delay trigger select */
#define TCD_DLYTRIG_CMPASET_GC (0x00<<2)  /* Compare A set */
#define TCD_DLYTRIG_CMPACLR_GC (0x01<<2)  /* Compare A clear */
#define TCD_DLYTRIG_CMPBSET_GC (0x02<<2)  /* Compare B set */
#define TCD_DLYTRIG_CMPBCLR_GC (0x03<<2)  /* Compare B clear */

/* edge select select */
#define TCD_EDGE_FALL_LOW_GC (0x00<<4)  /* The falling edge or low level of event generates retrigger or fault action */
#define TCD_EDGE_RISE_HIGH_GC (0x01<<4)  /* The rising edge or high level of event generates retrigger or fault action */

/* Input mode select */
#define TCD_INPUTMODE_NONE_GC (0x00<<0)  /* Input has no actions */
#define TCD_INPUTMODE_JMPWAIT_GC (0x01<<0)  /* Stop output, jump to opposite compare cycle and wait */
#define TCD_INPUTMODE_EXECWAIT_GC (0x02<<0)  /* Stop output, execute opposite compare cycle and wait */
#define TCD_INPUTMODE_EXECFAULT_GC (0x03<<0)  /* stop output, execute opposite compare cycle while fault active */
#define TCD_INPUTMODE_FREQ_GC (0x04<<0)  /* Stop all outputs, maintain frequency */
#define TCD_INPUTMODE_EXECDT_GC (0x05<<0)  /* Stop all outputs, execute dead time while fault active */
#define TCD_INPUTMODE_WAIT_GC (0x06<<0)  /* Stop all outputs, jump to next compare cycle and wait */
#define TCD_INPUTMODE_WAITSW_GC (0x07<<0)  /* Stop all outputs, wait for software action */
#define TCD_INPUTMODE_EDGETRIG_GC (0x08<<0)  /* Stop output on edge, jump to next compare cycle */
#define TCD_INPUTMODE_EDGETRIGFREQ_GC (0x09<<0)  /* Stop output on edge, maintain frequency */
#define TCD_INPUTMODE_LVLTRIGFREQ_GC (0x0A<<0)  /* Stop output at level, maintain frequency */

/* Syncronization prescaler select */
#define TCD_SYNCPRES_DIV1_GC (0x00<<1)  /* Selevted clock source divided by 1 */
#define TCD_SYNCPRES_DIV2_GC (0x01<<1)  /* Selevted clock source divided by 2 */
#define TCD_SYNCPRES_DIV4_GC (0x02<<1)  /* Selevted clock source divided by 4 */
#define TCD_SYNCPRES_DIV8_GC (0x03<<1)  /* Selevted clock source divided by 8 */

/* Waveform generation mode select */
#define TCD_WGMODE_ONERAMP_GC (0x00<<0)  /* One ramp mode */
#define TCD_WGMODE_TWORAMP_GC (0x01<<0)  /* Two ramp mode */
#define TCD_WGMODE_FOURRAMP_GC (0x02<<0)  /* Four ramp mode */
#define TCD_WGMODE_DS_GC (0x03<<0)  /* Dual slope mode */

/*------------------------------------------------------------------------
TWI - Two-Wire Interface
------------------------------------------------------------------------*/

/* Acknowledge Action select */
#define TWI_ACKACT_ACK_GC (0x00<<2)  /* Send ACK */
#define TWI_ACKACT_NACK_GC (0x01<<2)  /* Send NACK */

/* Slave Address or Stop select */
#define TWI_AP_STOP_GC (0x00<<0)  /* Stop condition generated APIF */
#define TWI_AP_ADR_GC (0x01<<0)  /* Address detection generated APIF */

/* Bus State select */
#define TWI_BUSSTATE_UNKNOWN_GC (0x00<<0)  /* Unknown Bus State */
#define TWI_BUSSTATE_IDLE_GC (0x01<<0)  /* Bus is Idle */
#define TWI_BUSSTATE_OWNER_GC (0x02<<0)  /* This Module Controls The Bus */
#define TWI_BUSSTATE_BUSY_GC (0x03<<0)  /* The Bus is Busy */

/* Command select */
#define TWI_MCMD_NOACT_GC (0x00<<0)  /* No Action */
#define TWI_MCMD_REPSTART_GC (0x01<<0)  /* Issue Repeated Start Condition */
#define TWI_MCMD_RECVTRANS_GC (0x02<<0)  /* Receive or Transmit Data, depending on DIR */
#define TWI_MCMD_STOP_GC (0x03<<0)  /* Issue Stop Condition */

/* Command select */
#define TWI_SCMD_NOACT_GC (0x00<<0)  /* No Action */
#define TWI_SCMD_COMPTRANS_GC (0x02<<0)  /* Used To Complete a Transaction */
#define TWI_SCMD_RESPONSE_GC (0x03<<0)  /* Used in Response to Address/Data Interrupt */

/* SDA Hold Time select */
#define TWI_SDAHOLD_OFF_GC (0x00<<2)  /* SDA hold time off */
#define TWI_SDAHOLD_50NS_GC (0x01<<2)  /* Typical 50ns hold time */
#define TWI_SDAHOLD_300NS_GC (0x02<<2)  /* Typical 300ns hold time */
#define TWI_SDAHOLD_500NS_GC (0x03<<2)  /* Typical 500ns hold time */

/* SDA Setup Time select */
#define TWI_SDASETUP_4CYC_GC (0x00<<4)  /* SDA setup time is 4 clock cycles */
#define TWI_SDASETUP_8CYC_GC (0x01<<4)  /* SDA setup time is 8 clock cycles */

/* Inactive Bus Timeout select */
#define TWI_TIMEOUT_DISABLED_GC (0x00<<2)  /* Bus Timeout Disabled */
#define TWI_TIMEOUT_50US_GC (0x01<<2)  /* 50 Microseconds */
#define TWI_TIMEOUT_100US_GC (0x02<<2)  /* 100 Microseconds */
#define TWI_TIMEOUT_200US_GC (0x03<<2)  /* 200 Microseconds */

/*------------------------------------------------------------------------
USART - Universal Synchronous and Asynchronous Receiver and Transmitter
------------------------------------------------------------------------*/

/* Character Size select */
#define USART_CHSIZE_5BIT_GC (0x00<<0)  /* Character size: 5 bit */
#define USART_CHSIZE_6BIT_GC (0x01<<0)  /* Character size: 6 bit */
#define USART_CHSIZE_7BIT_GC (0x02<<0)  /* Character size: 7 bit */
#define USART_CHSIZE_8BIT_GC (0x03<<0)  /* Character size: 8 bit */
#define USART_CHSIZE_9BITL_GC (0x06<<0)  /* Character size: 9 bit read low byte first */
#define USART_CHSIZE_9BITH_GC (0x07<<0)  /* Character size: 9 bit read high byte first */

/* Communication Mode select */
#define USART_CMODE_ASYNCHRONOUS_GC (0x00<<6)  /* Asynchronous Mode */
#define USART_CMODE_SYNCHRONOUS_GC (0x01<<6)  /* Synchronous Mode */
#define USART_CMODE_IRCOM_GC (0x02<<6)  /* Infrared Communication */
#define USART_CMODE_MSPI_GC (0x03<<6)  /* Master SPI Mode */

/* Parity Mode select */
#define USART_PMODE_DISABLED_GC (0x00<<4)  /* No Parity */
#define USART_PMODE_EVEN_GC (0x02<<4)  /* Even Parity */
#define USART_PMODE_ODD_GC (0x03<<4)  /* Odd Parity */

/* RS485 Mode internal transmitter select */
#define USART_RS485_OFF_GC (0x00<<0)  /* RS485 Mode disabled */
#define USART_RS485_EXT_GC (0x01<<0)  /* RS485 Mode External drive */
#define USART_RS485_INT_GC (0x02<<0)  /* RS485 Mode Internal drive */

/* Receiver Mode select */
#define USART_RXMODE_NORMAL_GC (0x00<<1)  /* Normal mode */
#define USART_RXMODE_CLK2X_GC (0x01<<1)  /* CLK2x mode */
#define USART_RXMODE_GENAUTO_GC (0x02<<1)  /* Generic autobaud mode */
#define USART_RXMODE_LINAUTO_GC (0x03<<1)  /* LIN constrained autobaud mode */

/* Stop Bit Mode select */
#define USART_SBMODE_1BIT_GC (0x00<<3)  /* 1 stop bit */
#define USART_SBMODE_2BIT_GC (0x01<<3)  /* 2 stop bits */

/*------------------------------------------------------------------------
USERROW - User Row
------------------------------------------------------------------------*/


/*------------------------------------------------------------------------
VPORT - Virtual Ports
------------------------------------------------------------------------*/


/*------------------------------------------------------------------------
VREF - Voltage reference
------------------------------------------------------------------------*/

/* ADC0 reference select select */
#define VREF_ADC0REFSEL_0V55_GC (0x00<<4)  /* Voltage reference at 0.55V */
#define VREF_ADC0REFSEL_1V1_GC (0x01<<4)  /* Voltage reference at 1.1V */
#define VREF_ADC0REFSEL_2V5_GC (0x02<<4)  /* Voltage reference at 2.5V */
#define VREF_ADC0REFSEL_4V34_GC (0x03<<4)  /* Voltage reference at 4.34V */
#define VREF_ADC0REFSEL_1V5_GC (0x04<<4)  /* Voltage reference at 1.5V */

/* ADC1 reference select select */
#define VREF_ADC1REFSEL_0V55_GC (0x00<<4)  /* Voltage reference at 0.55V */
#define VREF_ADC1REFSEL_1V1_GC (0x01<<4)  /* Voltage reference at 1.1V */
#define VREF_ADC1REFSEL_2V5_GC (0x02<<4)  /* Voltage reference at 2.5V */
#define VREF_ADC1REFSEL_4V34_GC (0x03<<4)  /* Voltage reference at 4.34V */
#define VREF_ADC1REFSEL_1V5_GC (0x04<<4)  /* Voltage reference at 1.5V */

/* DAC0/AC0 reference select select */
#define VREF_DAC0REFSEL_0V55_GC (0x00<<0)  /* Voltage reference at 0.55V */
#define VREF_DAC0REFSEL_1V1_GC (0x01<<0)  /* Voltage reference at 1.1V */
#define VREF_DAC0REFSEL_2V5_GC (0x02<<0)  /* Voltage reference at 2.5V */
#define VREF_DAC0REFSEL_4V34_GC (0x03<<0)  /* Voltage reference at 4.34V */
#define VREF_DAC0REFSEL_1V5_GC (0x04<<0)  /* Voltage reference at 1.5V */

/* DAC1/AC1 reference select select */
#define VREF_DAC1REFSEL_0V55_GC (0x00<<0)  /* Voltage reference at 0.55V */
#define VREF_DAC1REFSEL_1V1_GC (0x01<<0)  /* Voltage reference at 1.1V */
#define VREF_DAC1REFSEL_2V5_GC (0x02<<0)  /* Voltage reference at 2.5V */
#define VREF_DAC1REFSEL_4V34_GC (0x03<<0)  /* Voltage reference at 4.34V */
#define VREF_DAC1REFSEL_1V5_GC (0x04<<0)  /* Voltage reference at 1.5V */

/* DAC2/AC2 reference select select */
#define VREF_DAC2REFSEL_0V55_GC (0x00<<0)  /* Voltage reference at 0.55V */
#define VREF_DAC2REFSEL_1V1_GC (0x01<<0)  /* Voltage reference at 1.1V */
#define VREF_DAC2REFSEL_2V5_GC (0x02<<0)  /* Voltage reference at 2.5V */
#define VREF_DAC2REFSEL_4V34_GC (0x03<<0)  /* Voltage reference at 4.34V */
#define VREF_DAC2REFSEL_1V5_GC (0x04<<0)  /* Voltage reference at 1.5V */

/*------------------------------------------------------------------------
WDT - Watch-Dog Timer
------------------------------------------------------------------------*/

/* Period select */
#define WDT_PERIOD_OFF_GC (0x00<<0)  /* Off */
#define WDT_PERIOD_8CLK_GC (0x01<<0)  /* 8 cycles (8ms) */
#define WDT_PERIOD_16CLK_GC (0x02<<0)  /* 16 cycles (16ms) */
#define WDT_PERIOD_32CLK_GC (0x03<<0)  /* 32 cycles (32ms) */
#define WDT_PERIOD_64CLK_GC (0x04<<0)  /* 64 cycles (64ms) */
#define WDT_PERIOD_128CLK_GC (0x05<<0)  /* 128 cycles (0.128s) */
#define WDT_PERIOD_256CLK_GC (0x06<<0)  /* 256 cycles (0.256s) */
#define WDT_PERIOD_512CLK_GC (0x07<<0)  /* 512 cycles (0.512s) */
#define WDT_PERIOD_1KCLK_GC (0x08<<0)  /* 1K cycles (1.0s) */
#define WDT_PERIOD_2KCLK_GC (0x09<<0)  /* 2K cycles (2.0s) */
#define WDT_PERIOD_4KCLK_GC (0x0A<<0)  /* 4K cycles (4.1s) */
#define WDT_PERIOD_8KCLK_GC (0x0B<<0)  /* 8K cycles (8.2s) */

/* Window select */
#define WDT_WINDOW_OFF_GC (0x00<<4)  /* Off */
#define WDT_WINDOW_8CLK_GC (0x01<<4)  /* 8 cycles (8ms) */
#define WDT_WINDOW_16CLK_GC (0x02<<4)  /* 16 cycles (16ms) */
#define WDT_WINDOW_32CLK_GC (0x03<<4)  /* 32 cycles (32ms) */
#define WDT_WINDOW_64CLK_GC (0x04<<4)  /* 64 cycles (64ms) */
#define WDT_WINDOW_128CLK_GC (0x05<<4)  /* 128 cycles (0.128s) */
#define WDT_WINDOW_256CLK_GC (0x06<<4)  /* 256 cycles (0.256s) */
#define WDT_WINDOW_512CLK_GC (0x07<<4)  /* 512 cycles (0.512s) */
#define WDT_WINDOW_1KCLK_GC (0x08<<4)  /* 1K cycles (1.0s) */
#define WDT_WINDOW_2KCLK_GC (0x09<<4)  /* 2K cycles (2.0s) */
#define WDT_WINDOW_4KCLK_GC (0x0A<<4)  /* 4K cycles (4.1s) */
#define WDT_WINDOW_8KCLK_GC (0x0B<<4)  /* 8K cycles (8.2s) */

#endif
