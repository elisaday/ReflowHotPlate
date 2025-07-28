#pragma once

#include "fw_sys.h"

#define P_ENC_KEY P10
#define P_ENC_A P36
#define P_ENC_B P37

void EC11_init(void);
void EC11_roll_isr(void);
void EC11_set_range(uint16_t cur, uint16_t min, uint16_t max, int8_t loop);
int8_t EC11_is_button_pressed(void);

extern volatile uint16_t g_ec11_value;