/**
 ******************************************************************************
 * @file    amr_power.c
 * @brief   Hiện thực module quản lý chốt nguồn thông minh (Power Latch) và tắt máy an toàn.
 * @details
 * Quản lý máy trạng thái ngắt nguồn kết hợp với Raspberry Pi:
 * - Đọc và lọc chống rung tiếp điểm cơ khí cho nút nhấn nguồn SW1 (PB1).
 * - Giải mã cử chỉ nhấn nhả nhanh (Self-Test) và nhấn giữ (Power-Off).
 * - Lọc tín hiệu xác nhận tắt máy (GPIO26 Halt) từ Raspberry Pi qua chân PB10.
 * - Điều khiển chân giữ nguồn PB0 để đóng/ngắt P-MOSFET nguồn chính.
 *
 * @version 2.1
 * @date    2026
 ******************************************************************************
 */
#include "amr_power.h"
#include "amr_time.h"

static PowerState_t  s_state;
static PowerReason_t s_reason;
static uint32_t      s_state_since_ms;

/* --- Nut SW1 --- */
static bool     s_btn_raw_prev;
static bool     s_btn_stable;           /* true = dang nhan (da chong doi)   */
static uint32_t s_btn_change_ms;
static bool     s_btn_armed;            /* Chi nhan su kien sau khi da nha nut lan dau */
static uint32_t s_btn_press_ms;
static bool     s_btn_long_fired;

/* --- Tin hieu Halt cua Pi --- */
static bool     s_halt_stable;
static bool     s_halt_raw_prev;
static uint32_t s_halt_change_ms;

/* -------------------------------------------------------------------------- */
static inline bool btn_raw_pressed(void)
{
    return HAL_GPIO_ReadPin(PWR_BTN_GPIO_Port, PWR_BTN_Pin) == GPIO_PIN_RESET;
}

static inline bool halt_raw(void)
{
    return HAL_GPIO_ReadPin(PI_HALT_GPIO_Port, PI_HALT_Pin) == GPIO_PIN_SET;
}

static void set_state(PowerState_t st)
{
    s_state = st;
    s_state_since_ms = HAL_GetTick();
}

static ButtonEvent_t button_update(uint32_t now)
{
    ButtonEvent_t ev = BTN_EVENT_NONE;
    bool raw = btn_raw_pressed();

    if (raw != s_btn_raw_prev)
    {
        s_btn_raw_prev  = raw;
        s_btn_change_ms = now;
    }

    if ((raw != s_btn_stable) && ((now - s_btn_change_ms) >= PWR_BTN_DEBOUNCE_MS))
    {
        s_btn_stable = raw;
        if (raw)
        {
            s_btn_press_ms   = now;
            s_btn_long_fired = false;
        }
        else
        {
            /* Lan nha nut dau tien sau khi bat nguon chi de "vu trang" nut,
             * vi nguoi dung dang giu SW1 de bat may luc khoi dong. */
            if (s_btn_armed && !s_btn_long_fired &&
                ((now - s_btn_press_ms) < PWR_BTN_SHORT_MAX_MS))
            {
                ev = BTN_EVENT_SHORT;
            }
            s_btn_armed = true;
        }
    }

    if (s_btn_armed && s_btn_stable && !s_btn_long_fired &&
        ((now - s_btn_press_ms) >= PWR_BTN_LONG_MS))
    {
        s_btn_long_fired = true;
        ev = BTN_EVENT_LONG;
    }
    return ev;
}

static void halt_update(uint32_t now)
{
    bool raw = halt_raw();
    if (raw != s_halt_raw_prev)
    {
        s_halt_raw_prev  = raw;
        s_halt_change_ms = now;
    }
    if ((now - s_halt_change_ms) >= PI_HALT_DEBOUNCE_MS)
    {
        s_halt_stable = raw;
    }
}

/* -------------------------------------------------------------------------- */
void AMR_Power_Init(void)
{
    /* Dam bao chot nguon (MX_GPIO_Init da dat HIGH, ghi lai de chac chan). */
    HAL_GPIO_WritePin(PWR_HOLD_GPIO_Port, PWR_HOLD_Pin, GPIO_PIN_SET);

    uint32_t now = HAL_GetTick();
    s_reason = PWR_REASON_NONE;
    set_state(PWR_STATE_RUNNING);

    s_btn_raw_prev   = btn_raw_pressed();
    s_btn_stable     = s_btn_raw_prev;
    s_btn_change_ms  = now;
    s_btn_armed      = !s_btn_stable;   /* Neu luc boot nut khong bi giu -> vu trang ngay */
    s_btn_press_ms   = now;
    s_btn_long_fired = false;

    s_halt_raw_prev  = halt_raw();
    s_halt_stable    = false;
    s_halt_change_ms = now;
}

ButtonEvent_t AMR_Power_Update(void)
{
    uint32_t now = HAL_GetTick();
    ButtonEvent_t ev = button_update(now);
    halt_update(now);

    switch (s_state)
    {
    case PWR_STATE_RUNNING:
        if (ev == BTN_EVENT_LONG)
        {
            AMR_Power_RequestShutdown(PWR_REASON_BUTTON);
        }
        else if (s_halt_stable)
        {
            /* Pi tu tat (vd: `sudo poweroff` qua SSH) -> cat nguon theo. */
            AMR_Power_RequestShutdown(PWR_REASON_PI_HALTED);
        }
        break;

    case PWR_STATE_SHUTDOWN_REQUESTED:
        if (s_halt_stable || ((now - s_state_since_ms) >= PI_HALT_TIMEOUT_MS))
        {
            set_state(PWR_STATE_HALT_SETTLE);
        }
        break;

    case PWR_STATE_HALT_SETTLE:
        if ((now - s_state_since_ms) >= PI_HALT_SETTLE_MS)
        {
            HAL_GPIO_WritePin(PWR_HOLD_GPIO_Port, PWR_HOLD_Pin, GPIO_PIN_RESET);
            set_state(PWR_STATE_POWER_OFF);
        }
        break;

    case PWR_STATE_POWER_OFF:
    default:
        /* Neu MCU van con song o day: nguoi dung dang giu SW1 hoac dang
         * cap nguon USB/ST-Link. Giu PB0 LOW, cho mat nguon / reset. */
        HAL_GPIO_WritePin(PWR_HOLD_GPIO_Port, PWR_HOLD_Pin, GPIO_PIN_RESET);
        break;
    }

    /* Khi dang tat may, khong chuyen tiep su kien nut cho ung dung. */
    return (s_state == PWR_STATE_RUNNING) ? ev : BTN_EVENT_NONE;
}

void AMR_Power_RequestShutdown(PowerReason_t reason)
{
    if (s_state != PWR_STATE_RUNNING) { return; }
    s_reason = reason;
    /* Pi da Halt san -> bo qua buoc cho, vao thang giai doan settle. */
    set_state((reason == PWR_REASON_PI_HALTED) ? PWR_STATE_HALT_SETTLE
                                               : PWR_STATE_SHUTDOWN_REQUESTED);
}

PowerState_t  AMR_Power_GetState(void)       { return s_state;  }
PowerReason_t AMR_Power_GetReason(void)      { return s_reason; }
bool          AMR_Power_IsShuttingDown(void) { return s_state != PWR_STATE_RUNNING; }
bool          AMR_Power_IsPiHalted(void)     { return s_halt_stable; }
