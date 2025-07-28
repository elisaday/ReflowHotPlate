
#include "menu.h"
#include "app.h"
#include "const_temp.h"
#include "ec11.h"
#include "font.h"
#include "heat.h"
#include "lcd.h"
#include "reflow.h"
#include "setting.h"

static uint8_t _menu_index = 0;

static void show(void) {
    LCD_fill(0, 0, 8, LCD_HEIGHT, BLACK);
    LCD_fill(0, _menu_index * 27, 8, 26, GREEN);
}

void MENU_enter(void) {
    EC11_set_range(0, 0, 2, 1);

    LCD_clear(BLACK);

    show();
    for (uint8_t i = 0; i < 3; ++i) {
        LCD_show_chinese(28, 26 * i + 1, 14 + 4 * i, 4, WHITE);
    }
}

void MENU_run(void) {
    if (g_ec11_value != _menu_index) {
        _menu_index = g_ec11_value;
        show();
    }

    if (EC11_is_button_pressed()) {
        if (_menu_index == 0) {
            g_app_stage = STAGE_REFLOW;
            REFLOW_enter();
        } else if (_menu_index == 1) {
            g_app_stage = STAGE_CONST_TEMP;
            CONST_TEMP_enter();
        } else if (_menu_index == 2) {
            g_app_stage = STAGE_SETTING;
            SET_enter();
        }
    }

    if (HEAT_is_back_pressed()) {
        P_FAN = !P_FAN;
    }
}