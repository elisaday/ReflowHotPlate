#include "adc.h"
#include "fw_hal.h"

void ADC_init(void) {
    // ADC_SetPowerState(HAL_State_ON);
    // ADC_SetClockPrescaler(HAL_State_ON);
    // ADC_SetResultAlignmentRight();
    ADC_CONTR = 0x80;
    ADCCFG = 0x21;
}

uint16_t ADC_read(uint8_t channel) {
    ADC_CONTR = channel == 0 ? 0xCA : 0xCB;
    while (!(ADC_CONTR & 0x20))
        ;
    ADC_CONTR &= ~0x20;

    return (ADC_RES * 256 + ADC_RESL);
}
