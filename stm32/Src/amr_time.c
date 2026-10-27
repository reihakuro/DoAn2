/**
 ******************************************************************************
 * @file    amr_time.c
 * @brief   Hiện thực bộ định thời vi giây độc lập sử dụng trực tiếp thanh ghi TIM4.
 * @details
 * Cấu hình tần số đếm 1 MHz, chế độ tự do và các hàm đọc thời gian, trễ xung vi giây.
 *
 * @version 2.1
 * @date    2026
 ******************************************************************************
 */
#include "amr_time.h"

void AMR_Time_Init(void)
{
    __HAL_RCC_TIM4_CLK_ENABLE();

    TIM4->CR1  = 0U;                    /* Dung timer, dem len, khong preload */
    TIM4->CR2  = 0U;
    TIM4->SMCR = 0U;                    /* Nguon clock noi (CK_INT)           */
    TIM4->DIER = 0U;                    /* Khong dung ngat                    */
    TIM4->CCER = 0U;                    /* Khong xuat kenh nao ra chan PB6..9 */
    /* Clock timer APB1: neu bo chia APB1 != 1 thi phan cung tu nhan 2
     * (cau hinh hien tai: PCLK1 = 36 MHz -> TIM clock = 72 MHz -> PSC = 71). */
    uint32_t tim_clk = HAL_RCC_GetPCLK1Freq();
    if ((RCC->CFGR & RCC_CFGR_PPRE1) != RCC_CFGR_PPRE1_DIV1)
    {
        tim_clk *= 2U;
    }
    TIM4->PSC  = (uint16_t)(tim_clk / 1000000U - 1U);
    TIM4->ARR  = 0xFFFFU;
    TIM4->EGR  = TIM_EGR_UG;            /* Nap PSC ngay lap tuc               */
    TIM4->SR   = 0U;
    TIM4->CR1  = TIM_CR1_CEN;           /* Chay                               */
}

void AMR_Time_DelayUs(uint16_t us)
{
    uint16_t start = AMR_Time_Us();
    while (AMR_Time_ElapsedUs(start) < us)
    {
        /* busy-wait */
    }
}
