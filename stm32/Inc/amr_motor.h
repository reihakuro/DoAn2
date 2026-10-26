/**
 ******************************************************************************
 * @file    amr_motor.h
 * @brief   Dieu khien 2 cum dong co TT (vi sai 4 banh) qua L298N.
 *          - PWM TIM2 1 kHz: CH1 (PA0 = ENA, ben Trai), CH4 (PA3 = ENB, ben Phai)
 *          - Chieu: PA1/PA2 (IN1/IN2 Trai), PA4/PA5 (IN3/IN4 Phai)
 *          - Khong che tran CCR <= PWM_DUTY_MAX_LIMIT (55%) cho pin 4S
 *          - Tang/giam toc mem (ramp) 25 buoc / 10 ms
 *          - Phanh khan cap bang cach noi tat 2 dau dong co (dynamic brake)
 ******************************************************************************
 */
#ifndef INC_AMR_MOTOR_H_
#define INC_AMR_MOTOR_H_

#include "amr_config.h"

void AMR_Motor_Init(void);

/** Dat van toc mong muon cho 2 ben, thang -100..+100 (%). Gia tri ngoai
 *  thang tu dong bi kep. Dong co se tien toi gia tri nay theo ramp. */
void AMR_Motor_SetSpeed(int16_t left, int16_t right);

/** Goi dinh ky moi PWM_RAMP_PERIOD_MS (10 ms) tu vong lap chinh. */
void AMR_Motor_Update(void);

/** Dung khan cap: bo qua ramp, phanh dien MOTOR_BRAKE_HOLD_MS roi tha troi.
 *  Lenh nguoi dung (cmd) duoc giu nguyen; sau khi het phanh, dong co chi
 *  chay lai (co ramp) neu khong con co khoa nao. An toan khi goi lap lai. */
void AMR_Motor_EmergencyStop(void);

/** Cam toan bo chuyen dong (dung cho Stall / Shutdown / mat ket noi). */
void AMR_Motor_SetInhibit(bool inhibit);

/** Chan rieng chuyen dong TIEN (vat can phia truoc). Cho phep lui va
 *  quay tai cho (dieu kien: left + right <= 0). */
void AMR_Motor_SetForwardBlocked(bool blocked);

/** CCR co dau dang xuat ra (-550..+550), dung cho telemetry & stall. */
int16_t AMR_Motor_GetPwmLeft(void);
int16_t AMR_Motor_GetPwmRight(void);
/** Lenh van toc dang yeu cau (-100..+100). */
int16_t AMR_Motor_GetCmdLeft(void);
int16_t AMR_Motor_GetCmdRight(void);
bool    AMR_Motor_IsBraking(void);

#endif /* INC_AMR_MOTOR_H_ */
