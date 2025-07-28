#include "eeprom.h"
#include "fw_hal.h"
#include "heat.h"
#include "reflow.h"
#include "setting.h"

static void write(uint16_t addr, uint8_t *data, uint8_t size) {
    uint8_t pos = 0;
    for (pos = 0; pos < size; pos++) {
        IAP_WriteData(data[pos]);
        IAP_CmdWrite(addr + pos);
        if (IAP_IsCmdFailed()) {
            IAP_ClearCmdFailFlag();
        }
    }
}

static void erase(uint16_t addr) {
    uint8_t pos = 0;
    IAP_CmdErase(addr);
    if (IAP_IsCmdFailed()) {
        IAP_ClearCmdFailFlag();
    }
}

void EEPROM_init(void) {
    // IAP_SetWaitTime();
    // IAP_SetEnabled(HAL_State_ON);
    IAP_TPS = 0x23;
    IAP_CONTR = 0x80;

    EEPROM_read(REFLOW_CFG_ADDR, (uint8_t *)&g_reflow_cfg, sizeof(g_reflow_cfg));
    EEPROM_read(PID_CFG_ADDR, (uint8_t *)&g_pid_cfg, sizeof(g_pid_cfg));
}

void EEPROM_save_cfg(void) {
    erase(REFLOW_CFG_ADDR);
    write(REFLOW_CFG_ADDR, (uint8_t *)&g_reflow_cfg, sizeof(g_reflow_cfg));
    write(PID_CFG_ADDR, (uint8_t *)&g_pid_cfg, sizeof(g_pid_cfg));
}

void EEPROM_read(uint16_t addr, uint8_t *data, uint8_t size) {
    uint8_t pos = 0;
    for (pos = 0; pos < size; pos++) {
        IAP_CmdRead(addr + pos);
        if (IAP_IsCmdFailed()) {
            IAP_ClearCmdFailFlag();
        } else {
            data[pos] = IAP_ReadData();
        }
    }
}
