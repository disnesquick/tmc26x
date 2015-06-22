// Structure for configuration
typedef struct {
	uint32_t regDRVCTRL;
	uint32_t regCHOPCONF;
	uint32_t regSMARTEN;
	uint32_t regSGCSCONF;
	uint32_t regDRVCONF;
	uint8_t dirty;
	uint32_t validity;
	uint16_t stationaryCurrent;
	uint16_t drivingCurrent;
} TMC26XConfiguration;


enum {
	TMC26X_DIRTY_BITMASK_DRVCTRL  = 1,
	TMC26X_DIRTY_BITMASK_CHOPCONF = 2,
	TMC26X_DIRTY_BITMASK_SMARTEN  = 4,
	TMC26X_DIRTY_BITMASK_SGCSCONF = 8,
	TMC26X_DIRTY_BITMASK_DRVCONF  = 16
};


// Structure for defining profiles on the motor
typedef struct {
	int profileID;
	uint8_t stepInterpolation;
	uint8_t doubleEdge;
	uint16_t microStepResolution;
	uint8_t blankingTime;
	uint8_t randomTOff;
	uint8_t timeOff;
	uint8_t hysteresisDecrement;
	uint8_t hysteresisStart;
	int8_t hysteresisEnd;
	uint8_t minCoolStepCurrent;
	uint8_t currentDecSpeed;
	uint8_t highCoolStepThreshold;
	uint8_t currentIncSize;
	uint8_t lowCoolStepThreshold;
	uint8_t stallGuardFilter;
	int8_t stallGuardThreshold;
	uint8_t testMode;
	uint8_t slopeControlHigh;
	uint8_t slopeControlLow;
	uint8_t groundShortProtection;
	uint8_t groundShortTimer;
	uint16_t highCurrent;
	uint16_t lowCurrent;
} TMC26XProfileStepDirSpreadCycle;


enum {
	TMC26X_SUCCESS = 0,
	TMC26X_INVALID_MODE = -1,
	TMC26X_INVALID_VALUE = -2,
	TMC26X_INVALID_CONFIG = -3,
	TMC26X_INVALID_PROFILE = -4
};


enum {
	MOTOR_LG_23HS7430 = 1,
	MOTOR_LG_23HS0420,
	MOTOR_NT_STM5918M1008_A,
	MOTOR_ZA_SY42STH47_1684A
};


void TMC26XConfiguration_Init(TMC26XConfiguration* config);
int tmc26xCommitConfiguration(TMC26XConfiguration* config, int SGCSCONFFirst);
int tmc26xSetFullScaleCurrent(TMC26XConfiguration* config, uint16_t current_mA);
int tmc26xSetDrivingCurrent(TMC26XConfiguration* config);
int tmc26xSetStationaryCurrent(TMC26XConfiguration* config);
int initializeTMC26XWithProfile(TMC26XConfiguration* config, int motorProfile);
uint32_t tmc26xReadBack(TMC26XConfiguration* config);
uint16_t tmc26xReadStallGuardValue(TMC26XConfiguration* config);
uint16_t tmc26xReadMicroStepValue(TMC26XConfiguration* config);
uint16_t tmc26xReadCoolStepValue(TMC26XConfiguration* config);
uint32_t tmc26xReadRaw(TMC26XConfiguration* config);
