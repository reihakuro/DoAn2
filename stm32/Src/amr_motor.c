/**
 ******************************************************************************
 * @file    amr_motor.c
 * @brief   Hiện thực module điều khiển cầu H L298N, thuật toán tăng tốc độ dốc và phanh khẩn cấp.
 * @details
 * Quản lý trực tiếp các thanh ghi Capture/Compare Register (CCR) của Timer 2 và chân GPIO điều khiển hướng,
 * thực hiện bảo vệ quá dòng/sụt áp thông qua thuật toán phần mềm Slew-rate Ramp Limiter.
 *
 * ### Bảng sự thật trạng thái cầu H L298N (cho mỗi kênh):
 * | EN (PWM) | IN1 | IN2 | Chế độ hoạt động |
 * | :---: | :---: | :---: | :--- |
 * | PWM | 1 | 0 | Chạy tiến (vận tốc tỷ lệ thuận theo Duty Cycle) |
 * | PWM | 0 | 1 | Chạy lùi (vận tốc tỷ lệ thuận theo Duty Cycle) |
 * | 1 | 0 | 0 (hoặc 1, 1) | Phanh điện động học (nối tắt 2 đầu động cơ xuống GND) |
 * | 0 | x | x | Thả trôi tự do (Coast mode, tiêu tán nhiệt) |
 *
 * @version 2.1
 * @date    2026
 ******************************************************************************
 */
#include "amr_motor.h"
#include "amr_time.h"
#include "tim.h"

/**
 * @struct MotorChannel_t
 * @brief Cấu trúc dữ liệu cấu hình phần cứng và trạng thái điều khiển cho một kênh động cơ.
 */
typedef struct
{
    /** @brief Con trỏ trỏ trực tiếp tới thanh ghi CCR của kênh PWM tương ứng trên Timer 2. */
    volatile uint32_t *ccr;

    /** @brief Cổng GPIO chứa 2 chân điều khiển logic chiều quay (IN1/IN2 hoặc IN3/IN4). */
    GPIO_TypeDef      *port;

    /** @brief Chân GPIO logic chiều thứ nhất (IN1 hoặc IN3). */
    uint16_t           pin_a;

    /** @brief Chân GPIO logic chiều thứ hai (IN2 hoặc IN4). */
    uint16_t           pin_b;

    /** @brief Giá trị CCR có dấu mục tiêu cần đạt tới sau khi qua bộ kẹp biên an toàn. */
    int16_t            target;

    /** @brief Giá trị CCR có dấu đang xuất thực tế xuống thanh ghi sau khi qua bộ lọc dốc ramp. */
    int16_t            current;

    /** @brief Giá trị lệnh vận tốc người dùng/tầng trên yêu cầu trong dải chuẩn hóa `[-100, +100]`. */
    int16_t            cmd;

    /** @brief Cờ cấu hình đảo ngược chiều quay logic bằng phần mềm. */
    bool               invert;
} MotorChannel_t;

static MotorChannel_t s_left;
static MotorChannel_t s_right;

static bool     s_inhibit         = false;
static bool     s_forward_blocked = false;
static bool     s_braking         = false;
static uint32_t s_brake_start_ms  = 0U;

/* -------------------------------------------------------------------------- */
static int16_t clamp_i16(int16_t v, int16_t lo, int16_t hi)
{
    return (v < lo) ? lo : ((v > hi) ? hi : v);
}

/** Xuat trang thai phan cung cho 1 kenh tu gia tri CCR co dau. */
static void motor_apply(MotorChannel_t *m, int16_t signed_ccr)
{
    /* Phong thu nhieu lop: kep lan cuoi ngay truoc khi ghi thanh ghi. */
    signed_ccr = clamp_i16(signed_ccr, -PWM_DUTY_MAX_LIMIT, PWM_DUTY_MAX_LIMIT);

    int16_t hw = m->invert ? (int16_t)(-signed_ccr) : signed_ccr;

    if (hw > 0)
    {
        m->port->BSRR = (uint32_t)m->pin_a | ((uint32_t)m->pin_b << 16U);
        *m->ccr = (uint32_t)hw;
    }
    else if (hw < 0)
    {
        m->port->BSRR = (uint32_t)m->pin_b | ((uint32_t)m->pin_a << 16U);
        *m->ccr = (uint32_t)(-hw);
    }
    else
    {
        /* 0: tha troi - ca 2 chan chieu LOW, EN = 0 */
        m->port->BSRR = ((uint32_t)(m->pin_a | m->pin_b) << 16U);
        *m->ccr = 0U;
    }
}

/** Phanh dien: IN_a = IN_b = LOW, EN = duty tran -> 2 dau dong co noi GND. */
static void motor_brake(MotorChannel_t *m)
{
    m->port->BSRR = ((uint32_t)(m->pin_a | m->pin_b) << 16U);
    *m->ccr = (uint32_t)PWM_DUTY_MAX_LIMIT;
}

static int16_t ramp_toward(int16_t current, int16_t target)
{
    if (current < target)
    {
        current = (int16_t)(current + PWM_RAMP_STEP);
        if (current > target) { current = target; }
    }
    else if (current > target)
    {
        current = (int16_t)(current - PWM_RAMP_STEP);
        if (current < target) { current = target; }
    }
    return current;
}

static int16_t cmd_to_ccr(int16_t cmd)
{
    cmd = clamp_i16(cmd, -MOTOR_CMD_MAX, MOTOR_CMD_MAX);
    return (int16_t)(((int32_t)cmd * PWM_DUTY_MAX_LIMIT) / MOTOR_CMD_MAX);
}

/** Tinh lai target tu lenh + cac dieu kien khoa an toan. */
static void motor_recompute_targets(void)
{
    int16_t l = s_left.cmd;
    int16_t r = s_right.cmd;

    if (s_inhibit || s_braking)
    {
        l = 0;
        r = 0;
    }
    else if (s_forward_blocked && ((int32_t)l + (int32_t)r) > 0)
    {
        l = 0;
        r = 0;
    }

    s_left.target  = cmd_to_ccr(l);
    s_right.target = cmd_to_ccr(r);
}

/* -------------------------------------------------------------------------- */
void AMR_Motor_Init(void)
{
    s_left  = (MotorChannel_t){ &TIM2->CCR1, MOTOR_IN1_GPIO_Port, MOTOR_IN1_Pin,
                                MOTOR_IN2_Pin, 0, 0, 0, (MOTOR_LEFT_INVERT != 0) };
    s_right = (MotorChannel_t){ &TIM2->CCR4, MOTOR_IN3_GPIO_Port, MOTOR_IN3_Pin,
                                MOTOR_IN4_Pin, 0, 0, 0, (MOTOR_RIGHT_INVERT != 0) };

    motor_apply(&s_left, 0);
    motor_apply(&s_right, 0);

    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
}

void AMR_Motor_SetSpeed(int16_t left, int16_t right)
{
    s_left.cmd  = clamp_i16(left,  -MOTOR_CMD_MAX, MOTOR_CMD_MAX);
    s_right.cmd = clamp_i16(right, -MOTOR_CMD_MAX, MOTOR_CMD_MAX);
    motor_recompute_targets();
}

void AMR_Motor_Update(void)
{
    if (s_braking)
    {
        if (AMR_Time_ElapsedMs(s_brake_start_ms) < MOTOR_BRAKE_HOLD_MS)
        {
            return;                         /* Giu phanh */
        }
        s_braking = false;
        motor_apply(&s_left, 0);            /* Chuyen sang tha troi */
        motor_apply(&s_right, 0);
    }

    motor_recompute_targets();

    s_left.current  = ramp_toward(s_left.current,  s_left.target);
    s_right.current = ramp_toward(s_right.current, s_right.target);

    motor_apply(&s_left,  s_left.current);
    motor_apply(&s_right, s_right.current);
}

void AMR_Motor_EmergencyStop(void)
{
    /* Lenh (cmd) KHONG bi xoa o day: cac co khoa (inhibit / forward-blocked)
     * se quyet dinh co cho chay lai hay khong. Muon xoa lenh -> SetSpeed(0,0). */
    s_left.target = s_right.target = 0;

    /* Chi phanh khi dang thuc su quay; neu dang dung yen thi tha troi luon. */
    if ((s_left.current != 0) || (s_right.current != 0))
    {
        s_left.current = s_right.current = 0;
        motor_brake(&s_left);
        motor_brake(&s_right);
        if (!s_braking)
        {
            s_braking = true;
            s_brake_start_ms = HAL_GetTick();
        }
    }
}

void AMR_Motor_SetInhibit(bool inhibit)
{
    s_inhibit = inhibit;
    motor_recompute_targets();
}

void AMR_Motor_SetForwardBlocked(bool blocked)
{
    s_forward_blocked = blocked;
    motor_recompute_targets();
}

int16_t AMR_Motor_GetPwmLeft(void)  { return s_left.current;  }
int16_t AMR_Motor_GetPwmRight(void) { return s_right.current; }
int16_t AMR_Motor_GetCmdLeft(void)  { return s_left.cmd;      }
int16_t AMR_Motor_GetCmdRight(void) { return s_right.cmd;     }
bool    AMR_Motor_IsBraking(void)   { return s_braking;       }
