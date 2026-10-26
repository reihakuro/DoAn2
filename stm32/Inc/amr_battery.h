/**
 ******************************************************************************
 * @file    amr_battery.h
 * @brief   Do dien ap pin 4S qua ADC1_IN6 (PA6), loc IIR, phan cap trang thai.
 ******************************************************************************
 */
#ifndef INC_AMR_BATTERY_H_
#define INC_AMR_BATTERY_H_

#include "amr_config.h"

typedef enum
{
    BAT_STATE_ABSENT = 0,   /* < 6 V: dang cap nguon USB / ST-Link (ban thu)  */
    BAT_STATE_OK,
    BAT_STATE_LOW,          /* <= 13.6 V: canh bao                            */
    BAT_STATE_CRITICAL      /* <= 12.0 V lien tuc 3 s: phai tat may           */
} BatteryState_t;

void AMR_Battery_Init(void);

/** Goi trong vong lap chinh; tu lay mau moi BATTERY_SAMPLE_MS. */
void AMR_Battery_Update(void);

uint16_t       AMR_Battery_GetMillivolts(void);   /* Da loc IIR + hieu chinh */
uint16_t       AMR_Battery_GetRawAdc(void);
BatteryState_t AMR_Battery_GetState(void);

#endif /* INC_AMR_BATTERY_H_ */
