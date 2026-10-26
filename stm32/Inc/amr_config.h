/**
 * @file    amr_config.h
 * @brief   Cấu hình chung cho firmware tầng thấp AGV (Giai đoạn 1).
 * @details 
 * - Mỗi ngưỡng, thông số phần cứng, thời gian đểu được khai báo ở đây 
 * - Các giá trị có thể được điều chỉnh để tối ưu hóa hiệu suất và độ ổn định của hệ thống.
 * - MCU: STM32F103C8T6 @ 72 MHz (HSE 8 MHz x PLL9)
 */

#ifndef INC_AMR_CONFIG_H_
#define INC_AMR_CONFIG_H_

#include "main.h"
#include <stdint.h>
#include <stdbool.h>

/**
 * @section 0. Cấu hình Debug & Telemetry
 * @brief Thiết lập kiểm tra phần cứng và giám sát trạng thái hệ thống.
 * @par AMR_SELFTEST_ENABLE
 * - 1:chạy Debug động cơ
 * - 0:chế độ mặc định/nhận lệnh từ RPi qua UART
 * @par AMR_DEBUG_UART_ENABLE
 * - 1:bật debug văn bản qua USART1 CP2102
 * - 0:chế độ mặc định
 * @par AMR_DEBUG_PERIOD_MS 
 * - Chu kỳ phát chuỗi dữ liệu telemetry (ms).
 * @note Debug only
*/

#define AMR_SELFTEST_ENABLE         1     
#define AMR_DEBUG_UART_ENABLE       1     
#define AMR_DEBUG_PERIOD_MS         200U  

/**
 * @section 1. Cấu hình Động cơ và PWM ban đầu
 * @brief Thiết lập các thông số điều khiển động cơ và PWM.
 * @par PWM_ARR_PERIOD        
 * - Giá trị nạp lại cho Timer2 tạo xung 1kHz
 *      @note Công thức tính toán (với xung clock Timer là 72 MHz và Prescaler PSC = 71):
 *      @f[f_{\text{timer\_cnt}} = \frac{72\text{ MHz}}{71 + 1} = 1\text{MHz}@f]
 *      @note Để có tần số PWM 1 kHz:
 *      @f[\text{ARR} = \frac{1\text{ MHz}}{1\text{ kHz}} - 1 = 999@f]
 * @par PWM_DUTY_MAX_LIMIT    
 * - Khống chế giá trị tối đa ghi vào thanh ghi CCR không vượt quá 55% (Vrms ~ 8.1 V)
 * - Động cơ hoạt động trong dải điện áp 6V-8V
 * @par PWM_RAMP_STEP         
 * - Bước tăng/giảm tối đa CCR cho mỗi chu kỳ tăng/giảm.
 * - Giảm sụt áp và dòng tăng đột khi thay đổi tốc độ động cơ.
 * @par PWM_RAMP_PERIOD_MS    
 * - Chu kỳ cập nhật tăng/giảm (ms).
 * @par MOTOR_CMD_MAX         
 * - Chuẩn hoá khoảng lệnh điều khiển vận tốc từ -100 đến +100.
 * @par MOTOR_BRAKE_HOLD_MS   
 * - Thời gian giữ phanh điện (ms) khi dừng gấp bằng cách nối tắt 2 đầu động cơ qua L298N.
 * - Sau dó chuyển sang chế độ thả trôi để giảm nhiệt cho driver.
 * @par MOTOR_LEFT_INVERT & MOTOR_RIGHT_INVERT    
 * - Đảo chiều động cơ trái và phải nếu mắc ngược dây.
 * - 0: không đảo, 1: đảo chiều
*/

#define PWM_ARR_PERIOD              999U    
#define PWM_DUTY_MAX_LIMIT          550     
#define PWM_RAMP_STEP               25      
#define PWM_RAMP_PERIOD_MS          10U     
#define MOTOR_CMD_MAX               100     
#define MOTOR_BRAKE_HOLD_MS         300U
#define MOTOR_LEFT_INVERT           0
#define MOTOR_RIGHT_INVERT          0

/**
 * @section 2. Cấu hình Encoder quang LM393
 * @brief Thiết lập các thông số ban đầu cho 2 bộ đếm encoder.
 * @par ENCODER_PPR
 * - Số xung đếm được / vòng bánh xe (PPR = Pulse Per Revolution)      
 * @par WHEEL_DIAMETER_MM
 * - Chuẩn đường kính bánh cao su (mm) để tính vận tốc tuyến tính (mm/s) từ tốc độ góc (xung/s).
 * - Công thức tính vận tốc tuyến tính:
 *      @f[v_{\text{linear}} = \frac{\pi \cdot D_{\text{wheel}} \cdot f_{\text{pulse}}}{\text{PPR}}@f]
 * @par ENCODER_GLITCH_US
 * - Thời gian bỏ qua xung nhiễu do LM393 không có trễ. Bỏ qua xung mới đến sớm hơn khoảng này.
 * - Ngưỡng chặn loại bỏ triệt để nhiễu rung cơ học, đáp ứng cho động cơ quay nhanh.
 * @par ENCODER_SPEED_WINDOW_MS
 * - Cửa sổ tính tốc độ (xung/s).
 * - Mỗi 1 chu kỳ được định nghĩa, gom số xung đếm được trong khoảng chu kỳ để tính tốc độ.
 * @par STALL_CCR_THRESHOLD
 * - Ngưỡng CCR để phát hiện bánh xe bị kẹt.
 * - Hệ thống chỉ bắt đầu theo dõi kẹt bánh khi xung cấp ra đủ lớn.
 * - Nếu CCR thực tế lớn hơn ngưỡng này mà encoder không có xung thì coi như bị kẹt.
 * @par STALL_TIMEOUT_MS
 * - Thời gian bị kẹt liên tục (ms) để xác nhận là bị kẹt.
 * - Nếu CCR thực tế lớn hơn ngưỡng và encoder không có xung trong khoảng thời gian này thì coi như bị kẹt.
 * @details 
 * Nếu xe đâm vào tường, vướng gầm bàn hoặc kẹt dị vật mà bánh xe không thể quay được trong khi 
 * ta vẫn đang ra lệnh chạy:
 * - Dòng điện qua động cơ sẽ tăng vọt lên mức dòng kẹt rotor (Stall Current).
 * - Hậu quả: Cháy driver L298N, cuộn dây động cơ bốc khói hoặc sụt áp pin gây treo máy STM32.
 * Để chống cháy, 2 tham số này phối hợp với nhau trong hàm stall_check().
 * Hành động xử lý: Hệ thống xác nhận xe đã bị kẹt bánh (stalled = true).
 * Ngay lập tức: Cắt toàn bộ ga về 0.
 * Kích hoạt cờ khóa an toàn (AMR_Motor_SetInhibit(true)) cấm xe chạy tiếp.
 * Phanh khẩn cấp và hú còi báo động để người dùng đến kiểm tra.
*/

#define ENCODER_PPR                 20U     
#define WHEEL_DIAMETER_MM           65U     
#define ENCODER_GLITCH_US           1500U
#define ENCODER_SPEED_WINDOW_MS     100U    

#define STALL_CCR_THRESHOLD         200     
#define STALL_TIMEOUT_MS            500U

/**
 * @section 3. Cấu hình cảm biến siêu âm HC-SR04
 * @brief Thiết lập các thông số ban đầu cho cảm biến siêu âm HC-SR04, sử dụng theo chu kỳ TDM.
 * @par SONAR_COUNT
 * - Số lượng cảm biến siêu âm.
 * @par SONAR_SLOT_MS
 * - Chu kỳ quét cho mỗi cảm biến (ms). Mỗi cảm biến được cấp một khe thời gian riêng.
 * - Chu kỳ quét tổng cho tất cả cảm biến = SONAR_COUNT * SONAR_SLOT_MS
 * @par SONAR_TRIG_PULSE_US
 * - Thời gian xung Trig (us) để cảm biến phát sóng siêu âm.
 * - HC-SR04 yêu cầu xung Trig >= 10 us.
 * @par SONAR_RISE_TIMEOUT_US
 * - Thời gian tối đa để Echo lên mức cao (us).
 * - Nếu Echo không lên mức cao trong khoảng thời gian này thì coi như cảm biến bị lỗi.
 * @par SONAR_ECHO_MAX_US
 * - Thời gian tối đa để Echo xuống mức thấp (us).
 * - Nếu chân Echo giữ mức HIGH lâu hơn hàm AMR_Sonar_Process() sẽ kết luận phía trước hoàn toàn trống trải, không có vật cản.
 * @par SONAR_MIN_MM
 * - Khoảng cách tối thiểu vật lý mà cảm biến có thể đo được (mm).
 * @par SONAR_MAX_MM
 * - Khoảng cách tối đa vật lý mà cảm biến có thể đo được (mm).
 * @par SONAR_FAIL_LIMIT
 * - Số lần liên tiếp Echo không lên mức cao để xác nhận cảm biến bị lỗi.
 * - Nếu số lần liên tiếp Echo không lên mức cao >= SONAR_FAIL_LIMIT thì coi như cảm biến bị lỗi.
 * @par OBSTACLE_STOP_MM_DEFAULT
 * - Ngưỡng dừng khẩn cấp theo cảm biến siêu âm giữa (mm). 
 * - Nếu khoảng cách đo được bé hơn OBSTACLE_STOP_MM_DEFAULT thì coi như có vật cản và cấm xe chạy tiếp.
 * - Hàm supervise_obstacle() sẽ lập tức phanh dừng xe khẩn cấp.
 * @par OBSTACLE_HYSTERESIS_MM
 * - Độ trễ để nhả lệnh cấm chạy khi vật cản đã được loại bỏ (mm).
 * - Nếu khoảng cách đo được > OBSTACLE_STOP_MM_DEFAULT + OBSTACLE_HYSTERESIS_MM thì nhả lệnh cấm chạy.
 * - Xe sẽ dừng dứt khoát và chỉ cho phép chạy lại khi vật cản đã thực sự rời xa hẳn hoặc xe được lùi ra an toàn.
 * @par OBSTACLE_BLOCK_ON_SONAR_FAULT
 * - 1: Nếu cảm biến siêu âm giữa bị lỗi (Echo không phản hồi) thì coi như có vật cản và cấm xe chạy tiếp.
 * - 0: Nếu cảm biến siêu âm giữa bị lỗi thì không coi như có vật cản và cho phép xe chạy tiếp (tạm thời khi chưa có cảm biến siêu âm).
 * - Dùng để thử nghiệm trên bàn mà chưa có cảm biến siêu âm. Khi có cảm biến siêu âm thì nên đặt bằng 1 để đảm bảo an toàn.   
*/

#define SONAR_COUNT                 3U
#define SONAR_SLOT_MS               25U     
#define SONAR_TRIG_PULSE_US         12U     
#define SONAR_RISE_TIMEOUT_US       6000U   
#define SONAR_ECHO_MAX_US           24000U  
#define SONAR_MIN_MM                20U
#define SONAR_MAX_MM                4000U
#define SONAR_FAIL_LIMIT            5U      
#define OBSTACLE_STOP_MM_DEFAULT    250U
#define OBSTACLE_HYSTERESIS_MM      50U
#define OBSTACLE_BLOCK_ON_SONAR_FAULT 1

/** 
 * @section 4. Cấu hình đo áp pin Li-ion 4S (PA6 / ADC1_IN6)
 * @brief Thiết lập các thông số ban đầu cho việc đo áp pin Li-ion 4S.
 * @par BATTERY_MV_NUM & BATTERY_MV_DEN
 * - Hệ số chuyển đổi từ giá trị ADC raw sang điện áp pin (mV).
 * - Công thức tính điện áp pin: Vbat[mV] = raw * BATTERY_MV_NUM / BATTERY_MV_DEN
 * @par BATTERY_CAL_X10000
 * - Hệ số hiệu chỉnh 1 điểm bằng đồng hồ VOM (bu sai số điện trở & LDO 3.3 V).
 * - CAL = Vom_do_duoc / Vbat_hien_thi * 10000. Mặc định 10000 (= 1.0000).
 * @par BATTERY_SAMPLE_MS
 * - Chu kỳ lấy mẫu ADC (ms).
 * @par BATTERY_IIR_SHIFT
 * - Hệ số lọc IIR để làm mượt giá trị đo áp pin.
 * - alpha = 1/2^BATTERY_IIR_SHIFT -> hằng số thời gian ~0.4 s.
 * @par BATTERY_WARN_MV & BATTERY_WARN_RELEASE_MV
 * - Ngưỡng cảnh báo pin yếu (mV) và ngưỡng nhả cảnh báo (mV).
 * @par BATTERY_CUTOFF_MV & BATTERY_CUTOFF_HOLD_MS
 * - Ngưỡng cắt nguồn pin (mV) và thời gian giữ dưới ngưỡng cắt (ms).
 * @par BATTERY_ABSENT_MV
 * - Ngưỡng coi như không gắn pin (mV) -> bỏ qua bảo vệ pin để không tự tắt máy đột ngột.
*/

#define BATTERY_MV_NUM              702900UL
#define BATTERY_MV_DEN              135135UL
#define BATTERY_CAL_X10000          10000UL
#define BATTERY_SAMPLE_MS           50U
#define BATTERY_IIR_SHIFT           3U      
#define BATTERY_WARN_MV             13600U  
#define BATTERY_WARN_RELEASE_MV     13900U  
#define BATTERY_CUTOFF_MV           12000U  
#define BATTERY_CUTOFF_HOLD_MS      3000U   
#define BATTERY_ABSENT_MV           6000U

/**
 * @section 5. Cấu hình quản lý nguồn (Smart Power Latch PB0 / PB1 / PB10)
 * @brief Thiết lập các thông số ban đầu cho việc quản lý nguồn.
 * @par PWR_BTN_DEBOUNCE_MS
 * - Thời gian chống rung cho nút nhấn nguồn (ms).
 * @par PWR_BTN_SHORT_MAX_MS
 * - Thời gian nhấn ngắn tối đa (ms) để kích hoạt chế độ Self-Test.
 * @par PWR_BTN_LONG_MS
 * - Thời gian nhấn dài tối thiểu (ms) để yêu cầu tắt máy.
 * @par PI_HALT_DEBOUNCE_MS
 * - Thời gian chống rung cho tín hiệu GPIO26 từ Raspberry Pi (ms).
 * @par PI_HALT_SETTLE_MS
 * - Thời gian chờ để Linux đồng bộ trước khi cắt nguồn (ms).
 * @par PI_HALT_TIMEOUT_MS
 * - Thời gian tối đa để Raspberry Pi xác nhận lệnh Halt (ms).
 * - Nếu Pi không xác nhận Halt trong khoảng thời gian này thì cắt nguồn cưỡng bức
 */

#define PWR_BTN_DEBOUNCE_MS         50U
#define PWR_BTN_SHORT_MAX_MS        800U    /* Nhan nhanh < 0.8 s -> chay Self-Test    */
#define PWR_BTN_LONG_MS             1500U   /* Nhan giu >= 1.5 s -> yeu cau tat may    */
#define PI_HALT_DEBOUNCE_MS         200U    /* GPIO26 cua Pi phai HIGH on dinh 200 ms  */
#define PI_HALT_SETTLE_MS           800U    /* Cho Linux sync the nho truoc khi cat nguon */
#define PI_HALT_TIMEOUT_MS          45000U

/**
 * @section 6. Cờ trạng thái 
 * @brief Cấu hình các cờ lỗi và trạng thái
 * @a word32_t amr_flags
 * - Mỗi bit trong amr_flags đại diện cho một trạng thái hoặc lỗi cụ thể
 * - Các cờ này được sử dụng để giám sát tình trạng hệ thống và thực hiện các hành động bảo vệ khi cần thiết.
 * - Các cờ có thể được kiểm tra và đặt trong các hàm xử lý sự kiện hoặc vòng lặp chính của chương trình.
 */
#define AMR_FLAG_OBSTACLE           (1UL << 0)
#define AMR_FLAG_STALL_LEFT         (1UL << 1)
#define AMR_FLAG_STALL_RIGHT        (1UL << 2)
#define AMR_FLAG_BAT_LOW            (1UL << 3)
#define AMR_FLAG_BAT_CRITICAL       (1UL << 4)
#define AMR_FLAG_BAT_ABSENT         (1UL << 5)
#define AMR_FLAG_SONAR_FAULT_L      (1UL << 6)
#define AMR_FLAG_SONAR_FAULT_M      (1UL << 7)
#define AMR_FLAG_SONAR_FAULT_R      (1UL << 8)
#define AMR_FLAG_SHUTDOWN_PENDING   (1UL << 9)

#endif /* INC_AMR_CONFIG_H_ */
