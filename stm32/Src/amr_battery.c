/**
 ******************************************************************************
 * @file    amr_battery.c
 * @brief   Giam sat pin 4S: ADC 12-bit (da hieu chuan), loc IIR so nguyen,
 *          nguong co tre (hysteresis) va thoi gian xac nhan chong sut ap ao.
 *
 *  Vbat = raw * 3.3 / 4095 / Kdiv,  Kdiv = 3.3 / (18 + 3.3) = 0.15493
 *  -> 16.8 V (pin day) cho 2.60 V tai PA6 (< 3.3 V, an toan).
 ******************************************************************************
 */
#include "amr_battery.h"
#include "amr_time.h"
#include "adc.h"

static uint16_t       s_raw;
static uint32_t       s_filt_x8;            /* mV * 2^SHIFT (fixed-point)     */
static bool           s_has_sample;
static uint32_t       s_last_sample_ms;
static uint32_t       s_below_cutoff_since;
static bool           s_below_cutoff;
static BatteryState_t s_state = BAT_STATE_ABSENT;

/* -------------------------------------------------------------------------- */
static bool adc_read(uint16_t *out)
{
    if (HAL_ADC_Start(&hadc1) != HAL_OK)
    {
        return false;
    }
    /* 239.5 + 12.5 chu ky @ 12 MHz = 21 us -> timeout 1 ms la du. */
    if (HAL_ADC_PollForConversion(&hadc1, 1U) != HAL_OK)
    {
        HAL_ADC_Stop(&hadc1);
        return false;
    }
    *out = (uint16_t)HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);
    return true;
}

static uint32_t raw_to_mv(uint16_t raw)
{
    uint32_t mv = ((uint32_t)raw * BATTERY_MV_NUM) / BATTERY_MV_DEN;   /* max ~21.3 V */
    return (mv * BATTERY_CAL_X10000) / 10000UL;
}

static void update_state(uint32_t mv, uint32_t now)
{
    if (mv < BATTERY_ABSENT_MV)
    {
        s_state = BAT_STATE_ABSENT;
        s_below_cutoff = false;
        return;
    }

    /* Xac nhan cutoff: phai duoi nguong LIEN TUC BATTERY_CUTOFF_HOLD_MS. */
    if (mv <= BATTERY_CUTOFF_MV)
    {
        if (!s_below_cutoff)
        {
            s_below_cutoff = true;
            s_below_cutoff_since = now;
        }
    }
    else
    {
        s_below_cutoff = false;
    }

    if (s_state == BAT_STATE_CRITICAL)
    {
        return;                 /* Da chot CRITICAL -> chi co tat may moi thoat */
    }

    if (s_below_cutoff && ((now - s_below_cutoff_since) >= BATTERY_CUTOFF_HOLD_MS))
    {
        s_state = BAT_STATE_CRITICAL;
    }
    else if (mv <= BATTERY_WARN_MV)
    {
        s_state = BAT_STATE_LOW;
    }
    else if ((s_state != BAT_STATE_LOW) || (mv >= BATTERY_WARN_RELEASE_MV))
    {
        s_state = BAT_STATE_OK;
    }
    /* else: dang LOW va nam trong vung tre 13.6..13.9 V -> giu LOW */
}

/* -------------------------------------------------------------------------- */
void AMR_Battery_Init(void)
{
    /* Hieu chuan ADC noi (bat buoc voi STM32F1 de dat do chinh xac +-2 LSB). */
    (void)HAL_ADCEx_Calibration_Start(&hadc1);
    s_has_sample = false;
    s_last_sample_ms = HAL_GetTick() - BATTERY_SAMPLE_MS;   /* Lay mau ngay */
    AMR_Battery_Update();
}

void AMR_Battery_Update(void)
{
    uint32_t now = HAL_GetTick();
    if ((now - s_last_sample_ms) < BATTERY_SAMPLE_MS)
    {
        return;
    }
    s_last_sample_ms = now;

    uint16_t raw;
    if (!adc_read(&raw))
    {
        return;
    }
    s_raw = raw;

    uint32_t mv = raw_to_mv(raw);
    if (!s_has_sample)
    {
        s_filt_x8 = mv << BATTERY_IIR_SHIFT;    /* Khoi tao bo loc bang mau dau */
        s_has_sample = true;
    }
    else
    {
        /* y += (x - y) / 2^SHIFT  (tinh tren mien fixed-point, khong dung float) */
        s_filt_x8 = s_filt_x8 - (s_filt_x8 >> BATTERY_IIR_SHIFT) + mv;
    }

    update_state(s_filt_x8 >> BATTERY_IIR_SHIFT, now);
}

uint16_t AMR_Battery_GetMillivolts(void)
{
    return (uint16_t)(s_filt_x8 >> BATTERY_IIR_SHIFT);
}

uint16_t       AMR_Battery_GetRawAdc(void) { return s_raw;   }
BatteryState_t AMR_Battery_GetState(void)  { return s_state; }
