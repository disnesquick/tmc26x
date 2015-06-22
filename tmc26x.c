#include <stdint.h>
#include <stdio.h>
#include "tmc26x.h"
#include "tmc26x_regs.h"

/* Calculate the value for CS according to the hardware RSense resistance
** and the firmware VSense value (305 or 165)
**
** current_mA  - desired peak current in milliamps
** RSense_mOhm - The hardware resistance value in milliohms
** VSense      - The firmware voltage value
**
** returns the value to be provided to tmc26xSetStallGuard or -1 if the current is too high
*/
static int8_t tmc26xCalcCSValue(uint16_t RMSCurrent_mA, uint16_t RSense_mOhm, uint16_t VSense) {
	uint32_t tmp = RSense_mOhm;
	uint32_t remnant;
	
	tmp *= (uint32_t)RMSCurrent_mA;
	tmp *= 1414;
	
	tmp /= VSense;

	remnant = tmp % (1000000/32);
	tmp /= (1000000/32);
	if (remnant >= (500000/32))
		tmp += 1;

	if (tmp > 32)
		return -1;

	return tmp;
}

#ifndef UNIT_TESTING
#include "util.h"
#include "tmc26x_arch.h"

/* Sends a command, expressed as a 32-bit number down the line to the TMC26x
** as an MSB 20-bit value
**
** command - The 32-bit expresed command to send
** 
** returns - 20-bit return value as specified in data-sheet section 6.5
*/
void tmc26xSendCommand(uint32_t command) {
	tmc26xSPIChipEnable();
	
	tmc26xSPITransceiveByte(BYTE2(command));
	tmc26xSPITransceiveByte(BYTE1(command));
	tmc26xSPITransceiveByte(BYTE0(command));

	tmc26xSPIChipDisable();
}

/* Reads the currently set readback value from the TMC26X chip, this should
** previously have been set by writing the register DRVCONF to the chip.
** note that this will nearly always be done with one of the helper functions
** defined for each of the readback types.
**
** config - Configuration structure (needed only for current regDRVCONF)
**
** returns - the 20-bit value (correctly shifted) read from the chip.
*/
uint32_t tmc26xReadback(TMC26XConfiguration* config) {
	uint32_t build = 0;

	tmc26xSPIChipEnable();
	
	build  = tmc26xSPITransceiveByte(BYTE2(config->regDRVCONF));
	build<<=8;
	build |= tmc26xSPITransceiveByte(BYTE1(config->regDRVCONF));
	build<<=8;
	build |= tmc26xSPITransceiveByte(BYTE0(config->regDRVCONF));
	build>>=4;
	tmc26xSPIChipDisable();
	return build;
}

#else
uint32_t tmc26xSendCommand(uint32_t command) {
	printf("%05X\n",command);
	return 0;
}

#endif

/* Commits the configuration structure to the TMC26X chip itself
**
** config - Configuration structure
**
** returns TMC26X_SUCCESS if valid otherwise TMC26X_INVALID_CONFIG
*/
int tmc26xCommitConfiguration(TMC26XConfiguration* config, int SGCSCONFFirst) {
	if (config->validity != 0xFFFFFFFF)
		return TMC26X_INVALID_CONFIG;

	if (SGCSCONFFirst == 1 && (config->dirty & TMC26X_DIRTY_BITMASK_SGCSCONF))
		tmc26xSendCommand(config->regSGCSCONF);
	
	if (config->dirty & TMC26X_DIRTY_BITMASK_DRVCONF)
		tmc26xSendCommand(config->regDRVCONF);

	if (SGCSCONFFirst == 0 && (config->dirty & TMC26X_DIRTY_BITMASK_SGCSCONF))
		tmc26xSendCommand(config->regSGCSCONF);

	if (config->dirty & TMC26X_DIRTY_BITMASK_DRVCTRL)
		tmc26xSendCommand(config->regDRVCTRL);
		
	if (config->dirty & TMC26X_DIRTY_BITMASK_CHOPCONF)
		tmc26xSendCommand(config->regCHOPCONF);
		
	if (config->dirty & TMC26X_DIRTY_BITMASK_SMARTEN)
		tmc26xSendCommand(config->regSMARTEN);
	
	config->dirty = 0;

	return TMC26X_SUCCESS;
}

/* Sets the maximum current based on the VSENSE, RSENSE and desired current
** This will calculate and commit to chip, the values for CS and VSENSE
** in SGCSCONF and DRVCONF registers respectively.
**
** config - Current configuration structure
**
** returns - TMC26X_SUCCESS if everything goes well, otherwise the
**           TMC26X_INVALID_VALUE if the current cannot be computer
**           or TMC26X_INVALID_CONFIG if there were other configuration
**           problems
*/
int tmc26xSetFullScaleCurrent(TMC26XConfiguration* config, uint16_t current_mA) {
	uint16_t VSense, oldVSense;
	int8_t currentSetting;

	// Check the old VSense value (no VSense assumed to be 305, since
	// no safety precaution needed)
	if (!(config->validity & TMC26X_VALID_BITMASK_DRVCONF_VSENSE))
		oldVSense = TMC26X_VSENSE_FULL;
	else if (config->regDRVCONF & TMC26X_DRVCONF_VSENSE_BITMASK)
		oldVSense = TMC26X_VSENSE_HALFISH;
	else
		oldVSense = TMC26X_VSENSE_FULL;
	
	// Calculate the new VSense value
	if (current_mA > (((uint32_t)TMC26X_VSENSE_HALFISH * 1000000) / ((uint32_t)RSENSE_VALUE * 1414)))
		VSense = TMC26X_VSENSE_FULL;
	else
		VSense = TMC26X_VSENSE_HALFISH;

	// Calculate the CS setting and check validity
	if ((currentSetting = tmc26xCalcCSValue(current_mA, RSENSE_VALUE, VSense)) < 0)
		return TMC26X_INVALID_VALUE;

	// Set the new current scale value
	if (tmc26xSGCSCONFSetCurrentScale(config, currentSetting) != TMC26X_SUCCESS)
		return TMC26X_INVALID_VALUE;

	// Set the new VSense value
	tmc26xDRVCONFSetMaximumRSenseVoltage(config, VSense);

	// Commit the new changes. For safety reasons, when scaling up to 305mV from 165mV
	// the new current setting should be entered into the device first to prevent
	// over-current (i.e. if old CS was 32, then going to 305 would be disastrous)
	if (oldVSense == TMC26X_VSENSE_HALFISH && VSense == TMC26X_VSENSE_FULL)
		return tmc26xCommitConfiguration(config, 1);
	else
		return tmc26xCommitConfiguration(config, 0);
}

/* Sets the maximum current based on the previously defined driving current
** stored in the configuration structure for convienience.
**
** config - Current configuration structure
**
** returns - see tmc26xSetFullScaleCurrent
*/
int tmc26xSetDrivingCurrent(TMC26XConfiguration* config) {
	return tmc26xSetFullScaleCurrent(config, config->drivingCurrent);
}

/* Sets the maximum current based on the previously defined standstill current
** stored in the configuration structure for convienience.
**
** config - Current configuration structure
**
** returns - see tmc26xSetFullScaleCurrent
*/
int tmc26xSetStationaryCurrent(TMC26XConfiguration* config) {
	return tmc26xSetFullScaleCurrent(config, config->stationaryCurrent);
}

#ifndef UNIT_TESTING
/* Read the stallguard value from the TMC chip over SPI. this will sync the
** current configuration structure if any of the dirty bits are set
**
** config - Current configuration structure
**
** returns - the stallguard value (10 bits)
*/
uint16_t tmc26xReadStallGuardValue(TMC26XConfiguration* config) {
	uint32_t tmp;

	//Check whether any values need to be written to the chip. If the current readback
	//value is not StalLguard then change this to the correct value.
	if ((tmc26xDRVCONFGetReadbackValue(config) != TMC26X_READBACK_STALLGUARD) || (config->dirty > 0)) {
		tmc26xDRVCONFSetReadbackValue(config, TMC26X_READBACK_STALLGUARD);
		tmc26xCommitConfiguration(config, 0);
	}
	tmp = tmc26xReadback(config);
	tmp >>= 10;

	return (uint16_t)tmp;
}

/* Read the microstep value from the TMC chip over SPI. this will sync the
** current configuration structure if any of the dirty bits are set
**
** config - Current configuration structure
**
** returns - the microstep value (8 bits)
*/
uint16_t tmc26xReadMicroStepValue(TMC26XConfiguration* config) {
	uint32_t tmp;

	//Check whether any values need to be written to the chip. If the current readback
	//value is not StalLguard then change this to the correct value.
	if ((tmc26xDRVCONFGetReadbackValue(config) != TMC26X_READBACK_MICROSTEP) || (config->dirty > 0)) {
		tmc26xDRVCONFSetReadbackValue(config, TMC26X_READBACK_MICROSTEP);
		tmc26xCommitConfiguration(config, 0);
	}
	tmp = tmc26xReadback(config);
	tmp >>= 10;

	return (uint16_t)tmp;
}

/* Read the coolstep value from the TMC chip over SPI. this will sync the
** current configuration structure if any of the dirty bits are set
**
** config - Current configuration structure
**
** returns - the coolstep value (8 bits)
*/
uint16_t tmc26xReadCoolStepValue(TMC26XConfiguration* config) {
	uint32_t tmp;

	//Check whether any values need to be written to the chip. If the current readback
	//value is not StalLguard then change this to the correct value.
	if ((tmc26xDRVCONFGetReadbackValue(config) != TMC26X_READBACK_COOLSTEP) || (config->dirty > 0)) {
		tmc26xDRVCONFSetReadbackValue(config, TMC26X_READBACK_COOLSTEP);
		tmc26xCommitConfiguration(config, 0);
	}
	tmp = tmc26xReadback(config);
	tmp >>= 10;

	return (uint16_t)tmp&0x1F;
}

/* Read the raw return from the TMC chip over SPI. this will sync the
** current configuration structure if any of the dirty bits are set
**
** config - Current configuration structure
**
** returns - the stallguard value (10 bits)
*/
uint32_t tmc26xReadRaw(TMC26XConfiguration* config) {
	uint32_t tmp;

	tmp = tmc26xReadback(config);
	tmp >>= 4;

	return (uint32_t)tmp;
}
#endif

