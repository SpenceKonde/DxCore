From ioavr128db64.h

/*
--------------------------------------------------------------------------
OPAMP - Operational Amplifier System
--------------------------------------------------------------------------
*/



/* Operational Amplifier System */
typedef struct OPAMP_struct
{
    register8_t CTRLA;  /* Control A */
    register8_t DBGCTRL;  /* Debug Control */
    register8_t TIMEBASE;  /* Timebase Value */
    register8_t reserved_1[12];
    register8_t PWRCTRL;  /* Power Control */
    register8_t OP0CTRLA;  /* Op Amp 0 Control A */
    register8_t OP0STATUS;  /* Op Amp 0 Status */
    register8_t OP0RESMUX;  /* Op Amp 0 Resistor Ladder Multiplexer */
    register8_t OP0INMUX;  /* Op Amp 0 Input Multiplexer */
    register8_t OP0SETTLE;  /* Op Amp 0 Settle */
    register8_t OP0CAL;  /* Op Amp 0 Calibration */
    register8_t reserved_2[2];
    register8_t OP1CTRLA;  /* Op Amp 1 Control A */
    register8_t OP1STATUS;  /* Op Amp 1 Status */
    register8_t OP1RESMUX;  /* Op Amp 1 Resistor Ladder Multiplexer */
    register8_t OP1INMUX;  /* Op Amp 1 Input Multiplexer */
    register8_t OP1SETTLE;  /* Op Amp 1 Settle */
    register8_t OP1CAL;  /* Op Amp 1 Calibration */
    register8_t reserved_3[2];
    register8_t OP2CTRLA;  /* Op Amp 2 Control A */
    register8_t OP2STATUS;  /* Op Amp 2 Status */
    register8_t OP2RESMUX;  /* Op Amp 2 Resistor Ladder Multiplexer */
    register8_t OP2INMUX;  /* Op Amp 2 Input Multiplexer */
    register8_t OP2SETTLE;  /* Op Amp 2 Settle */
    register8_t OP2CAL;  /* Op Amp 2 Calibration */
    register8_t reserved_4[26];
} OPAMP_t;

/* Output Mode select */
typedef enum OPAMP_OP0CTRLA_OUTMODE_enum
{
    OPAMP_OP0CTRLA_OUTMODE_OFF_gc = (0x00<<2),  /* Output Driver Off */
    OPAMP_OP0CTRLA_OUTMODE_NORMAL_gc = (0x01<<2),  /* Output Driver in Normal Mode */
} OPAMP_OP0CTRLA_OUTMODE_t;

/* Negative Input Multiplexer select */
typedef enum OPAMP_OP0INMUX_MUXNEG_enum
{
    OPAMP_OP0INMUX_MUXNEG_INN_gc = (0x00<<4),  /* Negative input pin for OPn */
    OPAMP_OP0INMUX_MUXNEG_WIP_gc = (0x01<<4),  /* Wiper from OPn's resistor ladder */
    OPAMP_OP0INMUX_MUXNEG_OUT_gc = (0x02<<4),  /* OPn output (unity gain) */
    OPAMP_OP0INMUX_MUXNEG_DAC_gc = (0x03<<4),  /* DAC output */
} OPAMP_OP0INMUX_MUXNEG_t;

/* Positive Input Multiplexer select */
typedef enum OPAMP_OP0INMUX_MUXPOS_enum
{
    OPAMP_OP0INMUX_MUXPOS_INP_gc = (0x00<<0),  /* Positive input pin for OPn */
    OPAMP_OP0INMUX_MUXPOS_WIP_gc = (0x01<<0),  /* Wiper from OPn's resistor ladder */
    OPAMP_OP0INMUX_MUXPOS_DAC_gc = (0x02<<0),  /* DAC output */
    OPAMP_OP0INMUX_MUXPOS_GND_gc = (0x03<<0),  /* Ground */
    OPAMP_OP0INMUX_MUXPOS_VDDDIV2_gc = (0x04<<0),  /* VDD/2 */
} OPAMP_OP0INMUX_MUXPOS_t;

/* Multiplexer Bottom select */
typedef enum OPAMP_OP0RESMUX_MUXBOT_enum
{
    OPAMP_OP0RESMUX_MUXBOT_OFF_gc = (0x00<<2),  /* Multiplexer off */
    OPAMP_OP0RESMUX_MUXBOT_INP_gc = (0x01<<2),  /* Positive input pin for OPn */
    OPAMP_OP0RESMUX_MUXBOT_INN_gc = (0x02<<2),  /* Negative input pin for OPn */
    OPAMP_OP0RESMUX_MUXBOT_DAC_gc = (0x03<<2),  /* DAC output */
    OPAMP_OP0RESMUX_MUXBOT_LINKOUT_gc = (0x04<<2),  /* Link OP[n-1] output */
    OPAMP_OP0RESMUX_MUXBOT_GND_gc = (0x05<<2),  /* Ground */
} OPAMP_OP0RESMUX_MUXBOT_t;

/* Multiplexer Top select */
typedef enum OPAMP_OP0RESMUX_MUXTOP_enum
{
    OPAMP_OP0RESMUX_MUXTOP_OFF_gc = (0x00<<0),  /* Multiplexer off */
    OPAMP_OP0RESMUX_MUXTOP_OUT_gc = (0x01<<0),  /* OPn output */
    OPAMP_OP0RESMUX_MUXTOP_VDD_gc = (0x02<<0),  /* VDD */
} OPAMP_OP0RESMUX_MUXTOP_t;

/* Multiplexer Wiper selector select */
typedef enum OPAMP_OP0RESMUX_MUXWIP_enum
{
    OPAMP_OP0RESMUX_MUXWIP_WIP0_gc = (0x00<<5),  /* R1 = 15R, R2 = 1R, R2/R1 = 0.07 */
    OPAMP_OP0RESMUX_MUXWIP_WIP1_gc = (0x01<<5),  /* R1 = 14R, R2 = 2R, R2/R1 = 0.14 */
    OPAMP_OP0RESMUX_MUXWIP_WIP2_gc = (0x02<<5),  /* R1 = 12R, R2 = 4R, R2/R1 = 0.33 */
    OPAMP_OP0RESMUX_MUXWIP_WIP3_gc = (0x03<<5),  /* R1 = 8R, R2 = 8R, R2/R1 = 1 */
    OPAMP_OP0RESMUX_MUXWIP_WIP4_gc = (0x04<<5),  /* R1 = 6R, R2 = 10R, R2/R1 = 1.67 */
    OPAMP_OP0RESMUX_MUXWIP_WIP5_gc = (0x05<<5),  /* R1 = 4R, R2 = 12R, R2/R1 = 3 */
    OPAMP_OP0RESMUX_MUXWIP_WIP6_gc = (0x06<<5),  /* R1 = 2R, R2 = 14R, R2/R1 = 7 */
    OPAMP_OP0RESMUX_MUXWIP_WIP7_gc = (0x07<<5),  /* R1 = 1R, R2 = 15R, R2/R1 = 15 */
} OPAMP_OP0RESMUX_MUXWIP_t;

/* Output Mode select */
typedef enum OPAMP_OP1CTRLA_OUTMODE_enum
{
    OPAMP_OP1CTRLA_OUTMODE_OFF_gc = (0x00<<2),  /* Output Driver Off */
    OPAMP_OP1CTRLA_OUTMODE_NORMAL_gc = (0x01<<2),  /* Output Driver in Normal Mode */
} OPAMP_OP1CTRLA_OUTMODE_t;

/* Negative Input Multiplexer select */
typedef enum OPAMP_OP1INMUX_MUXNEG_enum
{
    OPAMP_OP1INMUX_MUXNEG_INN_gc = (0x00<<4),  /* Negative input pin for OPn */
    OPAMP_OP1INMUX_MUXNEG_WIP_gc = (0x01<<4),  /* Wiper from OPn's resistor ladder */
    OPAMP_OP1INMUX_MUXNEG_OUT_gc = (0x02<<4),  /* OPn output (unity gain) */
    OPAMP_OP1INMUX_MUXNEG_DAC_gc = (0x03<<4),  /* DAC output */
} OPAMP_OP1INMUX_MUXNEG_t;

/* Positive Input Multiplexer select */
typedef enum OPAMP_OP1INMUX_MUXPOS_enum
{
    OPAMP_OP1INMUX_MUXPOS_INP_gc = (0x00<<0),  /* Positive input pin for OPn */
    OPAMP_OP1INMUX_MUXPOS_WIP_gc = (0x01<<0),  /* Wiper from OPn's resistor ladder */
    OPAMP_OP1INMUX_MUXPOS_DAC_gc = (0x02<<0),  /* DAC output */
    OPAMP_OP1INMUX_MUXPOS_GND_gc = (0x03<<0),  /* Ground */
    OPAMP_OP1INMUX_MUXPOS_VDDDIV2_gc = (0x04<<0),  /* VDD/2 */
    OPAMP_OP1INMUX_MUXPOS_LINKOUT_gc = (0x05<<0),  /* Output from OP0 */
} OPAMP_OP1INMUX_MUXPOS_t;

/* Multiplexer Bottom select */
typedef enum OPAMP_OP1RESMUX_MUXBOT_enum
{
    OPAMP_OP1RESMUX_MUXBOT_OFF_gc = (0x00<<2),  /* Multiplexer off */
    OPAMP_OP1RESMUX_MUXBOT_INP_gc = (0x01<<2),  /* Positive input pin for OPn */
    OPAMP_OP1RESMUX_MUXBOT_INN_gc = (0x02<<2),  /* Negative input pin for OPn */
    OPAMP_OP1RESMUX_MUXBOT_DAC_gc = (0x03<<2),  /* DAC output */
    OPAMP_OP1RESMUX_MUXBOT_LINKOUT_gc = (0x04<<2),  /* Link OP[n-1] output */
    OPAMP_OP1RESMUX_MUXBOT_GND_gc = (0x05<<2),  /* Ground */
} OPAMP_OP1RESMUX_MUXBOT_t;

/* Multiplexer Top select */
typedef enum OPAMP_OP1RESMUX_MUXTOP_enum
{
    OPAMP_OP1RESMUX_MUXTOP_OFF_gc = (0x00<<0),  /* Multiplexer off */
    OPAMP_OP1RESMUX_MUXTOP_OUT_gc = (0x01<<0),  /* OPn output */
    OPAMP_OP1RESMUX_MUXTOP_VDD_gc = (0x02<<0),  /* VDD */
} OPAMP_OP1RESMUX_MUXTOP_t;

/* Multiplexer Wiper selector select */
typedef enum OPAMP_OP1RESMUX_MUXWIP_enum
{
    OPAMP_OP1RESMUX_MUXWIP_WIP0_gc = (0x00<<5),  /* R1 = 15R, R2 = 1R, R2/R1 = 0.07 */
    OPAMP_OP1RESMUX_MUXWIP_WIP1_gc = (0x01<<5),  /* R1 = 14R, R2 = 2R, R2/R1 = 0.14 */
    OPAMP_OP1RESMUX_MUXWIP_WIP2_gc = (0x02<<5),  /* R1 = 12R, R2 = 4R, R2/R1 = 0.33 */
    OPAMP_OP1RESMUX_MUXWIP_WIP3_gc = (0x03<<5),  /* R1 = 8R, R2 = 8R, R2/R1 = 1 */
    OPAMP_OP1RESMUX_MUXWIP_WIP4_gc = (0x04<<5),  /* R1 = 6R, R2 = 10R, R2/R1 = 1.67 */
    OPAMP_OP1RESMUX_MUXWIP_WIP5_gc = (0x05<<5),  /* R1 = 4R, R2 = 12R, R2/R1 = 3 */
    OPAMP_OP1RESMUX_MUXWIP_WIP6_gc = (0x06<<5),  /* R1 = 2R, R2 = 14R, R2/R1 = 7 */
    OPAMP_OP1RESMUX_MUXWIP_WIP7_gc = (0x07<<5),  /* R1 = 1R, R2 = 15R, R2/R1 = 15 */
} OPAMP_OP1RESMUX_MUXWIP_t;

/* Output Mode select */
typedef enum OPAMP_OP2CTRLA_OUTMODE_enum
{
    OPAMP_OP2CTRLA_OUTMODE_OFF_gc = (0x00<<2),  /* Output Driver Off */
    OPAMP_OP2CTRLA_OUTMODE_NORMAL_gc = (0x01<<2),  /* Output Driver in Normal Mode */
} OPAMP_OP2CTRLA_OUTMODE_t;

/* Negative Input Multiplexer select */
typedef enum OPAMP_OP2INMUX_MUXNEG_enum
{
    OPAMP_OP2INMUX_MUXNEG_INN_gc = (0x00<<4),  /* Negative input pin for OPn */
    OPAMP_OP2INMUX_MUXNEG_WIP_gc = (0x01<<4),  /* Wiper from OPn's resistor ladder */
    OPAMP_OP2INMUX_MUXNEG_OUT_gc = (0x02<<4),  /* OPn output (unity gain) */
    OPAMP_OP2INMUX_MUXNEG_DAC_gc = (0x03<<4),  /* DAC output */
} OPAMP_OP2INMUX_MUXNEG_t;

/* Positive Input Multiplexer select */
typedef enum OPAMP_OP2INMUX_MUXPOS_enum
{
    OPAMP_OP2INMUX_MUXPOS_INP_gc = (0x00<<0),  /* Positive input pin for OPn */
    OPAMP_OP2INMUX_MUXPOS_WIP_gc = (0x01<<0),  /* Wiper from OPn's resistor ladder */
    OPAMP_OP2INMUX_MUXPOS_DAC_gc = (0x02<<0),  /* DAC output */
    OPAMP_OP2INMUX_MUXPOS_GND_gc = (0x03<<0),  /* Ground */
    OPAMP_OP2INMUX_MUXPOS_VDDDIV2_gc = (0x04<<0),  /* VDD/2 */
    OPAMP_OP2INMUX_MUXPOS_LINKOUT_gc = (0x05<<0),  /* Output from OP1 */
    OPAMP_OP2INMUX_MUXPOS_LINKWIP_gc = (0x06<<0),  /* Wiper from OP0's resistor ladder */
} OPAMP_OP2INMUX_MUXPOS_t;

/* Multiplexer Bottom select */
typedef enum OPAMP_OP2RESMUX_MUXBOT_enum
{
    OPAMP_OP2RESMUX_MUXBOT_OFF_gc = (0x00<<2),  /* Multiplexer off */
    OPAMP_OP2RESMUX_MUXBOT_INP_gc = (0x01<<2),  /* Positive input pin for OPn */
    OPAMP_OP2RESMUX_MUXBOT_INN_gc = (0x02<<2),  /* Negative input pin for OPn */
    OPAMP_OP2RESMUX_MUXBOT_DAC_gc = (0x03<<2),  /* DAC output */
    OPAMP_OP2RESMUX_MUXBOT_LINKOUT_gc = (0x04<<2),  /* Link OP[n-1] output */
    OPAMP_OP2RESMUX_MUXBOT_GND_gc = (0x05<<2),  /* Ground */
} OPAMP_OP2RESMUX_MUXBOT_t;

/* Multiplexer Top select */
typedef enum OPAMP_OP2RESMUX_MUXTOP_enum
{
    OPAMP_OP2RESMUX_MUXTOP_OFF_gc = (0x00<<0),  /* Multiplexer off */
    OPAMP_OP2RESMUX_MUXTOP_OUT_gc = (0x01<<0),  /* OPn output */
    OPAMP_OP2RESMUX_MUXTOP_VDD_gc = (0x02<<0),  /* VDD */
} OPAMP_OP2RESMUX_MUXTOP_t;

/* Multiplexer Wiper selector select */
typedef enum OPAMP_OP2RESMUX_MUXWIP_enum
{
    OPAMP_OP2RESMUX_MUXWIP_WIP0_gc = (0x00<<5),  /* R1 = 15R, R2 = 1R, R2/R1 = 0.07 */
    OPAMP_OP2RESMUX_MUXWIP_WIP1_gc = (0x01<<5),  /* R1 = 14R, R2 = 2R, R2/R1 = 0.14 */
    OPAMP_OP2RESMUX_MUXWIP_WIP2_gc = (0x02<<5),  /* R1 = 12R, R2 = 4R, R2/R1 = 0.33 */
    OPAMP_OP2RESMUX_MUXWIP_WIP3_gc = (0x03<<5),  /* R1 = 8R, R2 = 8R, R2/R1 = 1 */
    OPAMP_OP2RESMUX_MUXWIP_WIP4_gc = (0x04<<5),  /* R1 = 6R, R2 = 10R, R2/R1 = 1.67 */
    OPAMP_OP2RESMUX_MUXWIP_WIP5_gc = (0x05<<5),  /* R1 = 4R, R2 = 12R, R2/R1 = 3 */
    OPAMP_OP2RESMUX_MUXWIP_WIP6_gc = (0x06<<5),  /* R1 = 2R, R2 = 14R, R2/R1 = 7 */
    OPAMP_OP2RESMUX_MUXWIP_WIP7_gc = (0x07<<5),  /* R1 = 1R, R2 = 15R, R2/R1 = 15 */
} OPAMP_OP2RESMUX_MUXWIP_t;

/* Input Range Select select */
typedef enum OPAMP_PWRCTRL_IRSEL_enum
{
    OPAMP_PWRCTRL_IRSEL_FULL_gc = (0x00<<0),  /* Full Input Range */
    OPAMP_PWRCTRL_IRSEL_REDUCED_gc = (0x01<<0),  /* Reduced Input Range */
} OPAMP_PWRCTRL_IRSEL_t;


