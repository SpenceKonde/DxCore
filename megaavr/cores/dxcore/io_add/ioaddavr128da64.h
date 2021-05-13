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
#define AC_HYSMODE_NONE_GC (0x00<<1)  /* No hysteresis */
#define AC_HYSMODE_SMALL_GC (0x01<<1)  /* Small hysteresis */
#define AC_HYSMODE_MEDIUM_GC (0x02<<1)  /* Medium hysteresis */
#define AC_HYSMODE_LARGE_GC (0x03<<1)  /* Large hysteresis */
/* AC Output Initial Value select */
#define AC_INITVAL_LOW_GC (0x00<<6)  /* Output initialized to 0 */
#define AC_INITVAL_HIGH_GC (0x01<<6)  /* Output initialized to 1 */
/* Interrupt Mode select */
#define AC_INTMODE_NORMAL_BOTHEDGE_GC (0x00<<4)  /* Positive and negative inputs crosses */
#define AC_INTMODE_NORMAL_NEGEDGE_GC (0x02<<4)  /* Positive input goes below negative input */
#define AC_INTMODE_NORMAL_POSEDGE_GC (0x03<<4)  /* Positive input goes above negative input */
/* Interrupt Mode select */
#define AC_INTMODE_WINDOW_ABOVE_GC (0x00<<4)  /* Window interrupt when input above both references */
#define AC_INTMODE_WINDOW_INSIDE_GC (0x01<<4)  /* Window interrupt when input betweeen references */
#define AC_INTMODE_WINDOW_BELOW_GC (0x02<<4)  /* Window interrupt when input below both references */
#define AC_INTMODE_WINDOW_OUTSIDE_GC (0x03<<4)  /* Window interrupt when input outside reference */
/* Negative Input MUX Selection select */
#define AC_MUXNEG_AINN0_GC (0x00<<0)  /* Negative Pin 0 */
#define AC_MUXNEG_AINN1_GC (0x01<<0)  /* Negative Pin 1 */
#define AC_MUXNEG_AINN2_GC (0x02<<0)  /* Negative Pin 2 */
#define AC_MUXNEG_DACREF_GC (0x03<<0)  /* DAC Reference */
/* Positive Input MUX Selection select */
#define AC_MUXPOS_AINP0_GC (0x00<<3)  /* Positive Pin 0 */
#define AC_MUXPOS_AINP1_GC (0x01<<3)  /* Positive Pin 1 */
#define AC_MUXPOS_AINP2_GC (0x02<<3)  /* Positive Pin 2 */
#define AC_MUXPOS_AINP3_GC (0x03<<3)  /* Positive Pin 3 */
/* Power profile select */
#define AC_POWER_PROFILE0_GC (0x00<<3)  /* Power profile 0, Shortest response time, highest consumption */
#define AC_POWER_PROFILE1_GC (0x01<<3)  /* Power profile 1 */
#define AC_POWER_PROFILE2_GC (0x02<<3)  /* Power profile 2 */
/* Window selection mode select */
#define AC_WINSEL_DISABLED_GC (0x00<<0)  /* Window function disabled */
#define AC_WINSEL_UPSEL1_GC (0x01<<0)  /* Select ACn+1 as upper limit in window compare */
#define AC_WINSEL_UPSEL2_GC (0x02<<0)  /* Select ACn+2 as upper limit in window compare */
/* Analog Comparator Window State select */
#define AC_WINSTATE_ABOVE_GC (0x00<<6)  /* Above window */
#define AC_WINSTATE_INSIDE_GC (0x01<<6)  /* Inside window */
#define AC_WINSTATE_BELOW_GC (0x02<<6)  /* Below window */

/*------------------------------------------------------------------------
ADC - Analog to Digital Converter
------------------------------------------------------------------------*/

/* Initial Delay Selection select */
#define ADC_INITDLY_DLY0_GC (0x00<<5)  /* Delay 0 CLK_ADC cycles */
#define ADC_INITDLY_DLY16_GC (0x01<<5)  /* Delay 16 CLK_ADC cycles */
#define ADC_INITDLY_DLY32_GC (0x02<<5)  /* Delay 32 CLK_ADC cycles */
#define ADC_INITDLY_DLY64_GC (0x03<<5)  /* Delay 64 CLK_ADC cycles */
#define ADC_INITDLY_DLY128_GC (0x04<<5)  /* Delay 128 CLK_ADC cycles */
#define ADC_INITDLY_DLY256_GC (0x05<<5)  /* Delay 256 CLK_ADC cycles */
/* Analog Channel Selection Bits select */
#define ADC_MUXNEG_AIN0_GC (0x00<<0)  /* ADC input pin 0 */
#define ADC_MUXNEG_AIN1_GC (0x01<<0)  /* ADC input pin 1 */
#define ADC_MUXNEG_AIN2_GC (0x02<<0)  /* ADC input pin 2 */
#define ADC_MUXNEG_AIN3_GC (0x03<<0)  /* ADC input pin 3 */
#define ADC_MUXNEG_AIN4_GC (0x04<<0)  /* ADC input pin 4 */
#define ADC_MUXNEG_AIN5_GC (0x05<<0)  /* ADC input pin 5 */
#define ADC_MUXNEG_AIN6_GC (0x06<<0)  /* ADC input pin 6 */
#define ADC_MUXNEG_AIN7_GC (0x07<<0)  /* ADC input pin 7 */
#define ADC_MUXNEG_AIN8_GC (0x08<<0)  /* ADC input pin 8 */
#define ADC_MUXNEG_AIN9_GC (0x09<<0)  /* ADC input pin 9 */
#define ADC_MUXNEG_AIN10_GC (0x0A<<0)  /* ADC input pin 10 */
#define ADC_MUXNEG_AIN11_GC (0x0B<<0)  /* ADC input pin 11 */
#define ADC_MUXNEG_AIN12_GC (0x0C<<0)  /* ADC input pin 12 */
#define ADC_MUXNEG_AIN13_GC (0x0D<<0)  /* ADC input pin 13 */
#define ADC_MUXNEG_AIN14_GC (0x0E<<0)  /* ADC input pin 14 */
#define ADC_MUXNEG_AIN15_GC (0x0F<<0)  /* ADC input pin 15 */
#define ADC_MUXNEG_GND_GC (0x40<<0)  /* Ground */
#define ADC_MUXNEG_DAC0_GC (0x48<<0)  /* DAC0 */
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
#define ADC_MUXPOS_AIN12_GC (0x0C<<0)  /* ADC input pin 12 */
#define ADC_MUXPOS_AIN13_GC (0x0D<<0)  /* ADC input pin 13 */
#define ADC_MUXPOS_AIN14_GC (0x0E<<0)  /* ADC input pin 14 */
#define ADC_MUXPOS_AIN15_GC (0x0F<<0)  /* ADC input pin 15 */
#define ADC_MUXPOS_AIN16_GC (0x10<<0)  /* ADC input pin 16 */
#define ADC_MUXPOS_AIN17_GC (0x11<<0)  /* ADC input pin 17 */
#define ADC_MUXPOS_AIN18_GC (0x12<<0)  /* ADC input pin 18 */
#define ADC_MUXPOS_AIN19_GC (0x13<<0)  /* ADC input pin 19 */
#define ADC_MUXPOS_AIN20_GC (0x14<<0)  /* ADC input pin 20 */
#define ADC_MUXPOS_AIN21_GC (0x15<<0)  /* ADC input pin 21 */
#define ADC_MUXPOS_GND_GC (0x40<<0)  /* Ground */
#define ADC_MUXPOS_TEMPSENSE_GC (0x42<<0)  /* Temperature sensor */
#define ADC_MUXPOS_DAC0_GC (0x48<<0)  /* DAC0 */
#define ADC_MUXPOS_DACREF0_GC (0x49<<0)  /* DACREF0 */
#define ADC_MUXPOS_DACREF1_GC (0x4A<<0)  /* DACREF1 */
#define ADC_MUXPOS_DACREF2_GC (0x4B<<0)  /* DACREF2 */
/* Clock Pre-scaler select */
#define ADC_PRESC_DIV2_GC (0x00<<0)  /* CLK_PER divided by 2 */
#define ADC_PRESC_DIV4_GC (0x01<<0)  /* CLK_PER divided by 4 */
#define ADC_PRESC_DIV8_GC (0x02<<0)  /* CLK_PER divided by 8 */
#define ADC_PRESC_DIV12_GC (0x03<<0)  /* CLK_PER divided by 12 */
#define ADC_PRESC_DIV16_GC (0x04<<0)  /* CLK_PER divided by 16 */
#define ADC_PRESC_DIV20_GC (0x05<<0)  /* CLK_PER divided by 20 */
#define ADC_PRESC_DIV24_GC (0x06<<0)  /* CLK_PER divided by 24 */
#define ADC_PRESC_DIV28_GC (0x07<<0)  /* CLK_PER divided by 28 */
#define ADC_PRESC_DIV32_GC (0x08<<0)  /* CLK_PER divided by 32 */
#define ADC_PRESC_DIV48_GC (0x09<<0)  /* CLK_PER divided by 48 */
#define ADC_PRESC_DIV64_GC (0x0A<<0)  /* CLK_PER divided by 64 */
#define ADC_PRESC_DIV96_GC (0x0B<<0)  /* CLK_PER divided by 96 */
#define ADC_PRESC_DIV128_GC (0x0C<<0)  /* CLK_PER divided by 128 */
#define ADC_PRESC_DIV256_GC (0x0D<<0)  /* CLK_PER divided by 256 */
/* Resolution selection select */
#define ADC_RESSEL_12BIT_GC (0x00<<2)  /* 12-bit mode */
#define ADC_RESSEL_10BIT_GC (0x01<<2)  /* 10-bit mode */
/* Sampling Delay Selection select */
#define ADC_SAMPDLY_DLY0_GC (0x00<<0)  /* Delay 0 CLK_ADC cycles */
#define ADC_SAMPDLY_DLY1_GC (0x01<<0)  /* Delay 1 CLK_ADC cycles */
#define ADC_SAMPDLY_DLY2_GC (0x02<<0)  /* Delay 2 CLK_ADC cycles */
#define ADC_SAMPDLY_DLY3_GC (0x03<<0)  /* Delay 3 CLK_ADC cycles */
#define ADC_SAMPDLY_DLY4_GC (0x04<<0)  /* Delay 4 CLK_ADC cycles */
#define ADC_SAMPDLY_DLY5_GC (0x05<<0)  /* Delay 5 CLK_ADC cycles */
#define ADC_SAMPDLY_DLY6_GC (0x06<<0)  /* Delay 6 CLK_ADC cycles */
#define ADC_SAMPDLY_DLY7_GC (0x07<<0)  /* Delay 7 CLK_ADC cycles */
#define ADC_SAMPDLY_DLY8_GC (0x08<<0)  /* Delay 8 CLK_ADC cycles */
#define ADC_SAMPDLY_DLY9_GC (0x09<<0)  /* Delay 9 CLK_ADC cycles */
#define ADC_SAMPDLY_DLY10_GC (0x0A<<0)  /* Delay 10 CLK_ADC cycles */
#define ADC_SAMPDLY_DLY11_GC (0x0B<<0)  /* Delay 11 CLK_ADC cycles */
#define ADC_SAMPDLY_DLY12_GC (0x0C<<0)  /* Delay 12 CLK_ADC cycles */
#define ADC_SAMPDLY_DLY13_GC (0x0D<<0)  /* Delay 13 CLK_ADC cycles */
#define ADC_SAMPDLY_DLY14_GC (0x0E<<0)  /* Delay 14 CLK_ADC cycles */
#define ADC_SAMPDLY_DLY15_GC (0x0F<<0)  /* Delay 15 CLK_ADC cycles */
/* Accumulation Samples select */
#define ADC_SAMPNUM_NONE_GC (0x00<<0)  /* No accumulation */
#define ADC_SAMPNUM_ACC2_GC (0x01<<0)  /* 2 results accumulated */
#define ADC_SAMPNUM_ACC4_GC (0x02<<0)  /* 4 results accumulated */
#define ADC_SAMPNUM_ACC8_GC (0x03<<0)  /* 8 results accumulated */
#define ADC_SAMPNUM_ACC16_GC (0x04<<0)  /* 16 results accumulated */
#define ADC_SAMPNUM_ACC32_GC (0x05<<0)  /* 32 results accumulated */
#define ADC_SAMPNUM_ACC64_GC (0x06<<0)  /* 64 results accumulated */
#define ADC_SAMPNUM_ACC128_GC (0x07<<0)  /* 128 results accumulated */
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
#define BOD_LVL_BODLEVEL0_GC (0x00<<0)  /* 1.9 V */
#define BOD_LVL_BODLEVEL1_GC (0x01<<0)  /* 2.45 V */
#define BOD_LVL_BODLEVEL2_GC (0x02<<0)  /* 2.7 V */
#define BOD_LVL_BODLEVEL3_GC (0x03<<0)  /* 2.85 V */
/* Sample frequency select */
#define BOD_SAMPFREQ_128HZ_GC (0x00<<4)  /* 128Hz sampling frequency */
#define BOD_SAMPFREQ_32HZ_GC (0x01<<4)  /* 32Hz sampling frequency */
/* Operation in sleep mode select */
#define BOD_SLEEP_DIS_GC (0x00<<0)  /* Disabled */
#define BOD_SLEEP_ENABLED_GC (0x01<<0)  /* Enabled */
#define BOD_SLEEP_SAMPLED_GC (0x02<<0)  /* Sampled */
/* Configuration select */
#define BOD_VLMCFG_FALLING_GC (0x00<<1)  /* VDD falls below VLM threshold */
#define BOD_VLMCFG_RISING_GC (0x01<<1)  /* VDD rises above VLM threshold */
#define BOD_VLMCFG_BOTH_GC (0x02<<1)  /* VDD crosses VLM threshold */
/* voltage level monitor level select */
#define BOD_VLMLVL_OFF_GC (0x00<<0)  /* VLM Disabled */
#define BOD_VLMLVL_5ABOVE_GC (0x01<<0)  /* VLM threshold 5% above BOD level */
#define BOD_VLMLVL_15ABOVE_GC (0x02<<0)  /* VLM threshold 15% above BOD level */
#define BOD_VLMLVL_25ABOVE_GC (0x03<<0)  /* VLM threshold 25% above BOD level */
/* Voltage level monitor status select */
#define BOD_VLMS_ABOVE_GC (0x00<<0)  /* The voltage is above the VLM threshold level */
#define BOD_VLMS_BELOW_GC (0x01<<0)  /* The voltage is below the VLM threshold level */

/*------------------------------------------------------------------------
CCL - Configurable Custom Logic
------------------------------------------------------------------------*/

/* Clock Source Selection select */
#define CCL_CLKSRC_CLKPER_GC (0x00<<1)  /* Peripheral Clock */
#define CCL_CLKSRC_IN2_GC (0x01<<1)  /* Selection by INSEL2 */
#define CCL_CLKSRC_OSCHF_GC (0x04<<1)  /* Internal high-frequency oscillator */
#define CCL_CLKSRC_OSC32K_GC (0x05<<1)  /* 32.768 kHz oscillator */
#define CCL_CLKSRC_OSC1K_GC (0x06<<1)  /* 32.768 kHz oscillator divided by 32 */
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
#define CCL_INSEL0_EVENTA_GC (0x03<<0)  /* Event input source A */
#define CCL_INSEL0_EVENTB_GC (0x04<<0)  /* Event input source B */
#define CCL_INSEL0_IN0_GC (0x05<<0)  /* IO pin LUTn-IN0 input source */
#define CCL_INSEL0_AC0_GC (0x06<<0)  /* AC0 OUT input source */
#define CCL_INSEL0_ZCD0_GC (0x07<<0)  /* ZCD0 OUT input source */
#define CCL_INSEL0_USART0_GC (0x08<<0)  /* USART0 TXD input source */
#define CCL_INSEL0_SPI0_GC (0x09<<0)  /* SPI0 MOSI input source */
#define CCL_INSEL0_TCA0_GC (0x0A<<0)  /* TCA0 WO0 input source */
#define CCL_INSEL0_TCA1_GC (0x0B<<0)  /* TCA1 WO0 input source */
#define CCL_INSEL0_TCB0_GC (0x0C<<0)  /* TCB0 WO input source */
#define CCL_INSEL0_TCD0_GC (0x0D<<0)  /* TCD0 WOA input source */
/* LUT Input 1 Source Selection select */
#define CCL_INSEL1_MASK_GC (0x00<<4)  /* Masked input */
#define CCL_INSEL1_FEEDBACK_GC (0x01<<4)  /* Feedback input source */
#define CCL_INSEL1_LINK_GC (0x02<<4)  /* Linked LUT input source */
#define CCL_INSEL1_EVENTA_GC (0x03<<4)  /* Event input source A */
#define CCL_INSEL1_EVENTB_GC (0x04<<4)  /* Event input source B */
#define CCL_INSEL1_IN1_GC (0x05<<4)  /* IO pin LUTn-IN1 input source */
#define CCL_INSEL1_AC1_GC (0x06<<4)  /* AC1 OUT input source */
#define CCL_INSEL1_ZCD1_GC (0x07<<4)  /* ZCD1 OUT input source */
#define CCL_INSEL1_USART1_GC (0x08<<4)  /* USART1 TXD input source */
#define CCL_INSEL1_SPI0_GC (0x09<<4)  /* SPI0 MOSI input source */
#define CCL_INSEL1_TCA0_GC (0x0A<<4)  /* TCA0 WO1 input source */
#define CCL_INSEL1_TCA1_GC (0x0B<<4)  /* TCA1 WO1 input source */
#define CCL_INSEL1_TCB1_GC (0x0C<<4)  /* TCB1 WO input source */
#define CCL_INSEL1_TCD0_GC (0x0D<<4)  /* TCD0 WOB input soruce */
/* LUT Input 2 Source Selection select */
#define CCL_INSEL2_MASK_GC (0x00<<0)  /* Masked input */
#define CCL_INSEL2_FEEDBACK_GC (0x01<<0)  /* Feedback input source */
#define CCL_INSEL2_LINK_GC (0x02<<0)  /* Linked LUT input source */
#define CCL_INSEL2_EVENTA_GC (0x03<<0)  /* Event input source A */
#define CCL_INSEL2_EVENTB_GC (0x04<<0)  /* Event input source B */
#define CCL_INSEL2_IN2_GC (0x05<<0)  /* IO pin LUTn-IN2 input source */
#define CCL_INSEL2_AC2_GC (0x06<<0)  /* AC2 OUT input source */
#define CCL_INSEL2_ZCD2_GC (0x07<<0)  /* ZCD2 OUT input source */
#define CCL_INSEL2_USART2_GC (0x08<<0)  /* USART2 TXD input source */
#define CCL_INSEL2_SPI0_GC (0x09<<0)  /* SPI0 SCK input source */
#define CCL_INSEL2_TCA0_GC (0x0A<<0)  /* TCA0 WO2 input source */
#define CCL_INSEL2_TCA1_GC (0x0B<<0)  /* TCA1 WO2 input source */
#define CCL_INSEL2_TCB2_GC (0x0C<<0)  /* TCB2 WO input source */
#define CCL_INSEL2_TCD0_GC (0x0D<<0)  /* TCD0 WOC input source */
/* Interrupt Mode for LUT0 select */
#define CCL_INTMODE0_INTDISABLE_GC (0x00<<0)  /* Interrupt disabled */
#define CCL_INTMODE0_RISING_GC (0x01<<0)  /* Sense rising edge */
#define CCL_INTMODE0_FALLING_GC (0x02<<0)  /* Sense falling edge */
#define CCL_INTMODE0_BOTH_GC (0x03<<0)  /* Sense both edges */
/* Interrupt Mode for LUT1 select */
#define CCL_INTMODE1_INTDISABLE_GC (0x00<<2)  /* Interrupt disabled */
#define CCL_INTMODE1_RISING_GC (0x01<<2)  /* Sense rising edge */
#define CCL_INTMODE1_FALLING_GC (0x02<<2)  /* Sense falling edge */
#define CCL_INTMODE1_BOTH_GC (0x03<<2)  /* Sense both edges */
/* Interrupt Mode for LUT2 select */
#define CCL_INTMODE2_INTDISABLE_GC (0x00<<4)  /* Interrupt disabled */
#define CCL_INTMODE2_RISING_GC (0x01<<4)  /* Sense rising edge */
#define CCL_INTMODE2_FALLING_GC (0x02<<4)  /* Sense falling edge */
#define CCL_INTMODE2_BOTH_GC (0x03<<4)  /* Sense both edges */
/* Interrupt Mode for LUT3 select */
#define CCL_INTMODE3_INTDISABLE_GC (0x00<<6)  /* Interrupt disabled */
#define CCL_INTMODE3_RISING_GC (0x01<<6)  /* Sense rising edge */
#define CCL_INTMODE3_FALLING_GC (0x02<<6)  /* Sense falling edge */
#define CCL_INTMODE3_BOTH_GC (0x03<<6)  /* Sense both edges */
/* Interrupt Mode for LUT4 select */
#define CCL_INTMODE4_INTDISABLE_GC (0x00<<0)  /* Interrupt disabled */
#define CCL_INTMODE4_RISING_GC (0x01<<0)  /* Sense rising edge */
#define CCL_INTMODE4_FALLING_GC (0x02<<0)  /* Sense falling edge */
#define CCL_INTMODE4_BOTH_GC (0x03<<0)  /* Sense both edges */
/* Interrupt Mode for LUT5 select */
#define CCL_INTMODE5_INTDISABLE_GC (0x00<<2)  /* Interrupt disabled */
#define CCL_INTMODE5_RISING_GC (0x01<<2)  /* Sense rising edge */
#define CCL_INTMODE5_FALLING_GC (0x02<<2)  /* Sense falling edge */
#define CCL_INTMODE5_BOTH_GC (0x03<<2)  /* Sense both edges */
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
#define CLKCTRL_CLKSEL_OSCHF_GC (0x00<<0)  /* Internal high-frequency oscillator */
#define CLKCTRL_CLKSEL_OSC32K_GC (0x01<<0)  /* Internal 32.768 kHz oscillator */
#define CLKCTRL_CLKSEL_XOSC32K_GC (0x02<<0)  /* 32.768 kHz crystal oscillator */
#define CLKCTRL_CLKSEL_EXTCLK_GC (0x03<<0)  /* External clock */
/* Crystal startup time select */
#define CLKCTRL_CSUT_1K_GC (0x00<<4)  /* 1k cycles */
#define CLKCTRL_CSUT_16K_GC (0x01<<4)  /* 16k cycles */
#define CLKCTRL_CSUT_32K_GC (0x02<<4)  /* 32k cycles */
#define CLKCTRL_CSUT_64K_GC (0x03<<4)  /* 64k cycles */
/* Frequency select select */
#define CLKCTRL_FREQSEL_1M_GC (0x00<<2)  /* 1 MHz system clock */
#define CLKCTRL_FREQSEL_2M_GC (0x01<<2)  /* 2 MHz system clock */
#define CLKCTRL_FREQSEL_3M_GC (0x02<<2)  /* 3 MHz system clock */
#define CLKCTRL_FREQSEL_4M_GC (0x03<<2)  /* 4 MHz system clock (default) */
#define CLKCTRL_FREQSEL_8M_GC (0x05<<2)  /* 8 MHz system clock */
#define CLKCTRL_FREQSEL_12M_GC (0x06<<2)  /* 12 MHz system clock */
#define CLKCTRL_FREQSEL_16M_GC (0x07<<2)  /* 16 MHz system clock */
#define CLKCTRL_FREQSEL_20M_GC (0x08<<2)  /* 20 MHz system clock */
#define CLKCTRL_FREQSEL_24M_GC (0x09<<2)  /* 24 MHz system clock */
/* Multiplication factor select */
#define CLKCTRL_MULFAC_DISABLE_GC (0x00<<0)  /* PLL is disabled */
#define CLKCTRL_MULFAC_2x_GC (0x01<<0)  /* 2 x multiplication factor */
#define CLKCTRL_MULFAC_3x_GC (0x02<<0)  /* 3 x multiplication factor */
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

/* Channel 0 generator select select */
#define EVSYS_CHANNEL0_OFF_GC (0x00<<0)  /* Off */
#define EVSYS_CHANNEL0_UPDI_SYNCH_GC (0x01<<0)  /* UPDI SYNCH character */
#define EVSYS_CHANNEL0_RTC_OVF_GC (0x06<<0)  /* Real Time Counter overflow */
#define EVSYS_CHANNEL0_RTC_CMP_GC (0x07<<0)  /* Real Time Counter compare */
#define EVSYS_CHANNEL0_RTC_PIT_DIV8192_GC (0x08<<0)  /* Periodic Interrupt Timer output 0 */
#define EVSYS_CHANNEL0_RTC_PIT_DIV4096_GC (0x09<<0)  /* Periodic Interrupt Timer output 1 */
#define EVSYS_CHANNEL0_RTC_PIT_DIV2048_GC (0x0A<<0)  /* Periodic Interrupt Timer output 2 */
#define EVSYS_CHANNEL0_RTC_PIT_DIV1024_GC (0x0B<<0)  /* Periodic Interrupt Timer output 3 */
#define EVSYS_CHANNEL0_CCL_LUT0_GC (0x10<<0)  /* Configurable Custom Logic LUT0 */
#define EVSYS_CHANNEL0_CCL_LUT1_GC (0x11<<0)  /* Configurable Custom Logic LUT1 */
#define EVSYS_CHANNEL0_CCL_LUT2_GC (0x12<<0)  /* Configurable Custom Logic LUT2 */
#define EVSYS_CHANNEL0_CCL_LUT3_GC (0x13<<0)  /* Configurable Custom Logic LUT3 */
#define EVSYS_CHANNEL0_CCL_LUT4_GC (0x14<<0)  /* Configurable Custom Logic LUT4 */
#define EVSYS_CHANNEL0_CCL_LUT5_GC (0x15<<0)  /* Configurable Custom Logic LUT5 */
#define EVSYS_CHANNEL0_AC0_OUT_GC (0x20<<0)  /* Analog Comparator 0 out */
#define EVSYS_CHANNEL0_AC1_OUT_GC (0x21<<0)  /* Analog Comparator 1 out */
#define EVSYS_CHANNEL0_AC2_OUT_GC (0x22<<0)  /* Analog Comparator 2 out */
#define EVSYS_CHANNEL0_ADC0_RESRDY_GC (0x24<<0)  /* ADC 0 Result Ready */
#define EVSYS_CHANNEL0_PTC_RESRDY_GC (0x28<<0)  /* PTC Result Ready */
#define EVSYS_CHANNEL0_ZCD0_GC (0x30<<0)  /* Zero Cross Detect 0 out */
#define EVSYS_CHANNEL0_ZCD1_GC (0x31<<0)  /* Zero Cross Detect 1 out */
#define EVSYS_CHANNEL0_ZCD2_GC (0x32<<0)  /* Zero Cross Detect 2 out */
#define EVSYS_CHANNEL0_PORTA_PIN0_GC (0x40<<0)  /* Port A Pin 0 */
#define EVSYS_CHANNEL0_PORTA_PIN1_GC (0x41<<0)  /* Port A Pin 1 */
#define EVSYS_CHANNEL0_PORTA_PIN2_GC (0x42<<0)  /* Port A Pin 2 */
#define EVSYS_CHANNEL0_PORTA_PIN3_GC (0x43<<0)  /* Port A Pin 3 */
#define EVSYS_CHANNEL0_PORTA_PIN4_GC (0x44<<0)  /* Port A Pin 4 */
#define EVSYS_CHANNEL0_PORTA_PIN5_GC (0x45<<0)  /* Port A Pin 5 */
#define EVSYS_CHANNEL0_PORTA_PIN6_GC (0x46<<0)  /* Port A Pin 6 */
#define EVSYS_CHANNEL0_PORTA_PIN7_GC (0x47<<0)  /* Port A Pin 7 */
#define EVSYS_CHANNEL0_PORTB_PIN0_GC (0x48<<0)  /* Port B Pin 0 */
#define EVSYS_CHANNEL0_PORTB_PIN1_GC (0x49<<0)  /* Port B Pin 1 */
#define EVSYS_CHANNEL0_PORTB_PIN2_GC (0x4A<<0)  /* Port B Pin 2 */
#define EVSYS_CHANNEL0_PORTB_PIN3_GC (0x4B<<0)  /* Port B Pin 3 */
#define EVSYS_CHANNEL0_PORTB_PIN4_GC (0x4C<<0)  /* Port B Pin 4 */
#define EVSYS_CHANNEL0_PORTB_PIN5_GC (0x4D<<0)  /* Port B Pin 5 */
#define EVSYS_CHANNEL0_PORTB_PIN6_GC (0x4E<<0)  /* Port B Pin 6 */
#define EVSYS_CHANNEL0_PORTB_PIN7_GC (0x4F<<0)  /* Port B Pin 7 */
#define EVSYS_CHANNEL0_USART0_XCK_GC (0x60<<0)  /* USART 0 XCK */
#define EVSYS_CHANNEL0_USART1_XCK_GC (0x61<<0)  /* USART 1 XCK */
#define EVSYS_CHANNEL0_USART2_XCK_GC (0x62<<0)  /* USART 2 XCK */
#define EVSYS_CHANNEL0_USART3_XCK_GC (0x63<<0)  /* USART 3 XCK */
#define EVSYS_CHANNEL0_USART4_XCK_GC (0x64<<0)  /* USART 4 XCK */
#define EVSYS_CHANNEL0_USART5_XCK_GC (0x65<<0)  /* USART 5 XCK */
#define EVSYS_CHANNEL0_SPI0_SCK_GC (0x68<<0)  /* SPI 0 SCK */
#define EVSYS_CHANNEL0_SPI1_SCK_GC (0x69<<0)  /* SPI 1 SCK */
#define EVSYS_CHANNEL0_TCA0_OVF_LUNF_GC (0x80<<0)  /* Timer/Counter A0 overflow / low byte timer underflow */
#define EVSYS_CHANNEL0_TCA0_HUNF_GC (0x81<<0)  /* Timer/Counter A0 high byte timer underflow */
#define EVSYS_CHANNEL0_TCA0_CMP0_LCMP0_GC (0x84<<0)  /* Timer/Counter A0 compare 0 / low byte timer compare 0 */
#define EVSYS_CHANNEL0_TCA0_CMP1_LCMP1_GC (0x85<<0)  /* Timer/Counter A0 compare 1 / low byte timer compare 1 */
#define EVSYS_CHANNEL0_TCA0_CMP2_LCMP2_GC (0x86<<0)  /* Timer/Counter A0 compare 2 / low byte timer compare 2 */
#define EVSYS_CHANNEL0_TCA1_OVF_LUNF_GC (0x88<<0)  /* Timer/Counter A1 overflow / low byte timer underflow */
#define EVSYS_CHANNEL0_TCA1_HUNF_GC (0x89<<0)  /* Timer/Counter A1 high byte timer underflow */
#define EVSYS_CHANNEL0_TCA1_CMP0_LCMP0_GC (0x8C<<0)  /* Timer/Counter A1 compare 0 / low byte timer compare 0 */
#define EVSYS_CHANNEL0_TCA1_CMP1_LCMP1_GC (0x8D<<0)  /* Timer/Counter A1 compare 1 / low byte timer compare 1 */
#define EVSYS_CHANNEL0_TCA1_CMP2_LCMP2_GC (0x8E<<0)  /* Timer/Counter A1 compare 2 / low byte timer compare 2 */
#define EVSYS_CHANNEL0_TCB0_CAPT_GC (0xA0<<0)  /* Timer/Counter B0 capture */
#define EVSYS_CHANNEL0_TCB0_OVF_GC (0xA1<<0)  /* Timer/Counter B0 overflow */
#define EVSYS_CHANNEL0_TCB1_CAPT_GC (0xA2<<0)  /* Timer/Counter B1 capture */
#define EVSYS_CHANNEL0_TCB1_OVF_GC (0xA3<<0)  /* Timer/Counter B1 overflow */
#define EVSYS_CHANNEL0_TCB2_CAPT_GC (0xA4<<0)  /* Timer/Counter B2 capture */
#define EVSYS_CHANNEL0_TCB2_OVF_GC (0xA5<<0)  /* Timer/Counter B2 overflow */
#define EVSYS_CHANNEL0_TCB3_CAPT_GC (0xA6<<0)  /* Timer/Counter B3 capture */
#define EVSYS_CHANNEL0_TCB3_OVF_GC (0xA7<<0)  /* Timer/Counter B3 overflow */
#define EVSYS_CHANNEL0_TCB4_CAPT_GC (0xA8<<0)  /* Timer/Counter B4 capture */
#define EVSYS_CHANNEL0_TCB4_OVF_GC (0xA9<<0)  /* Timer/Counter B4 overflow */
#define EVSYS_CHANNEL0_TCD0_CMPBCLR_GC (0xB0<<0)  /* Timer/Counter D0 event 0 */
#define EVSYS_CHANNEL0_TCD0_CMPASET_GC (0xB1<<0)  /* Timer/Counter D0 event 1 */
#define EVSYS_CHANNEL0_TCD0_CMPBSET_GC (0xB2<<0)  /* Timer/Counter D0 event 2 */
#define EVSYS_CHANNEL0_TCD0_PROGEV_GC (0xB3<<0)  /* Timer/Counter D0 event 3 */
/* Channel 1 generator select select */
#define EVSYS_CHANNEL1_OFF_GC (0x00<<0)  /* Off */
#define EVSYS_CHANNEL1_UPDI_SYNCH_GC (0x01<<0)  /* UPDI SYNCH character */
#define EVSYS_CHANNEL1_RTC_OVF_GC (0x06<<0)  /* Real Time Counter overflow */
#define EVSYS_CHANNEL1_RTC_CMP_GC (0x07<<0)  /* Real Time Counter compare */
#define EVSYS_CHANNEL1_RTC_PIT_DIV512_GC (0x08<<0)  /* Periodic Interrupt Timer output 0 */
#define EVSYS_CHANNEL1_RTC_PIT_DIV256_GC (0x09<<0)  /* Periodic Interrupt Timer output 1 */
#define EVSYS_CHANNEL1_RTC_PIT_DIV128_GC (0x0A<<0)  /* Periodic Interrupt Timer output 2 */
#define EVSYS_CHANNEL1_RTC_PIT_DIV64_GC (0x0B<<0)  /* Periodic Interrupt Timer output 3 */
#define EVSYS_CHANNEL1_CCL_LUT0_GC (0x10<<0)  /* Configurable Custom Logic LUT0 */
#define EVSYS_CHANNEL1_CCL_LUT1_GC (0x11<<0)  /* Configurable Custom Logic LUT1 */
#define EVSYS_CHANNEL1_CCL_LUT2_GC (0x12<<0)  /* Configurable Custom Logic LUT2 */
#define EVSYS_CHANNEL1_CCL_LUT3_GC (0x13<<0)  /* Configurable Custom Logic LUT3 */
#define EVSYS_CHANNEL1_CCL_LUT4_GC (0x14<<0)  /* Configurable Custom Logic LUT4 */
#define EVSYS_CHANNEL1_CCL_LUT5_GC (0x15<<0)  /* Configurable Custom Logic LUT5 */
#define EVSYS_CHANNEL1_AC0_OUT_GC (0x20<<0)  /* Analog Comparator 0 out */
#define EVSYS_CHANNEL1_AC1_OUT_GC (0x21<<0)  /* Analog Comparator 1 out */
#define EVSYS_CHANNEL1_AC2_OUT_GC (0x22<<0)  /* Analog Comparator 2 out */
#define EVSYS_CHANNEL1_ADC0_RESRDY_GC (0x24<<0)  /* ADC 0 Result Ready */
#define EVSYS_CHANNEL1_PTC_RESRDY_GC (0x28<<0)  /* PTC Result Ready */
#define EVSYS_CHANNEL1_ZCD0_GC (0x30<<0)  /* Zero Cross Detect 0 out */
#define EVSYS_CHANNEL1_ZCD1_GC (0x31<<0)  /* Zero Cross Detect 1 out */
#define EVSYS_CHANNEL1_ZCD2_GC (0x32<<0)  /* Zero Cross Detect 2 out */
#define EVSYS_CHANNEL1_PORTA_PIN0_GC (0x40<<0)  /* Port A Pin 0 */
#define EVSYS_CHANNEL1_PORTA_PIN1_GC (0x41<<0)  /* Port A Pin 1 */
#define EVSYS_CHANNEL1_PORTA_PIN2_GC (0x42<<0)  /* Port A Pin 2 */
#define EVSYS_CHANNEL1_PORTA_PIN3_GC (0x43<<0)  /* Port A Pin 3 */
#define EVSYS_CHANNEL1_PORTA_PIN4_GC (0x44<<0)  /* Port A Pin 4 */
#define EVSYS_CHANNEL1_PORTA_PIN5_GC (0x45<<0)  /* Port A Pin 5 */
#define EVSYS_CHANNEL1_PORTA_PIN6_GC (0x46<<0)  /* Port A Pin 6 */
#define EVSYS_CHANNEL1_PORTA_PIN7_GC (0x47<<0)  /* Port A Pin 7 */
#define EVSYS_CHANNEL1_PORTB_PIN0_GC (0x48<<0)  /* Port B Pin 0 */
#define EVSYS_CHANNEL1_PORTB_PIN1_GC (0x49<<0)  /* Port B Pin 1 */
#define EVSYS_CHANNEL1_PORTB_PIN2_GC (0x4A<<0)  /* Port B Pin 2 */
#define EVSYS_CHANNEL1_PORTB_PIN3_GC (0x4B<<0)  /* Port B Pin 3 */
#define EVSYS_CHANNEL1_PORTB_PIN4_GC (0x4C<<0)  /* Port B Pin 4 */
#define EVSYS_CHANNEL1_PORTB_PIN5_GC (0x4D<<0)  /* Port B Pin 5 */
#define EVSYS_CHANNEL1_PORTB_PIN6_GC (0x4E<<0)  /* Port B Pin 6 */
#define EVSYS_CHANNEL1_PORTB_PIN7_GC (0x4F<<0)  /* Port B Pin 7 */
#define EVSYS_CHANNEL1_USART0_XCK_GC (0x60<<0)  /* USART 0 XCK */
#define EVSYS_CHANNEL1_USART1_XCK_GC (0x61<<0)  /* USART 1 XCK */
#define EVSYS_CHANNEL1_USART2_XCK_GC (0x62<<0)  /* USART 2 XCK */
#define EVSYS_CHANNEL1_USART3_XCK_GC (0x63<<0)  /* USART 3 XCK */
#define EVSYS_CHANNEL1_USART4_XCK_GC (0x64<<0)  /* USART 4 XCK */
#define EVSYS_CHANNEL1_USART5_XCK_GC (0x65<<0)  /* USART 5 XCK */
#define EVSYS_CHANNEL1_SPI0_SCK_GC (0x68<<0)  /* SPI 0 SCK */
#define EVSYS_CHANNEL1_SPI1_SCK_GC (0x69<<0)  /* SPI 1 SCK */
#define EVSYS_CHANNEL1_TCA0_OVF_LUNF_GC (0x80<<0)  /* Timer/Counter A0 overflow / low byte timer underflow */
#define EVSYS_CHANNEL1_TCA0_HUNF_GC (0x81<<0)  /* Timer/Counter A0 high byte timer underflow */
#define EVSYS_CHANNEL1_TCA0_CMP0_LCMP0_GC (0x84<<0)  /* Timer/Counter A0 compare 0 / low byte timer compare 0 */
#define EVSYS_CHANNEL1_TCA0_CMP1_LCMP1_GC (0x85<<0)  /* Timer/Counter A0 compare 1 / low byte timer compare 1 */
#define EVSYS_CHANNEL1_TCA0_CMP2_LCMP2_GC (0x86<<0)  /* Timer/Counter A0 compare 2 / low byte timer compare 2 */
#define EVSYS_CHANNEL1_TCA1_OVF_LUNF_GC (0x88<<0)  /* Timer/Counter A1 overflow / low byte timer underflow */
#define EVSYS_CHANNEL1_TCA1_HUNF_GC (0x89<<0)  /* Timer/Counter A1 high byte timer underflow */
#define EVSYS_CHANNEL1_TCA1_CMP0_LCMP0_GC (0x8C<<0)  /* Timer/Counter A1 compare 0 / low byte timer compare 0 */
#define EVSYS_CHANNEL1_TCA1_CMP1_LCMP1_GC (0x8D<<0)  /* Timer/Counter A1 compare 1 / low byte timer compare 1 */
#define EVSYS_CHANNEL1_TCA1_CMP2_LCMP2_GC (0x8E<<0)  /* Timer/Counter A1 compare 2 / low byte timer compare 2 */
#define EVSYS_CHANNEL1_TCB0_CAPT_GC (0xA0<<0)  /* Timer/Counter B0 capture */
#define EVSYS_CHANNEL1_TCB0_OVF_GC (0xA1<<0)  /* Timer/Counter B0 overflow */
#define EVSYS_CHANNEL1_TCB1_CAPT_GC (0xA2<<0)  /* Timer/Counter B1 capture */
#define EVSYS_CHANNEL1_TCB1_OVF_GC (0xA3<<0)  /* Timer/Counter B1 overflow */
#define EVSYS_CHANNEL1_TCB2_CAPT_GC (0xA4<<0)  /* Timer/Counter B2 capture */
#define EVSYS_CHANNEL1_TCB2_OVF_GC (0xA5<<0)  /* Timer/Counter B2 overflow */
#define EVSYS_CHANNEL1_TCB3_CAPT_GC (0xA6<<0)  /* Timer/Counter B3 capture */
#define EVSYS_CHANNEL1_TCB3_OVF_GC (0xA7<<0)  /* Timer/Counter B3 overflow */
#define EVSYS_CHANNEL1_TCB4_CAPT_GC (0xA8<<0)  /* Timer/Counter B4 capture */
#define EVSYS_CHANNEL1_TCB4_OVF_GC (0xA9<<0)  /* Timer/Counter B4 overflow */
#define EVSYS_CHANNEL1_TCD0_CMPBCLR_GC (0xB0<<0)  /* Timer/Counter D0 event 0 */
#define EVSYS_CHANNEL1_TCD0_CMPASET_GC (0xB1<<0)  /* Timer/Counter D0 event 1 */
#define EVSYS_CHANNEL1_TCD0_CMPBSET_GC (0xB2<<0)  /* Timer/Counter D0 event 2 */
#define EVSYS_CHANNEL1_TCD0_PROGEV_GC (0xB3<<0)  /* Timer/Counter D0 event 3 */
/* Channel 2 generator select select */
#define EVSYS_CHANNEL2_OFF_GC (0x00<<0)  /* Off */
#define EVSYS_CHANNEL2_UPDI_SYNCH_GC (0x01<<0)  /* UPDI SYNCH character */
#define EVSYS_CHANNEL2_RTC_OVF_GC (0x06<<0)  /* Real Time Counter overflow */
#define EVSYS_CHANNEL2_RTC_CMP_GC (0x07<<0)  /* Real Time Counter compare */
#define EVSYS_CHANNEL2_RTC_PIT_DIV8192_GC (0x08<<0)  /* Periodic Interrupt Timer output 0 */
#define EVSYS_CHANNEL2_RTC_PIT_DIV4096_GC (0x09<<0)  /* Periodic Interrupt Timer output 1 */
#define EVSYS_CHANNEL2_RTC_PIT_DIV2048_GC (0x0A<<0)  /* Periodic Interrupt Timer output 2 */
#define EVSYS_CHANNEL2_RTC_PIT_DIV1024_GC (0x0B<<0)  /* Periodic Interrupt Timer output 3 */
#define EVSYS_CHANNEL2_CCL_LUT0_GC (0x10<<0)  /* Configurable Custom Logic LUT0 */
#define EVSYS_CHANNEL2_CCL_LUT1_GC (0x11<<0)  /* Configurable Custom Logic LUT1 */
#define EVSYS_CHANNEL2_CCL_LUT2_GC (0x12<<0)  /* Configurable Custom Logic LUT2 */
#define EVSYS_CHANNEL2_CCL_LUT3_GC (0x13<<0)  /* Configurable Custom Logic LUT3 */
#define EVSYS_CHANNEL2_CCL_LUT4_GC (0x14<<0)  /* Configurable Custom Logic LUT4 */
#define EVSYS_CHANNEL2_CCL_LUT5_GC (0x15<<0)  /* Configurable Custom Logic LUT5 */
#define EVSYS_CHANNEL2_AC0_OUT_GC (0x20<<0)  /* Analog Comparator 0 out */
#define EVSYS_CHANNEL2_AC1_OUT_GC (0x21<<0)  /* Analog Comparator 1 out */
#define EVSYS_CHANNEL2_AC2_OUT_GC (0x22<<0)  /* Analog Comparator 2 out */
#define EVSYS_CHANNEL2_ADC0_RESRDY_GC (0x24<<0)  /* ADC 0 Result Ready */
#define EVSYS_CHANNEL2_PTC_RESRDY_GC (0x28<<0)  /* PTC Result Ready */
#define EVSYS_CHANNEL2_ZCD0_GC (0x30<<0)  /* Zero Cross Detect 0 out */
#define EVSYS_CHANNEL2_ZCD1_GC (0x31<<0)  /* Zero Cross Detect 1 out */
#define EVSYS_CHANNEL2_ZCD2_GC (0x32<<0)  /* Zero Cross Detect 2 out */
#define EVSYS_CHANNEL2_PORTC_PIN0_GC (0x40<<0)  /* Port C Pin 0 */
#define EVSYS_CHANNEL2_PORTC_PIN1_GC (0x41<<0)  /* Port C Pin 1 */
#define EVSYS_CHANNEL2_PORTC_PIN2_GC (0x42<<0)  /* Port C Pin 2 */
#define EVSYS_CHANNEL2_PORTC_PIN3_GC (0x43<<0)  /* Port C Pin 3 */
#define EVSYS_CHANNEL2_PORTC_PIN4_GC (0x44<<0)  /* Port C Pin 4 */
#define EVSYS_CHANNEL2_PORTC_PIN5_GC (0x45<<0)  /* Port C Pin 5 */
#define EVSYS_CHANNEL2_PORTC_PIN6_GC (0x46<<0)  /* Port C Pin 6 */
#define EVSYS_CHANNEL2_PORTC_PIN7_GC (0x47<<0)  /* Port C Pin 7 */
#define EVSYS_CHANNEL2_PORTD_PIN0_GC (0x48<<0)  /* Port D Pin 0 */
#define EVSYS_CHANNEL2_PORTD_PIN1_GC (0x49<<0)  /* Port D Pin 1 */
#define EVSYS_CHANNEL2_PORTD_PIN2_GC (0x4A<<0)  /* Port D Pin 2 */
#define EVSYS_CHANNEL2_PORTD_PIN3_GC (0x4B<<0)  /* Port D Pin 3 */
#define EVSYS_CHANNEL2_PORTD_PIN4_GC (0x4C<<0)  /* Port D Pin 4 */
#define EVSYS_CHANNEL2_PORTD_PIN5_GC (0x4D<<0)  /* Port D Pin 5 */
#define EVSYS_CHANNEL2_PORTD_PIN6_GC (0x4E<<0)  /* Port D Pin 6 */
#define EVSYS_CHANNEL2_PORTD_PIN7_GC (0x4F<<0)  /* Port D Pin 7 */
#define EVSYS_CHANNEL2_USART0_XCK_GC (0x60<<0)  /* USART 0 XCK */
#define EVSYS_CHANNEL2_USART1_XCK_GC (0x61<<0)  /* USART 1 XCK */
#define EVSYS_CHANNEL2_USART2_XCK_GC (0x62<<0)  /* USART 2 XCK */
#define EVSYS_CHANNEL2_USART3_XCK_GC (0x63<<0)  /* USART 3 XCK */
#define EVSYS_CHANNEL2_USART4_XCK_GC (0x64<<0)  /* USART 4 XCK */
#define EVSYS_CHANNEL2_USART5_XCK_GC (0x65<<0)  /* USART 5 XCK */
#define EVSYS_CHANNEL2_SPI0_SCK_GC (0x68<<0)  /* SPI 0 SCK */
#define EVSYS_CHANNEL2_SPI1_SCK_GC (0x69<<0)  /* SPI 1 SCK */
#define EVSYS_CHANNEL2_TCA0_OVF_LUNF_GC (0x80<<0)  /* Timer/Counter A0 overflow / low byte timer underflow */
#define EVSYS_CHANNEL2_TCA0_HUNF_GC (0x81<<0)  /* Timer/Counter A0 high byte timer underflow */
#define EVSYS_CHANNEL2_TCA0_CMP0_LCMP0_GC (0x84<<0)  /* Timer/Counter A0 compare 0 / low byte timer compare 0 */
#define EVSYS_CHANNEL2_TCA0_CMP1_LCMP1_GC (0x85<<0)  /* Timer/Counter A0 compare 1 / low byte timer compare 1 */
#define EVSYS_CHANNEL2_TCA0_CMP2_LCMP2_GC (0x86<<0)  /* Timer/Counter A0 compare 2 / low byte timer compare 2 */
#define EVSYS_CHANNEL2_TCA1_OVF_LUNF_GC (0x88<<0)  /* Timer/Counter A1 overflow / low byte timer underflow */
#define EVSYS_CHANNEL2_TCA1_HUNF_GC (0x89<<0)  /* Timer/Counter A1 high byte timer underflow */
#define EVSYS_CHANNEL2_TCA1_CMP0_LCMP0_GC (0x8C<<0)  /* Timer/Counter A1 compare 0 / low byte timer compare 0 */
#define EVSYS_CHANNEL2_TCA1_CMP1_LCMP1_GC (0x8D<<0)  /* Timer/Counter A1 compare 1 / low byte timer compare 1 */
#define EVSYS_CHANNEL2_TCA1_CMP2_LCMP2_GC (0x8E<<0)  /* Timer/Counter A1 compare 2 / low byte timer compare 2 */
#define EVSYS_CHANNEL2_TCB0_CAPT_GC (0xA0<<0)  /* Timer/Counter B0 capture */
#define EVSYS_CHANNEL2_TCB0_OVF_GC (0xA1<<0)  /* Timer/Counter B0 overflow */
#define EVSYS_CHANNEL2_TCB1_CAPT_GC (0xA2<<0)  /* Timer/Counter B1 capture */
#define EVSYS_CHANNEL2_TCB1_OVF_GC (0xA3<<0)  /* Timer/Counter B1 overflow */
#define EVSYS_CHANNEL2_TCB2_CAPT_GC (0xA4<<0)  /* Timer/Counter B2 capture */
#define EVSYS_CHANNEL2_TCB2_OVF_GC (0xA5<<0)  /* Timer/Counter B2 overflow */
#define EVSYS_CHANNEL2_TCB3_CAPT_GC (0xA6<<0)  /* Timer/Counter B3 capture */
#define EVSYS_CHANNEL2_TCB3_OVF_GC (0xA7<<0)  /* Timer/Counter B3 overflow */
#define EVSYS_CHANNEL2_TCB4_CAPT_GC (0xA8<<0)  /* Timer/Counter B4 capture */
#define EVSYS_CHANNEL2_TCB4_OVF_GC (0xA9<<0)  /* Timer/Counter B4 overflow */
#define EVSYS_CHANNEL2_TCD0_CMPBCLR_GC (0xB0<<0)  /* Timer/Counter D0 event 0 */
#define EVSYS_CHANNEL2_TCD0_CMPASET_GC (0xB1<<0)  /* Timer/Counter D0 event 1 */
#define EVSYS_CHANNEL2_TCD0_CMPBSET_GC (0xB2<<0)  /* Timer/Counter D0 event 2 */
#define EVSYS_CHANNEL2_TCD0_PROGEV_GC (0xB3<<0)  /* Timer/Counter D0 event 3 */
/* Channel 3 generator select select */
#define EVSYS_CHANNEL3_OFF_GC (0x00<<0)  /* Off */
#define EVSYS_CHANNEL3_UPDI_SYNCH_GC (0x01<<0)  /* UPDI SYNCH character */
#define EVSYS_CHANNEL3_RTC_OVF_GC (0x06<<0)  /* Real Time Counter overflow */
#define EVSYS_CHANNEL3_RTC_CMP_GC (0x07<<0)  /* Real Time Counter compare */
#define EVSYS_CHANNEL3_RTC_PIT_DIV512_GC (0x08<<0)  /* Periodic Interrupt Timer output 0 */
#define EVSYS_CHANNEL3_RTC_PIT_DIV256_GC (0x09<<0)  /* Periodic Interrupt Timer output 1 */
#define EVSYS_CHANNEL3_RTC_PIT_DIV128_GC (0x0A<<0)  /* Periodic Interrupt Timer output 2 */
#define EVSYS_CHANNEL3_RTC_PIT_DIV64_GC (0x0B<<0)  /* Periodic Interrupt Timer output 3 */
#define EVSYS_CHANNEL3_CCL_LUT0_GC (0x10<<0)  /* Configurable Custom Logic LUT0 */
#define EVSYS_CHANNEL3_CCL_LUT1_GC (0x11<<0)  /* Configurable Custom Logic LUT1 */
#define EVSYS_CHANNEL3_CCL_LUT2_GC (0x12<<0)  /* Configurable Custom Logic LUT2 */
#define EVSYS_CHANNEL3_CCL_LUT3_GC (0x13<<0)  /* Configurable Custom Logic LUT3 */
#define EVSYS_CHANNEL3_CCL_LUT4_GC (0x14<<0)  /* Configurable Custom Logic LUT4 */
#define EVSYS_CHANNEL3_CCL_LUT5_GC (0x15<<0)  /* Configurable Custom Logic LUT5 */
#define EVSYS_CHANNEL3_AC0_OUT_GC (0x20<<0)  /* Analog Comparator 0 out */
#define EVSYS_CHANNEL3_AC1_OUT_GC (0x21<<0)  /* Analog Comparator 1 out */
#define EVSYS_CHANNEL3_AC2_OUT_GC (0x22<<0)  /* Analog Comparator 2 out */
#define EVSYS_CHANNEL3_ADC0_RESRDY_GC (0x24<<0)  /* ADC 0 Result Ready */
#define EVSYS_CHANNEL3_PTC_RESRDY_GC (0x28<<0)  /* PTC Result Ready */
#define EVSYS_CHANNEL3_ZCD0_GC (0x30<<0)  /* Zero Cross Detect 0 out */
#define EVSYS_CHANNEL3_ZCD1_GC (0x31<<0)  /* Zero Cross Detect 1 out */
#define EVSYS_CHANNEL3_ZCD2_GC (0x32<<0)  /* Zero Cross Detect 2 out */
#define EVSYS_CHANNEL3_PORTC_PIN0_GC (0x40<<0)  /* Port C Pin 0 */
#define EVSYS_CHANNEL3_PORTC_PIN1_GC (0x41<<0)  /* Port C Pin 1 */
#define EVSYS_CHANNEL3_PORTC_PIN2_GC (0x42<<0)  /* Port C Pin 2 */
#define EVSYS_CHANNEL3_PORTC_PIN3_GC (0x43<<0)  /* Port C Pin 3 */
#define EVSYS_CHANNEL3_PORTC_PIN4_GC (0x44<<0)  /* Port C Pin 4 */
#define EVSYS_CHANNEL3_PORTC_PIN5_GC (0x45<<0)  /* Port C Pin 5 */
#define EVSYS_CHANNEL3_PORTC_PIN6_GC (0x46<<0)  /* Port C Pin 6 */
#define EVSYS_CHANNEL3_PORTC_PIN7_GC (0x47<<0)  /* Port C Pin 7 */
#define EVSYS_CHANNEL3_PORTD_PIN0_GC (0x48<<0)  /* Port D Pin 0 */
#define EVSYS_CHANNEL3_PORTD_PIN1_GC (0x49<<0)  /* Port D Pin 1 */
#define EVSYS_CHANNEL3_PORTD_PIN2_GC (0x4A<<0)  /* Port D Pin 2 */
#define EVSYS_CHANNEL3_PORTD_PIN3_GC (0x4B<<0)  /* Port D Pin 3 */
#define EVSYS_CHANNEL3_PORTD_PIN4_GC (0x4C<<0)  /* Port D Pin 4 */
#define EVSYS_CHANNEL3_PORTD_PIN5_GC (0x4D<<0)  /* Port D Pin 5 */
#define EVSYS_CHANNEL3_PORTD_PIN6_GC (0x4E<<0)  /* Port D Pin 6 */
#define EVSYS_CHANNEL3_PORTD_PIN7_GC (0x4F<<0)  /* Port D Pin 7 */
#define EVSYS_CHANNEL3_USART0_XCK_GC (0x60<<0)  /* USART 0 XCK */
#define EVSYS_CHANNEL3_USART1_XCK_GC (0x61<<0)  /* USART 1 XCK */
#define EVSYS_CHANNEL3_USART2_XCK_GC (0x62<<0)  /* USART 2 XCK */
#define EVSYS_CHANNEL3_USART3_XCK_GC (0x63<<0)  /* USART 3 XCK */
#define EVSYS_CHANNEL3_USART4_XCK_GC (0x64<<0)  /* USART 4 XCK */
#define EVSYS_CHANNEL3_USART5_XCK_GC (0x65<<0)  /* USART 5 XCK */
#define EVSYS_CHANNEL3_SPI0_SCK_GC (0x68<<0)  /* SPI 0 SCK */
#define EVSYS_CHANNEL3_SPI1_SCK_GC (0x69<<0)  /* SPI 1 SCK */
#define EVSYS_CHANNEL3_TCA0_OVF_LUNF_GC (0x80<<0)  /* Timer/Counter A0 overflow / low byte timer underflow */
#define EVSYS_CHANNEL3_TCA0_HUNF_GC (0x81<<0)  /* Timer/Counter A0 high byte timer underflow */
#define EVSYS_CHANNEL3_TCA0_CMP0_LCMP0_GC (0x84<<0)  /* Timer/Counter A0 compare 0 / low byte timer compare 0 */
#define EVSYS_CHANNEL3_TCA0_CMP1_LCMP1_GC (0x85<<0)  /* Timer/Counter A0 compare 1 / low byte timer compare 1 */
#define EVSYS_CHANNEL3_TCA0_CMP2_LCMP2_GC (0x86<<0)  /* Timer/Counter A0 compare 2 / low byte timer compare 2 */
#define EVSYS_CHANNEL3_TCA1_OVF_LUNF_GC (0x88<<0)  /* Timer/Counter A1 overflow / low byte timer underflow */
#define EVSYS_CHANNEL3_TCA1_HUNF_GC (0x89<<0)  /* Timer/Counter A1 high byte timer underflow */
#define EVSYS_CHANNEL3_TCA1_CMP0_LCMP0_GC (0x8C<<0)  /* Timer/Counter A1 compare 0 / low byte timer compare 0 */
#define EVSYS_CHANNEL3_TCA1_CMP1_LCMP1_GC (0x8D<<0)  /* Timer/Counter A1 compare 1 / low byte timer compare 1 */
#define EVSYS_CHANNEL3_TCA1_CMP2_LCMP2_GC (0x8E<<0)  /* Timer/Counter A1 compare 2 / low byte timer compare 2 */
#define EVSYS_CHANNEL3_TCB0_CAPT_GC (0xA0<<0)  /* Timer/Counter B0 capture */
#define EVSYS_CHANNEL3_TCB0_OVF_GC (0xA1<<0)  /* Timer/Counter B0 overflow */
#define EVSYS_CHANNEL3_TCB1_CAPT_GC (0xA2<<0)  /* Timer/Counter B1 capture */
#define EVSYS_CHANNEL3_TCB1_OVF_GC (0xA3<<0)  /* Timer/Counter B1 overflow */
#define EVSYS_CHANNEL3_TCB2_CAPT_GC (0xA4<<0)  /* Timer/Counter B2 capture */
#define EVSYS_CHANNEL3_TCB2_OVF_GC (0xA5<<0)  /* Timer/Counter B2 overflow */
#define EVSYS_CHANNEL3_TCB3_CAPT_GC (0xA6<<0)  /* Timer/Counter B3 capture */
#define EVSYS_CHANNEL3_TCB3_OVF_GC (0xA7<<0)  /* Timer/Counter B3 overflow */
#define EVSYS_CHANNEL3_TCB4_CAPT_GC (0xA8<<0)  /* Timer/Counter B4 capture */
#define EVSYS_CHANNEL3_TCB4_OVF_GC (0xA9<<0)  /* Timer/Counter B4 overflow */
#define EVSYS_CHANNEL3_TCD0_CMPBCLR_GC (0xB0<<0)  /* Timer/Counter D0 event 0 */
#define EVSYS_CHANNEL3_TCD0_CMPASET_GC (0xB1<<0)  /* Timer/Counter D0 event 1 */
#define EVSYS_CHANNEL3_TCD0_CMPBSET_GC (0xB2<<0)  /* Timer/Counter D0 event 2 */
#define EVSYS_CHANNEL3_TCD0_PROGEV_GC (0xB3<<0)  /* Timer/Counter D0 event 3 */
/* Channel 4 generator select select */
#define EVSYS_CHANNEL4_OFF_GC (0x00<<0)  /* Off */
#define EVSYS_CHANNEL4_UPDI_SYNCH_GC (0x01<<0)  /* UPDI SYNCH character */
#define EVSYS_CHANNEL4_RTC_OVF_GC (0x06<<0)  /* Real Time Counter overflow */
#define EVSYS_CHANNEL4_RTC_CMP_GC (0x07<<0)  /* Real Time Counter compare */
#define EVSYS_CHANNEL4_RTC_PIT_DIV8192_GC (0x08<<0)  /* Periodic Interrupt Timer output 0 */
#define EVSYS_CHANNEL4_RTC_PIT_DIV4096_GC (0x09<<0)  /* Periodic Interrupt Timer output 1 */
#define EVSYS_CHANNEL4_RTC_PIT_DIV2048_GC (0x0A<<0)  /* Periodic Interrupt Timer output 2 */
#define EVSYS_CHANNEL4_RTC_PIT_DIV1024_GC (0x0B<<0)  /* Periodic Interrupt Timer output 3 */
#define EVSYS_CHANNEL4_CCL_LUT0_GC (0x10<<0)  /* Configurable Custom Logic LUT0 */
#define EVSYS_CHANNEL4_CCL_LUT1_GC (0x11<<0)  /* Configurable Custom Logic LUT1 */
#define EVSYS_CHANNEL4_CCL_LUT2_GC (0x12<<0)  /* Configurable Custom Logic LUT2 */
#define EVSYS_CHANNEL4_CCL_LUT3_GC (0x13<<0)  /* Configurable Custom Logic LUT3 */
#define EVSYS_CHANNEL4_CCL_LUT4_GC (0x14<<0)  /* Configurable Custom Logic LUT4 */
#define EVSYS_CHANNEL4_CCL_LUT5_GC (0x15<<0)  /* Configurable Custom Logic LUT5 */
#define EVSYS_CHANNEL4_AC0_OUT_GC (0x20<<0)  /* Analog Comparator 0 out */
#define EVSYS_CHANNEL4_AC1_OUT_GC (0x21<<0)  /* Analog Comparator 1 out */
#define EVSYS_CHANNEL4_AC2_OUT_GC (0x22<<0)  /* Analog Comparator 2 out */
#define EVSYS_CHANNEL4_ADC0_RESRDY_GC (0x24<<0)  /* ADC 0 Result Ready */
#define EVSYS_CHANNEL4_PTC_RESRDY_GC (0x28<<0)  /* PTC Result Ready */
#define EVSYS_CHANNEL4_ZCD0_GC (0x30<<0)  /* Zero Cross Detect 0 out */
#define EVSYS_CHANNEL4_ZCD1_GC (0x31<<0)  /* Zero Cross Detect 1 out */
#define EVSYS_CHANNEL4_ZCD2_GC (0x32<<0)  /* Zero Cross Detect 2 out */
#define EVSYS_CHANNEL4_PORTE_PIN0_GC (0x40<<0)  /* Port E Pin 0 */
#define EVSYS_CHANNEL4_PORTE_PIN1_GC (0x41<<0)  /* Port E Pin 1 */
#define EVSYS_CHANNEL4_PORTE_PIN2_GC (0x42<<0)  /* Port E Pin 2 */
#define EVSYS_CHANNEL4_PORTE_PIN3_GC (0x43<<0)  /* Port E Pin 3 */
#define EVSYS_CHANNEL4_PORTE_PIN4_GC (0x44<<0)  /* Port E Pin 4 */
#define EVSYS_CHANNEL4_PORTE_PIN5_GC (0x45<<0)  /* Port E Pin 5 */
#define EVSYS_CHANNEL4_PORTE_PIN6_GC (0x46<<0)  /* Port E Pin 6 */
#define EVSYS_CHANNEL4_PORTE_PIN7_GC (0x47<<0)  /* Port E Pin 7 */
#define EVSYS_CHANNEL4_PORTF_PIN0_GC (0x48<<0)  /* Port F Pin 0 */
#define EVSYS_CHANNEL4_PORTF_PIN1_GC (0x49<<0)  /* Port F Pin 1 */
#define EVSYS_CHANNEL4_PORTF_PIN2_GC (0x4A<<0)  /* Port F Pin 2 */
#define EVSYS_CHANNEL4_PORTF_PIN3_GC (0x4B<<0)  /* Port F Pin 3 */
#define EVSYS_CHANNEL4_PORTF_PIN4_GC (0x4C<<0)  /* Port F Pin 4 */
#define EVSYS_CHANNEL4_PORTF_PIN5_GC (0x4D<<0)  /* Port F Pin 5 */
#define EVSYS_CHANNEL4_PORTF_PIN6_GC (0x4E<<0)  /* Port F Pin 6 */
#define EVSYS_CHANNEL4_USART0_XCK_GC (0x60<<0)  /* USART 0 XCK */
#define EVSYS_CHANNEL4_USART1_XCK_GC (0x61<<0)  /* USART 1 XCK */
#define EVSYS_CHANNEL4_USART2_XCK_GC (0x62<<0)  /* USART 2 XCK */
#define EVSYS_CHANNEL4_USART3_XCK_GC (0x63<<0)  /* USART 3 XCK */
#define EVSYS_CHANNEL4_USART4_XCK_GC (0x64<<0)  /* USART 4 XCK */
#define EVSYS_CHANNEL4_USART5_XCK_GC (0x65<<0)  /* USART 5 XCK */
#define EVSYS_CHANNEL4_SPI0_SCK_GC (0x68<<0)  /* SPI 0 SCK */
#define EVSYS_CHANNEL4_SPI1_SCK_GC (0x69<<0)  /* SPI 1 SCK */
#define EVSYS_CHANNEL4_TCA0_OVF_LUNF_GC (0x80<<0)  /* Timer/Counter A0 overflow / low byte timer underflow */
#define EVSYS_CHANNEL4_TCA0_HUNF_GC (0x81<<0)  /* Timer/Counter A0 high byte timer underflow */
#define EVSYS_CHANNEL4_TCA0_CMP0_LCMP0_GC (0x84<<0)  /* Timer/Counter A0 compare 0 / low byte timer compare 0 */
#define EVSYS_CHANNEL4_TCA0_CMP1_LCMP1_GC (0x85<<0)  /* Timer/Counter A0 compare 1 / low byte timer compare 1 */
#define EVSYS_CHANNEL4_TCA0_CMP2_LCMP2_GC (0x86<<0)  /* Timer/Counter A0 compare 2 / low byte timer compare 2 */
#define EVSYS_CHANNEL4_TCA1_OVF_LUNF_GC (0x88<<0)  /* Timer/Counter A1 overflow / low byte timer underflow */
#define EVSYS_CHANNEL4_TCA1_HUNF_GC (0x89<<0)  /* Timer/Counter A1 high byte timer underflow */
#define EVSYS_CHANNEL4_TCA1_CMP0_LCMP0_GC (0x8C<<0)  /* Timer/Counter A1 compare 0 / low byte timer compare 0 */
#define EVSYS_CHANNEL4_TCA1_CMP1_LCMP1_GC (0x8D<<0)  /* Timer/Counter A1 compare 1 / low byte timer compare 1 */
#define EVSYS_CHANNEL4_TCA1_CMP2_LCMP2_GC (0x8E<<0)  /* Timer/Counter A1 compare 2 / low byte timer compare 2 */
#define EVSYS_CHANNEL4_TCB0_CAPT_GC (0xA0<<0)  /* Timer/Counter B0 capture */
#define EVSYS_CHANNEL4_TCB0_OVF_GC (0xA1<<0)  /* Timer/Counter B0 overflow */
#define EVSYS_CHANNEL4_TCB1_CAPT_GC (0xA2<<0)  /* Timer/Counter B1 capture */
#define EVSYS_CHANNEL4_TCB1_OVF_GC (0xA3<<0)  /* Timer/Counter B1 overflow */
#define EVSYS_CHANNEL4_TCB2_CAPT_GC (0xA4<<0)  /* Timer/Counter B2 capture */
#define EVSYS_CHANNEL4_TCB2_OVF_GC (0xA5<<0)  /* Timer/Counter B2 overflow */
#define EVSYS_CHANNEL4_TCB3_CAPT_GC (0xA6<<0)  /* Timer/Counter B3 capture */
#define EVSYS_CHANNEL4_TCB3_OVF_GC (0xA7<<0)  /* Timer/Counter B3 overflow */
#define EVSYS_CHANNEL4_TCB4_CAPT_GC (0xA8<<0)  /* Timer/Counter B4 capture */
#define EVSYS_CHANNEL4_TCB4_OVF_GC (0xA9<<0)  /* Timer/Counter B4 overflow */
#define EVSYS_CHANNEL4_TCD0_CMPBCLR_GC (0xB0<<0)  /* Timer/Counter D0 event 0 */
#define EVSYS_CHANNEL4_TCD0_CMPASET_GC (0xB1<<0)  /* Timer/Counter D0 event 1 */
#define EVSYS_CHANNEL4_TCD0_CMPBSET_GC (0xB2<<0)  /* Timer/Counter D0 event 2 */
#define EVSYS_CHANNEL4_TCD0_PROGEV_GC (0xB3<<0)  /* Timer/Counter D0 event 3 */
/* Channel 5 generator select select */
#define EVSYS_CHANNEL5_OFF_GC (0x00<<0)  /* Off */
#define EVSYS_CHANNEL5_UPDI_SYNCH_GC (0x01<<0)  /* UPDI SYNCH character */
#define EVSYS_CHANNEL5_RTC_OVF_GC (0x06<<0)  /* Real Time Counter overflow */
#define EVSYS_CHANNEL5_RTC_CMP_GC (0x07<<0)  /* Real Time Counter compare */
#define EVSYS_CHANNEL5_RTC_PIT_DIV512_GC (0x08<<0)  /* Periodic Interrupt Timer output 0 */
#define EVSYS_CHANNEL5_RTC_PIT_DIV256_GC (0x09<<0)  /* Periodic Interrupt Timer output 1 */
#define EVSYS_CHANNEL5_RTC_PIT_DIV128_GC (0x0A<<0)  /* Periodic Interrupt Timer output 2 */
#define EVSYS_CHANNEL5_RTC_PIT_DIV64_GC (0x0B<<0)  /* Periodic Interrupt Timer output 3 */
#define EVSYS_CHANNEL5_CCL_LUT0_GC (0x10<<0)  /* Configurable Custom Logic LUT0 */
#define EVSYS_CHANNEL5_CCL_LUT1_GC (0x11<<0)  /* Configurable Custom Logic LUT1 */
#define EVSYS_CHANNEL5_CCL_LUT2_GC (0x12<<0)  /* Configurable Custom Logic LUT2 */
#define EVSYS_CHANNEL5_CCL_LUT3_GC (0x13<<0)  /* Configurable Custom Logic LUT3 */
#define EVSYS_CHANNEL5_CCL_LUT4_GC (0x14<<0)  /* Configurable Custom Logic LUT4 */
#define EVSYS_CHANNEL5_CCL_LUT5_GC (0x15<<0)  /* Configurable Custom Logic LUT5 */
#define EVSYS_CHANNEL5_AC0_OUT_GC (0x20<<0)  /* Analog Comparator 0 out */
#define EVSYS_CHANNEL5_AC1_OUT_GC (0x21<<0)  /* Analog Comparator 1 out */
#define EVSYS_CHANNEL5_AC2_OUT_GC (0x22<<0)  /* Analog Comparator 2 out */
#define EVSYS_CHANNEL5_ADC0_RESRDY_GC (0x24<<0)  /* ADC 0 Result Ready */
#define EVSYS_CHANNEL5_PTC_RESRDY_GC (0x28<<0)  /* PTC Result Ready */
#define EVSYS_CHANNEL5_ZCD0_GC (0x30<<0)  /* Zero Cross Detect 0 out */
#define EVSYS_CHANNEL5_ZCD1_GC (0x31<<0)  /* Zero Cross Detect 1 out */
#define EVSYS_CHANNEL5_ZCD2_GC (0x32<<0)  /* Zero Cross Detect 2 out */
#define EVSYS_CHANNEL5_PORTE_PIN0_GC (0x40<<0)  /* Port E Pin 0 */
#define EVSYS_CHANNEL5_PORTE_PIN1_GC (0x41<<0)  /* Port E Pin 1 */
#define EVSYS_CHANNEL5_PORTE_PIN2_GC (0x42<<0)  /* Port E Pin 2 */
#define EVSYS_CHANNEL5_PORTE_PIN3_GC (0x43<<0)  /* Port E Pin 3 */
#define EVSYS_CHANNEL5_PORTE_PIN4_GC (0x44<<0)  /* Port E Pin 4 */
#define EVSYS_CHANNEL5_PORTE_PIN5_GC (0x45<<0)  /* Port E Pin 5 */
#define EVSYS_CHANNEL5_PORTE_PIN6_GC (0x46<<0)  /* Port E Pin 6 */
#define EVSYS_CHANNEL5_PORTE_PIN7_GC (0x47<<0)  /* Port E Pin 7 */
#define EVSYS_CHANNEL5_PORTF_PIN0_GC (0x48<<0)  /* Port F Pin 0 */
#define EVSYS_CHANNEL5_PORTF_PIN1_GC (0x49<<0)  /* Port F Pin 1 */
#define EVSYS_CHANNEL5_PORTF_PIN2_GC (0x4A<<0)  /* Port F Pin 2 */
#define EVSYS_CHANNEL5_PORTF_PIN3_GC (0x4B<<0)  /* Port F Pin 3 */
#define EVSYS_CHANNEL5_PORTF_PIN4_GC (0x4C<<0)  /* Port F Pin 4 */
#define EVSYS_CHANNEL5_PORTF_PIN5_GC (0x4D<<0)  /* Port F Pin 5 */
#define EVSYS_CHANNEL5_PORTF_PIN6_GC (0x4E<<0)  /* Port F Pin 6 */
#define EVSYS_CHANNEL5_USART0_XCK_GC (0x60<<0)  /* USART 0 XCK */
#define EVSYS_CHANNEL5_USART1_XCK_GC (0x61<<0)  /* USART 1 XCK */
#define EVSYS_CHANNEL5_USART2_XCK_GC (0x62<<0)  /* USART 2 XCK */
#define EVSYS_CHANNEL5_USART3_XCK_GC (0x63<<0)  /* USART 3 XCK */
#define EVSYS_CHANNEL5_USART4_XCK_GC (0x64<<0)  /* USART 4 XCK */
#define EVSYS_CHANNEL5_USART5_XCK_GC (0x65<<0)  /* USART 5 XCK */
#define EVSYS_CHANNEL5_SPI0_SCK_GC (0x68<<0)  /* SPI 0 SCK */
#define EVSYS_CHANNEL5_SPI1_SCK_GC (0x69<<0)  /* SPI 1 SCK */
#define EVSYS_CHANNEL5_TCA0_OVF_LUNF_GC (0x80<<0)  /* Timer/Counter A0 overflow / low byte timer underflow */
#define EVSYS_CHANNEL5_TCA0_HUNF_GC (0x81<<0)  /* Timer/Counter A0 high byte timer underflow */
#define EVSYS_CHANNEL5_TCA0_CMP0_LCMP0_GC (0x84<<0)  /* Timer/Counter A0 compare 0 / low byte timer compare 0 */
#define EVSYS_CHANNEL5_TCA0_CMP1_LCMP1_GC (0x85<<0)  /* Timer/Counter A0 compare 1 / low byte timer compare 1 */
#define EVSYS_CHANNEL5_TCA0_CMP2_LCMP2_GC (0x86<<0)  /* Timer/Counter A0 compare 2 / low byte timer compare 2 */
#define EVSYS_CHANNEL5_TCA1_OVF_LUNF_GC (0x88<<0)  /* Timer/Counter A1 overflow / low byte timer underflow */
#define EVSYS_CHANNEL5_TCA1_HUNF_GC (0x89<<0)  /* Timer/Counter A1 high byte timer underflow */
#define EVSYS_CHANNEL5_TCA1_CMP0_LCMP0_GC (0x8C<<0)  /* Timer/Counter A1 compare 0 / low byte timer compare 0 */
#define EVSYS_CHANNEL5_TCA1_CMP1_LCMP1_GC (0x8D<<0)  /* Timer/Counter A1 compare 1 / low byte timer compare 1 */
#define EVSYS_CHANNEL5_TCA1_CMP2_LCMP2_GC (0x8E<<0)  /* Timer/Counter A1 compare 2 / low byte timer compare 2 */
#define EVSYS_CHANNEL5_TCB0_CAPT_GC (0xA0<<0)  /* Timer/Counter B0 capture */
#define EVSYS_CHANNEL5_TCB0_OVF_GC (0xA1<<0)  /* Timer/Counter B0 overflow */
#define EVSYS_CHANNEL5_TCB1_CAPT_GC (0xA2<<0)  /* Timer/Counter B1 capture */
#define EVSYS_CHANNEL5_TCB1_OVF_GC (0xA3<<0)  /* Timer/Counter B1 overflow */
#define EVSYS_CHANNEL5_TCB2_CAPT_GC (0xA4<<0)  /* Timer/Counter B2 capture */
#define EVSYS_CHANNEL5_TCB2_OVF_GC (0xA5<<0)  /* Timer/Counter B2 overflow */
#define EVSYS_CHANNEL5_TCB3_CAPT_GC (0xA6<<0)  /* Timer/Counter B3 capture */
#define EVSYS_CHANNEL5_TCB3_OVF_GC (0xA7<<0)  /* Timer/Counter B3 overflow */
#define EVSYS_CHANNEL5_TCB4_CAPT_GC (0xA8<<0)  /* Timer/Counter B4 capture */
#define EVSYS_CHANNEL5_TCB4_OVF_GC (0xA9<<0)  /* Timer/Counter B4 overflow */
#define EVSYS_CHANNEL5_TCD0_CMPBCLR_GC (0xB0<<0)  /* Timer/Counter D0 event 0 */
#define EVSYS_CHANNEL5_TCD0_CMPASET_GC (0xB1<<0)  /* Timer/Counter D0 event 1 */
#define EVSYS_CHANNEL5_TCD0_CMPBSET_GC (0xB2<<0)  /* Timer/Counter D0 event 2 */
#define EVSYS_CHANNEL5_TCD0_PROGEV_GC (0xB3<<0)  /* Timer/Counter D0 event 3 */
/* Channel 6 generator select select */
#define EVSYS_CHANNEL6_OFF_GC (0x00<<0)  /* Off */
#define EVSYS_CHANNEL6_UPDI_SYNCH_GC (0x01<<0)  /* UPDI SYNCH character */
#define EVSYS_CHANNEL6_RTC_OVF_GC (0x06<<0)  /* Real Time Counter overflow */
#define EVSYS_CHANNEL6_RTC_CMP_GC (0x07<<0)  /* Real Time Counter compare */
#define EVSYS_CHANNEL6_RTC_PIT_DIV8192_GC (0x08<<0)  /* Periodic Interrupt Timer output 0 */
#define EVSYS_CHANNEL6_RTC_PIT_DIV4096_GC (0x09<<0)  /* Periodic Interrupt Timer output 1 */
#define EVSYS_CHANNEL6_RTC_PIT_DIV2048_GC (0x0A<<0)  /* Periodic Interrupt Timer output 2 */
#define EVSYS_CHANNEL6_RTC_PIT_DIV1024_GC (0x0B<<0)  /* Periodic Interrupt Timer output 3 */
#define EVSYS_CHANNEL6_CCL_LUT0_GC (0x10<<0)  /* Configurable Custom Logic LUT0 */
#define EVSYS_CHANNEL6_CCL_LUT1_GC (0x11<<0)  /* Configurable Custom Logic LUT1 */
#define EVSYS_CHANNEL6_CCL_LUT2_GC (0x12<<0)  /* Configurable Custom Logic LUT2 */
#define EVSYS_CHANNEL6_CCL_LUT3_GC (0x13<<0)  /* Configurable Custom Logic LUT3 */
#define EVSYS_CHANNEL6_CCL_LUT4_GC (0x14<<0)  /* Configurable Custom Logic LUT4 */
#define EVSYS_CHANNEL6_CCL_LUT5_GC (0x15<<0)  /* Configurable Custom Logic LUT5 */
#define EVSYS_CHANNEL6_AC0_OUT_GC (0x20<<0)  /* Analog Comparator 0 out */
#define EVSYS_CHANNEL6_AC1_OUT_GC (0x21<<0)  /* Analog Comparator 1 out */
#define EVSYS_CHANNEL6_AC2_OUT_GC (0x22<<0)  /* Analog Comparator 2 out */
#define EVSYS_CHANNEL6_ADC0_RESRDY_GC (0x24<<0)  /* ADC 0 Result Ready */
#define EVSYS_CHANNEL6_PTC_RESRDY_GC (0x28<<0)  /* PTC Result Ready */
#define EVSYS_CHANNEL6_ZCD0_GC (0x30<<0)  /* Zero Cross Detect 0 out */
#define EVSYS_CHANNEL6_ZCD1_GC (0x31<<0)  /* Zero Cross Detect 1 out */
#define EVSYS_CHANNEL6_ZCD2_GC (0x32<<0)  /* Zero Cross Detect 2 out */
#define EVSYS_CHANNEL6_PORTG_PIN0_GC (0x40<<0)  /* Port G Pin 0 */
#define EVSYS_CHANNEL6_PORTG_PIN1_GC (0x41<<0)  /* Port G Pin 1 */
#define EVSYS_CHANNEL6_PORTG_PIN2_GC (0x42<<0)  /* Port G Pin 2 */
#define EVSYS_CHANNEL6_PORTG_PIN3_GC (0x43<<0)  /* Port G Pin 3 */
#define EVSYS_CHANNEL6_PORTG_PIN4_GC (0x44<<0)  /* Port G Pin 4 */
#define EVSYS_CHANNEL6_PORTG_PIN5_GC (0x45<<0)  /* Port G Pin 5 */
#define EVSYS_CHANNEL6_PORTG_PIN6_GC (0x46<<0)  /* Port G Pin 6 */
#define EVSYS_CHANNEL6_PORTG_PIN7_GC (0x47<<0)  /* Port G Pin 7 */
#define EVSYS_CHANNEL6_USART0_XCK_GC (0x60<<0)  /* USART 0 XCK */
#define EVSYS_CHANNEL6_USART1_XCK_GC (0x61<<0)  /* USART 1 XCK */
#define EVSYS_CHANNEL6_USART2_XCK_GC (0x62<<0)  /* USART 2 XCK */
#define EVSYS_CHANNEL6_USART3_XCK_GC (0x63<<0)  /* USART 3 XCK */
#define EVSYS_CHANNEL6_USART4_XCK_GC (0x64<<0)  /* USART 4 XCK */
#define EVSYS_CHANNEL6_USART5_XCK_GC (0x65<<0)  /* USART 5 XCK */
#define EVSYS_CHANNEL6_SPI0_SCK_GC (0x68<<0)  /* SPI 0 SCK */
#define EVSYS_CHANNEL6_SPI1_SCK_GC (0x69<<0)  /* SPI 1 SCK */
#define EVSYS_CHANNEL6_TCA0_OVF_LUNF_GC (0x80<<0)  /* Timer/Counter A0 overflow / low byte timer underflow */
#define EVSYS_CHANNEL6_TCA0_HUNF_GC (0x81<<0)  /* Timer/Counter A0 high byte timer underflow */
#define EVSYS_CHANNEL6_TCA0_CMP0_LCMP0_GC (0x84<<0)  /* Timer/Counter A0 compare 0 / low byte timer compare 0 */
#define EVSYS_CHANNEL6_TCA0_CMP1_LCMP1_GC (0x85<<0)  /* Timer/Counter A0 compare 1 / low byte timer compare 1 */
#define EVSYS_CHANNEL6_TCA0_CMP2_LCMP2_GC (0x86<<0)  /* Timer/Counter A0 compare 2 / low byte timer compare 2 */
#define EVSYS_CHANNEL6_TCA1_OVF_LUNF_GC (0x88<<0)  /* Timer/Counter A1 overflow / low byte timer underflow */
#define EVSYS_CHANNEL6_TCA1_HUNF_GC (0x89<<0)  /* Timer/Counter A1 high byte timer underflow */
#define EVSYS_CHANNEL6_TCA1_CMP0_LCMP0_GC (0x8C<<0)  /* Timer/Counter A1 compare 0 / low byte timer compare 0 */
#define EVSYS_CHANNEL6_TCA1_CMP1_LCMP1_GC (0x8D<<0)  /* Timer/Counter A1 compare 1 / low byte timer compare 1 */
#define EVSYS_CHANNEL6_TCA1_CMP2_LCMP2_GC (0x8E<<0)  /* Timer/Counter A1 compare 2 / low byte timer compare 2 */
#define EVSYS_CHANNEL6_TCB0_CAPT_GC (0xA0<<0)  /* Timer/Counter B0 capture */
#define EVSYS_CHANNEL6_TCB0_OVF_GC (0xA1<<0)  /* Timer/Counter B0 overflow */
#define EVSYS_CHANNEL6_TCB1_CAPT_GC (0xA2<<0)  /* Timer/Counter B1 capture */
#define EVSYS_CHANNEL6_TCB1_OVF_GC (0xA3<<0)  /* Timer/Counter B1 overflow */
#define EVSYS_CHANNEL6_TCB2_CAPT_GC (0xA4<<0)  /* Timer/Counter B2 capture */
#define EVSYS_CHANNEL6_TCB2_OVF_GC (0xA5<<0)  /* Timer/Counter B2 overflow */
#define EVSYS_CHANNEL6_TCB3_CAPT_GC (0xA6<<0)  /* Timer/Counter B3 capture */
#define EVSYS_CHANNEL6_TCB3_OVF_GC (0xA7<<0)  /* Timer/Counter B3 overflow */
#define EVSYS_CHANNEL6_TCB4_CAPT_GC (0xA8<<0)  /* Timer/Counter B4 capture */
#define EVSYS_CHANNEL6_TCB4_OVF_GC (0xA9<<0)  /* Timer/Counter B4 overflow */
#define EVSYS_CHANNEL6_TCD0_CMPBCLR_GC (0xB0<<0)  /* Timer/Counter D0 event 0 */
#define EVSYS_CHANNEL6_TCD0_CMPASET_GC (0xB1<<0)  /* Timer/Counter D0 event 1 */
#define EVSYS_CHANNEL6_TCD0_CMPBSET_GC (0xB2<<0)  /* Timer/Counter D0 event 2 */
#define EVSYS_CHANNEL6_TCD0_PROGEV_GC (0xB3<<0)  /* Timer/Counter D0 event 3 */
/* Channel 7 generator select select */
#define EVSYS_CHANNEL7_OFF_GC (0x00<<0)  /* Off */
#define EVSYS_CHANNEL7_UPDI_SYNCH_GC (0x01<<0)  /* UPDI SYNCH character */
#define EVSYS_CHANNEL7_RTC_OVF_GC (0x06<<0)  /* Real Time Counter overflow */
#define EVSYS_CHANNEL7_RTC_CMP_GC (0x07<<0)  /* Real Time Counter compare */
#define EVSYS_CHANNEL7_RTC_PIT_DIV512_GC (0x08<<0)  /* Periodic Interrupt Timer output 0 */
#define EVSYS_CHANNEL7_RTC_PIT_DIV256_GC (0x09<<0)  /* Periodic Interrupt Timer output 1 */
#define EVSYS_CHANNEL7_RTC_PIT_DIV128_GC (0x0A<<0)  /* Periodic Interrupt Timer output 2 */
#define EVSYS_CHANNEL7_RTC_PIT_DIV64_GC (0x0B<<0)  /* Periodic Interrupt Timer output 3 */
#define EVSYS_CHANNEL7_CCL_LUT0_GC (0x10<<0)  /* Configurable Custom Logic LUT0 */
#define EVSYS_CHANNEL7_CCL_LUT1_GC (0x11<<0)  /* Configurable Custom Logic LUT1 */
#define EVSYS_CHANNEL7_CCL_LUT2_GC (0x12<<0)  /* Configurable Custom Logic LUT2 */
#define EVSYS_CHANNEL7_CCL_LUT3_GC (0x13<<0)  /* Configurable Custom Logic LUT3 */
#define EVSYS_CHANNEL7_CCL_LUT4_GC (0x14<<0)  /* Configurable Custom Logic LUT4 */
#define EVSYS_CHANNEL7_CCL_LUT5_GC (0x15<<0)  /* Configurable Custom Logic LUT5 */
#define EVSYS_CHANNEL7_AC0_OUT_GC (0x20<<0)  /* Analog Comparator 0 out */
#define EVSYS_CHANNEL7_AC1_OUT_GC (0x21<<0)  /* Analog Comparator 1 out */
#define EVSYS_CHANNEL7_AC2_OUT_GC (0x22<<0)  /* Analog Comparator 2 out */
#define EVSYS_CHANNEL7_ADC0_RESRDY_GC (0x24<<0)  /* ADC 0 Result Ready */
#define EVSYS_CHANNEL7_PTC_RESRDY_GC (0x28<<0)  /* PTC Result Ready */
#define EVSYS_CHANNEL7_ZCD0_GC (0x30<<0)  /* Zero Cross Detect 0 out */
#define EVSYS_CHANNEL7_ZCD1_GC (0x31<<0)  /* Zero Cross Detect 1 out */
#define EVSYS_CHANNEL7_ZCD2_GC (0x32<<0)  /* Zero Cross Detect 2 out */
#define EVSYS_CHANNEL7_PORTG_PIN0_GC (0x40<<0)  /* Port G Pin 0 */
#define EVSYS_CHANNEL7_PORTG_PIN1_GC (0x41<<0)  /* Port G Pin 1 */
#define EVSYS_CHANNEL7_PORTG_PIN2_GC (0x42<<0)  /* Port G Pin 2 */
#define EVSYS_CHANNEL7_PORTG_PIN3_GC (0x43<<0)  /* Port G Pin 3 */
#define EVSYS_CHANNEL7_PORTG_PIN4_GC (0x44<<0)  /* Port G Pin 4 */
#define EVSYS_CHANNEL7_PORTG_PIN5_GC (0x45<<0)  /* Port G Pin 5 */
#define EVSYS_CHANNEL7_PORTG_PIN6_GC (0x46<<0)  /* Port G Pin 6 */
#define EVSYS_CHANNEL7_PORTG_PIN7_GC (0x47<<0)  /* Port G Pin 7 */
#define EVSYS_CHANNEL7_USART0_XCK_GC (0x60<<0)  /* USART 0 XCK */
#define EVSYS_CHANNEL7_USART1_XCK_GC (0x61<<0)  /* USART 1 XCK */
#define EVSYS_CHANNEL7_USART2_XCK_GC (0x62<<0)  /* USART 2 XCK */
#define EVSYS_CHANNEL7_USART3_XCK_GC (0x63<<0)  /* USART 3 XCK */
#define EVSYS_CHANNEL7_USART4_XCK_GC (0x64<<0)  /* USART 4 XCK */
#define EVSYS_CHANNEL7_USART5_XCK_GC (0x65<<0)  /* USART 5 XCK */
#define EVSYS_CHANNEL7_SPI0_SCK_GC (0x68<<0)  /* SPI 0 SCK */
#define EVSYS_CHANNEL7_SPI1_SCK_GC (0x69<<0)  /* SPI 1 SCK */
#define EVSYS_CHANNEL7_TCA0_OVF_LUNF_GC (0x80<<0)  /* Timer/Counter A0 overflow / low byte timer underflow */
#define EVSYS_CHANNEL7_TCA0_HUNF_GC (0x81<<0)  /* Timer/Counter A0 high byte timer underflow */
#define EVSYS_CHANNEL7_TCA0_CMP0_LCMP0_GC (0x84<<0)  /* Timer/Counter A0 compare 0 / low byte timer compare 0 */
#define EVSYS_CHANNEL7_TCA0_CMP1_LCMP1_GC (0x85<<0)  /* Timer/Counter A0 compare 1 / low byte timer compare 1 */
#define EVSYS_CHANNEL7_TCA0_CMP2_LCMP2_GC (0x86<<0)  /* Timer/Counter A0 compare 2 / low byte timer compare 2 */
#define EVSYS_CHANNEL7_TCA1_OVF_LUNF_GC (0x88<<0)  /* Timer/Counter A1 overflow / low byte timer underflow */
#define EVSYS_CHANNEL7_TCA1_HUNF_GC (0x89<<0)  /* Timer/Counter A1 high byte timer underflow */
#define EVSYS_CHANNEL7_TCA1_CMP0_LCMP0_GC (0x8C<<0)  /* Timer/Counter A1 compare 0 / low byte timer compare 0 */
#define EVSYS_CHANNEL7_TCA1_CMP1_LCMP1_GC (0x8D<<0)  /* Timer/Counter A1 compare 1 / low byte timer compare 1 */
#define EVSYS_CHANNEL7_TCA1_CMP2_LCMP2_GC (0x8E<<0)  /* Timer/Counter A1 compare 2 / low byte timer compare 2 */
#define EVSYS_CHANNEL7_TCB0_CAPT_GC (0xA0<<0)  /* Timer/Counter B0 capture */
#define EVSYS_CHANNEL7_TCB0_OVF_GC (0xA1<<0)  /* Timer/Counter B0 overflow */
#define EVSYS_CHANNEL7_TCB1_CAPT_GC (0xA2<<0)  /* Timer/Counter B1 capture */
#define EVSYS_CHANNEL7_TCB1_OVF_GC (0xA3<<0)  /* Timer/Counter B1 overflow */
#define EVSYS_CHANNEL7_TCB2_CAPT_GC (0xA4<<0)  /* Timer/Counter B2 capture */
#define EVSYS_CHANNEL7_TCB2_OVF_GC (0xA5<<0)  /* Timer/Counter B2 overflow */
#define EVSYS_CHANNEL7_TCB3_CAPT_GC (0xA6<<0)  /* Timer/Counter B3 capture */
#define EVSYS_CHANNEL7_TCB3_OVF_GC (0xA7<<0)  /* Timer/Counter B3 overflow */
#define EVSYS_CHANNEL7_TCB4_CAPT_GC (0xA8<<0)  /* Timer/Counter B4 capture */
#define EVSYS_CHANNEL7_TCB4_OVF_GC (0xA9<<0)  /* Timer/Counter B4 overflow */
#define EVSYS_CHANNEL7_TCD0_CMPBCLR_GC (0xB0<<0)  /* Timer/Counter D0 event 0 */
#define EVSYS_CHANNEL7_TCD0_CMPASET_GC (0xB1<<0)  /* Timer/Counter D0 event 1 */
#define EVSYS_CHANNEL7_TCD0_CMPBSET_GC (0xB2<<0)  /* Timer/Counter D0 event 2 */
#define EVSYS_CHANNEL7_TCD0_PROGEV_GC (0xB3<<0)  /* Timer/Counter D0 event 3 */
/* Channel 8 generator select select */
#define EVSYS_CHANNEL8_OFF_GC (0x00<<0)  /* Off */
#define EVSYS_CHANNEL8_UPDI_SYNCH_GC (0x01<<0)  /* UPDI SYNCH character */
#define EVSYS_CHANNEL8_RTC_OVF_GC (0x06<<0)  /* Real Time Counter overflow */
#define EVSYS_CHANNEL8_RTC_CMP_GC (0x07<<0)  /* Real Time Counter compare */
#define EVSYS_CHANNEL8_RTC_PIT_DIV8192_GC (0x08<<0)  /* Periodic Interrupt Timer output 0 */
#define EVSYS_CHANNEL8_RTC_PIT_DIV4096_GC (0x09<<0)  /* Periodic Interrupt Timer output 1 */
#define EVSYS_CHANNEL8_RTC_PIT_DIV2048_GC (0x0A<<0)  /* Periodic Interrupt Timer output 2 */
#define EVSYS_CHANNEL8_RTC_PIT_DIV1024_GC (0x0B<<0)  /* Periodic Interrupt Timer output 3 */
#define EVSYS_CHANNEL8_CCL_LUT0_GC (0x10<<0)  /* Configurable Custom Logic LUT0 */
#define EVSYS_CHANNEL8_CCL_LUT1_GC (0x11<<0)  /* Configurable Custom Logic LUT1 */
#define EVSYS_CHANNEL8_CCL_LUT2_GC (0x12<<0)  /* Configurable Custom Logic LUT2 */
#define EVSYS_CHANNEL8_CCL_LUT3_GC (0x13<<0)  /* Configurable Custom Logic LUT3 */
#define EVSYS_CHANNEL8_CCL_LUT4_GC (0x14<<0)  /* Configurable Custom Logic LUT4 */
#define EVSYS_CHANNEL8_CCL_LUT5_GC (0x15<<0)  /* Configurable Custom Logic LUT5 */
#define EVSYS_CHANNEL8_AC0_OUT_GC (0x20<<0)  /* Analog Comparator 0 out */
#define EVSYS_CHANNEL8_AC1_OUT_GC (0x21<<0)  /* Analog Comparator 1 out */
#define EVSYS_CHANNEL8_AC2_OUT_GC (0x22<<0)  /* Analog Comparator 2 out */
#define EVSYS_CHANNEL8_ADC0_RESRDY_GC (0x24<<0)  /* ADC 0 Result Ready */
#define EVSYS_CHANNEL8_PTC_RESRDY_GC (0x28<<0)  /* PTC Result Ready */
#define EVSYS_CHANNEL8_ZCD0_GC (0x30<<0)  /* Zero Cross Detect 0 out */
#define EVSYS_CHANNEL8_ZCD1_GC (0x31<<0)  /* Zero Cross Detect 1 out */
#define EVSYS_CHANNEL8_ZCD2_GC (0x32<<0)  /* Zero Cross Detect 2 out */
#define EVSYS_CHANNEL8_USART0_XCK_GC (0x60<<0)  /* USART 0 XCK */
#define EVSYS_CHANNEL8_USART1_XCK_GC (0x61<<0)  /* USART 1 XCK */
#define EVSYS_CHANNEL8_USART2_XCK_GC (0x62<<0)  /* USART 2 XCK */
#define EVSYS_CHANNEL8_USART3_XCK_GC (0x63<<0)  /* USART 3 XCK */
#define EVSYS_CHANNEL8_USART4_XCK_GC (0x64<<0)  /* USART 4 XCK */
#define EVSYS_CHANNEL8_USART5_XCK_GC (0x65<<0)  /* USART 5 XCK */
#define EVSYS_CHANNEL8_SPI0_SCK_GC (0x68<<0)  /* SPI 0 SCK */
#define EVSYS_CHANNEL8_SPI1_SCK_GC (0x69<<0)  /* SPI 1 SCK */
#define EVSYS_CHANNEL8_TCA0_OVF_LUNF_GC (0x80<<0)  /* Timer/Counter A0 overflow / low byte timer underflow */
#define EVSYS_CHANNEL8_TCA0_HUNF_GC (0x81<<0)  /* Timer/Counter A0 high byte timer underflow */
#define EVSYS_CHANNEL8_TCA0_CMP0_LCMP0_GC (0x84<<0)  /* Timer/Counter A0 compare 0 / low byte timer compare 0 */
#define EVSYS_CHANNEL8_TCA0_CMP1_LCMP1_GC (0x85<<0)  /* Timer/Counter A0 compare 1 / low byte timer compare 1 */
#define EVSYS_CHANNEL8_TCA0_CMP2_LCMP2_GC (0x86<<0)  /* Timer/Counter A0 compare 2 / low byte timer compare 2 */
#define EVSYS_CHANNEL8_TCA1_OVF_LUNF_GC (0x88<<0)  /* Timer/Counter A1 overflow / low byte timer underflow */
#define EVSYS_CHANNEL8_TCA1_HUNF_GC (0x89<<0)  /* Timer/Counter A1 high byte timer underflow */
#define EVSYS_CHANNEL8_TCA1_CMP0_LCMP0_GC (0x8C<<0)  /* Timer/Counter A1 compare 0 / low byte timer compare 0 */
#define EVSYS_CHANNEL8_TCA1_CMP1_LCMP1_GC (0x8D<<0)  /* Timer/Counter A1 compare 1 / low byte timer compare 1 */
#define EVSYS_CHANNEL8_TCA1_CMP2_LCMP2_GC (0x8E<<0)  /* Timer/Counter A1 compare 2 / low byte timer compare 2 */
#define EVSYS_CHANNEL8_TCB0_CAPT_GC (0xA0<<0)  /* Timer/Counter B0 capture */
#define EVSYS_CHANNEL8_TCB0_OVF_GC (0xA1<<0)  /* Timer/Counter B0 overflow */
#define EVSYS_CHANNEL8_TCB1_CAPT_GC (0xA2<<0)  /* Timer/Counter B1 capture */
#define EVSYS_CHANNEL8_TCB1_OVF_GC (0xA3<<0)  /* Timer/Counter B1 overflow */
#define EVSYS_CHANNEL8_TCB2_CAPT_GC (0xA4<<0)  /* Timer/Counter B2 capture */
#define EVSYS_CHANNEL8_TCB2_OVF_GC (0xA5<<0)  /* Timer/Counter B2 overflow */
#define EVSYS_CHANNEL8_TCB3_CAPT_GC (0xA6<<0)  /* Timer/Counter B3 capture */
#define EVSYS_CHANNEL8_TCB3_OVF_GC (0xA7<<0)  /* Timer/Counter B3 overflow */
#define EVSYS_CHANNEL8_TCB4_CAPT_GC (0xA8<<0)  /* Timer/Counter B4 capture */
#define EVSYS_CHANNEL8_TCB4_OVF_GC (0xA9<<0)  /* Timer/Counter B4 overflow */
#define EVSYS_CHANNEL8_TCD0_CMPBCLR_GC (0xB0<<0)  /* Timer/Counter D0 event 0 */
#define EVSYS_CHANNEL8_TCD0_CMPASET_GC (0xB1<<0)  /* Timer/Counter D0 event 1 */
#define EVSYS_CHANNEL8_TCD0_CMPBSET_GC (0xB2<<0)  /* Timer/Counter D0 event 2 */
#define EVSYS_CHANNEL8_TCD0_PROGEV_GC (0xB3<<0)  /* Timer/Counter D0 event 3 */
/* Channel 9 generator select select */
#define EVSYS_CHANNEL9_OFF_GC (0x00<<0)  /* Off */
#define EVSYS_CHANNEL9_UPDI_SYNCH_GC (0x01<<0)  /* UPDI SYNCH character */
#define EVSYS_CHANNEL9_RTC_OVF_GC (0x06<<0)  /* Real Time Counter overflow */
#define EVSYS_CHANNEL9_RTC_CMP_GC (0x07<<0)  /* Real Time Counter compare */
#define EVSYS_CHANNEL9_RTC_PIT_DIV8192_GC (0x08<<0)  /* Periodic Interrupt Timer output 0 */
#define EVSYS_CHANNEL9_RTC_PIT_DIV4096_GC (0x09<<0)  /* Periodic Interrupt Timer output 1 */
#define EVSYS_CHANNEL9_RTC_PIT_DIV2048_GC (0x0A<<0)  /* Periodic Interrupt Timer output 2 */
#define EVSYS_CHANNEL9_RTC_PIT_DIV1024_GC (0x0B<<0)  /* Periodic Interrupt Timer output 3 */
#define EVSYS_CHANNEL9_CCL_LUT0_GC (0x10<<0)  /* Configurable Custom Logic LUT0 */
#define EVSYS_CHANNEL9_CCL_LUT1_GC (0x11<<0)  /* Configurable Custom Logic LUT1 */
#define EVSYS_CHANNEL9_CCL_LUT2_GC (0x12<<0)  /* Configurable Custom Logic LUT2 */
#define EVSYS_CHANNEL9_CCL_LUT3_GC (0x13<<0)  /* Configurable Custom Logic LUT3 */
#define EVSYS_CHANNEL9_CCL_LUT4_GC (0x14<<0)  /* Configurable Custom Logic LUT4 */
#define EVSYS_CHANNEL9_CCL_LUT5_GC (0x15<<0)  /* Configurable Custom Logic LUT5 */
#define EVSYS_CHANNEL9_AC0_OUT_GC (0x20<<0)  /* Analog Comparator 0 out */
#define EVSYS_CHANNEL9_AC1_OUT_GC (0x21<<0)  /* Analog Comparator 1 out */
#define EVSYS_CHANNEL9_AC2_OUT_GC (0x22<<0)  /* Analog Comparator 2 out */
#define EVSYS_CHANNEL9_ADC0_RESRDY_GC (0x24<<0)  /* ADC 0 Result Ready */
#define EVSYS_CHANNEL9_PTC_RESRDY_GC (0x28<<0)  /* PTC Result Ready */
#define EVSYS_CHANNEL9_ZCD0_GC (0x30<<0)  /* Zero Cross Detect 0 out */
#define EVSYS_CHANNEL9_ZCD1_GC (0x31<<0)  /* Zero Cross Detect 1 out */
#define EVSYS_CHANNEL9_ZCD2_GC (0x32<<0)  /* Zero Cross Detect 2 out */
#define EVSYS_CHANNEL9_USART0_XCK_GC (0x60<<0)  /* USART 0 XCK */
#define EVSYS_CHANNEL9_USART1_XCK_GC (0x61<<0)  /* USART 1 XCK */
#define EVSYS_CHANNEL9_USART2_XCK_GC (0x62<<0)  /* USART 2 XCK */
#define EVSYS_CHANNEL9_USART3_XCK_GC (0x63<<0)  /* USART 3 XCK */
#define EVSYS_CHANNEL9_USART4_XCK_GC (0x64<<0)  /* USART 4 XCK */
#define EVSYS_CHANNEL9_USART5_XCK_GC (0x65<<0)  /* USART 5 XCK */
#define EVSYS_CHANNEL9_SPI0_SCK_GC (0x68<<0)  /* SPI 0 SCK */
#define EVSYS_CHANNEL9_SPI1_SCK_GC (0x69<<0)  /* SPI 1 SCK */
#define EVSYS_CHANNEL9_TCA0_OVF_LUNF_GC (0x80<<0)  /* Timer/Counter A0 overflow / low byte timer underflow */
#define EVSYS_CHANNEL9_TCA0_HUNF_GC (0x81<<0)  /* Timer/Counter A0 high byte timer underflow */
#define EVSYS_CHANNEL9_TCA0_CMP0_LCMP0_GC (0x84<<0)  /* Timer/Counter A0 compare 0 / low byte timer compare 0 */
#define EVSYS_CHANNEL9_TCA0_CMP1_LCMP1_GC (0x85<<0)  /* Timer/Counter A0 compare 1 / low byte timer compare 1 */
#define EVSYS_CHANNEL9_TCA0_CMP2_LCMP2_GC (0x86<<0)  /* Timer/Counter A0 compare 2 / low byte timer compare 2 */
#define EVSYS_CHANNEL9_TCA1_OVF_LUNF_GC (0x88<<0)  /* Timer/Counter A1 overflow / low byte timer underflow */
#define EVSYS_CHANNEL9_TCA1_HUNF_GC (0x89<<0)  /* Timer/Counter A1 high byte timer underflow */
#define EVSYS_CHANNEL9_TCA1_CMP0_LCMP0_GC (0x8C<<0)  /* Timer/Counter A1 compare 0 / low byte timer compare 0 */
#define EVSYS_CHANNEL9_TCA1_CMP1_LCMP1_GC (0x8D<<0)  /* Timer/Counter A1 compare 1 / low byte timer compare 1 */
#define EVSYS_CHANNEL9_TCA1_CMP2_LCMP2_GC (0x8E<<0)  /* Timer/Counter A1 compare 2 / low byte timer compare 2 */
#define EVSYS_CHANNEL9_TCB0_CAPT_GC (0xA0<<0)  /* Timer/Counter B0 capture */
#define EVSYS_CHANNEL9_TCB0_OVF_GC (0xA1<<0)  /* Timer/Counter B0 overflow */
#define EVSYS_CHANNEL9_TCB1_CAPT_GC (0xA2<<0)  /* Timer/Counter B1 capture */
#define EVSYS_CHANNEL9_TCB1_OVF_GC (0xA3<<0)  /* Timer/Counter B1 overflow */
#define EVSYS_CHANNEL9_TCB2_CAPT_GC (0xA4<<0)  /* Timer/Counter B2 capture */
#define EVSYS_CHANNEL9_TCB2_OVF_GC (0xA5<<0)  /* Timer/Counter B2 overflow */
#define EVSYS_CHANNEL9_TCB3_CAPT_GC (0xA6<<0)  /* Timer/Counter B3 capture */
#define EVSYS_CHANNEL9_TCB3_OVF_GC (0xA7<<0)  /* Timer/Counter B3 overflow */
#define EVSYS_CHANNEL9_TCB4_CAPT_GC (0xA8<<0)  /* Timer/Counter B4 capture */
#define EVSYS_CHANNEL9_TCB4_OVF_GC (0xA9<<0)  /* Timer/Counter B4 overflow */
#define EVSYS_CHANNEL9_TCD0_CMPBCLR_GC (0xB0<<0)  /* Timer/Counter D0 event 0 */
#define EVSYS_CHANNEL9_TCD0_CMPASET_GC (0xB1<<0)  /* Timer/Counter D0 event 1 */
#define EVSYS_CHANNEL9_TCD0_CMPBSET_GC (0xB2<<0)  /* Timer/Counter D0 event 2 */
#define EVSYS_CHANNEL9_TCD0_PROGEV_GC (0xB3<<0)  /* Timer/Counter D0 event 3 */
/* Software event on channel select select */
#define EVSYS_SWEVENTA_CH0_GC (0x01<<0)  /* Software event on channel 0 */
#define EVSYS_SWEVENTA_CH1_GC (0x02<<0)  /* Software event on channel 1 */
#define EVSYS_SWEVENTA_CH2_GC (0x04<<0)  /* Software event on channel 2 */
#define EVSYS_SWEVENTA_CH3_GC (0x08<<0)  /* Software event on channel 3 */
#define EVSYS_SWEVENTA_CH4_GC (0x10<<0)  /* Software event on channel 4 */
#define EVSYS_SWEVENTA_CH5_GC (0x20<<0)  /* Software event on channel 5 */
#define EVSYS_SWEVENTA_CH6_GC (0x40<<0)  /* Software event on channel 6 */
#define EVSYS_SWEVENTA_CH7_GC (0x80<<0)  /* Software event on channel 7 */
/* Software event on channel select select */
#define EVSYS_SWEVENTB_CH8_GC (0x00<<0)  /* Software event on channel 8 */
#define EVSYS_SWEVENTB_CH9_GC (0x01<<0)  /* Software event on channel 9 */
/* User channel select select */
#define EVSYS_USER_OFF_GC (0x00<<0)  /* Off, No Eventsys Channel connected */
#define EVSYS_USER_CHANNEL0_GC (0x01<<0)  /* Connect user to event channel 0 */
#define EVSYS_USER_CHANNEL1_GC (0x02<<0)  /* Connect user to event channel 1 */
#define EVSYS_USER_CHANNEL2_GC (0x03<<0)  /* Connect user to event channel 2 */
#define EVSYS_USER_CHANNEL3_GC (0x04<<0)  /* Connect user to event channel 3 */
#define EVSYS_USER_CHANNEL4_GC (0x05<<0)  /* Connect user to event channel 4 */
#define EVSYS_USER_CHANNEL5_GC (0x06<<0)  /* Connect user to event channel 5 */
#define EVSYS_USER_CHANNEL6_GC (0x07<<0)  /* Connect user to event channel 6 */
#define EVSYS_USER_CHANNEL7_GC (0x08<<0)  /* Connect user to event channel 7 */
#define EVSYS_USER_CHANNEL8_GC (0x09<<0)  /* Connect user to event channel 8 */
#define EVSYS_USER_CHANNEL9_GC (0x0A<<0)  /* Connect user to event channel 9 */

/*------------------------------------------------------------------------
FUSE - Fuses
------------------------------------------------------------------------*/

/* BOD Operation in Active Mode select */
#define ACTIVE_DISABLE_GC (0x00<<2)  /* BOD disabled */
#define ACTIVE_ENABLE_GC (0x01<<2)  /* BOD enabled in continuous mode */
#define ACTIVE_SAMPLE_GC (0x02<<2)  /* BOD enabled in sampled mode */
#define ACTIVE_ENABLEWAIT_GC (0x03<<2)  /* BOD enabled in continuous mode. Execution is halted at wake-up until BOD is running. */
/* Frequency Select select */
#define CLKSEL_OSCHF_GC (0x00<<0)  /* 1-32MHz internal oscillator */
#define CLKSEL_OSC32K_GC (0x01<<0)  /* 32.768kHz internal oscillator */
/* CRC Select select */
#define CRCSEL_CRC16_GC (0x00<<5)  /* Enable CRC16 */
#define CRCSEL_CRC32_GC (0x01<<5)  /* Enable CRC32 */
/* CRC Source select */
#define CRCSRC_FLASH_GC (0x00<<6)  /* CRC of full Flash (boot, application code and application data) */
#define CRCSRC_BOOT_GC (0x01<<6)  /* CRC of boot section */
#define CRCSRC_BOOTAPP_GC (0x02<<6)  /* CRC of application code and boot sections */
#define CRCSRC_NOCRC_GC (0x03<<6)  /* No CRC */
/* BOD Level select */
#define LVL_BODLEVEL0_GC (0x00<<5)  /* 1.9V */
#define LVL_BODLEVEL1_GC (0x01<<5)  /* 2.45V */
#define LVL_BODLEVEL2_GC (0x02<<5)  /* 2.7V */
#define LVL_BODLEVEL3_GC (0x03<<5)  /* 2.85V */
/* Watchdog Timeout Period select */
#define PERIOD_OFF_GC (0x00<<0)  /* Watch-Dog timer Off */
#define PERIOD_8CLK_GC (0x01<<0)  /* 8 cycles (8ms) */
#define PERIOD_16CLK_GC (0x02<<0)  /* 16 cycles (16ms) */
#define PERIOD_32CLK_GC (0x03<<0)  /* 32 cycles (32ms) */
#define PERIOD_64CLK_GC (0x04<<0)  /* 64 cycles (64ms) */
#define PERIOD_128CLK_GC (0x05<<0)  /* 128 cycles (0.128s) */
#define PERIOD_256CLK_GC (0x06<<0)  /* 256 cycles (0.256s) */
#define PERIOD_512CLK_GC (0x07<<0)  /* 512 cycles (0.512s) */
#define PERIOD_1KCLK_GC (0x08<<0)  /* 1K cycles (1.0s) */
#define PERIOD_2KCLK_GC (0x09<<0)  /* 2K cycles (2.0s) */
#define PERIOD_4KCLK_GC (0x0A<<0)  /* 4K cycles (4.0s) */
#define PERIOD_8KCLK_GC (0x0B<<0)  /* 8K cycles (8.0s) */
/* Reset Pin Configuration select */
#define RSTPINCFG_GPIO_GC (0x00<<2)  /* GPIO mode */
#define RSTPINCFG_RST_GC (0x02<<2)  /* Reset mode */
/* BOD Sample Frequency select */
#define SAMPFREQ_128Hz_GC (0x00<<4)  /* Sample frequency is 128 Hz */
#define SAMPFREQ_32Hz_GC (0x01<<4)  /* Sample frequency is 32 Hz */
/* BOD Operation in Sleep Mode select */
#define SLEEP_DISABLE_GC (0x00<<0)  /* BOD disabled */
#define SLEEP_ENABLE_GC (0x01<<0)  /* BOD enabled in continuous mode */
#define SLEEP_SAMPLE_GC (0x02<<0)  /* BOD enabled in sampled mode */
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
#define WINDOW_OFF_GC (0x00<<4)  /* Window mode off */
#define WINDOW_8CLK_GC (0x01<<4)  /* 8 cycles (8ms) */
#define WINDOW_16CLK_GC (0x02<<4)  /* 16 cycles (16ms) */
#define WINDOW_32CLK_GC (0x03<<4)  /* 32 cycles (32ms) */
#define WINDOW_64CLK_GC (0x04<<4)  /* 64 cycles (64ms) */
#define WINDOW_128CLK_GC (0x05<<4)  /* 128 cycles (0.128s) */
#define WINDOW_256CLK_GC (0x06<<4)  /* 256 cycles (0.256s) */
#define WINDOW_512CLK_GC (0x07<<4)  /* 512 cycles (0.512s) */
#define WINDOW_1KCLK_GC (0x08<<4)  /* 1K cycles (1.0s) */
#define WINDOW_2KCLK_GC (0x09<<4)  /* 2K cycles (2.0s) */
#define WINDOW_4KCLK_GC (0x0A<<4)  /* 4K cycles (4.0s) */
#define WINDOW_8KCLK_GC (0x0B<<4)  /* 8K cycles (8.0s) */

/*------------------------------------------------------------------------
GPR - General Purpose Registers
------------------------------------------------------------------------*/


/*------------------------------------------------------------------------
LOCK - Lockbits
------------------------------------------------------------------------*/

/* Lock Key select */
#define LOCK_KEY_NOLOCK_GC (0x5CC5C55C<<0)  /* No locks */
#define LOCK_KEY_RWLOCK_GC (0xA33A3AA3<<0)  /* Read and write lock */

/*------------------------------------------------------------------------
NVMCTRL - Non-volatile Memory Controller
------------------------------------------------------------------------*/

/* Command select */
#define NVMCTRL_CMD_NONE_GC (0x00<<0)  /* No Command */
#define NVMCTRL_CMD_NOOP_GC (0x01<<0)  /* No Operation */
#define NVMCTRL_CMD_FLWR_GC (0x02<<0)  /* Flash Write */
#define NVMCTRL_CMD_FLPER_GC (0x08<<0)  /* Flash Page Erase */
#define NVMCTRL_CMD_FLMPER2_GC (0x09<<0)  /* Flash Multi-Page Erase 2 pages */
#define NVMCTRL_CMD_FLMPER4_GC (0x0A<<0)  /* Flash Multi-Page Erase 4 pages */
#define NVMCTRL_CMD_FLMPER8_GC (0x0B<<0)  /* Flash Multi-Page Erase 8 pages */
#define NVMCTRL_CMD_FLMPER16_GC (0x0C<<0)  /* Flash Multi-Page Erase 16 pages */
#define NVMCTRL_CMD_FLMPER32_GC (0x0D<<0)  /* Flash Multi-Page Erase 32 pages */
#define NVMCTRL_CMD_EEWR_GC (0x12<<0)  /* EEPROM Write */
#define NVMCTRL_CMD_EEERWR_GC (0x13<<0)  /* EEPROM Erase and Write */
#define NVMCTRL_CMD_EEBER_GC (0x18<<0)  /* EEPROM Byte Erase */
#define NVMCTRL_CMD_EEMBER2_GC (0x19<<0)  /* EEPROM Multi-Byte Erase 2 bytes */
#define NVMCTRL_CMD_EEMBER4_GC (0x1A<<0)  /* EEPROM Multi-Byte Erase 4 bytes */
#define NVMCTRL_CMD_EEMBER8_GC (0x1B<<0)  /* EEPROM Multi-Byte Erase 8 bytes */
#define NVMCTRL_CMD_EEMBER16_GC (0x1C<<0)  /* EEPROM Multi-Byte Erase 16 bytes */
#define NVMCTRL_CMD_EEMBER32_GC (0x1D<<0)  /* EEPROM Multi-Byte Erase 32 bytes */
#define NVMCTRL_CMD_CHER_GC (0x20<<0)  /* Chip Erase Command */
#define NVMCTRL_CMD_EECHER_GC (0x30<<0)  /* EEPROM Erase Command */
/* Write error select */
#define NVMCTRL_ERROR_NOERROR_GC (0x00<<4)  /* No Error */
#define NVMCTRL_ERROR_ILLEGALCMD_GC (0x01<<4)  /* Write command not selected */
#define NVMCTRL_ERROR_ILLEGALSADDR_GC (0x02<<4)  /* Write to section not allowed */
#define NVMCTRL_ERROR_DOUBLESELECT_GC (0x03<<4)  /* Selecting new write command while write command already seleted */
#define NVMCTRL_ERROR_ONGOINGPROG_GC (0x04<<4)  /* Starting a new programming operation before previous is completed */
/* Flash Mapping in Data space select */
#define NVMCTRL_FLMAP_SECTION0_GC (0x00<<4)  /* Flash section 0 */
#define NVMCTRL_FLMAP_SECTION1_GC (0x01<<4)  /* Flash section 1 */
#define NVMCTRL_FLMAP_SECTION2_GC (0x02<<4)  /* Flash section 2 */
#define NVMCTRL_FLMAP_SECTION3_GC (0x03<<4)  /* Flash section 3 */

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

/* Analog Comparator 0 Output select */
#define PORTMUX_AC0_DEFAULT_GC (0x00<<0)  /* OUT on PA7 */
#define PORTMUX_AC0_ALT1_GC (0x01<<0)  /* OUT on PC6 */
/* Analog Comparator 1 Output select */
#define PORTMUX_AC1_DEFAULT_GC (0x00<<1)  /* OUT on PA7 */
#define PORTMUX_AC1_ALT1_GC (0x01<<1)  /* OUT on PC6 */
/* Analog Comparator 2 Output select */
#define PORTMUX_AC2_DEFAULT_GC (0x00<<2)  /* OUT on PA7 */
#define PORTMUX_AC2_ALT1_GC (0x01<<2)  /* OUT on PC6 */
/* Event Output A select */
#define PORTMUX_EVOUTA_DEFAULT_GC (0x00<<0)  /* EVOUT on PA2 */
#define PORTMUX_EVOUTA_ALT1_GC (0x01<<0)  /* EVOUT on PA7 */
/* Event Output B select */
#define PORTMUX_EVOUTB_DEFAULT_GC (0x00<<1)  /* EVOUT on PB2 */
#define PORTMUX_EVOUTB_ALT1_GC (0x01<<1)  /* EVOUT on PB7 */
/* Event Output C select */
#define PORTMUX_EVOUTC_DEFAULT_GC (0x00<<2)  /* EVOUT on PC2 */
#define PORTMUX_EVOUTC_ALT1_GC (0x01<<2)  /* EVOUT on PC7 */
/* Event Output D select */
#define PORTMUX_EVOUTD_DEFAULT_GC (0x00<<3)  /* EVOUT on PD2 */
#define PORTMUX_EVOUTD_ALT1_GC (0x01<<3)  /* EVOUT on PD7 */
/* Event Output E select */
#define PORTMUX_EVOUTE_DEFAULT_GC (0x00<<4)  /* EVOUT on PE2 */
#define PORTMUX_EVOUTE_ALT1_GC (0x01<<4)  /* EVOUT on PE7 */
/* Event Output F select */
#define PORTMUX_EVOUTF_DEFAULT_GC (0x00<<5)  /* EVOUT on PF2 */
/* Event Output G select */
#define PORTMUX_EVOUTG_DEFAULT_GC (0x00<<6)  /* EVOUT on PG2 */
#define PORTMUX_EVOUTG_ALT1_GC (0x01<<6)  /* EVOUT on PG7 */
/* CCL Look-Up Table 0 Signals select */
#define PORTMUX_LUT0_DEFAULT_GC (0x00<<0)  /* Out: PA3 In: PA0, PA1, PA2 */
#define PORTMUX_LUT0_ALT1_GC (0x01<<0)  /* Out: PA6 In: PA0, PA1, PA2 */
/* CCL Look-Up Table 1 Signals select */
#define PORTMUX_LUT1_DEFAULT_GC (0x00<<1)  /* Out: PC3 In: PC0, PC1, PC2 */
#define PORTMUX_LUT1_ALT1_GC (0x01<<1)  /* Out: PC6 In: PC0, PC1, PC2 */
/* CCL Look-Up Table 2 Signals select */
#define PORTMUX_LUT2_DEFAULT_GC (0x00<<2)  /* Out: PD3 In: PD0, PD1, PD2 */
#define PORTMUX_LUT2_ALT1_GC (0x01<<2)  /* Out: PD6 In: PD0, PD1, PD2 */
/* CCL Look-Up Table 3 Signals select */
#define PORTMUX_LUT3_DEFAULT_GC (0x00<<3)  /* Out: PF3 In: PF0, PF1, PF2 */
/* CCL Look-Up Table 4 Signals select */
#define PORTMUX_LUT4_DEFAULT_GC (0x00<<4)  /* Out: PB3 In: PB0, PB1, PB2 */
#define PORTMUX_LUT4_ALT1_GC (0x01<<4)  /* Out: PB6 In: PB0, PB1, PB2 */
/* CCL Look-Up Table 5 Signals select */
#define PORTMUX_LUT5_DEFAULT_GC (0x00<<5)  /* Out: PG3 In: PG0, PG1, PG2 */
#define PORTMUX_LUT5_ALT1_GC (0x01<<5)  /* Out: PG6 In: PG0, PG1, PG2 */
/* SPI0 Signals select */
#define PORTMUX_SPI0_DEFAULT_GC (0x00<<0)  /* PA4, PA5, PA6, PA7 */
#define PORTMUX_SPI0_ALT1_GC (0x01<<0)  /* PE0, PE1, PE2, PE3 */
#define PORTMUX_SPI0_ALT2_GC (0x02<<0)  /* PG4, PG5, PG6, PG7 */
#define PORTMUX_SPI0_NONE_GC (0x03<<0)  /* Not connected to any pins */
/* SPI1 Signals select */
#define PORTMUX_SPI1_DEFAULT_GC (0x00<<2)  /* PC0, PC1, PC2, PC3 */
#define PORTMUX_SPI1_ALT1_GC (0x01<<2)  /* PC4, PC5, PC6, PC7 */
#define PORTMUX_SPI1_ALT2_GC (0x02<<2)  /* PB4, PB5, PB6, PB7 */
#define PORTMUX_SPI1_NONE_GC (0x03<<2)  /* Not connected to any pins */
/* TCA0 Signals select */
#define PORTMUX_TCA0_PORTA_GC (0x00<<0)  /* PA0, PA1, PA2, PA3, PA4, PA5 */
#define PORTMUX_TCA0_PORTB_GC (0x01<<0)  /* PB0, PB1, PB2, PB3, PB4, PB5 */
#define PORTMUX_TCA0_PORTC_GC (0x02<<0)  /* PC0, PC1, PC2, PC3, PC4, PC5 */
#define PORTMUX_TCA0_PORTD_GC (0x03<<0)  /* PD0, PD1, PD2, PD3, PD4, PD5 */
#define PORTMUX_TCA0_PORTE_GC (0x04<<0)  /* PE0, PE1, PE2, PE3, PE4, PE5 */
#define PORTMUX_TCA0_PORTF_GC (0x05<<0)  /* PF0, PF1, PF2, PF3, PF4, PF5 */
#define PORTMUX_TCA0_PORTG_GC (0x06<<0)  /* PG0, PG1, PG2, PG3, PG4, PG5 */
/* TCA1 Signals select */
#define PORTMUX_TCA1_PORTB_GC (0x00<<3)  /* PB0, PB1, PB2, PB3, PB4, PB5 */
#define PORTMUX_TCA1_PORTC_GC (0x01<<3)  /* PC4, PC5, PC6 */
#define PORTMUX_TCA1_PORTE_GC (0x02<<3)  /* PE4, PE5, PE6 */
#define PORTMUX_TCA1_PORTG_GC (0x03<<3)  /* PG0, PG1, PG2, PG3, PG4, PG5 */
/* TCB0 Output select */
#define PORTMUX_TCB0_DEFAULT_GC (0x00<<0)  /* WO on PA2 */
#define PORTMUX_TCB0_ALT1_GC (0x01<<0)  /* WO on PF4 */
/* TCB1 Output select */
#define PORTMUX_TCB1_DEFAULT_GC (0x00<<1)  /* WO on PA3 */
#define PORTMUX_TCB1_ALT1_GC (0x01<<1)  /* WO on PF5 */
/* TCB2 Output select */
#define PORTMUX_TCB2_DEFAULT_GC (0x00<<2)  /* WO on PC0 */
#define PORTMUX_TCB2_ALT1_GC (0x01<<2)  /* WO on PB4 */
/* TCB3 Output select */
#define PORTMUX_TCB3_DEFAULT_GC (0x00<<3)  /* WO on PB5 */
#define PORTMUX_TCB3_ALT1_GC (0x01<<3)  /* WO on PC1 */
/* TCB4 Output select */
#define PORTMUX_TCB4_DEFAULT_GC (0x00<<4)  /* WO on PG3 */
#define PORTMUX_TCB4_ALT1_GC (0x01<<4)  /* WO on PC6 */
/* TCD0 Signals select */
#define PORTMUX_TCD0_DEFAULT_GC (0x00<<0)  /* PA4, PA5, PA6, PA7 */
#define PORTMUX_TCD0_ALT1_GC (0x01<<0)  /* PB4, PB5, PB6, PB7 */
#define PORTMUX_TCD0_ALT2_GC (0x02<<0)  /* PF0, PF1, PF2, PF3 */
#define PORTMUX_TCD0_ALT3_GC (0x03<<0)  /* PG4, PG5, PG6, PG7 */
/* TWI0 Signals select */
#define PORTMUX_TWI0_DEFAULT_GC (0x00<<0)  /* PA2, PA3, PC2, PC3 */
#define PORTMUX_TWI0_ALT1_GC (0x01<<0)  /* PA2, PA3, PC6, PC7 */
#define PORTMUX_TWI0_ALT2_GC (0x02<<0)  /* PC2, PC3, PC6, PC7 */
/* TWI1 Signals select */
#define PORTMUX_TWI1_DEFAULT_GC (0x00<<2)  /* PF2, PF3, PB2, PB3 */
#define PORTMUX_TWI1_ALT1_GC (0x01<<2)  /* PF2, PF3, PB6, PB7 */
#define PORTMUX_TWI1_ALT2_GC (0x02<<2)  /* PB2, PB3, PB6, PB7 */
/* USART0 Signals select */
#define PORTMUX_USART0_DEFAULT_GC (0x00<<0)  /* PA0, PA1, PA2, PA3 */
#define PORTMUX_USART0_ALT1_GC (0x01<<0)  /* PA4, PA5, PA6, PA7 */
#define PORTMUX_USART0_NONE_GC (0x03<<0)  /* Not connected to any pins */
/* USART1 Signals select */
#define PORTMUX_USART1_DEFAULT_GC (0x00<<2)  /* PC0, PC1, PC2, PC3 */
#define PORTMUX_USART1_ALT1_GC (0x01<<2)  /* PC4, PC5, PC6, PC7 */
#define PORTMUX_USART1_NONE_GC (0x03<<2)  /* Not connected to any pins */
/* USART2 Signals select */
#define PORTMUX_USART2_DEFAULT_GC (0x00<<4)  /* PF0, PF1, PF2, PF3 */
#define PORTMUX_USART2_ALT1_GC (0x01<<4)  /* PF4, PF5 */
#define PORTMUX_USART2_NONE_GC (0x03<<4)  /* Not connected to any pins */
/* USART3  Signals select */
#define PORTMUX_USART3_DEFAULT_GC (0x00<<6)  /* PB0, PB1, PB2, PB3 */
#define PORTMUX_USART3_ALT1_GC (0x01<<6)  /* PB4, PB5, PB6, PB7 */
#define PORTMUX_USART3_NONE_GC (0x03<<6)  /* Not connected to any pins */
/* USART4 Signals select */
#define PORTMUX_USART4_DEFAULT_GC (0x00<<0)  /* PE0, PE1, PE2, PE3 */
#define PORTMUX_USART4_ALT1_GC (0x01<<0)  /* PE4, PE5, PE6, PE7 */
#define PORTMUX_USART4_NONE_GC (0x03<<0)  /* Not connected to any pins */
/* USART5 Signals select */
#define PORTMUX_USART5_DEFAULT_GC (0x00<<2)  /* PG0, PG1, PG2, PG3 */
#define PORTMUX_USART5_ALT1_GC (0x01<<2)  /* PG4, PG5, PG6, PG7 */
#define PORTMUX_USART5_NONE_GC (0x03<<2)  /* Not connected to any pins */
/* Zero Cross Detector 0 Output select */
#define PORTMUX_ZCD0_DEFAULT_GC (0x00<<0)  /* OUT on PA7 */
#define PORTMUX_ZCD0_ALT1_GC (0x01<<0)  /* OUT on PC7 */
/* Zero Cross Detector 1 Output select */
#define PORTMUX_ZCD1_DEFAULT_GC (0x00<<1)  /* OUT on PA7 */
#define PORTMUX_ZCD1_ALT1_GC (0x01<<1)  /* OUT on PC7 */
/* Zero Cross Detector 2 Output select */
#define PORTMUX_ZCD2_DEFAULT_GC (0x00<<2)  /* OUT on PA7 */
#define PORTMUX_ZCD2_ALT1_GC (0x01<<2)  /* OUT on PC7 */

/*------------------------------------------------------------------------
RSTCTRL - Reset controller
------------------------------------------------------------------------*/


/*------------------------------------------------------------------------
RTC - Real-Time Counter
------------------------------------------------------------------------*/

/* Clock Select select */
#define RTC_CLKSEL_OSC32K_GC (0x00<<0)  /* 32.768 kHz from OSC32K */
#define RTC_CLKSEL_OSC1K_GC (0x01<<0)  /* 1.024 kHz from OSC32K */
#define RTC_CLKSEL_XOSC32K_GC (0x02<<0)  /* 32.768 kHz from XOSC32K */
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

/* High Temperature Low Leakage Enable select */
#define SLPCTRL_HTLLEN_OFF_GC (0x00<<4)  /* Disabled */
#define SLPCTRL_HTLLEN_ON_GC (0x01<<4)  /* Enabled */
/* Performance Mode select */
#define SLPCTRL_PMODE_AUTO_GC (0x00<<0)  /*  */
#define SLPCTRL_PMODE_FULL_GC (0x01<<0)  /*  */
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
/* Event Action A select */
#define TCA_SINGLE_EVACTA_CNT_POSEDGE_GC (0x00<<1)  /* Count on positive edge event */
#define TCA_SINGLE_EVACTA_CNT_ANYEDGE_GC (0x01<<1)  /* Count on any edge event */
#define TCA_SINGLE_EVACTA_CNT_HIGHLVL_GC (0x02<<1)  /* Count on prescaled clock while event line is 1. */
#define TCA_SINGLE_EVACTA_UPDOWN_GC (0x03<<1)  /* Count on prescaled clock. Event controls count direction. Up-count when event line is 0, down-count when event line is 1. */
/* Event Action B select */
#define TCA_SINGLE_EVACTB_NONE_GC (0x00<<5)  /* No Action */
#define TCA_SINGLE_EVACTB_UPDOWN_GC (0x03<<5)  /* Count on prescaled clock. Event controls count direction. Up-count when event line is 0, down-count when event line is 1. */
#define TCA_SINGLE_EVACTB_RESTART_POSEDGE_GC (0x04<<5)  /* Restart counter at positive edge event */
#define TCA_SINGLE_EVACTB_RESTART_ANYEDGE_GC (0x05<<5)  /* Restart counter on any edge event */
#define TCA_SINGLE_EVACTB_RESTART_HIGHLVL_GC (0x06<<5)  /* Restart counter while event line is 1. */
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
/* Command Enable select */
#define TCA_SPLIT_CMDEN_NONE_GC (0x00<<0)  /* None */
#define TCA_SPLIT_CMDEN_BOTH_GC (0x03<<0)  /* Both low byte and high byte counter */

/*------------------------------------------------------------------------
TCB - 16-bit Timer Type B
------------------------------------------------------------------------*/

/* Clock Select select */
#define TCB_CLKSEL_DIV1_GC (0x00<<1)  /* CLK_PER */
#define TCB_CLKSEL_DIV2_GC (0x01<<1)  /* CLK_PER/2 */
#define TCB_CLKSEL_TCA0_GC (0x02<<1)  /* Use CLK_TCA from TCA0 */
#define TCB_CLKSEL_TCA1_GC (0x03<<1)  /* Use CLK_TCA from TCA1 */
#define TCB_CLKSEL_EVENT_GC (0x07<<1)  /* Count on event edge */
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
#define TCD_CLKSEL_OSCHF_GC (0x00<<5)  /* Internal High-Frequency Oscillator */
#define TCD_CLKSEL_PLL_GC (0x01<<5)  /* PLL */
#define TCD_CLKSEL_EXTCLK_GC (0x02<<5)  /* External Clock */
#define TCD_CLKSEL_CLKPER_GC (0x03<<5)  /* Peripheral Clock */
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
/* Synchronization prescaler select */
#define TCD_SYNCPRES_DIV1_GC (0x00<<1)  /* Selected clock source divided by 1 */
#define TCD_SYNCPRES_DIV2_GC (0x01<<1)  /* Selected clock source divided by 2 */
#define TCD_SYNCPRES_DIV4_GC (0x02<<1)  /* Selected clock source divided by 4 */
#define TCD_SYNCPRES_DIV8_GC (0x03<<1)  /* Selected clock source divided by 8 */
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
/* Client Address or Stop select */
#define TWI_AP_STOP_GC (0x00<<0)  /* Stop condition generated APIF */
#define TWI_AP_ADR_GC (0x01<<0)  /* Address detection generated APIF */
/* Bus State select */
#define TWI_BUSSTATE_UNKNOWN_GC (0x00<<0)  /* Unknown Bus State */
#define TWI_BUSSTATE_IDLE_GC (0x01<<0)  /* Bus is Idle */
#define TWI_BUSSTATE_OWNER_GC (0x02<<0)  /* This Module Controls The Bus */
#define TWI_BUSSTATE_BUSY_GC (0x03<<0)  /* The Bus is Busy */
/* Input Voltage Transition Level select */
#define TWI_INPUTLVL_I2C_GC (0x00<<6)  /* I2C input voltage transition level */
#define TWI_INPUTLVL_SMBUS_GC (0x01<<6)  /* SMBus 3.0 input voltage transition level */
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

/* Auto Baud Window select */
#define USART_ABW_WDW0_GC (0x00<<6)  /* 18% tolerance */
#define USART_ABW_WDW1_GC (0x01<<6)  /* 15% tolerance */
#define USART_ABW_WDW2_GC (0x02<<6)  /* 21% tolerance */
#define USART_ABW_WDW3_GC (0x03<<6)  /* 25% tolerance */
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
#define USART_CMODE_MSPI_GC (0x03<<6)  /* SPI Host Mode */
/* Parity Mode select */
#define USART_PMODE_DISABLED_GC (0x00<<4)  /* No Parity */
#define USART_PMODE_EVEN_GC (0x02<<4)  /* Even Parity */
#define USART_PMODE_ODD_GC (0x03<<4)  /* Odd Parity */
/* RS485 Mode internal transmitter select */
#define USART_RS485_DISABLE_GC (0x00<<0)  /* RS485 Mode disabled */
#define USART_RS485_ENABLE_GC (0x01<<0)  /* RS485 Mode enabled */
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

/* Reference select select */
#define VREF_REFSEL_1V024_GC (0x00<<0)  /* Internal 1.024V reference */
#define VREF_REFSEL_2V048_GC (0x01<<0)  /* Internal 2.048V reference */
#define VREF_REFSEL_4V096_GC (0x02<<0)  /* Internal 4.096V reference */
#define VREF_REFSEL_2V500_GC (0x03<<0)  /* Internal 2.500V reference */
#define VREF_REFSEL_VDD_GC (0x05<<0)  /* VDD as reference */
#define VREF_REFSEL_VREFA_GC (0x06<<0)  /* External reference on VREFA pin */

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

/*------------------------------------------------------------------------
ZCD - Zero Cross Detect
------------------------------------------------------------------------*/

/* Interrupt Mode select */
#define ZCD_INTMODE_NONE_GC (0x00<<0)  /* No interrupt */
#define ZCD_INTMODE_RISING_GC (0x01<<0)  /* Interrupt on rising input signal */
#define ZCD_INTMODE_FALLING_GC (0x02<<0)  /* Interrupt on falling input signal */
#define ZCD_INTMODE_BOTH_GC (0x03<<0)  /* Interrupt on both rising and falling input signal */
/* ZCD State select */
#define ZCD_STATE_LOW_GC (0x00<<4)  /* Output is 0 */
#define ZCD_STATE_HIGH_GC (0x01<<4)  /* Output is 1 */


#endif
