enum {
	TMC26X_DRVCONF_DRIVEMODE_BITMASK = 1 << 7,
	TMC26X_CHOPCONF_CHOPMODE_BITMASK = 1 << 14,
	TMC26X_DRVCONF_VSENSE_BITMASK = 1 << 6
};

enum {
	TMC26X_DRVCTRL_ADDRESS = 0x00000,
	TMC26X_CHOPCONF_ADDRESS = 0x80000,
	TMC26X_SMARTEN_ADDRESS = 0xA0000,
	TMC26X_SGCSCONF_ADDRESS = 0xC0000,
	TMC26X_DRVCONF_ADDRESS = 0xE0000
};

enum {
	TMC26X_HALF_CURRENT = 0,
	TMC26X_QUARTER_CURRENT = 1
};

enum {
	TMC26X_VSENSE_FULL = 305,
	TMC26X_VSENSE_HALFISH = 165
};

enum {
	TMC26X_SPREADCYCLE = 0,
	TMC26X_FASTDECAY = 1
};

enum {
	TMC26X_STEPDIR = 0,
	TMC26X_SPI = 1
};

enum {
	TMC26X_DISABLE = 0,
	TMC26X_ENABLE = 1
};
//Prime numbers to stop people kludging this with literals
enum {
	TMC26X_MINIMUM = 29,
	TMC26X_MEDIUM = 31,
	TMC26X_MAXIMUM = 37,
	TMC26X_MINIMUM_TEMPERATURE_COMPENSATION = 41,
	TMC26X_MEDIUM_TEMPERATURE_COMPENSATION = 43
};

//Prime numbers to stop people kludging this with literals
enum {
	TMC26X_READBACK_MICROSTEP = 29,
	TMC26X_READBACK_STALLGUARD = 31,
	TMC26X_READBACK_COOLSTEP = 37
};

#define TMC26X_VALID_BITMASK_DRVCTRL_STEP_INTERPOLATION   ((uint32_t)1<<0)
#define TMC26X_VALID_BITMASK_DRVCTRL_DOUBLE_STEP          ((uint32_t)1<<1)
#define TMC26X_VALID_BITMASK_DRVCTRL_MICROSTEP_RESOLUTION ((uint32_t)1<<2)
#define TMC26X_VALID_BITMASK_DRVCTRL_BIT3_ALWAYS_ONE      ((uint32_t)1<<3)
#define TMC26X_VALID_BITMASK_DRVCTRL_POLARITY_A           ((uint32_t)1<<0)
#define TMC26X_VALID_BITMASK_DRVCTRL_POLARITY_B           ((uint32_t)1<<1)
#define TMC26X_VALID_BITMASK_DRVCTRL_CURRENT_A            ((uint32_t)1<<2)
#define TMC26X_VALID_BITMASK_DRVCTRL_CURRENT_B            ((uint32_t)1<<3)
#define TMC26X_VALID_BITMASK_CHOPCONF_BLANKING_TIME       ((uint32_t)1<<4)
#define TMC26X_VALID_BITMASK_CHOPCONF_CHOPMODE            ((uint32_t)1<<5)
#define TMC26X_VALID_BITMASK_CHOPCONF_RANDOM_TOFF         ((uint32_t)1<<6)
#define TMC26X_VALID_BITMASK_CHOPCONF_OFF_TIME            ((uint32_t)1<<7)
#define TMC26X_VALID_BITMASK_CHOPCONF_FASTDECAY_MODE      ((uint32_t)1<<8)
#define TMC26X_VALID_BITMASK_CHOPCONF_SINE_OFFSET         ((uint32_t)1<<9)
#define TMC26X_VALID_BITMASK_CHOPCONF_FASTDECAY_TIME      ((uint32_t)1<<10)
#define TMC26X_VALID_BITMASK_CHOPCONF_HYSTERESIS_DEC      ((uint32_t)1<<8)
#define TMC26X_VALID_BITMASK_CHOPCONF_HYSTERESIS_START    ((uint32_t)1<<9)
#define TMC26X_VALID_BITMASK_CHOPCONF_HYSTERESIS_END      ((uint32_t)1<<10)
#define TMC26X_VALID_BITMASK_SMARTEN_MIN_COOLSTEP         ((uint32_t)1<<11)
#define TMC26X_VALID_BITMASK_SMARTEN_CURRENT_DEC_SPEED    ((uint32_t)1<<12)
#define TMC26X_VALID_BITMASK_SMARTEN_HIGH_COOLSTEP_THRESH ((uint32_t)1<<13)
#define TMC26X_VALID_BITMASK_SMARTEN_CURRENT_INC_SIZE     ((uint32_t)1<<14)
#define TMC26X_VALID_BITMASK_SMARTEN_LOW_COOLSTEP_THRESH  ((uint32_t)1<<15)
#define TMC26X_VALID_BITMASK_SGCSCONF_FILTER              ((uint32_t)1<<16)
#define TMC26X_VALID_BITMASK_SGCSCONF_THRESHOLD           ((uint32_t)1<<17)
#define TMC26X_VALID_BITMASK_SGCSCONF_CURRENT_SCALE       ((uint32_t)1<<18)
#define TMC26X_VALID_BITMASK_DRVCONF_TEST                 ((uint32_t)1<<19)
#define TMC26X_VALID_BITMASK_DRVCONF_SLOPE_HIGH           ((uint32_t)1<<20)
#define TMC26X_VALID_BITMASK_DRVCONF_SLOPE_LOW            ((uint32_t)1<<21)
#define TMC26X_VALID_BITMASK_DRVCONF_GND_SHORT_PROTECT    ((uint32_t)1<<22)
#define TMC26X_VALID_BITMASK_DRVCONF_GND_SHORT_TIMER      ((uint32_t)1<<23)
#define TMC26X_VALID_BITMASK_DRVCONF_DRIVEMODE            ((uint32_t)1<<24)
#define TMC26X_VALID_BITMASK_DRVCONF_VSENSE               ((uint32_t)1<<25)
#define TMC26X_VALID_BITMASK_DRVCONF_READBACK             ((uint32_t)1<<26)
#define TMC26X_VALID_BITMASK_DRVCONF_END_BIT              ((uint32_t)1<<27)

int tmc26xDRVCTRLSetStepInterpolation(TMC26XConfiguration* config, uint8_t value);
int tmc26xDRVCTRLSetDoubleEdge(TMC26XConfiguration* config, uint8_t value);
int tmc26xDRVCTRLSetMicrostepResolution(TMC26XConfiguration* config, int16_t value);
int tmc26xDRVCTRLSetPolarityA(TMC26XConfiguration* config, int8_t value);
int tmc26xDRVCTRLSetPolarityB(TMC26XConfiguration* config, int8_t value);
int tmc26xDRVCTRLSetCurrentA(TMC26XConfiguration* config, uint8_t value);
int tmc26xDRVCTRLSetCurrentB(TMC26XConfiguration* config, uint8_t value);
int tmc26xCHOPCONFSetChopperMode(TMC26XConfiguration* config, uint8_t value);
int tmc26xCHOPCONFSetBlankingTime(TMC26XConfiguration* config, uint8_t value);
int tmc26xCHOPCONFSetRandomTOff(TMC26XConfiguration* config, uint8_t value);
int tmc26xCHOPCONFSetTimeOff(TMC26XConfiguration* config, uint8_t value) ;
int tmc26xCHOPCONFSetHysteresisDecrement(TMC26XConfiguration* config, uint8_t value);
int tmc26xCHOPCONFSetHysteresisStart(TMC26XConfiguration* config, uint8_t value);
int tmc26xCHOPCONFSetHysteresisEnd(TMC26XConfiguration* config, int8_t value);
int tmc26xCHOPCONFSetFastDecayMode(TMC26XConfiguration* config, uint8_t value);
int tmc26xCHOPCONFSetSineOffset(TMC26XConfiguration* config, int8_t value);
int tmc26xCHOPCONFSetFastDecayTime(TMC26XConfiguration* config, uint8_t value);
int tmc26xSMARTENSetMinCoolStepCurrent(TMC26XConfiguration* config, uint8_t value);
int tmc26xSMARTENSetCurrentDecSpeed(TMC26XConfiguration* config, uint8_t value);
int tmc26xSMARTENSetHighCoolStepThreshold(TMC26XConfiguration* config, uint8_t value);
int tmc26xSMARTENSetCurrentIncSize(TMC26XConfiguration* config, uint8_t value);
int tmc26xSMARTENSetLowCoolStepThreshold(TMC26XConfiguration* config, uint8_t value);
int tmc26xSGCSCONFSetStallGuardFilter(TMC26XConfiguration* config, uint8_t value);
int tmc26xSGCSCONFSetStallGuardThreshold(TMC26XConfiguration* config, int8_t value);
int tmc26xSGCSCONFSetCurrentScale(TMC26XConfiguration* config, uint8_t value);
int tmc26xDRVCONFSetTestMode(TMC26XConfiguration* config, uint8_t value);
int tmc26xDRVCONFSetSlopeControlHigh(TMC26XConfiguration* config, uint8_t value);
int tmc26xDRVCONFSetSlopeControlLow(TMC26XConfiguration* config, uint8_t value);
int tmc26xDRVCONFSetGroundShortProtection(TMC26XConfiguration* config, uint8_t value);
int tmc26xDRVCONFSetGroundShortTimer(TMC26XConfiguration* config, uint8_t value);
int tmc26xDRVCONFSetDriveMode(TMC26XConfiguration* config, uint8_t value);
int tmc26xDRVCONFSetMaximumRSenseVoltage(TMC26XConfiguration* config, uint16_t value);
int tmc26xDRVCONFSetReadbackValue(TMC26XConfiguration* config, uint8_t value);

int8_t tmc26xDRVCONFGetReadbackValue(TMC26XConfiguration* config);
void TMC26XConfiguration_Init(TMC26XConfiguration* config);
