/**
 ******************************************************************************
 * @file    amr_buzzer.c
 * @brief   Hiện thực bộ điều khiển phát các mẫu âm thanh cảnh báo bằng còi Buzzer.
 * @details
 * Quản lý chuỗi thời gian bật/tắt tiếp điểm transistor 2N2222 (PB5) theo máy trạng thái thời gian thực,
 * hỗ trợ các kiểu âm thanh phản hồi, cảnh báo pin yếu và báo động khẩn cấp không chặn CPU.
 *
 * @author  Nhóm Đề án 2 - Robot vận chuyển nội bộ
 * @version 2.1
 * @date    2026
 ******************************************************************************
 */
#include "amr_buzzer.h"

/* Moi mau: day thoi gian ON, OFF, ON, OFF ... ket thuc bang 0. */
static const uint16_t k_short[]  = { 100U, 0U };
static const uint16_t k_double[] = { 150U, 120U, 150U, 0U };
static const uint16_t k_long[]   = { 1000U, 0U };
static const uint16_t k_alarm[]  = { 200U, 150U, 0U };     /* Lap vo han */

static const uint16_t *s_seq;
static uint8_t         s_step;
static uint32_t        s_step_start_ms;
static bool            s_alarm;

static inline void buzzer_out(bool on)
{
    BZ_GPIO_Port->BSRR = on ? (uint32_t)BZ_Pin : ((uint32_t)BZ_Pin << 16U);
}

static void start_seq(const uint16_t *seq)
{
    s_seq = seq;
    s_step = 0U;
    s_step_start_ms = HAL_GetTick();
    buzzer_out(seq != NULL);
}

void AMR_Buzzer_Init(void)
{
    s_seq = NULL;
    s_alarm = false;
    buzzer_out(false);
}

void AMR_Buzzer_Play(BuzzerPattern_t pattern)
{
    if (s_alarm) { return; }    /* Bao dong dang keu -> bo qua mau thuong */

    switch (pattern)
    {
    case BUZZ_SHORT:  start_seq(k_short);  break;
    case BUZZ_DOUBLE: start_seq(k_double); break;
    case BUZZ_LONG:   start_seq(k_long);   break;
    case BUZZ_ALARM:  AMR_Buzzer_SetAlarm(true); break;
    case BUZZ_OFF:
    default:          start_seq(NULL);     break;
    }
}

void AMR_Buzzer_SetAlarm(bool on)
{
    if (on == s_alarm) { return; }
    s_alarm = on;
    start_seq(on ? k_alarm : NULL);
}

void AMR_Buzzer_Update(void)
{
    if (s_seq == NULL) { return; }

    if ((HAL_GetTick() - s_step_start_ms) < s_seq[s_step]) { return; }

    s_step++;
    s_step_start_ms = HAL_GetTick();

    if (s_seq[s_step] == 0U)
    {
        if (s_alarm)
        {
            s_step = 0U;                /* Lap lai mau bao dong */
        }
        else
        {
            s_seq = NULL;
            buzzer_out(false);
            return;
        }
    }
    /* Buoc chan = ON, buoc le = OFF */
    buzzer_out((s_step & 1U) == 0U);
}
