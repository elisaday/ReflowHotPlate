#include "setting.h"
#include "app.h"
#include "ec11.h"
#include "eeprom.h"
#include "font.h"
#include "heat.h"
#include "lcd.h"
#include "menu.h"

static uint8_t _setting = 0;
static uint8_t _select_idx = 0;

static void show_setting(void) {
    //
    LCD_show_chinese(20, 25, 26 + _select_idx * 2, 2, WHITE);
}

static void show_value(void) {
    //
    LCD_show_number(75, 25, ((int16_t *)&g_pid_cfg)[_select_idx], 3, WHITE);
}

void SET_init(void) {
    //
}

void SET_enter(void) {
    _setting = 0;
    _select_idx = 0;

    EC11_set_range(0, 0, 3, 1);
    LCD_clear(BLACK);
    show_setting();
    show_value();
}

void SET_run(void) {
    int16_t *pid_cfg = (int16_t *)&g_pid_cfg;
    if (_setting == 0) {
        if (_select_idx != g_ec11_value) {
            _select_idx = g_ec11_value;
            show_setting();
            show_value();
        }

        if (EC11_is_button_pressed()) {
            _setting = 1;
            EC11_set_range(pid_cfg[_select_idx], 0, 300, 0);
            LCD_fill(0, 25, 8, 30, GREEN);
        }
    } else {
        if (pid_cfg[_select_idx] != g_ec11_value) {
            pid_cfg[_select_idx] = g_ec11_value;
            show_value();
        }

        if (EC11_is_button_pressed()) {
            _setting = 0;
            LCD_fill(0, 25, 8, 30, BLACK);

            EEPROM_save_cfg();
            EC11_set_range(_select_idx, 0, 3, 1);
        }
    }

    if (HEAT_is_back_pressed()) {
        g_app_stage = STAGE_MENU;
        MENU_enter();
    }
}
