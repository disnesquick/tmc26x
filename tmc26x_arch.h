#ifdef ARCH_XMEGA
#include <avr/io.h>
#include <avr/interrupt.h> 
#include "io_assignment.h"

static inline uint8_t SPITransceiveByte(uint8_t data) {
	GLOBAL_TMC26X_SPI_CONTROLLER.DATA = data;      // initiate write
	while((GLOBAL_TMC26X_SPI_CONTROLLER.STATUS & SPI_IF_bm) == 0);
	return GLOBAL_TMC26X_SPI_CONTROLLER.DATA;
}
#define tmc26xSPITransceiveByte SPITransceiveByte
	
#define tmc26xSPIChipEnable() GLOBAL_TMC26X_SPI_CONTROLLER.CTRL = SPI_ENABLE_bm | SPI_MASTER_bm | SPI_MODE_3_gc | SPI_PRESCALER_DIV16_gc; GLOBAL_TMC26X_SPI_SELECT_PORT.OUTCLR = GLOBAL_TMC26X_SPI_SELECT_PIN
#define tmc26xSPIChipDisable() GLOBAL_TMC26X_SPI_SELECT_PORT.OUTSET = GLOBAL_TMC26X_SPI_SELECT_PIN
#else
static inline uint8_t SPITransceiveByte(uint8_t data) {
}
#define tmc26xSPITransceiveByte SPITransceiveByte
	
#define tmc26xSPIChipEnable() 1+1
#define tmc26xSPIChipDisable() 1+1
#endif
