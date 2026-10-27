/**
 ******************************************************************************
 * @file    amr_time.h
 * @brief   Module định thời microsecond độc lập sử dụng Timer 4 (TIM4).
 * @details
 * Cung cấp bộ đếm thời gian độ phân giải cao 1 µs phục vụ các tác vụ đo đạc
 * thời gian thực đòi hỏi độ chính xác cao:
 * - Đo độ rộng xung Echo của cảm biến siêu âm HC-SR04.
 * - Lọc chống nảy xung (glitch filter) cho tín hiệu ngắt ngoài Encoder quang LM393.
 * - Đo lường thời gian thực thi (execution latency) của các chu kỳ vòng lặp chính.
 *
 * ### Đặc tính kỹ thuật phần cứng:
 * - **Ngoại vi sử dụng**: Bộ định thời TIM4 cấu hình trực tiếp qua thanh ghi phần cứng (Direct Register Access),
 *   hoàn toàn độc lập và không phụ thuộc vào mã nguồn tự sinh của STM32CubeMX.
 * - **Tần số đếm**: 1 MHz (Xung nhịp nguồn 72 MHz chia qua Prescaler PSC = 71).
 * - **Độ phân giải thanh ghi**: 16-bit, tràn tự nhiên sau mỗi 65.535 ms (65535 µs).
 * - **Cơ chế đo khoảng thời gian**: Sử dụng tính chất toán học của phép trừ số nguyên không dấu 16-bit (`uint16_t`),
 *   tự động xử lý chính xác hiện tượng tràn số khi khoảng thời gian cần đo nhỏ hơn 65.535 ms.
 *
 * @note Lý do lựa chọn TIM4 thay vì thanh ghi chu kỳ lệnh DWT (DWT->CYCCNT):
 *       Nhiều bo mạch STM32F103 Blue Pill trên thị trường sử dụng các vi điều khiển tương thích (CS32F103, CKS32F103, GD32F103)
 *       không tích hợp khối DWT của ARM Cortex-M3 tiêu chuẩn. Việc sử dụng TIM4 đảm bảo tính tương thích phần cứng 100%.
 *
 * @warning Tuyệt đối KHÔNG kích hoạt TIM4 trong tập tin cấu hình đồ họa `agv-low-level-peripherals.ioc` của CubeMX
 *          để tránh xung đột tài nguyên thanh ghi.
 * @version 2.1
 * @date    2026
 ******************************************************************************
 */

#ifndef INC_AMR_TIME_H_
#define INC_AMR_TIME_H_

#include "main.h"
#include <stdint.h>

/**
 * @brief Khởi tạo và kích hoạt bộ định thời TIM4 ở chế độ đếm tự do 1 MHz.
 * @details
 * Cấu hình trực tiếp các thanh ghi ngoại vi của TIM4:
 * 1. Bật xung nhịp cung cấp (Clock Gating) cho TIM4 trên bus APB1 (`RCC->APB1ENR |= RCC_APB1ENR_TIM4EN`).
 * 2. Thiết lập giá trị chia tần số Prescaler: `TIM4->PSC = 71` để đưa tần số 72 MHz về 1 MHz (mỗi nhịp đếm tương ứng 1 µs).
 * 3. Đặt giá trị nạp lại tự động cực đại: `TIM4->ARR = 0xFFFF` (65535).
 * 4. Kích hoạt bộ đếm thời gian: `TIM4->CR1 |= TIM_CR1_CEN`.
 *
 * @note Hàm này chỉ cần gọi một lần duy nhất trong quá trình khởi tạo hệ thống tại @ref AMR_App_Init.
 */
void AMR_Time_Init(void);

/**
 * @brief Đọc giá trị tức thời hiện tại của bộ đếm microsecond (16-bit).
 * @return Giá trị thanh ghi bộ đếm `TIM4->CNT` tại thời điểm gọi hàm.
 * @par Đơn vị:
 * Microseconds (us).
 * @note Giá trị này sẽ tự động quay vòng về 0 sau mỗi 65.535 ms.
 */
static inline uint16_t AMR_Time_Us(void)
{
    return (uint16_t)TIM4->CNT;
}

/**
 * @brief Tính toán khoảng thời gian trôi qua (Microseconds) kể từ một mốc thời gian trước đó.
 * @details
 * Sử dụng phép trừ modulo số nguyên không dấu 16-bit:
 * @f[\Delta t = (\text{uint16\_t})(t_{\text{now}} - t_{\text{since}})@f]
 * Phép toán này đảm bảo kết quả luôn hoàn toàn chính xác ngay cả khi bộ đếm `TIM4->CNT` đã bị tràn qua mốc 0,
 * với điều kiện khoảng thời gian cần đo nhỏ hơn chu kỳ tràn cực đại (65.535 ms).
 *
 * @param[in] since Mốc thời gian bắt đầu đo (đọc từ @ref AMR_Time_Us).
 * @return Khoảng thời gian đã trôi qua tính bằng microsecond (us).
 */
static inline uint16_t AMR_Time_ElapsedUs(uint16_t since)
{
    return (uint16_t)(AMR_Time_Us() - since);
}

/**
 * @brief Tạo khoảng thời gian trễ tích cực ngắn (Busy-wait delay) theo đơn vị microsecond.
 * @details
 * Thực hiện vòng lặp đọc liên tục giá trị `AMR_Time_Us()` cho tới khi khoảng thời gian trôi qua đạt đủ yêu cầu.
 *
 * @param[in] us Khoảng thời gian trễ cần tạo (Microseconds).
 *
 * @warning Hàm này chiếm dụng 100% tài nguyên CPU trong suốt thời gian trễ.
 *          Chỉ sử dụng cho các xung kích cực ngắn (<= 50 µs), chẳng hạn như xung kích chân Trigger của module HC-SR04.
 *          Tuyệt đối KHÔNG sử dụng hàm này cho các khoảng trễ lớn hơn 100 µs để tránh gây trễ vòng lặp chính.
 */
void AMR_Time_DelayUs(uint16_t us);

/**
 * @brief Tính khoảng thời gian trôi qua (Milliseconds) kể từ mốc thời gian SysTick.
 * @details
 * Dựa trên bộ đếm hệ thống 32-bit `HAL_GetTick()`. Nhờ vào số nguyên không dấu 32-bit (`uint32_t`),
 * hàm này tự động xử lý chính xác hiện tượng tràn số nguyên sau chu kỳ hoạt động liên tục xấp xỉ 49.7 ngày.
 *
 * @param[in] since Mốc thời gian bắt đầu đo (đọc từ `HAL_GetTick()`).
 * @return Khoảng thời gian đã trôi qua tính bằng millisecond (ms).
 */
static inline uint32_t AMR_Time_ElapsedMs(uint32_t since)
{
    return HAL_GetTick() - since;
}

#endif /* INC_AMR_TIME_H_ */
