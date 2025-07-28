#ifndef __H_HEAT__
#define __H_HEAT__

#include "fw_sys.h"

#define P_HEAT_PWM P11
#define P_FORCE_FAN P34
#define P_FAN P35
#define P_TEMP_REF P33
#define P_TEMP P32

void HEAT_init(void);

void HEAT_timer_isr(void);

void HEAT_read_temp(void);
void HEAT_run(void);
void HEAT_reset(void);

int8_t HEAT_is_back_pressed(void);

extern int16_t g_target_temp;
extern int16_t g_current_temp;
extern uint8_t g_time_sec;

struct PID_CFG {
    int16_t kp;
    int16_t ki;
    int16_t kd;
    int16_t delay;
};
extern struct PID_CFG g_pid_cfg;

#endif