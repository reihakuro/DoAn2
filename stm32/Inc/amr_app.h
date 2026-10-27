/**
 ******************************************************************************
 * @file    amr_app.h
 * @brief   
 * Giao diện debug module
 * @details
 * - Xử lý thông tin an toàn khi có vật cản, kẹt bánh, pin, tắt máy, 
 * - Self-Test và telemetry. 
 * - main.c chỉ cần gọi AMR_App_Init() và AMR_App_Loop().
 ******************************************************************************
 */
#ifndef INC_AMR_APP_H_
#define INC_AMR_APP_H_

#include "amr_config.h"

/** Anh chup trang thai toan he thong - xem truc tiep bang Live Expressions
 *  (STM32CubeIDE > Debug) hoac qua log UART. Giai doan 2 dong goi thanh
 *  ban tin telemetry gui len Raspberry Pi. */
typedef struct
{
    uint32_t uptime_ms;
    uint32_t fault_flags;           /* AMR_FLAG_xxx                         */
    uint16_t bat_mv;
    uint16_t bat_raw;
    uint8_t  bat_state;             /* BatteryState_t                       */
    uint8_t  pwr_state;             /* PowerState_t                         */
    uint8_t  pwr_reason;            /* PowerReason_t                        */
    uint8_t  selftest_step;         /* 0 = khong chay                       */
    uint16_t sonar_mm[SONAR_COUNT]; /* Trai, Giua, Phai (0xFFFF = xa)       */
    int16_t  cmd_left, cmd_right;   /* Lenh -100..100                       */
    int16_t  pwm_left, pwm_right;   /* CCR co dau dang xuat                 */
    int16_t  speed_left_pps, speed_right_pps;
    uint32_t enc_total_left, enc_total_right;
    int32_t  dist_left_mm, dist_right_mm;
    uint32_t loop_max_us;           /* Thoi gian 1 vong lap dai nhat (us)   */
    uint32_t loop_count;
} AmrTelemetry_t;

extern volatile AmrTelemetry_t g_amr;

void AMR_App_Init(void);
void AMR_App_Loop(void);

/** Lenh van toc tu tang tren (Giai doan 2: tu UART). -100..+100. */
void AMR_App_SetVelocity(int16_t left, int16_t right);

/** Doi nguong phanh vat can phia truoc (Giai doan 4: che do cap tram). */
void AMR_App_SetObstacleThreshold(uint16_t stop_mm);

/** Xoa cac loi da chot (ket banh) - tuong duong nhan nhanh SW1. */
void AMR_App_ClearFaults(void);

#endif /* INC_AMR_APP_H_ */
