
#ifndef __H_APP__
#define __H_APP__

#include "fw_sys.h"

#define STAGE_MENU 0
#define STAGE_REFLOW 1
#define STAGE_CONST_TEMP 2
#define STAGE_SETTING 3

extern int8_t g_app_stage;
extern uint8_t g_last_sec;

#endif