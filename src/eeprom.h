#ifndef __H_EEPROM__
#define __H_EEPROM__

#include "fw_sys.h"

#define REFLOW_CFG_ADDR 0x0000
#define PID_CFG_ADDR 0x0020
#define FONT_DIGIT_ADDR 0x0200
#define FONT_CHINESE_ADDR 0x0408

void EEPROM_init(void);
void EEPROM_read(uint16_t address, uint8_t *data, uint8_t size);
void EEPROM_save_cfg(void);

#endif