#include "ec11.h"
#include "fw_hal.h"

// 编码器计数值
static uint16_t encoder_value_min = 0;
static uint16_t encoder_value_max = 1;
static int8_t encoder_value_loop = 0;
static uint8_t encoder_button_pressed = 0;

volatile uint16_t g_ec11_value = 0;

void EC11_init(void) {
    // EXTI_Int2_SetIntState(HAL_State_ON);
    INTCLKO = 0x10;
}

void EC11_roll_isr(void) {
    if ((P_ENC_A ^ P_ENC_B) == 1) {
        if (g_ec11_value > encoder_value_min) {
            g_ec11_value--;
        } else if (encoder_value_loop) {
            g_ec11_value = encoder_value_max;
        }
    } else {
        if (g_ec11_value < encoder_value_max) {
            g_ec11_value++;
        } else if (encoder_value_loop) {
            g_ec11_value = encoder_value_min;
        }
    }
}

void EC11_set_range(uint16_t cur, uint16_t min, uint16_t max, int8_t loop) {
    encoder_value_min = min;
    encoder_value_max = max;
    g_ec11_value = cur;
    encoder_value_loop = loop;
}

int8_t EC11_is_button_pressed(void) {
    if (P_ENC_KEY == 0) {
        if (encoder_button_pressed == 0) {
            encoder_button_pressed = 1;
            return 1;
        }
    } else {
        encoder_button_pressed = 0;
    }
    return 0;
}
