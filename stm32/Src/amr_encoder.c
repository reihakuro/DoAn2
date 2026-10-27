/**
 ******************************************************************************
 * @file    amr_encoder.c
 * @brief   Hiện thực module đếm xung ngắt EXTI Encoder quang LM393 và chẩn đoán kẹt bánh.
 * @details
 * Xử lý ngắt ngoài cạnh lên từ 2 kênh encoder bánh trái (PB6) và bánh phải (PB7),
 * áp dụng bộ lọc khử nhiễu nảy xung, tích lũy quãng đường Odometry và máy trạng thái phát hiện kẹt cơ khí.
 *
 * @author  Nhóm Đề án 2 - Robot vận chuyển nội bộ
 * @version 2.1
 * @date    2026
 ******************************************************************************
 */
#include "amr_encoder.h"
#include "amr_time.h"

/**
 * @struct EncoderState_t
 * @brief Cấu trúc dữ liệu nội bộ lưu trạng thái đếm xung và đo lường cho một kênh encoder.
 */
typedef struct
{
    /* --- Các trường cập nhật bên trong trình phục vụ ngắt ISR --- */

    /** @brief Tổng số lượng xung ngắt ngoài tuyệt đối đếm được (tăng liên tục). */
    volatile uint32_t total;

    /** @brief Quãng đường tích lũy có dấu (tính theo số lượng xung, tiến cộng lùi trừ). */
    volatile int32_t  odometry;

    /** @brief Mốc thời gian microsecond của cạnh xung hợp lệ gần nhất (dùng cho lọc glitch). */
    volatile uint16_t last_us;

    /** @brief Mốc thời gian millisecond của cạnh xung hợp lệ gần nhất. */
    volatile uint32_t last_ms;

    /** @brief Chiều quay quy ước hiện tại (+1: tiến, -1: lùi) do vòng lặp chính suy diễn từ PWM. */
    volatile int8_t   dir;

    /** @brief Bộ đếm số lượng cạnh xung nhiễu bị bộ lọc phần mềm loại bỏ. */
    volatile uint32_t glitches;

    /* --- Các trường chỉ được truy cập và cập nhật trong vòng lặp chính --- */

    /** @brief Giá trị total ghi nhận tại thời điểm bắt đầu cửa sổ tính vận tốc. */
    uint32_t win_count;

    /** @brief Vận tốc góc tức thời có dấu tính toán được trong cửa sổ gần nhất (pps). */
    int16_t  speed_pps;

    /** @brief Giá trị total ghi nhận tại mốc bắt đầu theo dõi kẹt cơ khí. */
    uint32_t stall_count;

    /** @brief Mốc thời gian millisecond bắt đầu xuất hiện điều kiện nghi ngờ kẹt bánh. */
    uint32_t stall_ref_ms;

    /** @brief Cờ trạng thái xác nhận sự cố kẹt cơ khí bánh xe đã được chốt khóa. */
    bool     stalled;
} EncoderState_t;

static EncoderState_t s_enc[ENC_COUNT];
static uint32_t       s_win_start_ms;

/* -------------------------------------------------------------------------- */
static inline void encoder_edge(EncoderState_t *e)
{
    uint16_t now_us = AMR_Time_Us();
    uint32_t now_ms = HAL_GetTick();

    /* Neu da qua >= 3 ms thi chac chan hop le; nguoc lai dung bo dem us
     * (khi khoang cach < 3 ms thi phep tru 16-bit khong bi tran). */
    if (((now_ms - e->last_ms) >= 3U) ||
        ((uint16_t)(now_us - e->last_us) >= ENCODER_GLITCH_US))
    {
        e->total++;
        e->odometry += e->dir;
        e->last_us = now_us;
        e->last_ms = now_ms;
    }
    else
    {
        e->glitches++;
    }
}

/** Ghi de ham weak cua HAL - duoc goi tu EXTI9_5_IRQHandler. */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == EN_EXT_L_Pin)
    {
        encoder_edge(&s_enc[ENC_LEFT]);
    }
    else if (GPIO_Pin == EN_EXT_R_Pin)
    {
        encoder_edge(&s_enc[ENC_RIGHT]);
    }
}

/* -------------------------------------------------------------------------- */
static void stall_check(EncoderState_t *e, int16_t pwm, uint32_t now)
{
    int16_t mag = (pwm < 0) ? (int16_t)(-pwm) : pwm;
    uint32_t total = e->total;

    if ((mag < STALL_CCR_THRESHOLD) || (total != e->stall_count))
    {
        /* Duty thap (chua du luc keo) hoac banh van quay -> dat lai moc. */
        e->stall_count  = total;
        e->stall_ref_ms = now;
        return;
    }

    if ((now - e->stall_ref_ms) >= STALL_TIMEOUT_MS)
    {
        e->stalled = true;
    }
}

void AMR_Encoder_Init(void)
{
    uint32_t now = HAL_GetTick();
    for (uint32_t i = 0U; i < ENC_COUNT; i++)
    {
        s_enc[i] = (EncoderState_t){ 0 };
        s_enc[i].dir = 1;
        s_enc[i].last_ms = now;
        s_enc[i].stall_ref_ms = now;
    }
    s_win_start_ms = now;

    /* Uu tien ngat encoder thap hon SysTick de HAL_GetTick() luon chinh xac
     * trong ngat (SysTick = 0, EXTI = 1). Ngat van rat ngan (< 2 us). */
    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 1, 0);
}

void AMR_Encoder_Update(int16_t pwm_left, int16_t pwm_right)
{
    const int16_t pwm[ENC_COUNT] = { pwm_left, pwm_right };
    uint32_t now = HAL_GetTick();

    /* Chieu: giu chieu cu khi PWM = 0 (banh van tha troi theo quan tinh). */
    for (uint32_t i = 0U; i < ENC_COUNT; i++)
    {
        if (pwm[i] > 0)      { s_enc[i].dir = 1;  }
        else if (pwm[i] < 0) { s_enc[i].dir = -1; }
        stall_check(&s_enc[i], pwm[i], now);
    }

    uint32_t win = now - s_win_start_ms;
    if (win >= ENCODER_SPEED_WINDOW_MS)
    {
        for (uint32_t i = 0U; i < ENC_COUNT; i++)
        {
            uint32_t total = s_enc[i].total;
            uint32_t delta = total - s_enc[i].win_count;
            s_enc[i].win_count = total;
            int32_t pps = (int32_t)((delta * 1000U) / win);
            s_enc[i].speed_pps = (int16_t)(pps * s_enc[i].dir);
        }
        s_win_start_ms = now;
    }
}

uint32_t AMR_Encoder_GetTotal(EncoderSide_t side)    { return s_enc[side].total; }
int32_t  AMR_Encoder_GetOdometry(EncoderSide_t side) { return s_enc[side].odometry; }
int16_t  AMR_Encoder_GetSpeedPps(EncoderSide_t side) { return s_enc[side].speed_pps; }
bool     AMR_Encoder_IsStalled(EncoderSide_t side)   { return s_enc[side].stalled; }

int32_t AMR_Encoder_GetDistanceMm(EncoderSide_t side)
{
    /* mm = xung * pi * D / PPR ; pi ~ 355/113 */
    int64_t num = (int64_t)s_enc[side].odometry * 355 * (int64_t)WHEEL_DIAMETER_MM;
    return (int32_t)(num / (113 * (int64_t)ENCODER_PPR));
}

void AMR_Encoder_ClearStall(void)
{
    uint32_t now = HAL_GetTick();
    for (uint32_t i = 0U; i < ENC_COUNT; i++)
    {
        s_enc[i].stalled      = false;
        s_enc[i].stall_count  = s_enc[i].total;
        s_enc[i].stall_ref_ms = now;
    }
}
