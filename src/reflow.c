#include "reflow.h"
#include "app.h"
#include "ec11.h"
#include "eeprom.h"
#include "font.h"
#include "heat.h"
#include "lcd.h"
#include "menu.h"

static uint8_t _select_idx = 0;
static uint8_t _setting = 0;
static uint8_t _working = 0;

static uint8_t _starting_time = 0;
static uint8_t _stage = 0;

struct REFLOW_DATA_EEPROM g_reflow_cfg;

static void show_setting_select(void) {
    uint8_t i = _select_idx % 2;

    LCD_fill(0, 5, 8, 30, i == 0 ? (_setting == 0 ? GREEN : RED) : BLACK);
    LCD_fill(0, 45, 8, 30, i == 1 ? (_setting == 0 ? GREEN : RED) : BLACK);
}

static void show_setting_title(void) {
    uint8_t s = _select_idx / 2;
    LCD_show_chinese(20, 5, 1 + 2 * s, 2, WHITE);
    LCD_show_chinese(126, 5, 0, 1, WHITE);

    LCD_show_chinese(20, 45, 11, 2, s < 2 ? WHITE : BLACK);
    LCD_show_chinese(126, 45, 13, 1, s < 2 ? WHITE : BLACK);
}

static void show_setting_value(void) {
    uint8_t s = _select_idx / 2;

    LCD_show_number(75, 5, g_reflow_cfg.stages[s].target_temp, 3, WHITE);
    LCD_show_number(75, 45, g_reflow_cfg.stages[s].time, 3, s < 2 ? WHITE : BLACK);
}

static void show_stage(void) {
    uint8_t c = WHITE + _stage;
    LCD_show_chinese(0, 5, 1 + _stage * 2, 2, c);
    LCD_show_number(60, 5, g_reflow_cfg.stages[_stage].target_temp, 3, WHITE);
    LCD_show_chinese(120, 5, 0, 1, WHITE);

    LCD_show_chinese(0, 45, 9, 2, c);
    LCD_show_chinese(120, 45, 0, 1, WHITE);
}

static void show_stage_value(void) {
    //
    LCD_show_number(60, 45, g_current_temp / 10, 3, WHITE);
}

void REFLOW_enter(void) {
    _setting = 0;
    _working = 0;

    _starting_time = 0;
    _stage = 0;

    EC11_set_range(0, 0, 4, 1);

    LCD_clear(BLACK);
    show_setting_select();
    show_setting_title();
    show_setting_value();
}

void REFLOW_run(void) {
    if (!_working) {
        uint8_t s = _select_idx / 2;
        uint8_t i = _select_idx % 2;

        if (_setting == 0) {
            if (_select_idx != g_ec11_value) {
                uint8_t old = _select_idx;
                _select_idx = g_ec11_value;

                show_setting_select();

                if (old / 2 != g_ec11_value / 2) {
                    show_setting_title();
                    show_setting_value();
                }
            }

            if (EC11_is_button_pressed()) {
                _setting = 1;
                EC11_set_range(i == 0 ? g_reflow_cfg.stages[s].target_temp : g_reflow_cfg.stages[s].time, 0, 300, 0);

                show_setting_select();
                show_setting_value();
            } else if (HEAT_is_back_pressed()) {
                goto __main_menu;
            }
        } else {
            uint8_t changed = 0;
            if (i == 0) {
                if (g_reflow_cfg.stages[s].target_temp != g_ec11_value) {
                    g_reflow_cfg.stages[s].target_temp = g_ec11_value;
                    show_setting_value();
                    changed = 1;
                }
            } else {
                if (g_reflow_cfg.stages[s].time != g_ec11_value) {
                    g_reflow_cfg.stages[s].time = g_ec11_value;
                    show_setting_value();
                    changed = 1;
                }
            }

            if (changed) {
                uint16_t prev = 30;
                if (s > 0) {
                    prev = g_reflow_cfg.stages[s - 1].target_temp;
                }
                g_reflow_cfg.stages[s].speed = (g_reflow_cfg.stages[s].target_temp - prev) * 10 / g_reflow_cfg.stages[s].time;
            }

            if (HEAT_is_back_pressed()) {
                goto __main_menu;
            } else if (EC11_is_button_pressed()) {
                _setting = 0;
                EEPROM_save_cfg();
                EC11_set_range(_select_idx, 0, 4, 1);
                show_setting_select();
                show_setting_value();
            }
        }

        if (P_ENC_KEY == 1) {
            if (_starting_time > 0) {
                LCD_fill(152, 0, 10, LCD_HEIGHT, BLACK);
            }
            _starting_time = 0;
        } else {
            if (g_sec_elapsed) {
                _starting_time++;

                LCD_fill(152, LCD_HEIGHT - _starting_time * 20, 10, _starting_time * 20, GREEN);
                if (_starting_time >= 4) {
                    _working = 1;
                    HEAT_read_temp();
                    g_target_temp = g_current_temp;
                    _stage = 0;

                    LCD_clear(BLACK);
                    show_stage();
                    show_stage_value();
                }
            }
        }
    } else {
        if (HEAT_is_back_pressed()) {
            _working = 0;
            HEAT_reset();
            P_FAN = 0;
        } else {
            HEAT_read_temp();
            show_stage_value();

            if (_stage == 3) {
                P_HEAT_PWM = 0;
                P_FAN = 1;
            } else {
                if (_stage < 2) {
                    HEAT_run();
                }

                if (g_sec_elapsed) {
                    int16_t target_temp = g_reflow_cfg.stages[_stage].target_temp;
                    if (_stage == 2) {
                        P_HEAT_PWM = 1;
                    } else {
                        if (g_target_temp < target_temp * 10) {
                            g_target_temp += g_reflow_cfg.stages[_stage].speed;
                        }
                    }
                }

                if (g_current_temp >= g_reflow_cfg.stages[_stage].target_temp * 10) {
                    g_target_temp = g_current_temp;
                    _stage++;
                    show_stage();
                }
            }
        }
    }

    return;
__main_menu:
    HEAT_reset();
    g_app_stage = STAGE_MENU;
    MENU_enter();
}