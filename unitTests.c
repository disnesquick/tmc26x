#include <stdint.h>
#include "tmc26x.h"

TMC26XConfiguration config;

int main() {
	TMC26XConfiguration_Init(&config);
	initializeTMC26XWithProfile(&config, MOTOR_LG_23HS7430);
}
