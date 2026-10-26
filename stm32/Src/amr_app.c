/**
 ******************************************************************************
 * @file    amr_app.c
 * @brief   Supervisor Giai doan 1 - vong lap hop tac (cooperative super-loop).
 *
 *  Lich thuc thi:
 *    Moi vong lap  : AMR_Sonar_Process() (polling Echo do chinh xac us),
 *                    AMR_Debug_Pump()
 *    Moi 10 ms     : Nut/Nguon -> Pin -> Encoder/Stall -> Vat can -> Self-Test
 *                    -> Dong co (ramp) -> Coi
 *    Moi 200 ms    : Log telemetry qua UART (neu bat)
 *
 *  Thu tu uu tien an toan (cao -> thap):
 *    1. Dang tat may          : khoa dong co hoan toan
 *    2. Ket banh (latched)    : phanh + khoa, cho nguoi xoa loi
 *    3. Vat can phia truoc    : phanh + chan rieng chieu tien, tu nha khi het
 ******************************************************************************
 */
#include "amr_app.h"
#include "amr_time.h"
#include "amr_motor.h"
#include "amr_encoder.h"
#include "amr_sonar.h"
#include "amr_battery.h"
#include "amr_buzzer.h"
#include "amr_power.h"
#include "amr_debug.h"

#define APP_TICK_MS             10U
#define BAT_LOW_BEEP_PERIOD_MS  5000U

volatile AmrTelemetry_t g_amr;

static uint32_t s_tick_ms;
static uint32_t s_dbg_ms;
static uint32_t s_bat_beep_ms;
static uint16_t s_obstacle_stop_mm = OBSTACLE_STOP_MM_DEFAULT;
static bool     s_obstacle;
static bool     s_stall_latched;
static bool     s_shutdown_handled;
static uint32_t s_flags;

/* ===========================================================================
 *  SELF-TEST (Giai doan 1): kich ban chay thu khi nhan nhanh SW1
 *  -> KE GAM XE LEN CAO cho 4 banh quay tu do truoc khi thu!
 * ===========================================================================*/
typedef struct { int8_t left; int8_t right; uint16_t ms; } SelfTestStep_t;

static const SelfTestStep_t k_selftest[] =
{
    {   0,   0, 1000U },    /* 1: cho 1 s sau tieng bip                 */
    {  40,  40, 3000U },    /* 2: tien 40%                              */
    {   0,   0, 1000U },    /* 3: dung (quan sat ramp giam toc)         */
    { -40, -40, 3000U },    /* 4: lui 40%                               */
    {   0,   0, 1000U },    /* 5: dung                                  */
    { -40,  40, 2000U },    /* 6: quay trai tai cho                     */
    {   0,   0, 1000U },    /* 7: dung                                  */
    {  40, -40, 2000U },    /* 8: quay phai tai cho                     */
    { 100, 100, 2000U },    /* 9: tien 100% lenh = tran 55% duty        */
    {   0,   0, 1000U },    /* 10: dung                                 */
};
#define SELFTEST_STEPS  (sizeof(k_selftest) / sizeof(k_selftest[0]))

static uint8_t  s_st_step;      /* 0 = khong chay, 1..N = buoc dang chay */
static uint32_t s_st_step_ms;

static void selftest_start(void)
{
#if AMR_SELFTEST_ENABLE
    s_st_step = 1U;
    s_st_step_ms = HAL_GetTick();
    AMR_Debug_Write("[TEST] Bat dau Self-Test dong co\r\n");
#endif
}

static void selftest_stop(void)
{
    if (s_st_step != 0U)
    {
        s_st_step = 0U;
        AMR_Motor_SetSpeed(0, 0);
        AMR_Debug_Write("[TEST] Ket thuc Self-Test\r\n");
    }
}

static void selftest_update(void)
{
    if (s_st_step == 0U) { return; }

    if (AMR_Time_ElapsedMs(s_st_step_ms) >= k_selftest[s_st_step - 1U].ms)
    {
        s_st_step++;
        s_st_step_ms = HAL_GetTick();
        if (s_st_step > SELFTEST_STEPS)
        {
            selftest_stop();
            AMR_Buzzer_Play(BUZZ_SHORT);
            return;
        }
        AMR_Debug_Printf("[TEST] Buoc %u: L=%d R=%d\r\n", (unsigned)s_st_step,
                         (int)k_selftest[s_st_step - 1U].left,
                         (int)k_selftest[s_st_step - 1U].right);
    }
    const SelfTestStep_t *st = &k_selftest[s_st_step - 1U];
    AMR_Motor_SetSpeed(st->left, st->right);
}

/* ===========================================================================
 *  CAC KHOI GIAM SAT
 * ===========================================================================*/
static void set_flag(uint32_t flag, bool on)
{
    if (on) { s_flags |= flag; } else { s_flags &= ~flag; }
}

static void handle_button(ButtonEvent_t ev)
{
    if (ev != BTN_EVENT_SHORT) { return; }

    AMR_Buzzer_Play(BUZZ_SHORT);
    if (s_stall_latched)
    {
        AMR_App_ClearFaults();
    }
    else if (s_st_step != 0U)
    {
        selftest_stop();
    }
    else
    {
        selftest_start();
    }
}

static void supervise_power(void)
{
    if (AMR_Power_IsShuttingDown() && !s_shutdown_handled)
    {
        s_shutdown_handled = true;
        selftest_stop();
        AMR_Motor_SetSpeed(0, 0);
        AMR_Motor_SetInhibit(true);
        AMR_Motor_EmergencyStop();
        AMR_Buzzer_SetAlarm(false);
        AMR_Buzzer_Play(BUZZ_LONG);
        AMR_Debug_Printf("[PWR] Tat may, ly do=%u. Cho Pi Halt...\r\n",
                         (unsigned)AMR_Power_GetReason());
    }
    set_flag(AMR_FLAG_SHUTDOWN_PENDING, AMR_Power_IsShuttingDown());
}

static void supervise_battery(void)
{
    AMR_Battery_Update();
    BatteryState_t st = AMR_Battery_GetState();

    set_flag(AMR_FLAG_BAT_ABSENT,   st == BAT_STATE_ABSENT);
    set_flag(AMR_FLAG_BAT_LOW,      st == BAT_STATE_LOW);
    set_flag(AMR_FLAG_BAT_CRITICAL, st == BAT_STATE_CRITICAL);

    if (st == BAT_STATE_CRITICAL)
    {
        AMR_Power_RequestShutdown(PWR_REASON_BATTERY);
    }
    else if (st == BAT_STATE_LOW)
    {
        if (AMR_Time_ElapsedMs(s_bat_beep_ms) >= BAT_LOW_BEEP_PERIOD_MS)
        {
            s_bat_beep_ms = HAL_GetTick();
            AMR_Buzzer_Play(BUZZ_DOUBLE);
        }
    }
}

static void supervise_stall(void)
{
    AMR_Encoder_Update(AMR_Motor_GetPwmLeft(), AMR_Motor_GetPwmRight());

    bool sl = AMR_Encoder_IsStalled(ENC_LEFT);
    bool sr = AMR_Encoder_IsStalled(ENC_RIGHT);
    set_flag(AMR_FLAG_STALL_LEFT,  sl);
    set_flag(AMR_FLAG_STALL_RIGHT, sr);

    if ((sl || sr) && !s_stall_latched)
    {
        s_stall_latched = true;
        selftest_stop();
        AMR_Motor_SetSpeed(0, 0);
        AMR_Motor_SetInhibit(true);
        AMR_Motor_EmergencyStop();
        AMR_Debug_Printf("[SAFE] KET BANH! L=%u R=%u -> nhan nhanh SW1 de xoa\r\n",
                         (unsigned)sl, (unsigned)sr);
    }
}

static void supervise_obstacle(void)
{
    uint16_t mid   = AMR_Sonar_GetDistanceMm(SONAR_MID);
    bool     fault = AMR_Sonar_IsFaulty(SONAR_MID);

    set_flag(AMR_FLAG_SONAR_FAULT_L, AMR_Sonar_IsFaulty(SONAR_LEFT));
    set_flag(AMR_FLAG_SONAR_FAULT_M, fault);
    set_flag(AMR_FLAG_SONAR_FAULT_R, AMR_Sonar_IsFaulty(SONAR_RIGHT));

    bool fault_block = (OBSTACLE_BLOCK_ON_SONAR_FAULT != 0) && fault;
    bool was = s_obstacle;

    if (fault_block || (mid <= s_obstacle_stop_mm))
    {
        s_obstacle = true;
    }
    else if (mid > (uint16_t)(s_obstacle_stop_mm + OBSTACLE_HYSTERESIS_MM))
    {
        s_obstacle = false;
    }
    /* else: vung tre -> giu nguyen */

    if (s_obstacle && !was)
    {
        /* Chi phanh gap neu xe dang thuc su tien. */
        if (((int32_t)AMR_Motor_GetPwmLeft() + AMR_Motor_GetPwmRight()) > 0)
        {
            AMR_Motor_EmergencyStop();
        }
        AMR_Debug_Printf("[SAFE] Vat can phia truoc %u mm\r\n", (unsigned)mid);
    }
    AMR_Motor_SetForwardBlocked(s_obstacle);
    set_flag(AMR_FLAG_OBSTACLE, s_obstacle);
}

static void update_alarm(void)
{
    bool forward_wanted = ((int32_t)AMR_Motor_GetCmdLeft() + AMR_Motor_GetCmdRight()) > 0;
    bool alarm = !AMR_Power_IsShuttingDown() &&
                 (s_stall_latched || (s_obstacle && forward_wanted));
    AMR_Buzzer_SetAlarm(alarm);
}

static void update_telemetry(void)
{
    g_amr.uptime_ms       = HAL_GetTick();
    g_amr.fault_flags     = s_flags;
    g_amr.bat_mv          = AMR_Battery_GetMillivolts();
    g_amr.bat_raw         = AMR_Battery_GetRawAdc();
    g_amr.bat_state       = (uint8_t)AMR_Battery_GetState();
    g_amr.pwr_state       = (uint8_t)AMR_Power_GetState();
    g_amr.pwr_reason      = (uint8_t)AMR_Power_GetReason();
    g_amr.selftest_step   = s_st_step;
    g_amr.sonar_mm[0]     = AMR_Sonar_GetDistanceMm(SONAR_LEFT);
    g_amr.sonar_mm[1]     = AMR_Sonar_GetDistanceMm(SONAR_MID);
    g_amr.sonar_mm[2]     = AMR_Sonar_GetDistanceMm(SONAR_RIGHT);
    g_amr.cmd_left        = AMR_Motor_GetCmdLeft();
    g_amr.cmd_right       = AMR_Motor_GetCmdRight();
    g_amr.pwm_left        = AMR_Motor_GetPwmLeft();
    g_amr.pwm_right       = AMR_Motor_GetPwmRight();
    g_amr.speed_left_pps  = AMR_Encoder_GetSpeedPps(ENC_LEFT);
    g_amr.speed_right_pps = AMR_Encoder_GetSpeedPps(ENC_RIGHT);
    g_amr.enc_total_left  = AMR_Encoder_GetTotal(ENC_LEFT);
    g_amr.enc_total_right = AMR_Encoder_GetTotal(ENC_RIGHT);
    g_amr.dist_left_mm    = AMR_Encoder_GetDistanceMm(ENC_LEFT);
    g_amr.dist_right_mm   = AMR_Encoder_GetDistanceMm(ENC_RIGHT);
}

static void print_telemetry(void)
{
#if AMR_DEBUG_UART_ENABLE
    AMR_Debug_Printf("t=%lu VB=%umV(%u) US=%u/%u/%umm ENC=%lu/%lu SPD=%d/%d PWM=%d/%d F=0x%03lX LOOP=%luus\r\n",
                     (unsigned long)(g_amr.uptime_ms),
                     (unsigned)g_amr.bat_mv, (unsigned)g_amr.bat_state,
                     (unsigned)g_amr.sonar_mm[0], (unsigned)g_amr.sonar_mm[1],
                     (unsigned)g_amr.sonar_mm[2],
                     (unsigned long)g_amr.enc_total_left, (unsigned long)g_amr.enc_total_right,
                     (int)g_amr.speed_left_pps, (int)g_amr.speed_right_pps,
                     (int)g_amr.pwm_left, (int)g_amr.pwm_right,
                     (unsigned long)g_amr.fault_flags,
                     (unsigned long)g_amr.loop_max_us);
    g_amr.loop_max_us = 0U;     /* Do lai dinh moi chu ky log */
#endif
}

/* ===========================================================================
 *  API
 * ===========================================================================*/
void AMR_App_Init(void)
{
    AMR_Power_Init();               /* Dau tien: xac nhan chot nguon PB0   */
    AMR_Time_Init();
    AMR_Buzzer_Init();
    AMR_Motor_Init();
    AMR_Encoder_Init();
    AMR_Sonar_Init();
    AMR_Battery_Init();
    AMR_Debug_Init();

    s_tick_ms = s_dbg_ms = HAL_GetTick();
    s_bat_beep_ms = s_tick_ms - BAT_LOW_BEEP_PERIOD_MS;

    AMR_Buzzer_Play(BUZZ_SHORT);    /* Bao khoi dong xong */
    AMR_Debug_Write("\r\n[BOOT] AGV low-level firmware - Giai doan 1\r\n");
}

void AMR_App_Loop(void)
{
    uint16_t loop_start = AMR_Time_Us();

    /* --- Tac vu tan so cao (moi vong lap) --- */
    AMR_Sonar_Process();
    AMR_Debug_Pump();

    /* --- Tac vu 10 ms --- */
    if (AMR_Time_ElapsedMs(s_tick_ms) >= APP_TICK_MS)
    {
        s_tick_ms += APP_TICK_MS;
        if (AMR_Time_ElapsedMs(s_tick_ms) >= APP_TICK_MS)
        {
            s_tick_ms = HAL_GetTick();      /* Bi tre qua nhieu -> dong bo lai, khong don lenh */
        }

        handle_button(AMR_Power_Update());
        supervise_power();
        supervise_battery();
        supervise_stall();
        supervise_obstacle();
        if (!AMR_Power_IsShuttingDown() && !s_stall_latched)
        {
            selftest_update();
        }
        update_alarm();
        AMR_Motor_Update();
        AMR_Buzzer_Update();
        update_telemetry();
    }

    /* --- Tac vu 200 ms --- */
    if (AMR_Time_ElapsedMs(s_dbg_ms) >= AMR_DEBUG_PERIOD_MS)
    {
        s_dbg_ms = HAL_GetTick();
        print_telemetry();
    }

    uint32_t dt = AMR_Time_ElapsedUs(loop_start);
    if (dt > g_amr.loop_max_us) { g_amr.loop_max_us = dt; }
    g_amr.loop_count++;
}

void AMR_App_SetVelocity(int16_t left, int16_t right)
{
    if (AMR_Power_IsShuttingDown() || s_stall_latched) { return; }
    selftest_stop();
    AMR_Motor_SetSpeed(left, right);
}

void AMR_App_SetObstacleThreshold(uint16_t stop_mm)
{
    s_obstacle_stop_mm = stop_mm;
}

void AMR_App_ClearFaults(void)
{
    if (AMR_Power_IsShuttingDown()) { return; }
    AMR_Encoder_ClearStall();
    s_stall_latched = false;
    AMR_Motor_SetSpeed(0, 0);
    AMR_Motor_SetInhibit(false);
    AMR_Debug_Write("[SAFE] Da xoa loi\r\n");
}
