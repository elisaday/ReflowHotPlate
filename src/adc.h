#pragma once

#include "fw_sys.h"

#define P_TEMP_REF P33
#define P_TEMP P32

void ADC_init(void);
uint16_t ADC_read(uint8_t channel);
