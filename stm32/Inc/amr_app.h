/**
 ******************************************************************************
 * @file    amr_app.h
 * @brief   Giao diện tầng điều phối giám sát hệ thống (Supervisor Layer) cho AMR.
 * @details
 * Tệp tiêu đề định nghĩa giao diện lập trình ứng dụng (API) cấp cao nhất của firmware
 * tầng thấp STM32F103C8T6. Tầng giám sát có nhiệm vụ:
 * - Khởi tạo toàn bộ chuỗi phần cứng ngoại vi và các bộ điều khiển thành phần.
 * - Quản lý vòng lặp điều phối hợp tác đa chu kỳ (Cooperative Super-Loop Scheduler).
 * - Giám sát an toàn đa tầng: phát hiện vật cản siêu âm, chống kẹt động cơ, bảo vệ pin xả cạn,
 *   và điều phối quy trình tắt máy an toàn với Raspberry Pi.
 * - Thu thập toàn diện dữ liệu cảm biến, trạng thái nguồn và xuất telemetry thời gian thực.
 * - Thực thi kịch bản tự kiểm tra phần cứng (Self-Test) khi xuất xưởng hoặc gỡ lỗi.
 *
 * Chu trình tích hợp mã nguồn trong `main.c`:
 * @code
 *   int main(void)
 *   {
 *       HAL_Init();
 *       SystemClock_Config();
 *       AMR_App_Init();    // Khởi tạo toàn bộ phần cứng & supervisor
 *
 *       while (1)
 *       {
 *           AMR_App_Loop(); // Vòng lặp điều phối chính không chặn (Non-blocking)
 *       }
 *   }
 * @endcode
 *
 * @author  Nhóm Đề án 2 - Robot vận chuyển nội bộ
 * @version 2.1
 * @date    2026
 ******************************************************************************
 */

#ifndef INC_AMR_APP_H_
#define INC_AMR_APP_H_

#include "amr_config.h"

/**
 * @struct AmrTelemetry_t
 * @brief Cấu trúc dữ liệu Telemetry lưu trữ toàn bộ trạng thái hoạt động của xe.
 * @details
 * Cung cấp một bản chụp tức thời (snapshot) toàn diện về trạng thái vận hành của robot,
 * được cập nhật liên tục theo chu kỳ vòng lặp chính của tầng giám sát.
 *
 * Cấu trúc này hỗ trợ:
 * - Theo dõi trực quan qua tính năng Live Expressions của STM32CubeIDE khi nạp gỡ lỗi ST-Link.
 * - Đóng gói thành chuỗi ký tự ASCII định dạng văn bản truyền qua cổng UART (Debug Telemetry).
 * - Làm nguồn dữ liệu cho giao thức truyền thông nhị phân gửi lên máy tính nhúng Raspberry Pi.
 */
typedef struct
{
    /**
     * @brief Thời gian hệ thống hoạt động liên tục từ lúc khởi động.
     * @par Đơn vị:
     * Milliseconds (ms).
     * @note Lấy trực tiếp từ bộ đếm tích lũy SysTick thông qua hàm HAL_GetTick().
     */
    uint32_t uptime_ms;

    /**
     * @brief Thanh ghi mặt nạ bit (Bitmask) báo lỗi và trạng thái hệ thống toàn cục.
     * @details
     * Tổ hợp logic OR bit từ các cờ trạng thái định nghĩa trong amr_config.h:
     * - `AMR_FLAG_OBSTACLE` (Bit 0): Phát hiện vật cản dừng khẩn cấp.
     * - `AMR_FLAG_STALL_LEFT` (Bit 1): Sự cố kẹt cơ khí động cơ trái.
     * - `AMR_FLAG_STALL_RIGHT` (Bit 2): Sự cố kẹt cơ khí động cơ phải.
     * - `AMR_FLAG_BAT_LOW` (Bit 3): Cảnh báo điện áp pin yếu.
     * - `AMR_FLAG_BAT_CRITICAL` (Bit 4): Ngưỡng ngắt khẩn cấp bảo vệ pin cạn.
     * - `AMR_FLAG_BAT_ABSENT` (Bit 5): Không kết nối khối pin vật lý (chạy nguồn USB/ST-Link).
     * - `AMR_FLAG_SONAR_FAULT_L` (Bit 6): Mất tín hiệu cảm biến siêu âm trái.
     * - `AMR_FLAG_SONAR_FAULT_M` (Bit 7): Mất tín hiệu cảm biến siêu âm giữa.
     * - `AMR_FLAG_SONAR_FAULT_R` (Bit 8): Mất tín hiệu cảm biến siêu âm phải.
     * - `AMR_FLAG_SHUTDOWN_PENDING` (Bit 9): Đang đếm ngược tắt máy an toàn.
     */
    uint32_t fault_flags;

    /**
     * @brief Điện áp tức thời của khối pin Li-ion sau khi qua bộ lọc số thông thấp IIR.
     * @par Đơn vị:
     * Millivolts (mV).
     * @details Được tính toán từ giá trị ADC raw thông qua hệ số phân áp và bộ lọc số bậc 1.
     */
    uint16_t bat_mv;

    /**
     * @brief Giá trị số thô đọc trực tiếp từ bộ chuyển đổi ADC1 (kênh ADC1_IN6 / chân PA6).
     * @par Dải giá trị:
     * 0 - 4095 (Độ phân giải 12-bit).
     */
    uint16_t bat_raw;

    /**
     * @brief Trạng thái đánh giá mức điện áp pin hiện tại.
     * @details
     * Phân loại theo kiểu liệt kê `BatteryState_t` (từ module `amr_battery.h`):
     * - `BAT_OK`: Điện áp pin an toàn, hoạt động bình thường.
     * - `BAT_LOW`: Điện áp pin chạm ngưỡng cảnh báo, cần đưa xe về trạm sạc.
     * - `BAT_CRITICAL`: Điện áp pin chạm ngưỡng xả cạn nguy hiểm, kích hoạt ngắt tải.
     * - `BAT_ABSENT`: Không có pin, đang cấp nguồn thử nghiệm trên bàn nạp ST-Link.
     */
    uint8_t  bat_state;

    /**
     * @brief Trạng thái hiện tại của máy trạng thái quản lý nguồn (Smart Power State).
     * @details
     * Phân loại theo kiểu liệt kê `PowerState_t` (từ module `amr_power.h`):
     * - `PWR_STATE_RUNNING`: Hệ thống đang giữ nguồn và vận hành bình thường.
     * - `PWR_STATE_WAIT_HALT`: Đang phát tín hiệu chờ Raspberry Pi shutdown hoàn tất.
     * - `PWR_STATE_HALT_DETECTED`: Đã nhận tín hiệu GPIO26 Halt từ Pi, đang đếm ngược chờ sync ổ đĩa.
     * - `PWR_STATE_CUTOFF`: Đã nhả chân giữ nguồn PB0 để ngắt relay cắt nguồn chính.
     */
    uint8_t  pwr_state;

    /**
     * @brief Nguyên nhân hoặc sự kiện kích hoạt tiến trình tắt nguồn hệ thống.
     * @details
     * Phân loại theo kiểu liệt kê `PowerReason_t` (từ module `amr_power.h`):
     * - `PWR_REASON_NONE`: Chưa có sự kiện tắt nguồn.
     * - `PWR_REASON_BTN_LONG`: Người dùng nhấn giữ nút nguồn PB1 liên tục >= 1.5 giây.
     * - `PWR_REASON_BAT_CRITICAL`: Pin bị sụt áp dưới ngưỡng cắt duy trì liên tục quá 3 giây.
     * - `PWR_REASON_CMD`: Nhận lệnh yêu cầu tắt máy từ phần mềm tầng trên qua UART.
     */
    uint8_t  pwr_reason;

    /**
     * @brief Bước thực thi hiện tại trong kịch bản tự kiểm tra phần cứng (Self-Test).
     * @details
     * - `0`: Không chạy chế độ tự kiểm tra (xe đang ở chế độ nhận lệnh vận tốc bình thường).
     * - `1..10`: Số thứ tự của bước test động cơ đang thực thi (tiến, lùi, quay trái, quay phải, phanh).
     */
    uint8_t  selftest_step;

    /**
     * @brief Mảng lưu trữ giá trị khoảng cách đo được từ các cảm biến siêu âm HC-SR04.
     * @par Đơn vị:
     * Millimeters (mm).
     * @details
     * Phân bổ chỉ số mảng:
     * - `sonar_mm[0]`: Khoảng cách từ cảm biến góc quét bên trái.
     * - `sonar_mm[1]`: Khoảng cách từ cảm biến hướng tâm chính giữa.
     * - `sonar_mm[2]`: Khoảng cách từ cảm biến góc quét bên phải.
     *
     * @note Giá trị quy ước `0xFFFF` (65535) thể hiện khoảng cách vượt ngoài dải đo (vùng không gian trống)
     *       hoặc cảm biến bị lỗi mất phản hồi xung Echo.
     */
    uint16_t sonar_mm[SONAR_COUNT];

    /**
     * @brief Lệnh vận tốc đặt cho cụm động cơ bên trái.
     * @par Phạm vi:
     * `[-MOTOR_CMD_MAX, +MOTOR_CMD_MAX]` tức `-100` đến `+100`.
     * @details Giá trị nhận từ Raspberry Pi qua cổng UART hoặc từ kịch bản Self-Test.
     */
    int16_t  cmd_left;

    /**
     * @brief Lệnh vận tốc đặt cho cụm động cơ bên phải.
     * @par Phạm vi:
     * `[-MOTOR_CMD_MAX, +MOTOR_CMD_MAX]` tức `-100` đến `+100`.
     * @details Giá trị nhận từ Raspberry Pi qua cổng UART hoặc từ kịch bản Self-Test.
     */
    int16_t  cmd_right;

    /**
     * @brief Giá trị thanh ghi Capture/Compare (CCR) có dấu đang xuất thực tế xuống driver cầu H cho bánh trái.
     * @par Phạm vi:
     * `[-PWM_DUTY_MAX_LIMIT, +PWM_DUTY_MAX_LIMIT]` tức `-550` đến `+550`.
     * @details Dấu dương biểu thị chiều quay tiến, dấu âm biểu thị chiều quay lùi.
     */
    int16_t  pwm_left;

    /**
     * @brief Giá trị thanh ghi Capture/Compare (CCR) có dấu đang xuất thực tế xuống driver cầu H cho bánh phải.
     * @par Phạm vi:
     * `[-PWM_DUTY_MAX_LIMIT, +PWM_DUTY_MAX_LIMIT]` tức `-550` đến `+550`.
     * @details Dấu dương biểu thị chiều quay tiến, dấu âm biểu thị chiều quay lùi.
     */
    int16_t  pwm_right;

    /**
     * @brief Tần số xung tức thời đo được từ encoder quang của bánh xe bên trái.
     * @par Đơn vị:
     * Xung / giây (Pulses Per Second - pps).
     * @details Tương quan trực tiếp với tốc độ góc vòng quay bánh xe.
     */
    int16_t  speed_left_pps;

    /**
     * @brief Tần số xung tức thời đo được từ encoder quang của bánh xe bên phải.
     * @par Đơn vị:
     * Xung / giây (Pulses Per Second - pps).
     * @details Tương quan trực tiếp với tốc độ góc vòng quay bánh xe.
     */
    int16_t  speed_right_pps;

    /**
     * @brief Tổng số lượng xung ngắt ngoài tích lũy được từ encoder bánh trái kể từ khi khởi động hệ thống.
     * @par Đơn vị:
     * Xung (Pulses).
     * @note Biến đếm không tràn số nguyên dương 32-bit, dùng cho bài toán ước lượng vị trí Odometry.
     */
    uint32_t enc_total_left;

    /**
     * @brief Tổng số lượng xung ngắt ngoài tích lũy được từ encoder bánh phải kể từ khi khởi động hệ thống.
     * @par Đơn vị:
     * Xung (Pulses).
     * @note Biến đếm không tràn số nguyên dương 32-bit, dùng cho bài toán ước lượng vị trí Odometry.
     */
    uint32_t enc_total_right;

    /**
     * @brief Quãng đường di chuyển tịnh tiến tích lũy tính toán được của bánh xe bên trái.
     * @par Đơn vị:
     * Millimeters (mm).
     * @details Quy đổi từ tổng số xung encoder tích lũy kết hợp với đường kính danh định bánh xe.
     */
    int32_t  dist_left_mm;

    /**
     * @brief Quãng đường di chuyển tịnh tiến tích lũy tính toán được của bánh xe bên phải.
     * @par Đơn vị:
     * Millimeters (mm).
     * @details Quy đổi từ tổng số xung encoder tích lũy kết hợp với đường kính danh định bánh xe.
     */
    int32_t  dist_right_mm;

    /**
     * @brief Thời gian thực thi lâu nhất của một chu kỳ vòng lặp chính AMR_App_Loop.
     * @par Đơn vị:
     * Microseconds (us).
     * @details Sử dụng bộ định thời microsecond để giám sát tính tiền định (Determinism)
     *          và phát hiện các điểm nghẽn (bottlenecks) trong vòng lặp super-loop.
     */
    uint32_t loop_max_us;

    /**
     * @brief Tổng số chu kỳ vòng lặp chính AMR_App_Loop đã hoàn thành kể từ lúc khởi động.
     * @details Dùng để theo dõi tần số quét trung bình và tỷ lệ chu kỳ CPU tải hệ thống.
     */
    uint32_t loop_count;
} AmrTelemetry_t;

/**
 * @brief Biến toàn cục duy nhất chứa ảnh chụp trạng thái telemetry của robot AMR.
 * @details
 * Biến được cập nhật liên tục bởi tầng giám sát Supervisor (`amr_app.c`) theo chu kỳ 10 ms.
 * Có thể được quan sát trực tiếp bằng công cụ Live Expressions của STM32CubeIDE khi gỡ lỗi
 * hoặc được đóng gói phát qua kênh UART lên máy tính nhúng Raspberry Pi.
 *
 * Từ khóa `volatile` đảm bảo trình biên dịch không tối ưu hóa ghi đè biến khi truy cập từ ngắt
 * hoặc các tác vụ nền.
 */
extern volatile AmrTelemetry_t g_amr;

/**
 * @brief Khởi tạo toàn bộ các module phần cứng và tầng điều phối giám sát AMR.
 * @details
 * Thiết lập thứ tự khởi động tuần tự cho tất cả các ngoại vi và hệ thống con của robot:
 * 1. Khởi tạo bộ đo thời gian microsecond (`AMR_Time_Init`).
 * 2. Cấu hình các chân GPIO điều khiển logic và ngoại vi (`MX_GPIO_Init`).
 * 3. Kích hoạt bộ tạo xung PWM Timer 2 điều khiển động cơ (`AMR_Motor_Init`).
 * 4. Cấu hình ngắt ngoài EXTI bộ đếm xung Encoder quang LM393 (`AMR_Encoder_Init`).
 * 5. Kích hoạt chu kỳ quét ghép kênh TDM cảm biến siêu âm HC-SR04 (`AMR_Sonar_Init`).
 * 6. Khởi động bộ chuyển đổi ADC1 DMA đo điện áp pin Li-ion (`AMR_Battery_Init`).
 * 7. Thiết lập mạch tự giữ nguồn Power Latch và nút nhấn nguồn (`AMR_Power_Init`).
 * 8. Khởi tạo cổng truyền thông nối tiếp USART1 xuất nhật ký telemetry (`AMR_Debug_Init`).
 * 9. Phát tín hiệu âm thanh khởi động ngắn qua còi Buzzer (`AMR_Buzzer_Play`).
 * 10. Kích hoạt quy trình Self-Test nếu macro @ref AMR_SELFTEST_ENABLE được cấu hình là 1.
 *
 * @note Hàm này phải được gọi một lần duy nhất trong hàm `main()` trước khi bước vào vòng lặp vô hạn `while(1)`.
 */
void AMR_App_Init(void);

/**
 * @brief Vòng lặp điều phối chính (Super-Loop Cooperative Scheduler) của firmware AMR.
 * @details
 * Đóng vai trò là bộ giám sát trung tâm (Supervisor Layer), điều phối luồng thực thi không khóa (Non-blocking)
 * cho toàn bộ hệ thống theo lịch biểu thời gian thực:
 *
 * ### 1. Tác vụ thực thi liên tục trong mọi chu kỳ vòng lặp (Continuous Tasks):
 * - `AMR_Sonar_Process()`: Lấy mẫu và đo độ rộng xung Echo của cảm biến siêu âm với độ phân giải microsecond.
 * - `AMR_Debug_Pump()`: Bơm dữ liệu từ hàng đợi vòng tròn truyền thông (Ring Buffer) ra thanh ghi UART qua ngắt/DMA.
 *
 * ### 2. Tác vụ thực thi định kỳ mỗi 10 ms (Periodic Tick Tasks - 100 Hz):
 * - Quản lý nguồn & Nút nhấn (`AMR_Power_Process`): Chống rung nút nguồn, phát hiện nhấn ngắn/nhấn giữ, giám sát tín hiệu GPIO26 Halt từ Pi.
 * - Đo lường & Bảo vệ pin (`AMR_Battery_Process`): Lọc số IIR điện áp pin, phát hiện pin yếu, kích hoạt đếm lùi ngắt nguồn nếu pin cạn.
 * - Vận tốc & Chống kẹt (`AMR_Encoder_Process`): Lấy mẫu số xung encoder, tính toán vận tốc bánh xe và phát hiện kẹt cơ khí (Stall Protection).
 * - Đánh giá an toàn chướng ngại vật: So sánh khoảng cách siêu âm với ngưỡng @ref OBSTACLE_STOP_MM_DEFAULT và độ trễ @ref OBSTACLE_HYSTERESIS_MM.
 * - Cập nhật kịch bản Self-Test: Chuyển bước kiểm tra tự động theo lịch trình thời gian định sẵn.
 * - Điều khiển động cơ: Áp dụng thuật toán giới hạn tốc độ tăng dốc CCR (@ref PWM_RAMP_STEP) để bảo vệ mạch nguồn và cơ cấu truyền động.
 * - Cập nhật trạng thái còi Buzzer (`AMR_Buzzer_Process`).
 *
 * ### 3. Tác vụ thực thi định kỳ mỗi 200 ms (Telemetry Task - 5 Hz):
 * - Đóng gói toàn bộ biến cấu trúc @ref g_amr và phát chuỗi dữ liệu trạng thái qua cổng USART1 (nếu bật @ref AMR_DEBUG_UART_ENABLE).
 *
 * ### Thứ tự ưu tiên can thiệp an toàn (Safety Priority Hierarchy):
 * 1. **Mức 1 - Tắt máy (Shutdown Pending)**: Cưỡng bức ngắt hoàn toàn công suất động cơ, đưa CCR về 0.
 * 2. **Mức 2 - Kẹt cơ khí bánh xe (Stall Latched)**: Kích hoạt phanh dừng, khóa cứng lệnh ga cho tới khi người vận hành can thiệp xóa lỗi.
 * 3. **Mức 3 - Phát hiện vật cản (Obstacle Detected)**: Phanh dừng khẩn cấp và khóa riêng chiều chuyển động tiến, cho phép xe lùi hoặc quay góc.
 *
 * @note Hàm này được gọi tuần hoàn liên tục bên trong vòng lặp `while(1)` của `main.c`.
 *       Tuyệt đối không chèn bất kỳ hàm trễ chặn nào (chẳng hạn như `HAL_Delay()`) vào bên trong vòng lặp này.
 */
void AMR_App_Loop(void);

/**
 * @brief Thiết lập lệnh vận tốc điều khiển hai bánh xe từ tầng điều khiển cấp cao (Raspberry Pi).
 * @details
 * Cập nhật điểm đặt vận tốc mong muốn cho hệ thống điều khiển chuyển động vi sai của robot.
 * Các giá trị đầu vào sẽ được kẹp (clamp) trong dải chuẩn hóa và đưa qua bộ giám sát an toàn (Supervisor)
 * trước khi áp dụng vào thuật toán làm mượt gia tốc dốc PWM.
 *
 * @param[in] left  Lệnh vận tốc bánh xe bên trái.
 *                  Dải giá trị hợp lệ: `[-MOTOR_CMD_MAX, +MOTOR_CMD_MAX]` tức từ `-100` (lùi tối đa) đến `+100` (tiến tối đa).
 * @param[in] right Lệnh vận tốc bánh xe bên phải.
 *                  Dải giá trị hợp lệ: `[-MOTOR_CMD_MAX, +MOTOR_CMD_MAX]` tức từ `-100` (lùi tối đa) đến `+100` (tiến tối đa).
 *
 * @note Lệnh vận tốc sẽ bị bộ giám sát an toàn vô hiệu hóa hoặc chặn chiều tiến trong các trường hợp:
 *       - Hệ thống đang trong trạng thái lỗi kẹt bánh (`s_stall_latched == true`).
 *       - Phát hiện vật cản phía trước trong dải dừng an toàn (`s_obstacle == true`).
 *       - Hệ thống đang trong tiến trình đếm ngược tắt máy (`AMR_FLAG_SHUTDOWN_PENDING`).
 *       - Kịch bản Self-Test đang nắm quyền điều khiển động cơ.
 */
void AMR_App_SetVelocity(int16_t left, int16_t right);

/**
 * @brief Cấu hình động ngưỡng khoảng cách phát hiện vật cản kích hoạt phanh dừng an toàn.
 * @details
 * Cho phép tầng điều khiển cấp cao (Raspberry Pi) thay đổi khoảng cách dừng phanh khẩn cấp
 * phù hợp với từng ngữ cảnh vận hành thực tế của robot (ví dụ: chế độ di chuyển tốc độ cao ngoài hành lang
 * hoặc chế độ cập trạm sạc/băng chuyền yêu cầu tiếp cận cự ly gần).
 *
 * @param[in] stop_mm Khoảng cách an toàn mới (đơn vị: Millimeters).
 *                    Ngưỡng giải phóng trễ hồi tiếp sẽ tự động được bù thêm @ref OBSTACLE_HYSTERESIS_MM (50 mm).
 *
 * @note Giá trị này sẽ được ghi đè tạm thời trong bộ nhớ RAM và được khởi tạo lại bằng giá trị mặc định
 *       @ref OBSTACLE_STOP_MM_DEFAULT (250 mm) mỗi khi hệ thống khởi động lại.
 */
void AMR_App_SetObstacleThreshold(uint16_t stop_mm);

/**
 * @brief Xóa các cờ trạng thái sự cố đã được chốt khóa (Latched Faults) của hệ thống.
 * @details
 * Giải phóng trạng thái khóa an toàn động cơ sau khi xảy ra sự cố kẹt bánh xe (Stall Fault).
 * Sau khi người vận hành đã kiểm tra cơ khí và loại bỏ vật cản làm kẹt bánh, hàm này được gọi
 * để xóa cờ @ref AMR_FLAG_STALL_LEFT và @ref AMR_FLAG_STALL_RIGHT, đưa robot trở lại trạng thái sẵn sàng vận hành.
 *
 * @note Tác vụ này có hiệu lực tương đương với thao tác người vận hành nhấn nhả nhanh nút nhấn vật lý SW1
 *       trên thân bo mạch điều khiển khi xe đang hoạt động.
 */
void AMR_App_ClearFaults(void);

#endif /* INC_AMR_APP_H_ */
