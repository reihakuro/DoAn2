/**
 ******************************************************************************
 * @file    amr_power.h
 * @brief   Quan ly chot nguon thong minh (Smart Power Latch):
 *            PB0  POWER_HOLD  : HIGH = giu P-MOSFET Q2 dan (da set HIGH tu MX_GPIO_Init)
 *            PB1  PWR_BTN     : LOW khi nhan SW1 (qua diode D5, pull-up noi)
 *            PB10 PI_HALT     : HIGH khi Raspberry Pi da Halt xong (gpio-poweroff)
 *
 *  Quy trinh tat may an toan:
 *   RUNNING --(giu SW1 >= 1.5 s | pin CRITICAL | Pi tu Halt)--> SHUTDOWN_REQUESTED
 *   SHUTDOWN_REQUESTED: dung dong co, bao Pi (Giai doan 2 gui lenh UART 0x04),
 *                       cho PB10 HIGH on dinh (toi da 45 s)
 *   -> HALT_SETTLE: cho them 800 ms cho Linux sync the nho
 *   -> POWER_OFF  : PB0 = LOW -> Q2 ngat, dong tieu thu ~0 uA
 ******************************************************************************
 */
#ifndef INC_AMR_POWER_H_
#define INC_AMR_POWER_H_

#include "amr_config.h"

typedef enum
{
    PWR_STATE_RUNNING = 0,
    PWR_STATE_SHUTDOWN_REQUESTED,
    PWR_STATE_HALT_SETTLE,
    PWR_STATE_POWER_OFF
} PowerState_t;

typedef enum
{
    PWR_REASON_NONE = 0,
    PWR_REASON_BUTTON,
    PWR_REASON_BATTERY,
    PWR_REASON_PI_HALTED,
    PWR_REASON_REMOTE           /* Giai doan 2: lenh tu Raspberry Pi */
} PowerReason_t;

typedef enum
{
    BTN_EVENT_NONE = 0,
    BTN_EVENT_SHORT,            /* Nhan nhanh < 0.8 s */
    BTN_EVENT_LONG              /* Giu >= 1.5 s       */
} ButtonEvent_t;

void AMR_Power_Init(void);

/** Goi dinh ky (moi 10 ms). Tra ve su kien nut nhan (neu co). */
ButtonEvent_t AMR_Power_Update(void);

/** Yeu cau tat may (bo qua neu dang trong qua trinh tat). */
void AMR_Power_RequestShutdown(PowerReason_t reason);

PowerState_t  AMR_Power_GetState(void);
PowerReason_t AMR_Power_GetReason(void);
bool          AMR_Power_IsShuttingDown(void);
/** Muc logic hien tai cua PB10 (da chong doi). */
bool          AMR_Power_IsPiHalted(void);

#endif /* INC_AMR_POWER_H_ */
