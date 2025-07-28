#include "adc.h"
#include "app.h"
#include "const_temp.h"
#include "ec11.h"
#include "eeprom.h"
#include "font.h"
#include "fw_hal.h"
#include "heat.h"
#include "lcd.h"
#include "menu.h"
#include "reflow.h"
#include "setting.h"

uint8_t g_last_sec = 0;

INTERRUPT(TM0_Rountine, EXTI_VectTimer0) { HEAT_timer_isr(); }
INTERRUPT(INT2_Routine, EXTI_VectInt2) { EC11_roll_isr(); }

void gpio_setup(void) {
    // // P11 P_HEAT_PWM
    // GPIO_P1_SetMode(GPIO_Pin_1, GPIO_Mode_Output_PP);
    // // P10 P_ENC_KEY
    // GPIO_P1_SetMode(GPIO_Pin_0, GPIO_Mode_Input_HIP);
    // // LCD
    // GPIO_P1_SetMode(GPIO_Pin_5 | GPIO_Pin_3 | GPIO_Pin_6 | GPIO_Pin_7, GPIO_Mode_Output_PP);
    // GPIO_P1_SetMode(GPIO_Pin_2, GPIO_Mode_InOut_QBD);
    P1M0 = 0xea;
    P1M1 = 0x01;

    // // P34 P_FORCE_FAN
    // GPIO_P3_SetMode(GPIO_Pin_4, GPIO_Mode_InOut_QBD);
    // // P35 P_FAN
    // GPIO_P3_SetMode(GPIO_Pin_5, GPIO_Mode_Output_PP);
    // // P37 P_ENC_B
    // // P36 P_ENC_A
    // GPIO_P3_SetMode(GPIO_Pin_6 | GPIO_Pin_7, GPIO_Mode_Input_HIP);
    // // P33 P_TEMP_REF
    // GPIO_P3_SetMode(GPIO_Pin_3, GPIO_Mode_Input_HIP);
    // // P32 P_TEMP
    // GPIO_P3_SetMode(GPIO_Pin_2, GPIO_Mode_Input_HIP);
    P3M0 = 0x20;
    P3M1 = 0xcc;

    P_FAN = 0;
    P_HEAT_PWM = 0;
}

void main(void) {
    // RSTCFG |= ((1 << 6) | (1 << 4)); // Enable low voltage reset and set P5.4 as RESET pin

    // // 串口打印配置 115200波特率
    // SCON = 0x50;  // 8位数据,可变波特率
    // AUXR |= 0x40; // 定时器时钟1T模式
    // AUXR &= 0xFE; // 串口1选择定时器1为波特率发生器
    // TMOD &= 0x0F; // 设置定时器模式
    // TL1 = 0xB5;   // 设置定时初始值
    // TH1 = 0xFF;   // 设置定时初始值
    // ET1 = 0;      // 禁止定时器中断
    // TR1 = 1;      // 定时器1开始

    gpio_setup();

    SFRX_ON();
    LCD_init();
    LCD_clear(WHITE);

    ADC_init();
    HEAT_init();
    EC11_init();
    EEPROM_init();
    SET_init();

    // EXTI_Global_SetIntState(HAL_State_ON);
    EA = 1;

    SYS_Delay(500);

    MENU_enter();

    while (1) {
        if (g_app_stage == STAGE_MENU) {
            MENU_run();
        } else if (g_app_stage == STAGE_CONST_TEMP) {
            CONST_TEMP_run();
        } else if (g_app_stage == STAGE_REFLOW) {
            REFLOW_run();
        } else if (g_app_stage == STAGE_SETTING) {
            SET_run();
        }
        g_last_sec = g_time_sec;
    }
}
