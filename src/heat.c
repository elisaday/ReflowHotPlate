#include "heat.h"
#include "adc.h"
#include "app.h"
#include "ec11.h"
#include "fw_hal.h"
#include "lcd.h"
#include "menu.h"

// 单位: 0.1°C
int16_t g_target_temp = 300;
int16_t g_current_temp = 300;

uint8_t g_time_sec;

static uint8_t _force_fan_pressed = 0;

static volatile uint16_t _time_ms = 0;

static __CODE uint16_t PT1000_TEMP_TABLE[] = {
    10000, 10390, 10779, 11167, 11554, 11940, 12324, 12708, 13090, 13471, 13851, 14229, 14607, 14983, 15358, 15733,
    16105, 16477, 16848, 17217, 17586, 17953, 18319, 18684, 19047, 19410, 19771, 20131, 20490, 20848, 21205,
};

static int8_t _do_pid = 0;

struct PID_CFG g_pid_cfg;

// 输出限制
#define PWM_MIN 0
#define PWM_MAX 10000

// 内部状态变量
static int32_t _integral = 0;
static int16_t _last_measure = 0;
static int16_t _last_output = 0;

int16_t _pwm = 0;

void calc_pwm(void) {
    int32_t error = g_target_temp - g_current_temp;

    int32_t change_rate = g_current_temp - _last_measure;
    int32_t pred_measure = g_current_temp + change_rate * g_pid_cfg.delay;
    int32_t pred_error = g_target_temp - pred_measure;

    int32_t p_term = g_pid_cfg.kp * pred_error;

    int32_t i_term = _integral;
    if (_last_output > PWM_MIN && _last_output < PWM_MAX) {
        i_term += g_pid_cfg.ki * error;

        if (i_term > PWM_MAX)
            i_term = PWM_MAX;
        else if (i_term < PWM_MIN)
            i_term = PWM_MIN;
    }
    _integral = i_term;

    int32_t d_term = -g_pid_cfg.kd * change_rate;

    int32_t pwm = p_term + i_term + d_term;

    if (pwm > PWM_MAX) {
        pwm = PWM_MAX;
    } else if (pwm < PWM_MIN) {
        pwm = PWM_MIN;
    }

    _pwm = pwm;
    _last_measure = g_current_temp;
    _last_output = _pwm;
}

// timer: 1us
void HEAT_timer_isr(void) {
    _time_ms++;
    if (_time_ms == 1000) {
        _time_ms = 0;
        _do_pid = 1;

        g_time_sec++;
    }
}

void HEAT_init(void) {
    TL0 = 0x48; // 设置定时初始值
    TH0 = 0x77;

    // TIM_Timer0_Set1TMode(1);
    // TIM_Timer0_SetMode(TIM_TimerMode_16BitAuto);
    AUXR = 0xC0;
    TMOD = 0x00;

    // EXTI_Timer0_SetIntState(HAL_State_ON);
    // EXTI_Timer0_SetIntPriority(EXTI_IntPriority_High);
    // TIM_Timer0_SetRunState(HAL_State_ON);
    ET0 = 0x01;
    IP = 0x00;
    IPH = 0x02;
    TR0 = 0x01;
}

void HEAT_read_temp(void) {
    uint32_t ADC_TEMP = 0;
    uint32_t ADC_TEMP_REF = 0;
    for (uint8_t i = 0; i < 8; ++i) {
        ADC_TEMP += ADC_read(0);
        ADC_TEMP_REF += ADC_read(1);
    }
    ADC_TEMP /= 8;
    ADC_TEMP_REF /= 8;

    uint32_t PT1000 = ADC_TEMP * 10000 / (ADC_TEMP_REF - ADC_TEMP);
    if (PT1000 <= 10000) {
        g_current_temp = 0;
    } else if (PT1000 >= 21205) {
        g_current_temp = 3000;
    } else {
        for (uint8_t i = 0; i < sizeof(PT1000_TEMP_TABLE) / sizeof(uint16_t) + 1; i++) {
            uint32_t t1 = PT1000_TEMP_TABLE[i];
            uint32_t t2 = PT1000_TEMP_TABLE[i + 1];
            if (t1 < PT1000 && PT1000 < t2) {
                g_current_temp = i * 100 + (PT1000 - t1) * 100 / (t2 - t1);
                break;
            }
        }
    }
}

void HEAT_reset(void) {
    _integral = 0;
    _last_measure = 0;
    _last_output = 0;
    _pwm = 0;
    P_HEAT_PWM = 0;
}

void HEAT_run(void) {
    if (_do_pid) {
        _do_pid = 0;
        calc_pwm();
    }

    if (_pwm / 1000 * 100 > _time_ms) {
        P_HEAT_PWM = 1;
        P_FAN = 0;
    } else {
        P_HEAT_PWM = 0;

        if (g_current_temp > g_target_temp + 30) {
            P_FAN = 1;
        } else {
            P_FAN = 0;
        }
    }
}

int8_t HEAT_is_back_pressed(void) {
    if (P_FORCE_FAN == 0) {
        if (_force_fan_pressed == 0) {
            _force_fan_pressed = 1;
            return 1;
        }
    } else {
        _force_fan_pressed = 0;
    }
    return 0;
}
