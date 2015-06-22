#include <stdint.h>
#include "tmc26x.h"
#include "tmc26x_regs.h"

/* Helper function used to plug in a value, of a certain width, to a certain bit
** position. This is used to update registers.
**
** reg      - pointer to the 20-bit register (stored as a 32-bit
** bitPos   - bit position of the LSB of the value to write
** bitWidth - width, in bits, of the value to write
** value    - value to write with bits to write shifted to have LSB at 0
*/
static void alterRegisterValue(uint32_t* reg, int bitPos, int bitWidth, uint16_t value) {
	uint32_t mask;
	mask = 1;
	mask = ~(((mask << bitWidth) - 1) << bitPos);
	*reg &= mask;
	mask = 1;
	mask = (mask << bitWidth) - 1;
	mask &= value;
	mask <<= bitPos;
	*reg |= mask;
}

/* Helper function to retrieve a value, if a certain width, at a certain bit
** position, from a register value. This is used to test registers (primarily
** for testing mode settings.
**
** reg      - pointer to the 20-bit register (stored as a 32-bit
** bitPos   - bit position of the LSB of the value to read
** bitWidth - width, in bits, of the value to read
**
** returns  - Value from register shifted to have LSB at 0
*/
static uint16_t retrieveRegisterValue(uint32_t* reg, int bitPos, int bitWidth) {
	uint32_t mask;
	mask = 1;
	mask = ~(((mask << bitWidth) - 1) << bitPos);
	mask &= *reg;
	mask >>= bitPos;
	return (uint16_t)mask;
}

/* Initializes a TMC26XConfiguration structure (all zeroes, except for the register
** address bits
**
** config - configuration structure
*/
void TMC26XConfiguration_Init(TMC26XConfiguration* config) {
	config->regDRVCTRL =  TMC26X_DRVCTRL_ADDRESS;
	config->regCHOPCONF = TMC26X_CHOPCONF_ADDRESS;
	config->regSMARTEN =  TMC26X_SMARTEN_ADDRESS;
	config->regSGCSCONF = TMC26X_SGCSCONF_ADDRESS;
	config->regDRVCONF =  TMC26X_DRVCONF_ADDRESS;
	config->validity = (~(TMC26X_VALID_BITMASK_DRVCONF_END_BIT - 1)) | TMC26X_VALID_BITMASK_DRVCTRL_BIT3_ALWAYS_ONE;
	config->dirty = TMC26X_DIRTY_BITMASK_DRVCTRL | TMC26X_DIRTY_BITMASK_CHOPCONF | TMC26X_DIRTY_BITMASK_SMARTEN |
	                TMC26X_DIRTY_BITMASK_SGCSCONF | TMC26X_DIRTY_BITMASK_DRVCONF;
}


/* Sets the "Enable STEP interpolation" (bit 9) of the DRVCTRL register
** (page 19 TMC260 and TMC261 DATASHEET (Rev. 205 / 2012-NOV-05)
** Config structure must indicate Step/Dir mode
**
** config - Configuration structure
** value  - 0: Disable STEP pulse interpolation
**          1: Enable STEP pulse multiplication by 16
**
** returns TMC26X_SUCCESS if correct mode, otherwise TMC26X_INVALID_MODE
*/
int tmc26xDRVCTRLSetStepInterpolation(TMC26XConfiguration* config, uint8_t value) {
	//Check to see if in Step/Dir mode
	if (!(config->validity & TMC26X_VALID_BITMASK_DRVCONF_DRIVEMODE)
	 || (config->regDRVCONF & TMC26X_DRVCONF_DRIVEMODE_BITMASK))
		return TMC26X_INVALID_MODE;

	alterRegisterValue(&config->regDRVCTRL, 9, 1, value);
	config->dirty |= TMC26X_DIRTY_BITMASK_DRVCTRL;	
	config->validity |= TMC26X_VALID_BITMASK_DRVCTRL_STEP_INTERPOLATION;
	
	return TMC26X_SUCCESS;
}

/* Sets the "Enable double edge STEP pulses" (bit 8) of the DRVCTRL register
** (page 19 TMC260 and TMC261 DATASHEET (Rev. 205 / 2012-NOV-05)
** Config structure must indicate Step/Dir mode
**
** config - Configuration structure
** value  - 0: Rising STEP pulse edge is active, falling edge is inactive
**          1: Both rising and falling STEP pulse edges are active 
**
** returns TMC26X_SUCCESS if correct mode, otherwise TMC26X_INVALID_MODE
*/
int tmc26xDRVCTRLSetDoubleEdge(TMC26XConfiguration* config, uint8_t value) {
	//Check to see if in Step/Dir mode
	if (!(config->validity & TMC26X_VALID_BITMASK_DRVCONF_DRIVEMODE)
	 || (config->regDRVCONF & TMC26X_DRVCONF_DRIVEMODE_BITMASK))
		return TMC26X_INVALID_MODE;

	alterRegisterValue(&config->regDRVCTRL, 8, 1, value);
	config->dirty |= TMC26X_DIRTY_BITMASK_DRVCTRL;
	config->validity |= TMC26X_VALID_BITMASK_DRVCTRL_DOUBLE_STEP;
	
	return TMC26X_SUCCESS;
}

/* Sets the "Microstep resolution for STEP/DIR mode" (bit 0-3) of the DRVCTRL register
** (page 19 TMC260 and TMC261 DATASHEET (Rev. 205 / 2012-NOV-05)
** Config structure must indicate Step/Dir mode
**
** config - Configuration structure
** value - microsteps per step.
**
** returns TMC26X_SUCCESS if value is valid, otherwise TMC26X_INVALID_VALUE or
**         TMC26X_INVALID_MODE if in wrong mode.
*/
int tmc26xDRVCTRLSetMicrostepResolution(TMC26XConfiguration* config, int16_t value) {
	uint8_t setVal;
	//Check to see if in Step/Dir mode
	if (!(config->validity & TMC26X_VALID_BITMASK_DRVCONF_DRIVEMODE)
	 || (config->regDRVCONF & TMC26X_DRVCONF_DRIVEMODE_BITMASK))
		return TMC26X_INVALID_MODE;

	//Convert integer value through lookup or indicate invalid value
	switch(value) {
	case 256:
		setVal = 0;
		break;
	case 128:
		setVal = 1;
		break;
	case 64:
		setVal = 2;
		break;
	case 32:
		setVal = 3;
		break;
	case 16:
		setVal = 4;
		break;
	case 8:
		setVal = 5;
		break;
	case 4:
		setVal = 6;
		break;
	case 2:
		setVal = 7;
		break;
	case 1:
		setVal = 8;
		break;
	default:
		return TMC26X_INVALID_VALUE;
	}

	alterRegisterValue(&config->regDRVCTRL, 0, 4, setVal);
	config->dirty |= TMC26X_DIRTY_BITMASK_DRVCTRL;
	config->validity |= TMC26X_VALID_BITMASK_DRVCTRL_MICROSTEP_RESOLUTION;

	return TMC26X_SUCCESS;
}

/* Sets the "Polarity A" (bit 17) of the DRVCTRL register
** (page 19 TMC260 and TMC261 DATASHEET (Rev. 205 / 2012-NOV-05)
** Config structure must indicate SPI mode
**
** config - Configuration structure
** value  - 0: Current flows from OA1 pins to OA2 pins
**          1: Current flows from OA2 pins to OA1 pins
**
** returns TMC26X_SUCCESS if correct mode, otherwise TMC26X_INVALID_MODE
*/
int tmc26xDRVCTRLSetPolarityA(TMC26XConfiguration* config, int8_t value) {
	//Check to see if in SPI mode
	if (!(config->validity & TMC26X_VALID_BITMASK_DRVCONF_DRIVEMODE)
	 || (!(config->regDRVCONF & TMC26X_DRVCONF_DRIVEMODE_BITMASK)))
		return TMC26X_INVALID_MODE;

	alterRegisterValue(&config->regDRVCTRL, 17, 1, value);
	config->dirty |= TMC26X_DIRTY_BITMASK_DRVCTRL;
	config->validity |= TMC26X_VALID_BITMASK_DRVCTRL_MICROSTEP_RESOLUTION;
	config->validity |= TMC26X_VALID_BITMASK_DRVCTRL_POLARITY_A;

	return TMC26X_SUCCESS;
}

/* Sets the "Polarity B" (bit 8) of the DRVCTRL register
** (page 19 TMC260 and TMC261 DATASHEET (Rev. 205 / 2012-NOV-05)
** Config structure must indicate SPI mode
**
** config - Configuration structure
** value  - 0: Current flows from OB1 pins to OB2 pins
**          1: Current flows from OB2 pins to OB1 pins
**
** returns TMC26X_SUCCESS if correct mode, otherwise TMC26X_INVALID_MODE
*/
int tmc26xDRVCTRLSetPolarityB(TMC26XConfiguration* config, int8_t value) {
	//Check to see if in SPI mode
	if (!(config->validity & TMC26X_VALID_BITMASK_DRVCONF_DRIVEMODE)
	 || (!(config->regDRVCONF & TMC26X_DRVCONF_DRIVEMODE_BITMASK)))
		return TMC26X_INVALID_MODE;

	alterRegisterValue(&config->regDRVCTRL, 8, 1, value);
	config->dirty |= TMC26X_DIRTY_BITMASK_DRVCTRL;
	config->validity |= TMC26X_VALID_BITMASK_DRVCTRL_POLARITY_B;

	return TMC26X_SUCCESS;
}

/* Sets the "Current A" (bits 9-16) of the DRVCTRL register
** (page 19 TMC260 and TMC261 DATASHEET (Rev. 205 / 2012-NOV-05)
** Config structure must indicate SPI mode
**
** config - Configuration structure
** value  - Magnitude of current flow through coil A.
** TODO This value + hysteresis or offset cannot be > 255
**
** returns TMC26X_SUCCESS if value is valid, or
**         TMC26X_INVALID_MODE if in wrong mode.
*/
int tmc26xDRVCTRLSetCurrentA(TMC26XConfiguration* config, uint8_t value) {
	//Check to see if in SPI mode
	if (!(config->validity & TMC26X_VALID_BITMASK_DRVCONF_DRIVEMODE)
	 || (!(config->regDRVCONF & TMC26X_DRVCONF_DRIVEMODE_BITMASK)))
		return TMC26X_INVALID_MODE;

	alterRegisterValue(&config->regDRVCTRL, 9, 8, value);
	config->dirty |= TMC26X_DIRTY_BITMASK_DRVCTRL;
	config->validity |= TMC26X_VALID_BITMASK_DRVCTRL_CURRENT_A;

	return TMC26X_SUCCESS;
}

/* Sets the "Current B" (bits 0-7) of the DRVCTRL register
** (page 19 TMC260 and TMC261 DATASHEET (Rev. 205 / 2012-NOV-05)
** Config structure must indicate SPI mode
**
** config - Configuration structure
** value  - Magnitude of current flow through coil B.
** TODO This value + hysteresis or offset cannot be > 255
**
** returns TMC26X_SUCCESS if value is valid, or
**         TMC26X_INVALID_MODE if in wrong mode.
*/
int tmc26xDRVCTRLSetCurrentB(TMC26XConfiguration* config, uint8_t value) {
	//Check to see if in SPI mode
	if (!(config->validity & TMC26X_VALID_BITMASK_DRVCONF_DRIVEMODE)
	 || (!(config->regDRVCONF & TMC26X_DRVCONF_DRIVEMODE_BITMASK)))
		return TMC26X_INVALID_MODE;

	alterRegisterValue(&config->regDRVCTRL, 0, 8, value);
	config->dirty |= TMC26X_DIRTY_BITMASK_DRVCTRL;
	config->validity |= TMC26X_VALID_BITMASK_DRVCTRL_CURRENT_B;

	return TMC26X_SUCCESS;
}

/* Sets the "Blanking time" (bits 15-16) of the CHOPCONF register
** (page 20 TMC260 and TMC261 DATASHEET (Rev. 205 / 2012-NOV-05)
**
** config - Configuration structure
** value - Blanking time interval, in system clock periods
**
** returns TMC26X_SUCCESS if value is valid, otherwise TMC26X_INVALID_VALUE
*/
int tmc26xCHOPCONFSetBlankingTime(TMC26XConfiguration* config, uint8_t value) {
	uint8_t setVal;

	//Check to see if proposed the OFF_TIME has not been set to 1
	if ((config->validity & TMC26X_VALID_BITMASK_CHOPCONF_OFF_TIME)
	 && value < 24
	 && (retrieveRegisterValue(&config->regCHOPCONF,0,4)==1))
		return TMC26X_INVALID_VALUE;

	//Convert integer value through lookup or indicate invalid value
	switch(value) {
	case 16:
		setVal = 0;
		break;
	case 24:
		setVal = 1;
		break;
	case 36:
		setVal = 2;
		break;
	case 54:
		setVal = 3;
		break;
	default:
		return TMC26X_INVALID_VALUE;
	}

	alterRegisterValue(&config->regCHOPCONF, 15, 2, setVal);
	config->dirty |= TMC26X_DIRTY_BITMASK_CHOPCONF;
	config->validity |= TMC26X_VALID_BITMASK_CHOPCONF_BLANKING_TIME;

	return TMC26X_SUCCESS;
}

/* Sets the "Chopper Mode" (bit 14) of the CHOPCONF register
** (page 20 TMC260 and TMC261 DATASHEET (Rev. 205 / 2012-NOV-05)
** Changing this value will reset bits 4-12 of the CHOPCONF register
** as these values are dependent on this setting.
**
** config - Configuration structure
** value - 0: Standard Mode (spreadCycle)
**         1: Constant tOFF with fast decay time.
**
** returns TMC26X_SUCESS
*/
int tmc26xCHOPCONFSetChopperMode(TMC26XConfiguration* config, uint8_t value) {
	alterRegisterValue(&config->regCHOPCONF, 14, 1, value);
	alterRegisterValue(&config->regCHOPCONF, 4, 9, 0);
	config->dirty |= TMC26X_DIRTY_BITMASK_CHOPCONF;
	config->validity |= TMC26X_VALID_BITMASK_CHOPCONF_CHOPMODE;
	config->validity &= ~(TMC26X_VALID_BITMASK_CHOPCONF_HYSTERESIS_DEC | TMC26X_VALID_BITMASK_CHOPCONF_HYSTERESIS_START | TMC26X_VALID_BITMASK_CHOPCONF_HYSTERESIS_END);
	
	return TMC26X_SUCCESS;
}

/* Sets the "Random TOFF time" (bit 13) of the CHOPCONF register
** (page 20 TMC260 and TMC261 DATASHEET (Rev. 205 / 2012-NOV-05)
**
** config - Configuration structure
** value - 0: Chopper off time is fixed as set by bits tOFF
**         1: Random mode, tOFF is random modulated by -12..+3 clocks
**
** returns TMC26X_SUCESS
*/
int tmc26xCHOPCONFSetRandomTOff(TMC26XConfiguration* config, uint8_t value) {
	alterRegisterValue(&config->regCHOPCONF, 13, 1, value);
	config->dirty |= TMC26X_DIRTY_BITMASK_CHOPCONF;
	config->validity |= TMC26X_VALID_BITMASK_CHOPCONF_RANDOM_TOFF;
	
	return TMC26X_SUCCESS;
}

/* Sets the "Off time/MOSFET disable" (bits 0-3) of the CHOPCONF register
** (page 21 TMC260 and TMC261 DATASHEET (Rev. 205 / 2012-NOV-05)
**
** config - Configuration structure
** value - Duration of slow decay phase. if TOff is 0, the MOSFETs are shut off
**         if TOff is 1 then TBL must be greater than 16
**         range 0 .. 15
** returns - TMC26X_SUCCESS if value is valid, otherwise TMC26X_INVALID_VALUE 
*/
int tmc26xCHOPCONFSetTimeOff(TMC26XConfiguration* config, uint8_t value) {
	//Check to see if proposed the OFF_TIME has not been set to 1
	if ((config->validity & TMC26X_VALID_BITMASK_CHOPCONF_BLANKING_TIME)
	 && value == 1
	 && (retrieveRegisterValue(&config->regCHOPCONF,15,2)==0))
		return TMC26X_INVALID_VALUE;

	// Check to see if value is not too high
	if (value > 15)
		return TMC26X_INVALID_VALUE;

	alterRegisterValue(&config->regCHOPCONF, 0, 4, value);
	config->dirty |= TMC26X_DIRTY_BITMASK_CHOPCONF;
	config->validity |= TMC26X_VALID_BITMASK_CHOPCONF_OFF_TIME;
	
	return TMC26X_SUCCESS;
}

/* Sets the "Hysteresis decrement" (bit 11-12) of the CHOPCONF register
** (page 20 TMC260 and TMC261 DATASHEET (Rev. 205 / 2012-NOV-05)
** config structure must indicate spreadCycle mode
**
** config - Configuration structure
** value  - Hysteresis decrement period setting
**
** returns TMC26X_SUCCESS if value is valid, otherwise TMC26X_INVALID_VALUE or
**         TMC26X_INVALID_MODE if in wrong mode.
*/
int tmc26xCHOPCONFSetHysteresisDecrement(TMC26XConfiguration* config, uint8_t value) {
	uint8_t setVal;
	//Check to see if in spreadCycle mode
	if (!(config->validity & TMC26X_VALID_BITMASK_CHOPCONF_CHOPMODE)
	 || (config->regCHOPCONF & TMC26X_CHOPCONF_CHOPMODE_BITMASK))
		return TMC26X_INVALID_MODE;

	//Convert integer value through lookup or indicate invalid value
	switch(value) {
	case 16:
		setVal = 0;
		break;
	case 32:
		setVal = 1;
		break;
	case 48:
		setVal = 2;
		break;
	case 64:
		setVal = 3;
		break;
	default:
		return TMC26X_INVALID_VALUE;
	}

	alterRegisterValue(&config->regCHOPCONF, 11, 2, setVal);
	config->dirty |= TMC26X_DIRTY_BITMASK_CHOPCONF;
	config->validity |= TMC26X_VALID_BITMASK_CHOPCONF_HYSTERESIS_DEC;
	
	return TMC26X_SUCCESS;
}

/* Sets the "Hysteresis end" (bit 7-10) of the CHOPCONF register
** (page 20 TMC260 and TMC261 DATASHEET (Rev. 205 / 2012-NOV-05)
** config structure must indicate spreadCycle mode
**
** config - Configuration structure
** value  - Hysteresis end setting (-3 .. 12) 1/512 adds to current setting
**
** returns  TMC26X_SUCCESS if value is valid, otherwise TMC26X_INVALID_VALUE or
**          TMC26X_INVALID_MODE if in wrong mode.
*/
int tmc26xCHOPCONFSetHysteresisEnd(TMC26XConfiguration* config, int8_t value) {
	//Check to see if in spreadCycle mode
	if (!(config->validity & TMC26X_VALID_BITMASK_CHOPCONF_CHOPMODE)
	 || (config->regCHOPCONF & TMC26X_CHOPCONF_CHOPMODE_BITMASK))
		return TMC26X_INVALID_MODE;

	//Check to see if value falls within the required range
	if (value < -3 || value > 12)
		return TMC26X_INVALID_VALUE;

	//Check to see if proposed HSTART + HEND <= 15
	if ((config->validity & TMC26X_VALID_BITMASK_CHOPCONF_HYSTERESIS_START)
	 && ((int8_t)retrieveRegisterValue(&config->regCHOPCONF,4,3) + 1 + value > 15))
		return TMC26X_INVALID_VALUE;

	alterRegisterValue(&config->regCHOPCONF, 7, 4, (uint8_t)(value+3));
	config->dirty |= TMC26X_DIRTY_BITMASK_CHOPCONF;
	config->validity |= TMC26X_VALID_BITMASK_CHOPCONF_HYSTERESIS_END;
	
	return TMC26X_SUCCESS;
}

/* Sets the "Hysteresis start" (bit 4-6) of the CHOPCONF register
** (page 20 TMC260 and TMC261 DATASHEET (Rev. 205 / 2012-NOV-05)
** config structure must indicate spreadCycle mode
**
** config - Configuration structure
** value  - Hysteresis end setting (1 .. 8) 1/512 adds to current setting
**
** returns TMC26X_SUCCESS if value is valid, otherwise TMC26X_INVALID_VALUE or
**          TMC26X_INVALID_MODE if in wrong mode.
*/
int tmc26xCHOPCONFSetHysteresisStart(TMC26XConfiguration* config, uint8_t value) {
	//Check to see if in spreadCycle mode
	if (!(config->validity & TMC26X_VALID_BITMASK_CHOPCONF_CHOPMODE)
	 || (config->regCHOPCONF & TMC26X_CHOPCONF_CHOPMODE_BITMASK))
		return TMC26X_INVALID_MODE;

	//Check to see if value falls within the required range
	if (value < 1 || value > 8)
		return TMC26X_INVALID_VALUE;

	//Check to see if proposed HSTART + HEND <= 15
	if ((config->validity & TMC26X_VALID_BITMASK_CHOPCONF_HYSTERESIS_END)
	 && ((int8_t)retrieveRegisterValue(&config->regCHOPCONF,7,4) - 3 + value > 15))
		return TMC26X_INVALID_VALUE;

	alterRegisterValue(&config->regCHOPCONF, 4, 3, value-1);
	config->dirty |= TMC26X_DIRTY_BITMASK_CHOPCONF;
	config->validity |= TMC26X_VALID_BITMASK_CHOPCONF_HYSTERESIS_START;
	
	return TMC26X_SUCCESS;
}

/* Sets the "Fast decay mode" (bit 12) of the CHOPCONF register
** (page 20 TMC260 and TMC261 DATASHEET (Rev. 205 / 2012-NOV-05)
** config structure must indicate fast decay mode
**
** config - Configuration structure
** value  - 0: Current comparator can terminate the fast decay
**             phase before timer expires.
**          1: Only the timer terminates the fast decay phase
**
** returns TMC26X_SUCCESS if value is valid, or
**         TMC26X_INVALID_MODE if in wrong mode.
*/
int tmc26xCHOPCONFSetFastDecayMode(TMC26XConfiguration* config, uint8_t value) {
	//Check to see if in fast decay mode
	if (!(config->validity & TMC26X_VALID_BITMASK_CHOPCONF_CHOPMODE)
	 || (!(config->regCHOPCONF & TMC26X_CHOPCONF_CHOPMODE_BITMASK)))
		return TMC26X_INVALID_MODE;

	alterRegisterValue(&config->regCHOPCONF, 12, 1, value);
	config->dirty |= TMC26X_DIRTY_BITMASK_CHOPCONF;
	config->validity |= TMC26X_VALID_BITMASK_CHOPCONF_FASTDECAY_MODE;
	
	return TMC26X_SUCCESS;
}

/* Sets the "Sine wave offset" (bit 7 - 10) of the CHOPCONF register
** (page 20 TMC260 and TMC261 DATASHEET (Rev. 205 / 2012-NOV-05)
** config structure must indicate fast decay mode
**
** config - Configuration structure
** value  - sine wave offset, 1/512 of the value becomes added to the absolute
**          value of each sine wave entry. range -3 .. 12
**
** returns TMC26X_SUCCESS if value is valid, otherwise TMC26X_INVALID_VALUE or
**          TMC26X_INVALID_MODE if in wrong mode.
*/
int tmc26xCHOPCONFSetSineOffset(TMC26XConfiguration* config, int8_t value) {
	//Check to see if in spreadCycle mode
	if (!(config->validity & TMC26X_VALID_BITMASK_CHOPCONF_CHOPMODE)
	 || (!(config->regCHOPCONF & TMC26X_CHOPCONF_CHOPMODE_BITMASK)))
		return TMC26X_INVALID_MODE;

	//Check to see if value falls within the required range
	if (value < -3 || value > 12)
		return TMC26X_INVALID_VALUE;

	alterRegisterValue(&config->regCHOPCONF, 7, 4, (uint8_t)(value+3));
	config->dirty |= TMC26X_DIRTY_BITMASK_CHOPCONF;
	config->validity |= TMC26X_VALID_BITMASK_CHOPCONF_SINE_OFFSET;
	
	return TMC26X_SUCCESS;
}

/* Sets the "Fast decay time" (bit 4 - 6, 11) of the CHOPCONF register
** (page 20 TMC260 and TMC261 DATASHEET (Rev. 205 / 2012-NOV-05)
** config structure must indicate fast decay mode
**
** config - Configuration structure
** value  - duration of the fast decay phase.
**          range 0 .. 15
**
** returns TMC26X_SUCCESS or TMC26X_INVALID_MODE if in wrong mode or
**         TMC26X_INVALID_VALUE if value is out of range.
*/
int tmc26xCHOPCONFSetFastDecayTime(TMC26XConfiguration* config, uint8_t value) {
	//Check to see if in spreadCycle mode
	if (!(config->validity & TMC26X_VALID_BITMASK_CHOPCONF_CHOPMODE)
	 || (!(config->regCHOPCONF & TMC26X_CHOPCONF_CHOPMODE_BITMASK)))
		return TMC26X_INVALID_MODE;

	if (value > 15)
		return TMC26X_INVALID_VALUE;

	//LS 3 bits go into bits 4-6
	alterRegisterValue(&config->regCHOPCONF, 4, 3, value);
	//MSB goes into bit 11
	alterRegisterValue(&config->regCHOPCONF, 11, 1, value>>3);
	
	config->dirty |= TMC26X_DIRTY_BITMASK_CHOPCONF;
	config->validity |= TMC26X_VALID_BITMASK_CHOPCONF_FASTDECAY_TIME;
	
	return TMC26X_SUCCESS;
}

/* Sets the "Minimum coolStep current" (bit 15) of the SMARTEN register
** (page 21 TMC260 and TMC261 DATASHEET (Rev. 205 / 2012-NOV-05)
**
** config - Configuration structure
** value  - 0: One half CS current setting (use TMC26X_HALF_CURRENT)
**          1: One quarter CS current settign (use TMC26X_QUARTER_CURRENT)
**
** returns TMC26X_SUCCESS
*/
int tmc26xSMARTENSetMinCoolStepCurrent(TMC26XConfiguration* config, uint8_t value) {
	alterRegisterValue(&config->regSMARTEN, 15, 1, value);
	config->dirty |= TMC26X_DIRTY_BITMASK_SMARTEN;
	config->validity |= TMC26X_VALID_BITMASK_SMARTEN_MIN_COOLSTEP;

	return TMC26X_SUCCESS;
}

/* Sets the "Current decrement speed" (bits 13-14) of the SMARTEN register
** (page 21 TMC260 and TMC261 DATASHEET (Rev. 205 / 2012-NOV-05)
**
** config - Configuration structure
** value  - Number of times that the SG2 value must be sampled equal to
**          or above the upper threshold.
**
** returns TMC26X_SUCCESS if value is valid, otherwise TMC26X_INVALID_VALUE
*/
int tmc26xSMARTENSetCurrentDecSpeed(TMC26XConfiguration* config, uint8_t value) {
	uint8_t setVal;

	//Convert integer value through lookup or indicate invalid value
	switch(value) {
	case 32:
		setVal = 0;
		break;
	case 8:
		setVal = 1;
		break;
	case 2:
		setVal = 2;
		break;
	case 1:
		setVal = 3;
		break;
	default:
		return TMC26X_INVALID_VALUE;
	}

	alterRegisterValue(&config->regSMARTEN, 13, 2, setVal);
	config->dirty |= TMC26X_DIRTY_BITMASK_SMARTEN;
	config->validity |= TMC26X_VALID_BITMASK_SMARTEN_CURRENT_DEC_SPEED;
	
	return TMC26X_SUCCESS;
}

/* Sets the "Upper coolStep threshold" (bits 8-11) of the SMARTEN register
** (page 21 TMC260 and TMC261 DATASHEET (Rev. 205 / 2012-NOV-05)
**
** config - Configuration structure
** value  - if the SG2 measurement value is sampled >= SEMIN+SEMAX enough
**          times then the current scaling factor is decremented
**          range 0 .. 15
**
** returns TMC26X_SUCCESS if value is valid, otherwise TMC26X_INVALID_VALUE
*/
int tmc26xSMARTENSetHighCoolStepThreshold(TMC26XConfiguration* config, uint8_t value) {
	//Check that value is in range
	if (value > 15)
		return TMC26X_INVALID_VALUE;

	alterRegisterValue(&config->regSMARTEN, 8, 4, value);
	config->dirty |= TMC26X_DIRTY_BITMASK_SMARTEN;
	config->validity |= TMC26X_VALID_BITMASK_SMARTEN_HIGH_COOLSTEP_THRESH;

	return TMC26X_SUCCESS;
}

/* Sets the "Current increment size" (bits 5-6) of the SMARTEN register
** (page 21 TMC260 and TMC261 DATASHEET (Rev. 205 / 2012-NOV-05)
**
** config - Configuration structure
** value - Number of current increment steps for each time the SG2 value is
**         sampled below the lower threshold
**
** returns TMC26X_SUCCESS if value is valid, otherwise TMC26X_INVALID_VALUE
*/
int tmc26xSMARTENSetCurrentIncSize(TMC26XConfiguration* config, uint8_t value) {
	uint8_t setVal;

	//Convert integer value through lookup or indicate invalid value
	switch(value) {
	case 1:
		setVal = 0;
		break;
	case 2:
		setVal = 1;
		break;
	case 4:
		setVal = 2;
		break;
	case 8:
		setVal = 3;
		break;
	default:
		return TMC26X_INVALID_VALUE;
	}

	alterRegisterValue(&config->regSMARTEN, 5, 2, setVal);
	config->dirty |= TMC26X_DIRTY_BITMASK_SMARTEN;
	config->validity |= TMC26X_VALID_BITMASK_SMARTEN_CURRENT_INC_SIZE;
	
	return TMC26X_SUCCESS;
}

/* Sets the "Lower coolStep threshold" (bits 0-3) of the SMARTEN register
** (page 21 TMC260 and TMC261 DATASHEET (Rev. 205 / 2012-NOV-05)
**
** config - Configuration structure
** value  - if 0, coolStep is disabled. if SG2 measurement value is sampled
**          < SEMIN the current scaling factor is increased
**
** returns TMC26X_SUCCESS if value is valid, otherwise TMC26X_INVALID_VALUE
*/
int tmc26xSMARTENSetLowCoolStepThreshold(TMC26XConfiguration* config, uint8_t value) {
	//Check that value is in range
	if (value > 15)
		return TMC26X_INVALID_VALUE;

	alterRegisterValue(&config->regSMARTEN, 0, 4, value);
	config->dirty |= TMC26X_DIRTY_BITMASK_SMARTEN;
	config->validity |= TMC26X_VALID_BITMASK_SMARTEN_LOW_COOLSTEP_THRESH;

	return TMC26X_SUCCESS;
}

/* Sets the "StallGuard2 filter enable" (bit 16) of the SGCSCONF register
** (page 22 TMC260 and TMC261 DATASHEET (Rev. 205 / 2012-NOV-05)
**
** config - Configuration structure
** value - 0: Standard Mode, fastest response time
**         1: Filtered mode, updated once for each four fullsteps
**
** returns TMC26X_SUCCESS
*/
int tmc26xSGCSCONFSetStallGuardFilter(TMC26XConfiguration* config, uint8_t value) {
	alterRegisterValue(&config->regSGCSCONF, 16, 1, value);
	config->dirty |= TMC26X_DIRTY_BITMASK_SGCSCONF;
	config->validity |= TMC26X_VALID_BITMASK_SGCSCONF_FILTER;

	return TMC26X_SUCCESS;
}

/* Sets the "StallGuard2 threshold value" (bits 8-14) of the SGCSCONF register
** (page 22 TMC260 and TMC261 DATASHEET (Rev. 205 / 2012-NOV-05)
**
** config - Configuration structure
** value  - Signed integer controlling the optimum measurement range for
**          readout. A lower value results in higher sensitivity.
**          range -64 .. 63
**
** returns TMC26X_SUCCESS if value is valid otherwise TMC26X_INVALID_VALUE
*/
int tmc26xSGCSCONFSetStallGuardThreshold(TMC26XConfiguration* config, int8_t value) {
	//Ensure that value is in the valid range
	if (value < -64 || value > 63)
		return TMC26X_INVALID_VALUE;

	alterRegisterValue(&config->regSGCSCONF, 8, 7, value);
	config->dirty |= TMC26X_DIRTY_BITMASK_SGCSCONF;
	config->validity |= TMC26X_VALID_BITMASK_SGCSCONF_THRESHOLD;

	return TMC26X_SUCCESS;
}

/* Sets the "Current scale" (bits 0 - 4) of the SGCSCONF register
** (page 22 TMC260 and TMC261 DATASHEET (Rev. 205 / 2012-NOV-05)
**
** config - Configuration structure
** value  - Current scaling for SPI and step/dir mode
**          range 1 .. 32
**
** returns TMC26X_SUCCESS if value is valid otherwise TMC26X_INVALID_VALUE
*/
int tmc26xSGCSCONFSetCurrentScale(TMC26XConfiguration* config, uint8_t value) {
	//Ensure that value is in the valid range
	if (value < 1 || value > 32)
		return TMC26X_INVALID_VALUE;

	alterRegisterValue(&config->regSGCSCONF, 0, 5, value-1);
	config->dirty |= TMC26X_DIRTY_BITMASK_SGCSCONF;
	config->validity |= TMC26X_VALID_BITMASK_SGCSCONF_CURRENT_SCALE;

	return TMC26X_SUCCESS;
}

/* Sets the "Reserved TEST mode" (bit 16) of the DRVCONF register
** (page 23 TMC260 and TMC261 DATASHEET (Rev. 205 / 2012-NOV-05)
**
** config - Configuration structure
** value  - 0: Normal operation
**          1: SG_TST will expose digital test values
**             TEST_ANA will expose analogue test values
**
** returns TMC26X_SUCCESS
*/
int tmc26xDRVCONFSetTestMode(TMC26XConfiguration* config, uint8_t value) {
	alterRegisterValue(&config->regDRVCONF, 16, 1, value);
	config->dirty |= TMC26X_DIRTY_BITMASK_DRVCONF;
	config->validity |= TMC26X_VALID_BITMASK_DRVCONF_TEST;
	
	return TMC26X_SUCCESS;
}

/* Sets the "Slope control, high side" (bits 14-15) of the DRVCONF register
** (page 23 TMC260 and TMC261 DATASHEET (Rev. 205 / 2012-NOV-05)
**
** config - Configuration structure
** value  - Use TMC26X_MINIMUM etc.
**
** returns TMC26X_SUCCESS if value is valid otherwise TMC26X_INVALID_VALUE
*/
int tmc26xDRVCONFSetSlopeControlHigh(TMC26XConfiguration* config, uint8_t value) {
	uint8_t setVal;

	//Convert value through lookup or indicate invalid value
	switch(value) {
	case TMC26X_MINIMUM:
		setVal = 0;
		break;
	case TMC26X_MINIMUM_TEMPERATURE_COMPENSATION:
		setVal = 1;
		break;
	case TMC26X_MEDIUM_TEMPERATURE_COMPENSATION:
		setVal = 2;
		break;
	case TMC26X_MAXIMUM:
		setVal = 3;
		break;
	default:
		return TMC26X_INVALID_VALUE;
	}

	alterRegisterValue(&config->regDRVCONF, 14, 2, setVal);
	config->dirty |= TMC26X_DIRTY_BITMASK_DRVCONF;
	config->validity |= TMC26X_VALID_BITMASK_DRVCONF_SLOPE_HIGH;
	
	return TMC26X_SUCCESS;
}

/* Sets the "Slope control, low side" (bits 12-13) of the DRVCONF register
** (page 23 TMC260 and TMC261 DATASHEET (Rev. 205 / 2012-NOV-05)
**
** config - Configuration structure
** value  - Use TMC26X_MINIMUM etc.
**
** returns TMC26X_SUCCESS if value is valid otherwise TMC26X_INVALID_VALUE
*/
int tmc26xDRVCONFSetSlopeControlLow(TMC26XConfiguration* config, uint8_t value) {
	uint8_t setVal;

	//Convert value through lookup or indicate invalid value
	switch(value) {
	case TMC26X_MINIMUM:
		setVal = 0;
		break;
	case TMC26X_MEDIUM:
		setVal = 2;
		break;
	case TMC26X_MAXIMUM:
		setVal = 3;
		break;
	default:
		return TMC26X_INVALID_VALUE;
	}

	alterRegisterValue(&config->regDRVCONF, 12, 2, setVal);
	config->dirty |= TMC26X_DIRTY_BITMASK_DRVCONF;
	config->validity |= TMC26X_VALID_BITMASK_DRVCONF_SLOPE_LOW;
	
	return TMC26X_SUCCESS;
}

/* Sets the "Short to GND Protection" (bit 10) of the DRVCONF register
** (page 23 TMC260 and TMC261 DATASHEET (Rev. 205 / 2012-NOV-05)
**
** config - Configuration structure
** value  - 0: Short to GND protection is disabled
**          1: Short to GND protection is enabled
**
** returns TMC26X_SUCCESS if value is valid otherwise TMC26X_INVALID_VALUE
*/
int tmc26xDRVCONFSetGroundShortProtection(TMC26XConfiguration* config, uint8_t value) {
	//Invert value
	switch (value) {
	case 0:
		value = 1;
		break;
	case 1:
		value = 0;
		break;
	default:
		return TMC26X_INVALID_VALUE;
	}

	alterRegisterValue(&config->regDRVCONF, 10, 1, value);
	config->dirty |= TMC26X_DIRTY_BITMASK_DRVCONF;
	config->validity |= TMC26X_VALID_BITMASK_DRVCONF_GND_SHORT_PROTECT;
	
	return TMC26X_SUCCESS;
}

/* Sets the "Short to GND detection timer" (bits 8-9) of the DRVCONF register
** (page 23 TMC260 and TMC261 DATASHEET (Rev. 205 / 2012-NOV-05)
**
** config - Configuration structure
** value  - time in tengths of microseconds
**
** returns TMC26X_SUCCESS if value is valid otherwise TMC26X_INVALID_VALUE
*/
int tmc26xDRVCONFSetGroundShortTimer(TMC26XConfiguration* config, uint8_t value) {
	uint8_t setVal;

	//Convert value through lookup or indicate invalid value
	switch(value) {
	case 32:
		setVal = 0;
		break;
	case 16:
		setVal = 1;
		break;
	case 12:
		setVal = 2;
		break;
	case 8:
		setVal = 3;
		break;
	default:
		return TMC26X_INVALID_VALUE;
	}

	alterRegisterValue(&config->regDRVCONF, 8, 2, setVal);
	config->dirty |= TMC26X_DIRTY_BITMASK_DRVCONF;
	config->validity |= TMC26X_VALID_BITMASK_DRVCONF_GND_SHORT_TIMER;
	
	return TMC26X_SUCCESS;
}

/* Sets the "STEP/DIR Interface disable" (bit 7) of the DRVCONF register
** (page 23 TMC260 and TMC261 DATASHEET (Rev. 205 / 2012-NOV-05)
** Changing this value will reset bits 0-17 of the DRVCTRL register
** as these values are dependent on this setting.
**
** config - Configuration structure
** value  - 0: STEP and DIR interface
**          1: SPI interface
**
** returns TMC26X_SUCCESS if value is valid otherwise TMC26X_INVALID_VALUE
*/
int tmc26xDRVCONFSetDriveMode(TMC26XConfiguration* config, uint8_t value) {
	alterRegisterValue(&config->regDRVCONF, 7, 1, value);
	config->dirty |= TMC26X_DIRTY_BITMASK_DRVCONF | TMC26X_DIRTY_BITMASK_DRVCTRL;
	config->validity |= TMC26X_VALID_BITMASK_DRVCONF_DRIVEMODE;

	//Completely clear the DRVCTRL register
	config->regDRVCTRL = TMC26X_DRVCTRL_ADDRESS;

	//Invalidate the hit values in the DRVCTRL register
	if (value == TMC26X_SPI)
		config->validity &= ~(
		    TMC26X_VALID_BITMASK_DRVCTRL_POLARITY_A | TMC26X_VALID_BITMASK_DRVCTRL_POLARITY_B
		  | TMC26X_VALID_BITMASK_DRVCTRL_CURRENT_A | TMC26X_VALID_BITMASK_DRVCTRL_CURRENT_B);
	else {
		config->validity &= ~(
		    TMC26X_VALID_BITMASK_DRVCTRL_STEP_INTERPOLATION | TMC26X_VALID_BITMASK_DRVCTRL_DOUBLE_STEP 
		  | TMC26X_VALID_BITMASK_DRVCTRL_MICROSTEP_RESOLUTION);
		config->validity |= TMC26X_VALID_BITMASK_DRVCTRL_BIT3_ALWAYS_ONE;
	}

	return TMC26X_SUCCESS;
}

/* Sets the "Sense resistor voltage-based current scaling" (bit 6) of
** the DRVCONF register
** (page 23 TMC260 and TMC261 DATASHEET (Rev. 205 / 2012-NOV-05)
**
** config - Configuration structure
** value  - Full-scale sense resistor voltage in mV
**
** returns TMC26X_SUCCESS if value is valid otherwise TMC26X_INVALID_VALUE
*/
int tmc26xDRVCONFSetMaximumRSenseVoltage(TMC26XConfiguration* config, uint16_t value) {
	uint8_t setVal;

	//Convert value through lookup or indicate invalid value
	switch(value) {
	case TMC26X_VSENSE_FULL:
		setVal = 0;
		break;
	case TMC26X_VSENSE_HALFISH:
		setVal = 1;
		break;
	default:
		return TMC26X_INVALID_VALUE;
	}

	alterRegisterValue(&config->regDRVCONF, 6, 1, setVal);
	config->dirty |= TMC26X_DIRTY_BITMASK_DRVCONF;
	config->validity |= TMC26X_VALID_BITMASK_DRVCONF_VSENSE;
	
	return TMC26X_SUCCESS;
}

/* Sets the "Select value for read out" (bits 4-5) of the DRVCONF register
** (page 23 TMC260 and TMC261 DATASHEET (Rev. 205 / 2012-NOV-05)
**
** config - Configuration structure
** value  - use TMC26X_READBACK...
**
** returns TMC26X_SUCCESS if value is valid otherwise TMC26X_INVALID_VALUE
*/
int tmc26xDRVCONFSetReadbackValue(TMC26XConfiguration* config, uint8_t value) {
	uint8_t setVal;

	//Convert value through lookup or indicate invalid value
	switch(value) {
	case TMC26X_READBACK_MICROSTEP:
		setVal = 0;
		break;
	case TMC26X_READBACK_STALLGUARD:
		setVal = 1;
		break;
	case TMC26X_READBACK_COOLSTEP:
		setVal = 2;
		break;
	default:
		return TMC26X_INVALID_VALUE;
	}

	alterRegisterValue(&config->regDRVCONF, 4, 2, setVal);
	config->dirty |= TMC26X_DIRTY_BITMASK_DRVCONF;
	config->validity |= TMC26X_VALID_BITMASK_DRVCONF_READBACK;
	
	return TMC26X_SUCCESS;
}

/* Gets the "Select value for read out" (bits 4-5) of the DRVCONF register
** needed when reading values to see if the register must be written twice
** (page 23 TMC260 and TMC261 DATASHEET (Rev. 205 / 2012-NOV-05)
**
** config - Configuration structure
** value  - use TMC26X_READBACK...
**
** returns value if value is valid otherwise TMC26X_INVALID_VALUE
*/

int8_t tmc26xDRVCONFGetReadbackValue(TMC26XConfiguration* config) {
	uint16_t value = retrieveRegisterValue(&config->regDRVCONF, 4, 2);

	if (!(config->validity & TMC26X_VALID_BITMASK_DRVCONF_READBACK))
		return TMC26X_INVALID_VALUE;
	
	switch(value) {
	case 0:
		return TMC26X_READBACK_MICROSTEP;
	case 1:
		return TMC26X_READBACK_STALLGUARD;
	case 2:
		return TMC26X_READBACK_COOLSTEP;
	default:
		return TMC26X_INVALID_VALUE;
	}
}

