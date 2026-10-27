/**
 ******************************************************************************
 * @file    amr_sonar.c
 * @brief   Hiện thực máy trạng thái quét TDM 3 cảm biến siêu âm HC-SR04 và bộ lọc trung vị 3 mẫu.
 * @details
 * Điều phối chu kỳ quét tuần tự theo thời gian thực:
 * - t = 0: Kích xung Trig mức HIGH trong 12 us, sau đó kéo xuống LOW.
 * - t xấp xỉ 0.45 ms: Chân Echo dựng lên HIGH sau khi module phát xong chùm 8 chu kỳ sóng 40 kHz.
 * - Chân Echo hạ xuống LOW: Đo khoảng thời gian xung phản xạ để tính khoảng cách theo vận tốc 343 m/s.
 * - t = 25 ms: Kết thúc khe thời gian hiện tại, tiêu tán hoàn toàn âm vang trước khi chuyển sang cảm biến kế tiếp.
 *
 * @version 2.1
 * @date    2026
 ******************************************************************************
 */
#include "amr_sonar.h"
#include "amr_time.h"

/**
 * @enum SonarState_t
 * @brief Các trạng thái trong chu trình quét của một khe thời gian cảm biến siêu âm.
 */
typedef enum
{
    /** @brief Trạng thái phát xung kích hoạt Trigger mức HIGH (12 µs). */
    SONAR_ST_TRIGGER = 0,

    /** @brief Trạng thái chờ cạnh lên (Rising Edge) của chân Echo phản hồi. */
    SONAR_ST_WAIT_RISE,

    /** @brief Trạng thái chờ cạnh xuống (Falling Edge) của chân Echo để đo độ rộng xung. */
    SONAR_ST_WAIT_FALL,

    /** @brief Trạng thái chờ hết khe thời gian (25 ms) để triệt tiêu sóng phản xạ dội âm. */
    SONAR_ST_WAIT_SLOT_END
} SonarState_t;

/**
 * @struct Sonar_t
 * @brief Cấu trúc dữ liệu cấu hình phần cứng và bộ đệm lịch sử đo đạc cho một cảm biến siêu âm.
 */
typedef struct
{
    /** @brief Cổng GPIO của chân phát xung Trigger. */
    GPIO_TypeDef *trig_port;

    /** @brief Chân GPIO của tín hiệu Trigger. */
    uint16_t      trig_pin;

    /** @brief Cổng GPIO của chân thu tín hiệu Echo. */
    GPIO_TypeDef *echo_port;

    /** @brief Chân GPIO của tín hiệu Echo. */
    uint16_t      echo_pin;

    /** @brief Mảng lưu 3 mẫu đo khoảng cách gần nhất dùng cho bộ lọc trung vị (Median Filter). */
    uint16_t hist[3];

    /** @brief Chỉ số vị trí ghi mẫu kế tiếp vào mảng lịch sử. */
    uint8_t  hist_idx;

    /** @brief Cờ đánh dấu mảng lịch sử đã được lấp đầy đủ 3 mẫu hay chưa. */
    bool     hist_filled;

    /** @brief Khoảng cách đo được thô ở lần lấy mẫu gần nhất chưa qua lọc (mm). */
    uint16_t raw_mm;

    /** @brief Khoảng cách đo được sau khi đã lọc trung vị 3 mẫu (mm). */
    uint16_t filt_mm;

    /** @brief Bộ đếm số chu kỳ liên tiếp không nhận được phản hồi xung Echo. */
    uint8_t  fail_count;

    /** @brief Cờ báo trạng thái lỗi phần cứng cảm biến (mất kết nối / hỏng module). */
    bool     faulty;
} Sonar_t;

static Sonar_t      s_sonar[SONAR_COUNT];
static SonarState_t s_state;
static uint8_t      s_active;           /* Cam bien dang trong khe       */
static uint32_t     s_slot_start_ms;
static uint16_t     s_t_trig_us;
static uint16_t     s_t_rise_us;
static uint32_t     s_cycles;

/* -------------------------------------------------------------------------- */
static inline bool echo_high(const Sonar_t *s)
{
    return (s->echo_port->IDR & s->echo_pin) != 0U;
}

static uint16_t median3(uint16_t a, uint16_t b, uint16_t c)
{
    if (a > b) { uint16_t t = a; a = b; b = t; }
    if (b > c) { uint16_t t = b; b = c; c = t; }
    if (a > b) { b = a; }
    return b;
}

static void sonar_push(Sonar_t *s, uint16_t mm)
{
    s->raw_mm = mm;
    if (!s->hist_filled)
    {
        s->hist[0] = s->hist[1] = s->hist[2] = mm;
        s->hist_filled = true;
    }
    else
    {
        s->hist[s->hist_idx] = mm;
    }
    s->hist_idx = (uint8_t)((s->hist_idx + 1U) % 3U);
    s->filt_mm  = median3(s->hist[0], s->hist[1], s->hist[2]);
}

static void sonar_echo_missing(Sonar_t *s)
{
    if (s->fail_count < 0xFFU) { s->fail_count++; }
    if (s->fail_count >= SONAR_FAIL_LIMIT)
    {
        s->faulty = true;
    }
}

/* -------------------------------------------------------------------------- */
void AMR_Sonar_Init(void)
{
    s_sonar[SONAR_LEFT]  = (Sonar_t){ L_TRIG_GPIO_Port, L_TRIG_Pin, L_ECHO_GPIO_Port, L_ECHO_Pin };
    s_sonar[SONAR_MID]   = (Sonar_t){ M_TRIG_GPIO_Port, M_TRIG_Pin, M_ECHO_GPIO_Port, M_ECHO_Pin };
    s_sonar[SONAR_RIGHT] = (Sonar_t){ R_TRIG_GPIO_Port, R_TRIG_Pin, R_ECHO_GPIO_Port, R_ECHO_Pin };

    for (uint32_t i = 0U; i < SONAR_COUNT; i++)
    {
        s_sonar[i].trig_port->BSRR = ((uint32_t)s_sonar[i].trig_pin << 16U);
        s_sonar[i].raw_mm  = SONAR_DIST_FAR_MM;
        s_sonar[i].filt_mm = SONAR_DIST_FAR_MM;
    }

    s_active = 0U;
    s_state  = SONAR_ST_TRIGGER;
    s_slot_start_ms = HAL_GetTick();
}

void AMR_Sonar_Process(void)
{
    Sonar_t *s = &s_sonar[s_active];
    uint32_t slot_ms = AMR_Time_ElapsedMs(s_slot_start_ms);

    switch (s_state)
    {
    case SONAR_ST_TRIGGER:
        s_slot_start_ms = HAL_GetTick();
        s->trig_port->BSRR = s->trig_pin;
        AMR_Time_DelayUs(SONAR_TRIG_PULSE_US);
        s->trig_port->BSRR = ((uint32_t)s->trig_pin << 16U);
        s_t_trig_us = AMR_Time_Us();
        s_state = SONAR_ST_WAIT_RISE;
        break;

    case SONAR_ST_WAIT_RISE:
        if (echo_high(s))
        {
            s_t_rise_us = AMR_Time_Us();
            s_state = SONAR_ST_WAIT_FALL;
        }
        else if ((AMR_Time_ElapsedUs(s_t_trig_us) > SONAR_RISE_TIMEOUT_US) ||
                 (slot_ms > SONAR_SLOT_MS))
        {
            sonar_echo_missing(s);          /* Cam bien khong phan hoi */
            s_state = SONAR_ST_WAIT_SLOT_END;
        }
        break;

    case SONAR_ST_WAIT_FALL:
    {
        uint16_t width = AMR_Time_ElapsedUs(s_t_rise_us);
        if (!echo_high(s))
        {
            s->fail_count = 0U;
            s->faulty = false;
            uint32_t mm = ((uint32_t)width * 343U) / 2000U;
            if (mm < SONAR_MIN_MM) { mm = SONAR_MIN_MM; }
            sonar_push(s, (mm > SONAR_MAX_MM) ? SONAR_DIST_FAR_MM : (uint16_t)mm);
            s_state = SONAR_ST_WAIT_SLOT_END;
        }
        else if ((width > SONAR_ECHO_MAX_US) || (slot_ms > SONAR_SLOT_MS))
        {
            /* Xung qua dai: khong co vat can trong tam do. */
            s->fail_count = 0U;
            s->faulty = false;
            sonar_push(s, SONAR_DIST_FAR_MM);
            s_state = SONAR_ST_WAIT_SLOT_END;
        }
        break;
    }

    case SONAR_ST_WAIT_SLOT_END:
    default:
        if (slot_ms >= SONAR_SLOT_MS)
        {
            s_active = (uint8_t)((s_active + 1U) % SONAR_COUNT);
            if (s_active == 0U) { s_cycles++; }
            s_state = SONAR_ST_TRIGGER;
        }
        break;
    }
}

uint16_t AMR_Sonar_GetDistanceMm(SonarId_t id) { return s_sonar[id].filt_mm; }
uint16_t AMR_Sonar_GetRawMm(SonarId_t id)      { return s_sonar[id].raw_mm;  }
bool     AMR_Sonar_IsFaulty(SonarId_t id)      { return s_sonar[id].faulty;  }
uint32_t AMR_Sonar_GetCycleCount(void)         { return s_cycles;            }
