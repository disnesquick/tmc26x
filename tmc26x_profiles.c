#include <stdint.h>
#include <stdio.h>
#include "tmc26x.h"
#include "tmc26x_regs.h"

//TODO: Need a way of including all kinds of profiles in this array

TMC26XProfileStepDirSpreadCycle profiles[] = {
	// Long's 23HS7430
	// http://www.longs-motor.com/productinfo/detail_12_25_114.aspx
	{    .profileID             = MOTOR_LG_23HS7430
	,    .stepInterpolation     = TMC26X_DISABLE
	,    .doubleEdge            = TMC26X_DISABLE
	,    .microStepResolution   = 256
	,    .blankingTime          = 16
	,    .randomTOff            = TMC26X_ENABLE
	,    .timeOff               = 4
	,    .hysteresisDecrement   = 16
	,    .hysteresisStart       = 5
	,    .hysteresisEnd         = 0
	,    .minCoolStepCurrent    = TMC26X_QUARTER_CURRENT
	,    .currentDecSpeed       = 32
	,    .highCoolStepThreshold = 4
	,    .currentIncSize        = 1
	,    .lowCoolStepThreshold  = TMC26X_DISABLE
	,    .stallGuardFilter      = TMC26X_DISABLE
	,    .stallGuardThreshold   = 16
	,    .testMode              = TMC26X_DISABLE
	,    .slopeControlHigh      = TMC26X_MINIMUM
	,    .slopeControlLow       = TMC26X_MINIMUM
	,    .groundShortProtection = TMC26X_ENABLE
	,    .groundShortTimer      = 32
#ifdef MOTORDRIVER_TMC262
	,    .highCurrent           = 2000
	,    .lowCurrent            = 250
#endif
#ifdef MOTORDRIVER_TMC261
	,    .highCurrent           = 1150
	,    .lowCurrent            = 250
#endif
	// Long's 23HS0420
	// http://www.longs-motor.com/productinfo/detail_12_25_114.aspx
	},{  .profileID             = MOTOR_LG_23HS0420
	,    .stepInterpolation     = TMC26X_DISABLE
	,    .doubleEdge            = TMC26X_DISABLE
	,    .microStepResolution   = 256
	,    .blankingTime          = 24
	,    .randomTOff            = TMC26X_ENABLE
	,    .timeOff               = 2
	,    .hysteresisDecrement   = 16
	,    .hysteresisStart       = 3
	,    .hysteresisEnd         = 0
	,    .minCoolStepCurrent    = TMC26X_QUARTER_CURRENT
	,    .currentDecSpeed       = 32
	,    .highCoolStepThreshold = 4
	,    .currentIncSize        = 1
	,    .lowCoolStepThreshold  = TMC26X_DISABLE
	,    .stallGuardFilter      = TMC26X_DISABLE
	,    .stallGuardThreshold   = 4
	,    .testMode              = TMC26X_DISABLE
	,    .slopeControlHigh      = TMC26X_MINIMUM
	,    .slopeControlLow       = TMC26X_MINIMUM
	,    .groundShortProtection = TMC26X_ENABLE
	,    .groundShortTimer      = 32
#ifdef MOTORDRIVER_TMC262
	,    .highCurrent           = 2000
	,    .lowCurrent            = 400
#endif
#ifdef MOTORDRIVER_TMC261
	,    .highCurrent           = 950
	,    .lowCurrent            = 950
#endif
	// NanoTec STM5918M1008-A (run in parallel coil mode)
	// http://en.nanotec.com/fileadmin/files/Datenblaetter/Schrittmotoren/ST5918/ST5918M1008-A.pdf
	},{  .profileID             = MOTOR_NT_STM5918M1008_A
	,    .stepInterpolation     = TMC26X_DISABLE
	,    .doubleEdge            = TMC26X_DISABLE
	,    .microStepResolution   = 256
	,    .blankingTime          = 24
	,    .randomTOff            = TMC26X_ENABLE
	,    .timeOff               = 2
	,    .hysteresisDecrement   = 16
	,    .hysteresisStart       = 5
	,    .hysteresisEnd         = 0
	,    .minCoolStepCurrent    = TMC26X_QUARTER_CURRENT
	,    .currentDecSpeed       = 32
	,    .highCoolStepThreshold = 4
	,    .currentIncSize        = 1
	,    .lowCoolStepThreshold  = TMC26X_DISABLE
	,    .stallGuardFilter      = TMC26X_DISABLE
	,    .stallGuardThreshold   = 4
	,    .testMode              = TMC26X_DISABLE
	,    .slopeControlHigh      = TMC26X_MINIMUM
	,    .slopeControlLow       = TMC26X_MINIMUM
	,    .groundShortProtection = TMC26X_ENABLE
	,    .groundShortTimer      = 32
#ifdef MOTORDRIVER_TMC262
	,    .highCurrent           = 1410
	,    .lowCurrent            = 400
#endif
#ifdef MOTORDRIVER_TMC261
	,    .highCurrent           = 1200
	,    .lowCurrent            = 250
#endif
	// Zapp Automation SY42STH47-1684A
	// http://www.zappautomation.co.uk/electrical-products/stepper-motors/nema-17-stepper-motors/sy42sth47-1684a-high-torque-hybrid-stepper-motors.html
	},{  .profileID             = MOTOR_ZA_SY42STH47_1684A
	,    .stepInterpolation     = TMC26X_DISABLE
	,    .doubleEdge            = TMC26X_DISABLE
	,    .microStepResolution   = 256
	,    .blankingTime          = 16
	,    .randomTOff            = TMC26X_DISABLE
	,    .timeOff               = 2
	,    .hysteresisDecrement   = 16
	,    .hysteresisStart       = 5
	,    .hysteresisEnd         = 0
	,    .minCoolStepCurrent    = TMC26X_QUARTER_CURRENT
	,    .currentDecSpeed       = 32
	,    .highCoolStepThreshold = 4
	,    .currentIncSize        = 1
	,    .lowCoolStepThreshold  = TMC26X_DISABLE
	,    .stallGuardFilter      = TMC26X_DISABLE
	,    .stallGuardThreshold   = 24
	,    .testMode              = TMC26X_DISABLE
	,    .slopeControlHigh      = TMC26X_MINIMUM
	,    .slopeControlLow       = TMC26X_MINIMUM
	,    .groundShortProtection = TMC26X_ENABLE
	,    .groundShortTimer      = 32
#ifdef MOTORDRIVER_TMC262
	,    .highCurrent           = 1280
	,    .lowCurrent            = 200
#endif
#ifdef MOTORDRIVER_TMC261
	,    .highCurrent           = 1200
	,    .lowCurrent            = 100
#endif
	}
};	

/* This function applies a profile structure, of the form DRIVEMODE: Step/Dir, CHOPPER: SpreadCycle, to
** a configuration structure, then syncs this profile to the TMC261/262 chip.
**
** config  - Configuration profile to apply the in-built profile to.
** profile - In-built profile of the form TMC26XProfileStepDirSpreadCycle as defined above
**
** returns - TMC26X_SUCCESS if succesful, or an error code (see tmc26xSetFullScaleCurrent)
*/
int tmc26xSetProfileStepDirSpreadCycle(TMC26XConfiguration* config, TMC26XProfileStepDirSpreadCycle* profile) {
	// Set-up the two mode bits first
	tmc26xCHOPCONFSetChopperMode(config, TMC26X_SPREADCYCLE);
	tmc26xDRVCONFSetDriveMode(config, TMC26X_STEPDIR);

	// Settings for DRVCTRL register
	tmc26xDRVCTRLSetStepInterpolation(config, profile->stepInterpolation);
	tmc26xDRVCTRLSetDoubleEdge(config, profile->doubleEdge);
	tmc26xDRVCTRLSetMicrostepResolution(config, profile->microStepResolution);

	// Settings for CHOPCONF register
	tmc26xCHOPCONFSetBlankingTime(config, profile->blankingTime);
	tmc26xCHOPCONFSetRandomTOff(config, profile->randomTOff);
	tmc26xCHOPCONFSetTimeOff(config, profile->timeOff);
	tmc26xCHOPCONFSetHysteresisDecrement(config, profile->hysteresisDecrement);
	tmc26xCHOPCONFSetHysteresisStart(config, profile->hysteresisStart);
	tmc26xCHOPCONFSetHysteresisEnd(config, profile->hysteresisEnd);

	// Settings for SGCSCONF register (except current)
	tmc26xSGCSCONFSetStallGuardFilter(config, profile->stallGuardFilter);
	tmc26xSGCSCONFSetStallGuardThreshold(config, profile->stallGuardThreshold);
	
	// Settings for SMARTEN register
	tmc26xSMARTENSetMinCoolStepCurrent(config, profile->minCoolStepCurrent);
	tmc26xSMARTENSetCurrentDecSpeed(config, profile->currentDecSpeed);
	tmc26xSMARTENSetHighCoolStepThreshold(config, profile->highCoolStepThreshold);
	tmc26xSMARTENSetCurrentIncSize(config, profile->currentIncSize);
	tmc26xSMARTENSetLowCoolStepThreshold(config, profile->lowCoolStepThreshold);

	// Settings for DRVCONF register
	tmc26xDRVCONFSetTestMode(config, profile->testMode);
	tmc26xDRVCONFSetSlopeControlHigh(config, profile->slopeControlHigh);
	tmc26xDRVCONFSetSlopeControlLow(config, profile->slopeControlLow);
	tmc26xDRVCONFSetGroundShortProtection(config, profile->groundShortProtection);
	tmc26xDRVCONFSetGroundShortTimer(config, profile->groundShortTimer);
	tmc26xDRVCONFSetReadbackValue(config, TMC26X_READBACK_STALLGUARD);

	// This takes care of DRVCONF.RSense and SGCSCONF.CS and commits the
	// new profile
	config->drivingCurrent = profile->highCurrent;
	config->stationaryCurrent = profile->lowCurrent;
	return tmc26xSetFullScaleCurrent(config, profile->highCurrent);
}

/* This function searches through the list of profiles for the profile
** in question and then tries to apply it to the config and TMC chip.
**
** config       - The Configuration profile to apply the profile to
** motorProfile - Constant integer  to match to the .profileID of the profile
**                structures.
**
** returns      - TMC26X_SUCCESS if succesful, otherwise TMC26X_SUCCESS
*/
int initializeTMC26XWithProfile(TMC26XConfiguration* config, int motorProfile) {
	int i;
	//Basic configuration
	for (i=0; i<sizeof(profiles) / sizeof(TMC26XProfileStepDirSpreadCycle); i++)
		if (profiles[i].profileID == motorProfile) {
			TMC26XConfiguration_Init(config);
			tmc26xSetProfileStepDirSpreadCycle(config, &profiles[i]);
			return TMC26X_SUCCESS;
		}

	return TMC26X_INVALID_PROFILE;
}

/* A few little functions that I wrote for unit testing, leaving them in here in 
** case they are useful at some point in the future.
*/

#ifdef UNIT_TESTING
void unitTestTMC26X(int motorProfile) {
	TMC26XConfiguration _config;
	TMC26XConfiguration* config = &_config;
	
	initializeTMC26XWithProfile(config, motorProfile);
}

int main(int argc, char** argv) {
	unitTestTMC26X( MOTOR_LG_23HS7430);

	return 1;
}
#endif

