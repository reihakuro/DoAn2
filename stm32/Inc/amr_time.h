/**
 ******************************************************************************
 * @file    amr_time.h
 * @brief   Bo dem micro-giay chay tu do dung TIM4 (cau hinh truc tiep bang
 *          thanh ghi, KHONG can bat TIM4 trong CubeMX).
 *
 *  - Tan so dem 1 MHz (72 MHz / 72), 16-bit, tran sau 65.535 ms.
 *  - Dung phep tru uint16_t de do khoang thoi gian < 65 ms (tu dong xu ly tran).
 *  - Chon TIM4 thay vi DWT->CYCCNT vi nhieu bo Blue Pill dung chip "clone"
 *    (CS32/CKS32/GD32) khong co DWT cycle counter.
 *
 *  LUU Y: KHONG kich hoat TIM4 trong file .ioc (tai nguyen da duoc dat truoc).
 ******************************************************************************
 */
#ifndef INC_AMR_TIME_H_
#define INC_AMR_TIME_H_

#include "main.h"
#include <stdint.h>

void AMR_Time_Init(void);

/** Doc gia tri bo dem micro-giay hien tai (16-bit, quay vong). */
static inline uint16_t AMR_Time_Us(void)
{
    return (uint16_t)TIM4->CNT;
}

/** Khoang thoi gian (us) tu moc `since` den hien tai, hop le khi < 65 ms. */
static inline uint16_t AMR_Time_ElapsedUs(uint16_t since)
{
    return (uint16_t)(AMR_Time_Us() - since);
}

/** Cho ban (busy-wait) mot khoang ngan, CHI dung cho xung < 50 us. */
void AMR_Time_DelayUs(uint16_t us);

/** Khoang thoi gian (ms) tu moc HAL_GetTick() `since` (xu ly tran 32-bit). */
static inline uint32_t AMR_Time_ElapsedMs(uint32_t since)
{
    return HAL_GetTick() - since;
}

#endif /* INC_AMR_TIME_H_ */
