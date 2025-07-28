
#include "const_temp.h"
#include "app.h"
#include "ec11.h"
#include "eeprom.h"
#include "font.h"
#include "heat.h"
#include "lcd.h"
#include "menu.h"

static int8_t _working = 0;

void CONST_TEMP_enter(void) {
    _working = 0;

    EC11_set_range(30, 0, 256, 0);

    LCD_clear(BLACK);

    LCD_show_chinese(20, 5, 3, 2, WHITE);
    LCD_show_chinese(126, 5, 0, 1, WHITE);

    LCD_show_chinese(20, 45, 9, 2, WHITE);
    LCD_show_chinese(126, 45, 0, 1, WHITE);

    LCD_show_number(75, 5, g_target_temp / 10, 3, WHITE);
    LCD_show_number(75, 45, g_current_temp / 10, 3, WHITE);
}

void CONST_TEMP_run(void) {
    HEAT_read_temp();

    if (g_last_sec != g_time_sec) {
        LCD_show_number(75, 45, g_current_temp / 10, 3, WHITE);
    }

    uint16_t v = g_ec11_value * 10;
    if (g_target_temp != v) {
        g_target_temp = v;
        LCD_show_number(75, 5, g_ec11_value, 3, WHITE);
    }

    if (EC11_is_button_pressed()) {
        _working = 1;
        LCD_fill(0, 0, 8, LCD_HEIGHT, GREEN);
    } else if (HEAT_is_back_pressed()) {
        HEAT_reset();
        P_FAN = 0;

        if (_working) {
            _working = 0;
            LCD_fill(0, 0, 8, LCD_HEIGHT, BLACK);
            return;
        } else {
            g_app_stage = STAGE_MENU;
            MENU_enter();
            return;
        }
    }

    if (_working) {
        HEAT_run();
    }
}
