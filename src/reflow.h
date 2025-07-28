#ifndef __H_REFLOW__
#define __H_REFLOW__

#include "fw_sys.h"

void REFLOW_enter(void);
void REFLOW_run(void);

struct REFLOW_STAGE_CFG {
    int16_t target_temp;
    uint8_t time;
    // 单位: 0.1°C/秒
    int8_t speed;
};

struct REFLOW_DATA_EEPROM {
    // 预热，恒温，回流，冷却
    struct REFLOW_STAGE_CFG stages[4];
};

extern struct REFLOW_DATA_EEPROM g_reflow_cfg;

#endif