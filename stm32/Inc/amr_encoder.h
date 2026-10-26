/**
 ******************************************************************************
 * @file    amr_encoder.h
 * @brief   Dem xung encoder quang LM393 (PB6 Trai / PB7 Phai, EXTI canh len),
 *          tinh toc do, quang duong co dau va phat hien ket banh (stall).
 *
 *  Encoder 1 kenh khong biet chieu quay -> dau (+/-) duoc suy ra tu chieu
 *  PWM dang xuat cho ben tuong ung.
 ******************************************************************************
 */
#ifndef INC_AMR_ENCODER_H_
#define INC_AMR_ENCODER_H_

#include "amr_config.h"

typedef enum
{
    ENC_LEFT  = 0,
    ENC_RIGHT = 1,
    ENC_COUNT = 2
} EncoderSide_t;

void AMR_Encoder_Init(void);

/** Goi trong vong lap chinh (khong chan). Cap nhat toc do theo cua so
 *  ENCODER_SPEED_WINDOW_MS va kiem tra ket banh. Tham so la CCR co dau
 *  dang xuat (lay tu AMR_Motor_GetPwmX()). */
void AMR_Encoder_Update(int16_t pwm_left, int16_t pwm_right);

/** Tong so xung tuyet doi tu luc khoi dong (dung kiem tra 20 xung/vong). */
uint32_t AMR_Encoder_GetTotal(EncoderSide_t side);
/** Quang duong co dau tinh bang xung (tien +, lui -). */
int32_t  AMR_Encoder_GetOdometry(EncoderSide_t side);
/** Toc do co dau (xung/giay) trong cua so gan nhat. */
int16_t  AMR_Encoder_GetSpeedPps(EncoderSide_t side);
/** Quang duong co dau (mm). */
int32_t  AMR_Encoder_GetDistanceMm(EncoderSide_t side);

/** Trang thai ket banh da chot (latched). */
bool AMR_Encoder_IsStalled(EncoderSide_t side);
/** Xoa chot ket banh (goi khi lenh dong co da ve 0). */
void AMR_Encoder_ClearStall(void);

#endif /* INC_AMR_ENCODER_H_ */
