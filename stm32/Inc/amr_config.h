/**
 * @file    amr_config.h
 * @brief   Cấu hình chung cho firmware tầng thấp AGV (Giai đoạn 1).
 * @details
 * - Mỗi ngưỡng, thông số phần cứng, thời gian đều được khai báo ở đây.
 * - Các giá trị có thể được điều chỉnh để tối ưu hóa hiệu suất và độ ổn định của hệ thống.
 * - MCU: STM32F103C8T6 @ 72 MHz (HSE 8 MHz x PLL9).
 */

#ifndef INC_AMR_CONFIG_H_
#define INC_AMR_CONFIG_H_

#include "main.h"
#include <stdint.h>
#include <stdbool.h>

/**
 * @name 0. Cấu hình Debug & Telemetry
 * Thiết lập kiểm tra phần cứng và giám sát trạng thái hệ thống.
 *
 * @def AMR_SELFTEST_ENABLE
 * Chế độ Self-Test: `1` là chạy Debug động cơ, `0` là nhận lệnh từ RPi qua UART.
 *
 * @def AMR_DEBUG_UART_ENABLE
 * `1` là bật debug văn bản qua USART1 CP2102, `0` là tắt (mặc định).
 *
 * @def AMR_DEBUG_PERIOD_MS
 * Chu kỳ phát chuỗi dữ liệu telemetry định kỳ (ms).
 * @{
 */
#define AMR_SELFTEST_ENABLE             1
#define AMR_DEBUG_UART_ENABLE           1
#define AMR_DEBUG_PERIOD_MS             200U
/** @} */

/**
 * @name 1. Cấu hình Động cơ và PWM ban đầu
 * Thiết lập các thông số điều khiển động cơ và bộ tạo xung PWM Timer2.
 *
 * @def PWM_ARR_PERIOD
 * Giá trị nạp lại cho Timer2 tạo chu kỳ PWM 1 kHz (Timer clock 72 MHz, PSC = 71):
 * @f[\text{ARR} = \frac{72\text{ MHz}}{(71 + 1) \cdot 1\text{ kHz}} - 1 = 999@f]
 *
 * @def PWM_DUTY_MAX_LIMIT
 * Giới hạn tối đa ghi vào thanh ghi CCR không vượt quá 55% (~8.1 V) cho dải áp 6 V - 8 V.
 *
 * @def PWM_RAMP_STEP
 * Bước tăng/giảm tối đa giá trị CCR mỗi chu kỳ để tránh sụt áp và dòng khởi động lớn.
 *
 * @def PWM_RAMP_PERIOD_MS
 * Chu kỳ thời gian cập nhật tăng/giảm gia tốc (ms).
 *
 * @def MOTOR_CMD_MAX
 * Chuẩn hoá thang lệnh vận tốc đầu vào từ -100 đến +100.
 *
 * @def MOTOR_BRAKE_HOLD_MS
 * Thời gian nối tắt 2 đầu động cơ qua L298N để phanh điện (ms) trước khi thả trôi.
 *
 * @def MOTOR_LEFT_INVERT
 * Đảo chiều quay động cơ trái (`1`: đảo chiều, `0`: giữ nguyên).
 *
 * @def MOTOR_RIGHT_INVERT
 * Đảo chiều quay động cơ phải (`1`: đảo chiều, `0`: giữ nguyên).
 * @{
 */
#define PWM_ARR_PERIOD                  999U
#define PWM_DUTY_MAX_LIMIT              550
#define PWM_RAMP_STEP                   25
#define PWM_RAMP_PERIOD_MS              10U
#define MOTOR_CMD_MAX                   100
#define MOTOR_BRAKE_HOLD_MS             300U
#define MOTOR_LEFT_INVERT               0
#define MOTOR_RIGHT_INVERT              0
/** @} */

/**
 * @name 2. Cấu hình Encoder quang LM393
 * Thiết lập bộ đếm xung và cơ chế bảo vệ chống kẹt bánh (Stall Protection).
 *
 * @def ENCODER_PPR
 * Số lượng xung đếm được trên một vòng quay bánh xe (Pulse Per Revolution).
 *
 * @def WHEEL_DIAMETER_MM
 * Đường kính bánh xe cao su (mm) dùng tính vận tốc dài:
 * @f[v_{\text{linear}} = \frac{\pi \cdot D_{\text{wheel}} \cdot f_{\text{pulse}}}{\text{PPR}}@f]
 *
 * @def ENCODER_GLITCH_US
 * Cửa sổ thời gian lọc bỏ nhiễu cơ học không trễ của LM393 (us).
 *
 * @def ENCODER_SPEED_WINDOW_MS
 * Chu kỳ lấy mẫu gom xung để tính vận tốc bánh xe (ms).
 *
 * @def STALL_CCR_THRESHOLD
 * Ngưỡng PWM tối thiểu bắt đầu kích hoạt thuật toán theo dõi kẹt động cơ.
 *
 * @def STALL_TIMEOUT_MS
 * Thời gian kẹt liên tục (ms) để kích hoạt ngắt ga, khóa an toàn và hú còi cảnh báo.
 * @{
 */
#define ENCODER_PPR                     20U
#define WHEEL_DIAMETER_MM               65U
#define ENCODER_GLITCH_US               1500U
#define ENCODER_SPEED_WINDOW_MS         100U
#define STALL_CCR_THRESHOLD             200
#define STALL_TIMEOUT_MS                500U
/** @} */

/**
 * @name 3. Cấu hình cảm biến siêu âm HC-SR04
 * Quản lý chu kỳ quét TDM và thuật toán phanh dừng khẩn cấp.
 *
 * @def SONAR_COUNT
 * Tổng số lượng cảm biến siêu âm gắn trên thân xe.
 *
 * @def SONAR_SLOT_MS
 * Khe thời gian quét TDM cho mỗi cảm biến (ms). Tổng chu kỳ quét = SONAR_COUNT * SONAR_SLOT_MS.
 *
 * @def SONAR_TRIG_PULSE_US
 * Thời gian kích xung chân Trig phát sóng siêu âm (yêu cầu >= 10 us).
 *
 * @def SONAR_RISE_TIMEOUT_US
 * Thời gian chờ tối đa cho cạnh lên chân Echo (us). Vượt quá xem như lỗi cảm biến.
 *
 * @def SONAR_ECHO_MAX_US
 * Thời gian chờ tối đa cho cạnh xuống chân Echo (us) tương ứng không gian trống.
 *
 * @def SONAR_MIN_MM
 * Khoảng cách vật lý tối thiểu đo được (mm).
 *
 * @def SONAR_MAX_MM
 * Khoảng cách vật lý tối đa đo được (mm).
 *
 * @def SONAR_FAIL_LIMIT
 * Số chu kỳ mất tín hiệu Echo liên tiếp để xác nhận lỗi phần cứng cảm biến.
 *
 * @def OBSTACLE_STOP_MM_DEFAULT
 * Ngưỡng khoảng cách phía trước kích hoạt phanh dừng khẩn cấp (mm).
 *
 * @def OBSTACLE_HYSTERESIS_MM
 * Độ trễ giải phóng trạng thái phanh dừng khi vật cản rời xa (mm).
 *
 * @def OBSTACLE_BLOCK_ON_SONAR_FAULT
 * `1`: Coi mất cảm biến là có vật cản (an toàn); `0`: Cho phép chạy để test bàn.
 * @{
 */
#define SONAR_COUNT                     3U
#define SONAR_SLOT_MS                   25U
#define SONAR_TRIG_PULSE_US             12U
#define SONAR_RISE_TIMEOUT_US           6000U
#define SONAR_ECHO_MAX_US               24000U
#define SONAR_MIN_MM                    20U
#define SONAR_MAX_MM                    4000U
#define SONAR_FAIL_LIMIT                5U
#define OBSTACLE_STOP_MM_DEFAULT        250U
#define OBSTACLE_HYSTERESIS_MM          50U
#define OBSTACLE_BLOCK_ON_SONAR_FAULT   1
/** @} */

/**
 * @name 4. Cấu hình đo áp pin Li-ion 4S (PA6 / ADC1_IN6)
 * Giám sát điện áp nguồn pin, lọc số IIR và các ngưỡng bảo vệ xả cạn.
 *
 * @def BATTERY_MV_NUM
 * Tử số hệ số phân áp chuyển đổi từ ADC raw sang mV.
 *
 * @def BATTERY_MV_DEN
 * Mẫu số hệ số phân áp chuyển đổi từ ADC raw sang mV.
 *
 * @def BATTERY_CAL_X10000
 * Hệ số cân chỉnh thực tế bằng VOM (nhân tỉ lệ 10000, mặc định 10000 = 1.0000).
 *
 * @def BATTERY_SAMPLE_MS
 * Chu kỳ lấy mẫu ADC đo điện áp pin (ms).
 *
 * @def BATTERY_IIR_SHIFT
 * Hệ số dịch bit bộ lọc số IIR thông thấp (alpha = 1 / 2^SHIFT).
 *
 * @def BATTERY_WARN_MV
 * Ngưỡng kích hoạt cảnh báo pin yếu (mV).
 *
 * @def BATTERY_WARN_RELEASE_MV
 * Ngưỡng giải phóng cảnh báo pin yếu (mV).
 *
 * @def BATTERY_CUTOFF_MV
 * Ngưỡng cắt tải bảo vệ cell pin không bị xả cạn kiệt (mV).
 *
 * @def BATTERY_CUTOFF_HOLD_MS
 * Thời gian duy trì áp dưới ngưỡng cắt trước khi tắt nguồn cưỡng bức (ms).
 *
 * @def BATTERY_ABSENT_MV
 * Ngưỡng nhận diện không gắn pin để bỏ qua bảo vệ khi cắm mạch nạp (mV).
 * @{
 */
#define BATTERY_MV_NUM                  702900UL
#define BATTERY_MV_DEN                  135135UL
#define BATTERY_CAL_X10000              10000UL
#define BATTERY_SAMPLE_MS               50U
#define BATTERY_IIR_SHIFT               3U
#define BATTERY_WARN_MV                 13600U
#define BATTERY_WARN_RELEASE_MV         13900U
#define BATTERY_CUTOFF_MV               12000U
#define BATTERY_CUTOFF_HOLD_MS          3000U
#define BATTERY_ABSENT_MV               6000U
/** @} */

/**
 * @name 5. Cấu hình quản lý nguồn (Smart Power Latch PB0 / PB1 / PB10)
 * Giao tiếp giữ nguồn thông minh và phối hợp ngắt nguồn an toàn với Raspberry Pi.
 *
 * @def PWR_BTN_DEBOUNCE_MS
 * Thời gian chống rung cho nút nhấn nguồn (ms).
 *
 * @def PWR_BTN_SHORT_MAX_MS
 * Giới hạn thời gian nhấn nhanh (< 0.8 s) kích hoạt chế độ Self-Test (ms).
 *
 * @def PWR_BTN_LONG_MS
 * Giới hạn thời gian nhấn giữ (>= 1.5 s) yêu cầu tắt máy (ms).
 *
 * @def PI_HALT_DEBOUNCE_MS
 * Thời gian ổn định mức HIGH chân GPIO26 của Pi báo hiệu tắt OS (ms).
 *
 * @def PI_HALT_SETTLE_MS
 * Thời gian chờ Linux đồng bộ thẻ nhớ trước khi cắt relay nguồn (ms).
 *
 * @def PI_HALT_TIMEOUT_MS
 * Thời gian tối đa chờ Pi phản hồi trước khi STM32 cắt nguồn cưỡng bức (ms).
 * @{
 */
#define PWR_BTN_DEBOUNCE_MS             50U
#define PWR_BTN_SHORT_MAX_MS            800U
#define PWR_BTN_LONG_MS                 1500U
#define PI_HALT_DEBOUNCE_MS             200U
#define PI_HALT_SETTLE_MS               800U
#define PI_HALT_TIMEOUT_MS              45000U
/** @} */

/**
 * @name 6. Cờ trạng thái
 * Mặt nạ bit (Bitmask) cho thanh ghi trạng thái toàn cục `amr_flags`.
 *
 * @def AMR_FLAG_OBSTACLE
 * Bit 0: Cờ cảnh báo phát hiện chướng ngại vật phía trước.
 *
 * @def AMR_FLAG_STALL_LEFT
 * Bit 1: Cờ phát hiện kẹt cơ khí động cơ bên trái.
 *
 * @def AMR_FLAG_STALL_RIGHT
 * Bit 2: Cờ phát hiện kẹt cơ khí động cơ bên phải.
 *
 * @def AMR_FLAG_BAT_LOW
 * Bit 3: Cờ cảnh báo mức pin yếu.
 *
 * @def AMR_FLAG_BAT_CRITICAL
 * Bit 4: Cờ ngắt khẩn cấp do pin chạm ngưỡng xả cạn nguy hiểm.
 *
 * @def AMR_FLAG_BAT_ABSENT
 * Bit 5: Cờ nhận diện không có kết nối khối pin vật lý.
 *
 * @def AMR_FLAG_SONAR_FAULT_L
 * Bit 6: Cờ báo lỗi mất tín hiệu cảm biến siêu âm bên trái.
 *
 * @def AMR_FLAG_SONAR_FAULT_M
 * Bit 7: Cờ báo lỗi mất tín hiệu cảm biến siêu âm ở giữa.
 *
 * @def AMR_FLAG_SONAR_FAULT_R
 * Bit 8: Cờ báo lỗi mất tín hiệu cảm biến siêu âm bên phải.
 *
 * @def AMR_FLAG_SHUTDOWN_PENDING
 * Bit 9: Cờ báo hệ điều hành đang trong tiến trình chờ cắt nguồn.
 * @{
 */
#define AMR_FLAG_OBSTACLE               (1UL << 0)
#define AMR_FLAG_STALL_LEFT             (1UL << 1)
#define AMR_FLAG_STALL_RIGHT            (1UL << 2)
#define AMR_FLAG_BAT_LOW                (1UL << 3)
#define AMR_FLAG_BAT_CRITICAL           (1UL << 4)
#define AMR_FLAG_BAT_ABSENT             (1UL << 5)
#define AMR_FLAG_SONAR_FAULT_L          (1UL << 6)
#define AMR_FLAG_SONAR_FAULT_M          (1UL << 7)
#define AMR_FLAG_SONAR_FAULT_R          (1UL << 8)
#define AMR_FLAG_SHUTDOWN_PENDING       (1UL << 9)
/** @} */

#endif /* INC_AMR_CONFIG_H_ */