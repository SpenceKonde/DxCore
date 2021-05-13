
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
#define AC_INTMODE_BOTHEDGE_GC (0x00<<4)  /* Both Edge */
#define AC_INTMODE_NEGEDGE_GC (0x02<<4)  /* Negative Edge */
#define AC_INTMODE_POSEDGE_GC (0x03<<4)  /* Positive Edge */

/* Low Power Mode select */
#define AC_LPMODE_DIS_GC (0x00<<3)  /* Low power mode disabled */
#define AC_LPMODE_EN_GC (0x01<<3)  /* Low power mode enabled */

/* Negative Input MUX Selection select */
#define AC_MUXNEG_PIN0_GC (0x00<<0)  /* Negative Pin 0 */
#define AC_MUXNEG_PIN1_GC (0x01<<0)  /* Negative Pin 1 */
#define AC_MUXNEG_PIN2_GC (0x02<<0)  /* Negative Pin 2 */
#define AC_MUXNEG_DACREF_GC (0x03<<0)  /* DAC Voltage Reference */

/* Positive Input MUX Selection select */
#define AC_MUXPOS_PIN0_GC (0x00<<3)  /* Positive Pin 0 */
#define AC_MUXPOS_PIN1_GC (0x01<<3)  /* Positive Pin 1 */
#define AC_MUXPOS_PIN2_GC (0x02<<3)  /* Positive Pin 2 */

/*------------------------------------------------------------------------
ADC - Analog to Digital Converter
------------------------------------------------------------------------*/

/* ADC PGA Sample Duration select */
#define ADC_ADCPGASAMPDUR_6CLK_GC (0x00<<1)  /* 6 ADC cycles */
#define ADC_ADCPGASAMPDUR_15CLK_GC (0x01<<1)  /* 15 ADC cycles */
#define ADC_ADCPGASAMPDUR_20CLK_GC (0x02<<1)  /* 20 ADC cycles */
#define ADC_ADCPGASAMPDUR_32CLK_GC (0x03<<1)  /* 32 ADC cycles */

/* Gain select */
#define ADC_GAIN_1X_GC (0x00<<5)  /* 1X Gain */
#define ADC_GAIN_2X_GC (0x01<<5)  /* 2X Gain */
#define ADC_GAIN_4X_GC (0x02<<5)  /* 4X Gain */
#define ADC_GAIN_8X_GC (0x03<<5)  /* 8X Gain */
#define ADC_GAIN_16X_GC (0x04<<5)  /* 16X Gain */

/* Mode select */
#define ADC_MODE_SINGLE_8BIT_GC (0x00<<4)  /* Single Conversion 8-bit */
#define ADC_MODE_SINGLE_12BIT_GC (0x01<<4)  /* Single Conversion 12-bit */
#define ADC_MODE_SERIES_GC (0x02<<4)  /* Series Accumulation */
#define ADC_MODE_SERIES_SCALING_GC (0x03<<4)  /* Series Accumulation with Scaling */
#define ADC_MODE_BURST_GC (0x04<<4)  /* Burst Accumulation */
#define ADC_MODE_BURST_SCALING_GC (0x05<<4)  /* Burst Accumulation with Scaling */

/* Analog Channel Selection Bits select */
#define ADC_MUXNEG_AIN1_GC (0x01<<0)  /* ADC input pin 1 */
#define ADC_MUXNEG_AIN2_GC (0x02<<0)  /* ADC input pin 2 */
#define ADC_MUXNEG_AIN3_GC (0x03<<0)  /* ADC input pin 3 */
#define ADC_MUXNEG_AIN4_GC (0x04<<0)  /* ADC input pin 4 */
#define ADC_MUXNEG_AIN5_GC (0x05<<0)  /* ADC input pin 5 */
#define ADC_MUXNEG_AIN6_GC (0x06<<0)  /* ADC input pin 6 */
#define ADC_MUXNEG_AIN7_GC (0x07<<0)  /* ADC input pin 7 */
#define ADC_MUXNEG_GND_GC (0x30<<0)  /* Ground */
#define ADC_MUXNEG_VDDDIV10_GC (0x31<<0)  /* VDD/10 */
#define ADC_MUXNEG_DACREF0_GC (0x33<<0)  /* DACREF from AC0 */

/* Analog Channel Selection Bits select */
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
#define ADC_MUXPOS_GND_GC (0x30<<0)  /* Ground */
#define ADC_MUXPOS_VDDDIV10_GC (0x31<<0)  /* VDD/10 */
#define ADC_MUXPOS_TEMPSENSE_GC (0x32<<0)  /* Temperature sensor */
#define ADC_MUXPOS_DACREF0_GC (0x33<<0)  /* DACREF from AC0 */

/* PGA BIAS Select select */
#define ADC_PGABIASSEL_1X_GC (0x00<<3)  /* 1x BIAS current */
#define ADC_PGABIASSEL_3_4X_GC (0x01<<3)  /* 3/4x BIAS current */
#define ADC_PGABIASSEL_1_2X_GC (0x02<<3)  /* 1/2x BIAS current */
#define ADC_PGABIASSEL_1_4X_GC (0x03<<3)  /* 1/4x BIAS current */

/* Prescaler Value select */
#define ADC_PRESC_DIV2_GC (0x00<<0)  /* System clock divided by 2 */
#define ADC_PRESC_DIV4_GC (0x01<<0)  /* System clock divided by 4 */
#define ADC_PRESC_DIV6_GC (0x02<<0)  /* System clock divided by 6 */
#define ADC_PRESC_DIV8_GC (0x03<<0)  /* System clock divided by 8 */
#define ADC_PRESC_DIV10_GC (0x04<<0)  /* System clock divided by 10 */
#define ADC_PRESC_DIV12_GC (0x05<<0)  /* System clock divided by 12 */
#define ADC_PRESC_DIV14_GC (0x06<<0)  /* System clock divided by 14 */
#define ADC_PRESC_DIV16_GC (0x07<<0)  /* System clock divided by 16 */
#define ADC_PRESC_DIV20_GC (0x08<<0)  /* System clock divided by 20 */
#define ADC_PRESC_DIV24_GC (0x09<<0)  /* System clock divided by 24 */
#define ADC_PRESC_DIV28_GC (0x0A<<0)  /* System clock divided by 28 */
#define ADC_PRESC_DIV32_GC (0x0B<<0)  /* System clock divided by 32 */
#define ADC_PRESC_DIV40_GC (0x0C<<0)  /* System clock divided by 40 */
#define ADC_PRESC_DIV48_GC (0x0D<<0)  /* System clock divided by 48 */
#define ADC_PRESC_DIV56_GC (0x0E<<0)  /* System clock divided by 56 */
#define ADC_PRESC_DIV64_GC (0x0F<<0)  /* System clock divided by 64 */

/* Reference select select */
#define ADC_REFSEL_VDD_GC (0x00<<0)  /* VDD */
#define ADC_REFSEL_VREFA_GC (0x02<<0)  /* External Reference */
#define ADC_REFSEL_1024MV_GC (0x04<<0)  /* Internal 1.024V Reference */
#define ADC_REFSEL_2048MV_GC (0x05<<0)  /* Internal 2.048V Reference */
#define ADC_REFSEL_2500MV_GC (0x06<<0)  /* Internal 2.5V Reference */
#define ADC_REFSEL_4096MV_GC (0x07<<0)  /* Internal 4.096V Reference */

/* Sample numbers select */
#define ADC_SAMPNUM_NONE_GC (0x00<<0)  /* No accumulation */
#define ADC_SAMPNUM_ACC2_GC (0x01<<0)  /* 2 samples accumulation */
#define ADC_SAMPNUM_ACC4_GC (0x02<<0)  /* 4 samples accumulation */
#define ADC_SAMPNUM_ACC8_GC (0x03<<0)  /* 8 samples accumulation */
#define ADC_SAMPNUM_ACC16_GC (0x04<<0)  /* 16 samples accumulation */
#define ADC_SAMPNUM_ACC32_GC (0x05<<0)  /* 32 samples accumulation */
#define ADC_SAMPNUM_ACC64_GC (0x06<<0)  /* 64 samples accumulation */
#define ADC_SAMPNUM_ACC128_GC (0x07<<0)  /* 128 samples accumulation */
#define ADC_SAMPNUM_ACC256_GC (0x08<<0)  /* 256 samples accumulation */
#define ADC_SAMPNUM_ACC512_GC (0x09<<0)  /* 512 samples accumulation */
#define ADC_SAMPNUM_ACC1024_GC (0x0A<<0)  /* 1024 samples accumulation */

/* Start command select */
#define ADC_START_STOP_GC (0x00<<0)  /* Stop an ongoing conversion */
#define ADC_START_IMMEDIATE_GC (0x01<<0)  /* Start immediately */
#define ADC_START_MUXPOS_WRITE_GC (0x02<<0)  /* Start on MUXPOS write */
#define ADC_START_MUXNEG_WRITE_GC (0x03<<0)  /* Start on MUXNEG write */
#define ADC_START_EVENT_TRIGGER_GC (0x04<<0)  /* Start on event */

/* VIA select */
#define ADC_VIA_ADC_GC (0x00<<6)  /* Via ADC */
#define ADC_VIA_PGA_GC (0x01<<6)  /* Via PGA */

/* Window Comparator Mode select */
#define ADC_WINCM_NONE_GC (0x00<<0)  /* No Window Comparison */
#define ADC_WINCM_BELOW_GC (0x01<<0)  /* Below Window */
#define ADC_WINCM_ABOVE_GC (0x02<<0)  /* Above Window */
#define ADC_WINCM_INSIDE_GC (0x03<<0)  /* Inside Window */
#define ADC_WINCM_OUTSIDE_GC (0x04<<0)  /* Outside Window */

/* Window Mode Source select */
#define ADC_WINSRC_RESULT_GC (0x00<<3)  /* Result register */
#define ADC_WINSRC_SAMPLE_GC (0x01<<3)  /* Sample register */

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

/* Clock Source Selection select */
#define CCL_CLKSRC_CLKPER_GC (0x00<<1)  /* CLK_PER is clocking the LUT */
#define CCL_CLKSRC_IN2_GC (0x01<<1)  /* IN[2] is clocking the LUT */
#define CCL_CLKSRC_OSC20M_GC (0x04<<1)  /* 20MHz oscillator before prescaler is clocking the LUT */
#define CCL_CLKSRC_OSCULP32K_GC (0x05<<1)  /* 32kHz oscillator is clocking the LUT */
#define CCL_CLKSRC_OSCULP1K_GC (0x06<<1)  /* 32kHz oscillator after DIV32 is clocking the LUT */

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
#define CCL_INSEL0_IO_GC (0x05<<0)  /* IO pin LUTn-IN0 input source */
#define CCL_INSEL0_AC0_GC (0x06<<0)  /* AC0 OUT input source */
#define CCL_INSEL0_USART0_GC (0x08<<0)  /* USART0 TXD input source */
#define CCL_INSEL0_SPI0_GC (0x09<<0)  /* SPI0 MOSI input source */
#define CCL_INSEL0_TCA0_GC (0x0A<<0)  /* TCA0 WO0 input source */
#define CCL_INSEL0_TCB0_GC (0x0C<<0)  /* TCB0 WO input source */

/* LUT Input 1 Source Selection select */
#define CCL_INSEL1_MASK_GC (0x00<<4)  /* Masked input */
#define CCL_INSEL1_FEEDBACK_GC (0x01<<4)  /* Feedback input source */
#define CCL_INSEL1_LINK_GC (0x02<<4)  /* Linked LUT input source */
#define CCL_INSEL1_EVENTA_GC (0x03<<4)  /* Event input source A */
#define CCL_INSEL1_EVENTB_GC (0x04<<4)  /* Event input source B */
#define CCL_INSEL1_IO_GC (0x05<<4)  /* IO pin LUTn-IN1 input source */
#define CCL_INSEL1_AC0_GC (0x06<<4)  /* AC0 OUT input source */
#define CCL_INSEL1_USART1_GC (0x08<<4)  /* USART1 TXD input source */
#define CCL_INSEL1_SPI0_GC (0x09<<4)  /* SPI0 MOSI input source */
#define CCL_INSEL1_TCA0_GC (0x0A<<4)  /* TCA0 WO1 input source */
#define CCL_INSEL1_TCB1_GC (0x0C<<4)  /* TCB1 WO input source */

/* LUT Input 2 Source Selection select */
#define CCL_INSEL2_MASK_GC (0x00<<0)  /* Masked input */
#define CCL_INSEL2_FEEDBACK_GC (0x01<<0)  /* Feedback input source */
#define CCL_INSEL2_LINK_GC (0x02<<0)  /* Linked LUT input source */
#define CCL_INSEL2_EVENTA_GC (0x03<<0)  /* Event input source A */
#define CCL_INSEL2_EVENTB_GC (0x04<<0)  /* Event input source B */
#define CCL_INSEL2_IO_GC (0x05<<0)  /* IO pin LUTn-IN2 input source */
#define CCL_INSEL2_AC0_GC (0x06<<0)  /* AC0 OUT input source */
#define CCL_INSEL2_SPI0_GC (0x09<<0)  /* SPI0 SCK input source */
#define CCL_INSEL2_TCA0_GC (0x0A<<0)  /* TCA0 WO2 input source */

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

/* Sequential Selection select */
#define CCL_SEQSEL_DISABLE_GC (0x00<<0)  /* Sequential logic disabled */
#define CCL_SEQSEL_DFF_GC (0x01<<0)  /* D FlipFlop */
#define CCL_SEQSEL_JK_GC (0x02<<0)  /* JK FlipFlop */
#define CCL_SEQSEL_LATCH_GC (0x03<<0)  /* D Latch */
#define CCL_SEQSEL_RS_GC (0x04<<0)  /* RS Latch */

/*------------------------------------------------------------------------
CLKCTRL - Clock controller
------------------------------------------------------------------------*/

/* clock select */
#define CLKCTRL_CLKSEL_OSC20M_GC (0x00<<0)  /* 20MHz oscillator */
#define CLKCTRL_CLKSEL_OSCULP32K_GC (0x01<<0)  /* 32KHz oscillator */
#define CLKCTRL_CLKSEL_XOSC32K_GC (0x02<<0)  /* 32.768kHz crystal oscillator */
#define CLKCTRL_CLKSEL_EXTCLK_GC (0x03<<0)  /* External clock */

/* Crystal startup time select */
#define CLKCTRL_CSUT_1K_GC (0x00<<4)  /* 1k cycles */
#define CLKCTRL_CSUT_16K_GC (0x01<<4)  /* 16k cycles */
#define CLKCTRL_CSUT_32K_GC (0x02<<4)  /* 32k cycles */
#define CLKCTRL_CSUT_64K_GC (0x03<<4)  /* 64k cycles */

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
EVSYS - Event System
------------------------------------------------------------------------*/

/* Channel 0 generator select select */
#define EVSYS_CHANNEL0_OFF_GC (0x00<<0)  /* Off */
#define EVSYS_CHANNEL0_UPDI_GC (0x01<<0)  /* Unified Program and Debug Interface */
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
#define EVSYS_CHANNEL0_AC0_OUT_GC (0x20<<0)  /* Analog Comparator 0 out */
#define EVSYS_CHANNEL0_ADC0_RES_GC (0x24<<0)  /* ADC 0 Result ready event */
#define EVSYS_CHANNEL0_ADC0_SAMP_GC (0x25<<0)  /* ADC 0 Sample ready event */
#define EVSYS_CHANNEL0_ADC0_WCMP_GC (0x26<<0)  /* ADC 0 Window compare match event */
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
#define EVSYS_CHANNEL0_USART0_XCK_GC (0x60<<0)  /* USART 0 XCK */
#define EVSYS_CHANNEL0_USART1_XCK_GC (0x61<<0)  /* USART 1 XCK */
#define EVSYS_CHANNEL0_SPI0_SCK_GC (0x68<<0)  /* SPI 0 Sclock */
#define EVSYS_CHANNEL0_TCA0_OVF_LUNF_GC (0x80<<0)  /* Timer/Counter A0 overflow / low byte underflow */
#define EVSYS_CHANNEL0_TCA0_HUNF_GC (0x81<<0)  /* Timer/Counter A0 high byte underflow */
#define EVSYS_CHANNEL0_TCA0_CMP0_LCMP0_GC (0x84<<0)  /* Timer/Counter A0 compare 0 / low byte timer compare 0 */
#define EVSYS_CHANNEL0_TCA0_CMP1_LCMP1_GC (0x85<<0)  /* Timer/Counter A0 compare 1 / low byte timer compare 1 */
#define EVSYS_CHANNEL0_TCA0_CMP2_LCMP2_GC (0x86<<0)  /* Timer/Counter A0 compare 2 / low byte timer compare 2 */
#define EVSYS_CHANNEL0_TCB0_CAPT_GC (0xA0<<0)  /* Timer/Counter B0 capture */
#define EVSYS_CHANNEL0_TCB0_OVF_GC (0xA1<<0)  /* Timer/Counter B0 overflow */
#define EVSYS_CHANNEL0_TCB1_CAPT_GC (0xA2<<0)  /* Timer/Counter B1 capture */
#define EVSYS_CHANNEL0_TCB1_OVF_GC (0xA3<<0)  /* Timer/Counter B1 overflow */

/* Channel 1 generator select select */
#define EVSYS_CHANNEL1_OFF_GC (0x00<<0)  /* Off */
#define EVSYS_CHANNEL1_UPDI_GC (0x01<<0)  /* Unified Program and Debug Interface */
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
#define EVSYS_CHANNEL1_AC0_OUT_GC (0x20<<0)  /* Analog Comparator 0 out */
#define EVSYS_CHANNEL1_ADC0_RES_GC (0x24<<0)  /* ADC 0 Result ready event */
#define EVSYS_CHANNEL1_ADC0_SAMP_GC (0x25<<0)  /* ADC 0 Sample ready event */
#define EVSYS_CHANNEL1_ADC0_WCMP_GC (0x26<<0)  /* ADC 0 Window compare match event */
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
#define EVSYS_CHANNEL1_USART0_XCK_GC (0x60<<0)  /* USART 0 XCK */
#define EVSYS_CHANNEL1_USART1_XCK_GC (0x61<<0)  /* USART 1 XCK */
#define EVSYS_CHANNEL1_SPI0_SCK_GC (0x68<<0)  /* SPI 0 Sclock */
#define EVSYS_CHANNEL1_TCA0_OVF_LUNF_GC (0x80<<0)  /* Timer/Counter A0 overflow / low byte underflow */
#define EVSYS_CHANNEL1_TCA0_HUNF_GC (0x81<<0)  /* Timer/Counter A0 high byte underflow */
#define EVSYS_CHANNEL1_TCA0_CMP0_LCMP0_GC (0x84<<0)  /* Timer/Counter A0 compare 0 / low byte timer compare 0 */
#define EVSYS_CHANNEL1_TCA0_CMP1_LCMP1_GC (0x85<<0)  /* Timer/Counter A0 compare 1 / low byte timer compare 1 */
#define EVSYS_CHANNEL1_TCA0_CMP2_LCMP2_GC (0x86<<0)  /* Timer/Counter A0 compare 2 / low byte timer compare 2 */
#define EVSYS_CHANNEL1_TCB0_CAPT_GC (0xA0<<0)  /* Timer/Counter B0 capture */
#define EVSYS_CHANNEL1_TCB0_OVF_GC (0xA1<<0)  /* Timer/Counter B0 overflow */
#define EVSYS_CHANNEL1_TCB1_CAPT_GC (0xA2<<0)  /* Timer/Counter B1 capture */
#define EVSYS_CHANNEL1_TCB1_OVF_GC (0xA3<<0)  /* Timer/Counter B1 overflow */

/* Channel 2 generator select select */
#define EVSYS_CHANNEL2_OFF_GC (0x00<<0)  /* Off */
#define EVSYS_CHANNEL2_UPDI_GC (0x01<<0)  /* Unified Program and Debug Interface */
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
#define EVSYS_CHANNEL2_AC0_OUT_GC (0x20<<0)  /* Analog Comparator 0 out */
#define EVSYS_CHANNEL2_ADC0_RES_GC (0x24<<0)  /* ADC 0 Result ready event */
#define EVSYS_CHANNEL2_ADC0_SAMP_GC (0x25<<0)  /* ADC 0 Sample ready event */
#define EVSYS_CHANNEL2_ADC0_WCMP_GC (0x26<<0)  /* ADC 0 Window compare match event */
#define EVSYS_CHANNEL2_PORTC_PIN0_GC (0x40<<0)  /* Port C Pin 0 */
#define EVSYS_CHANNEL2_PORTC_PIN1_GC (0x41<<0)  /* Port C Pin 1 */
#define EVSYS_CHANNEL2_PORTC_PIN2_GC (0x42<<0)  /* Port C Pin 2 */
#define EVSYS_CHANNEL2_PORTC_PIN3_GC (0x43<<0)  /* Port C Pin 3 */
#define EVSYS_CHANNEL2_PORTA_PIN0_GC (0x48<<0)  /* Port A Pin 0 */
#define EVSYS_CHANNEL2_PORTA_PIN1_GC (0x49<<0)  /* Port A Pin 1 */
#define EVSYS_CHANNEL2_PORTA_PIN2_GC (0x4A<<0)  /* Port A Pin 2 */
#define EVSYS_CHANNEL2_PORTA_PIN3_GC (0x4B<<0)  /* Port A Pin 3 */
#define EVSYS_CHANNEL2_PORTA_PIN4_GC (0x4C<<0)  /* Port A Pin 4 */
#define EVSYS_CHANNEL2_PORTA_PIN5_GC (0x4D<<0)  /* Port A Pin 5 */
#define EVSYS_CHANNEL2_PORTA_PIN6_GC (0x4E<<0)  /* Port A Pin 6 */
#define EVSYS_CHANNEL2_PORTA_PIN7_GC (0x4F<<0)  /* Port A Pin 7 */
#define EVSYS_CHANNEL2_USART0_XCK_GC (0x60<<0)  /* USART 0 XCK */
#define EVSYS_CHANNEL2_USART1_XCK_GC (0x61<<0)  /* USART 1 XCK */
#define EVSYS_CHANNEL2_SPI0_SCK_GC (0x68<<0)  /* SPI 0 Sclock */
#define EVSYS_CHANNEL2_TCA0_OVF_LUNF_GC (0x80<<0)  /* Timer/Counter A0 overflow / low byte underflow */
#define EVSYS_CHANNEL2_TCA0_HUNF_GC (0x81<<0)  /* Timer/Counter A0 high byte underflow */
#define EVSYS_CHANNEL2_TCA0_CMP0_LCMP0_GC (0x84<<0)  /* Timer/Counter A0 compare 0 / low byte timer compare 0 */
#define EVSYS_CHANNEL2_TCA0_CMP1_LCMP1_GC (0x85<<0)  /* Timer/Counter A0 compare 1 / low byte timer compare 1 */
#define EVSYS_CHANNEL2_TCA0_CMP2_LCMP2_GC (0x86<<0)  /* Timer/Counter A0 compare 2 / low byte timer compare 2 */
#define EVSYS_CHANNEL2_TCB0_CAPT_GC (0xA0<<0)  /* Timer/Counter B0 capture */
#define EVSYS_CHANNEL2_TCB0_OVF_GC (0xA1<<0)  /* Timer/Counter B0 overflow */
#define EVSYS_CHANNEL2_TCB1_CAPT_GC (0xA2<<0)  /* Timer/Counter B1 capture */
#define EVSYS_CHANNEL2_TCB1_OVF_GC (0xA3<<0)  /* Timer/Counter B1 overflow */

/* Channel 3 generator select select */
#define EVSYS_CHANNEL3_OFF_GC (0x00<<0)  /* Off */
#define EVSYS_CHANNEL3_UPDI_GC (0x01<<0)  /* Unified Program and Debug Interface */
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
#define EVSYS_CHANNEL3_AC0_OUT_GC (0x20<<0)  /* Analog Comparator 0 out */
#define EVSYS_CHANNEL3_ADC0_RES_GC (0x24<<0)  /* ADC 0 Result ready event */
#define EVSYS_CHANNEL3_ADC0_SAMP_GC (0x25<<0)  /* ADC 0 Sample ready event */
#define EVSYS_CHANNEL3_ADC0_WCMP_GC (0x26<<0)  /* ADC 0 Window compare match event */
#define EVSYS_CHANNEL3_PORTC_PIN0_GC (0x40<<0)  /* Port C Pin 0 */
#define EVSYS_CHANNEL3_PORTC_PIN1_GC (0x41<<0)  /* Port C Pin 1 */
#define EVSYS_CHANNEL3_PORTC_PIN2_GC (0x42<<0)  /* Port C Pin 2 */
#define EVSYS_CHANNEL3_PORTC_PIN3_GC (0x43<<0)  /* Port C Pin 3 */
#define EVSYS_CHANNEL3_PORTA_PIN0_GC (0x48<<0)  /* Port A Pin 0 */
#define EVSYS_CHANNEL3_PORTA_PIN1_GC (0x49<<0)  /* Port A Pin 1 */
#define EVSYS_CHANNEL3_PORTA_PIN2_GC (0x4A<<0)  /* Port A Pin 2 */
#define EVSYS_CHANNEL3_PORTA_PIN3_GC (0x4B<<0)  /* Port A Pin 3 */
#define EVSYS_CHANNEL3_PORTA_PIN4_GC (0x4C<<0)  /* Port A Pin 4 */
#define EVSYS_CHANNEL3_PORTA_PIN5_GC (0x4D<<0)  /* Port A Pin 5 */
#define EVSYS_CHANNEL3_PORTA_PIN6_GC (0x4E<<0)  /* Port A Pin 6 */
#define EVSYS_CHANNEL3_PORTA_PIN7_GC (0x4F<<0)  /* Port A Pin 7 */
#define EVSYS_CHANNEL3_USART0_XCK_GC (0x60<<0)  /* USART 0 XCK */
#define EVSYS_CHANNEL3_USART1_XCK_GC (0x61<<0)  /* USART 1 XCK */
#define EVSYS_CHANNEL3_SPI0_SCK_GC (0x68<<0)  /* SPI 0 Sclock */
#define EVSYS_CHANNEL3_TCA0_OVF_LUNF_GC (0x80<<0)  /* Timer/Counter A0 overflow / low byte underflow */
#define EVSYS_CHANNEL3_TCA0_HUNF_GC (0x81<<0)  /* Timer/Counter A0 high byte underflow */
#define EVSYS_CHANNEL3_TCA0_CMP0_LCMP0_GC (0x84<<0)  /* Timer/Counter A0 compare 0 / low byte timer compare 0 */
#define EVSYS_CHANNEL3_TCA0_CMP1_LCMP1_GC (0x85<<0)  /* Timer/Counter A0 compare 1 / low byte timer compare 1 */
#define EVSYS_CHANNEL3_TCA0_CMP2_LCMP2_GC (0x86<<0)  /* Timer/Counter A0 compare 2 / low byte timer compare 2 */
#define EVSYS_CHANNEL3_TCB0_CAPT_GC (0xA0<<0)  /* Timer/Counter B0 capture */
#define EVSYS_CHANNEL3_TCB0_OVF_GC (0xA1<<0)  /* Timer/Counter B0 overflow */
#define EVSYS_CHANNEL3_TCB1_CAPT_GC (0xA2<<0)  /* Timer/Counter B1 capture */
#define EVSYS_CHANNEL3_TCB1_OVF_GC (0xA3<<0)  /* Timer/Counter B1 overflow */

/* Channel 4 generator select select */
#define EVSYS_CHANNEL4_OFF_GC (0x00<<0)  /* Off */
#define EVSYS_CHANNEL4_UPDI_GC (0x01<<0)  /* Unified Program and Debug Interface */
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
#define EVSYS_CHANNEL4_AC0_OUT_GC (0x20<<0)  /* Analog Comparator 0 out */
#define EVSYS_CHANNEL4_ADC0_RES_GC (0x24<<0)  /* ADC 0 Result ready event */
#define EVSYS_CHANNEL4_ADC0_SAMP_GC (0x25<<0)  /* ADC 0 Sample ready event */
#define EVSYS_CHANNEL4_ADC0_WCMP_GC (0x26<<0)  /* ADC 0 Window compare match event */
#define EVSYS_CHANNEL4_PORTB_PIN0_GC (0x40<<0)  /* Port B Pin 0 */
#define EVSYS_CHANNEL4_PORTB_PIN1_GC (0x41<<0)  /* Port B Pin 1 */
#define EVSYS_CHANNEL4_PORTB_PIN2_GC (0x42<<0)  /* Port B Pin 2 */
#define EVSYS_CHANNEL4_PORTB_PIN3_GC (0x43<<0)  /* Port B Pin 3 */
#define EVSYS_CHANNEL4_PORTB_PIN4_GC (0x44<<0)  /* Port B Pin 4 */
#define EVSYS_CHANNEL4_PORTB_PIN5_GC (0x45<<0)  /* Port B Pin 5 */
#define EVSYS_CHANNEL4_PORTC_PIN0_GC (0x48<<0)  /* Port C Pin 0 */
#define EVSYS_CHANNEL4_PORTC_PIN1_GC (0x49<<0)  /* Port C Pin 1 */
#define EVSYS_CHANNEL4_PORTC_PIN2_GC (0x4A<<0)  /* Port C Pin 2 */
#define EVSYS_CHANNEL4_PORTC_PIN3_GC (0x4B<<0)  /* Port C Pin 3 */
#define EVSYS_CHANNEL4_USART0_XCK_GC (0x60<<0)  /* USART 0 XCK */
#define EVSYS_CHANNEL4_USART1_XCK_GC (0x61<<0)  /* USART 1 XCK */
#define EVSYS_CHANNEL4_SPI0_SCK_GC (0x68<<0)  /* SPI 0 Sclock */
#define EVSYS_CHANNEL4_TCA0_OVF_LUNF_GC (0x80<<0)  /* Timer/Counter A0 overflow / low byte underflow */
#define EVSYS_CHANNEL4_TCA0_HUNF_GC (0x81<<0)  /* Timer/Counter A0 high byte underflow */
#define EVSYS_CHANNEL4_TCA0_CMP0_LCMP0_GC (0x84<<0)  /* Timer/Counter A0 compare 0 / low byte timer compare 0 */
#define EVSYS_CHANNEL4_TCA0_CMP1_LCMP1_GC (0x85<<0)  /* Timer/Counter A0 compare 1 / low byte timer compare 1 */
#define EVSYS_CHANNEL4_TCA0_CMP2_LCMP2_GC (0x86<<0)  /* Timer/Counter A0 compare 2 / low byte timer compare 2 */
#define EVSYS_CHANNEL4_TCB0_CAPT_GC (0xA0<<0)  /* Timer/Counter B0 capture */
#define EVSYS_CHANNEL4_TCB0_OVF_GC (0xA1<<0)  /* Timer/Counter B0 overflow */
#define EVSYS_CHANNEL4_TCB1_CAPT_GC (0xA2<<0)  /* Timer/Counter B1 capture */
#define EVSYS_CHANNEL4_TCB1_OVF_GC (0xA3<<0)  /* Timer/Counter B1 overflow */

/* Channel 5 generator select select */
#define EVSYS_CHANNEL5_OFF_GC (0x00<<0)  /* Off */
#define EVSYS_CHANNEL5_UPDI_GC (0x01<<0)  /* Unified Program and Debug Interface */
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
#define EVSYS_CHANNEL5_AC0_OUT_GC (0x20<<0)  /* Analog Comparator 0 out */
#define EVSYS_CHANNEL5_ADC0_RES_GC (0x24<<0)  /* ADC 0 Result ready event */
#define EVSYS_CHANNEL5_ADC0_SAMP_GC (0x25<<0)  /* ADC 0 Sample ready event */
#define EVSYS_CHANNEL5_ADC0_WCMP_GC (0x26<<0)  /* ADC 0 Window compare match event */
#define EVSYS_CHANNEL5_PORTB_PIN0_GC (0x40<<0)  /* Port B Pin 0 */
#define EVSYS_CHANNEL5_PORTB_PIN1_GC (0x41<<0)  /* Port B Pin 1 */
#define EVSYS_CHANNEL5_PORTB_PIN2_GC (0x42<<0)  /* Port B Pin 2 */
#define EVSYS_CHANNEL5_PORTB_PIN3_GC (0x43<<0)  /* Port B Pin 3 */
#define EVSYS_CHANNEL5_PORTB_PIN4_GC (0x44<<0)  /* Port B Pin 4 */
#define EVSYS_CHANNEL5_PORTB_PIN5_GC (0x45<<0)  /* Port B Pin 5 */
#define EVSYS_CHANNEL5_PORTC_PIN0_GC (0x48<<0)  /* Port C Pin 0 */
#define EVSYS_CHANNEL5_PORTC_PIN1_GC (0x49<<0)  /* Port C Pin 1 */
#define EVSYS_CHANNEL5_PORTC_PIN2_GC (0x4A<<0)  /* Port C Pin 2 */
#define EVSYS_CHANNEL5_PORTC_PIN3_GC (0x4B<<0)  /* Port C Pin 3 */
#define EVSYS_CHANNEL5_USART0_XCK_GC (0x60<<0)  /* USART 0 XCK */
#define EVSYS_CHANNEL5_USART1_XCK_GC (0x61<<0)  /* USART 1 XCK */
#define EVSYS_CHANNEL5_SPI0_SCK_GC (0x68<<0)  /* SPI 0 Sclock */
#define EVSYS_CHANNEL5_TCA0_OVF_LUNF_GC (0x80<<0)  /* Timer/Counter A0 overflow / low byte underflow */
#define EVSYS_CHANNEL5_TCA0_HUNF_GC (0x81<<0)  /* Timer/Counter A0 high byte underflow */
#define EVSYS_CHANNEL5_TCA0_CMP0_LCMP0_GC (0x84<<0)  /* Timer/Counter A0 compare 0 / low byte timer compare 0 */
#define EVSYS_CHANNEL5_TCA0_CMP1_LCMP1_GC (0x85<<0)  /* Timer/Counter A0 compare 1 / low byte timer compare 1 */
#define EVSYS_CHANNEL5_TCA0_CMP2_LCMP2_GC (0x86<<0)  /* Timer/Counter A0 compare 2 / low byte timer compare 2 */
#define EVSYS_CHANNEL5_TCB0_CAPT_GC (0xA0<<0)  /* Timer/Counter B0 capture */
#define EVSYS_CHANNEL5_TCB0_OVF_GC (0xA1<<0)  /* Timer/Counter B0 overflow */
#define EVSYS_CHANNEL5_TCB1_CAPT_GC (0xA2<<0)  /* Timer/Counter B1 capture */
#define EVSYS_CHANNEL5_TCB1_OVF_GC (0xA3<<0)  /* Timer/Counter B1 overflow */

/* Software event on channel select */
#define EVSYS_SWEVENTA_CH0_GC (0x01<<0)  /* Software event on channel 0 */
#define EVSYS_SWEVENTA_CH1_GC (0x02<<0)  /* Software event on channel 1 */
#define EVSYS_SWEVENTA_CH2_GC (0x04<<0)  /* Software event on channel 2 */
#define EVSYS_SWEVENTA_CH3_GC (0x08<<0)  /* Software event on channel 3 */
#define EVSYS_SWEVENTA_CH4_GC (0x10<<0)  /* Software event on channel 4 */
#define EVSYS_SWEVENTA_CH5_GC (0x20<<0)  /* Software event on channel 5 */

/* Channel selector select */
#define EVSYS_USER_OFF_GC (0x00<<0)  /* Off */
#define EVSYS_USER_CHANNEL0_GC (0x01<<0)  /* Connect user to event channel 0 */
#define EVSYS_USER_CHANNEL1_GC (0x02<<0)  /* Connect user to event channel 1 */
#define EVSYS_USER_CHANNEL2_GC (0x03<<0)  /* Connect user to event channel 2 */
#define EVSYS_USER_CHANNEL3_GC (0x04<<0)  /* Connect user to event channel 3 */
#define EVSYS_USER_CHANNEL4_GC (0x05<<0)  /* Connect user to event channel 4 */
#define EVSYS_USER_CHANNEL5_GC (0x06<<0)  /* Connect user to event channel 5 */

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
#define RSTPINCFG_PDIRST_GC (0x03<<2)  /* PDI on PDI pad, reset on alternative reset pad */

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

/* Event Output A select */
#define PORTMUX_EVOUTA_DEFAULT_GC (0x00<<0)  /* EVOUT on PA2 */
#define PORTMUX_EVOUTA_ALT1_GC (0x01<<0)  /* EVOUT on PA7 */

/* Event Output B select */
#define PORTMUX_EVOUTB_DEFAULT_GC (0x00<<1)  /* EVOUT on PB2 */

/* Event Output C select */
#define PORTMUX_EVOUTC_DEFAULT_GC (0x00<<2)  /* EVOUT on PC2 */

/* CCL LUT0 select */
#define PORTMUX_LUT0_DEFAULT_GC (0x00<<0)  /* Out: PA4 In: PA0, PA1, PA2 */
#define PORTMUX_LUT0_ALT1_GC (0x01<<0)  /* Out: PB4 In: PA0, PA1, PA2 */

/* CCL LUT1 select */
#define PORTMUX_LUT1_DEFAULT_GC (0x00<<1)  /* Out: PA7 In: PC3, PC4, PC5 */
#define PORTMUX_LUT1_ALT1_GC (0x01<<1)  /* Out: PC1 In: PC3, PC4, PC5 */

/* CCL LUT2 select */
#define PORTMUX_LUT2_DEFAULT_GC (0x00<<2)  /* Out: PB3 In: PB0, PB1, PB2 */

/* CCL LUT3 select */
#define PORTMUX_LUT3_ALT1_GC (0x01<<3)  /* Out: PA5 In: PC0, PC1, PC2 */

/* Port Multiplexer SPI0 select */
#define PORTMUX_SPI0_DEFAULT_GC (0x00<<0)  /* PA0, PA1, PA2, PA3 */
#define PORTMUX_SPI0_ALT1_GC (0x01<<0)  /* PC2, PC1, PC0, PC3 */
#define PORTMUX_SPI0_NONE_GC (0x03<<0)  /* Not connected to any pins */

/* Port Multiplexer TCA0 Output 0 select */
#define PORTMUX_TCA00_DEFAULT_GC (0x00<<0)  /* WO0 on PB0 */
#define PORTMUX_TCA00_ALT1_GC (0x01<<0)  /* WO0 on PB3 */

/* Port Multiplexer TCA0 Output 1 select */
#define PORTMUX_TCA01_DEFAULT_GC (0x00<<1)  /* WO1 on PB1 */
#define PORTMUX_TCA01_ALT1_GC (0x01<<1)  /* WO1 on PB4 */

/* Port Multiplexer TCA0 Output 2 select */
#define PORTMUX_TCA02_DEFAULT_GC (0x00<<2)  /* WO2 on PB2 */
#define PORTMUX_TCA02_ALT1_GC (0x01<<2)  /* WO2 on PB5 */

/* Port Multiplexer TCA0 Output 3 select */
#define PORTMUX_TCA03_DEFAULT_GC (0x00<<3)  /* WO3 on PA3 */
#define PORTMUX_TCA03_ALT1_GC (0x01<<3)  /* WO3 on PC3 */

/* Port Multiplexer TCA0 Output 4 select */
#define PORTMUX_TCA04_DEFAULT_GC (0x00<<4)  /* WO4 on PA4 */

/* Port Multiplexer TCA0 Output 5 select */
#define PORTMUX_TCA05_DEFAULT_GC (0x00<<5)  /* WO5 on PA5 */

/* Port Multiplexer TCB0 select */
#define PORTMUX_TCB0_DEFAULT_GC (0x00<<0)  /* WO on PA5 */
#define PORTMUX_TCB0_ALT1_GC (0x01<<0)  /* WO on PC0 */

/* Port Multiplexer TCB1 select */
#define PORTMUX_TCB1_DEFAULT_GC (0x00<<1)  /* WO on PA3 */

/* Port Multiplexer USART0 select */
#define PORTMUX_USART0_DEFAULT_GC (0x00<<0)  /* USART0 on PB[3:0] */
#define PORTMUX_USART0_ALT1_GC (0x01<<0)  /* USART0 on PA[4:1] */
#define PORTMUX_USART0_NONE_GC (0x03<<0)  /* Not connected to any pins */

/* Port Multiplexer USART1 select */
#define PORTMUX_USART1_DEFAULT_GC (0x00<<2)  /* USART1 on PA[4:1] */
#define PORTMUX_USART1_ALT1_GC (0x01<<2)  /* USART1 on PC[3:0] */
#define PORTMUX_USART1_NONE_GC (0x03<<2)  /* Not connected to any pins */

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

/* AC0 reference select select */
#define VREF_AC0REFSEL_1V024_GC (0x00<<0)  /* Voltage reference at 1.024V */
#define VREF_AC0REFSEL_2V048_GC (0x01<<0)  /* Voltage reference at 2.048V */
#define VREF_AC0REFSEL_2V5_GC (0x02<<0)  /* Voltage reference at 2.5V */
#define VREF_AC0REFSEL_4V096_GC (0x03<<0)  /* Voltage reference at 4.096V */
#define VREF_AC0REFSEL_AVDD_GC (0x07<<0)  /* voltage reference at supply */

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
