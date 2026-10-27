/**
 * @file    amr_config.h
 * @brief   Tập tin định cấu hình tập trung cho firmware tầng thấp robot tự hành AMR (Giai đoạn 1).
 * @details
 * Tập tin chứa toàn bộ định nghĩa hằng số cấu hình hệ thống, ngưỡng bảo vệ an toàn,
 * chu kỳ lấy mẫu cảm biến, thuật toán lọc số và thông số điều khiển chấp hành của xe AMR.
 *
 * Hệ thống phần cứng :
 * - **Vi điều khiển**: STM32F103C8T6 (ARM Cortex-M3 32-bit @ 72 MHz).
 * - **Nguồn cấp**: Khối pin Li-ion 4S (14.8 V, dải điện áp 12.0 V - 16.8 V).
 * - **Động cơ**: IC L298N kép điều khiển 2 động cơ DC giảm tốc có gắn encoder.
 * - **Hệ thống cảm biến**:
 *   - 03 x Cảm biến siêu âm HC-SR04 (quét ghép kênh TDM tránh phản xạ chéo).
 *   - 02 x Cảm biến encoder đĩa quang LM393 (20 xung/vòng).
 *   - 01 x Kênh ADC1_IN6 (PA6) qua mạch cầu phân áp giám sát điện áp pin.
 * - **Giao tiếp chủ**: Kênh UART kết nối máy tính nhúng Raspberry Pi 4 (tầng High-Level).
 *
 * @version 2.1
 * @date    2026
 */

#ifndef INC_AMR_CONFIG_H_
#define INC_AMR_CONFIG_H_

#include "main.h"
#include <stdint.h>
#include <stdbool.h>

/**
 * @name 0. Cấu hình Debug & Telemetry
 * Thiết lập các chế độ kiểm tra phần cứng độc lập và giám sát hệ thống thời gian thực.
 * @{
 */

/**
 * @def AMR_SELFTEST_ENABLE
 * @brief Chế độ tự kiểm tra phần cứng khi khởi động (Self-Test Mode).
 * @details
 * Cấu hình hành vi khởi động ban đầu của firmware ngay sau khi cấp nguồn hoặc reset vi điều khiển:
 * - `1`: Kích hoạt quy trình tự kiểm tra phần cứng độc lập (Standalone Self-Test).
 *        Xe sẽ tuần tự kích hoạt động cơ chạy tiến, lùi, quay trái, quay phải và phanh dừng
 *        theo kịch bản cài sẵn mà không cần kết nối hoặc nhận lệnh từ Raspberry Pi.
 * - `0`: Chế độ vận hành bình thường (Normal Operation).
 *        Firmware sau khi khởi tạo phần cứng sẽ chuyển sang trạng thái chờ lệnh điều khiển vận tốc
 *        từ máy tính nhúng Raspberry Pi qua cổng UART.
 *
 * @note Tùy chọn này chỉ nên bật khi kiểm thử bo mạch độc lập trên bàn test hoặc trong quy trình nghiệm thu xuất xưởng (QA/QC).
 */
#define AMR_SELFTEST_ENABLE             1

/**
 * @def AMR_DEBUG_UART_ENABLE
 * @brief Bật/tắt luồng xuất dữ liệu gỡ lỗi (Debug Log) và Telemetry qua UART.
 * @details
 * Quy định việc xuất chuỗi ký tự ASCII gỡ lỗi qua cổng truyền thông nối tiếp:
 * - `1`: Kích hoạt truyền dữ liệu telemetry và nhật ký trạng thái qua cổng USART1
 *        (kết nối trực tiếp với mạch chuyển đổi USB-to-UART CP2102).
 * - `0`: Vô hiệu hóa luồng dữ liệu gỡ lỗi để giảm thiểu tải xử lý CPU, giảm chu kỳ ngắt UART
 *        và tránh nghẽn luồng truyền thông khi vận hành chính thức.
 *
 * @note Phần cứng sử dụng cổng USART1 (TX: PA9, RX: PA10), cấu hình Baudrate 115200 bps, khung truyền 8N1.
 */
#define AMR_DEBUG_UART_ENABLE           1

/**
 * @def AMR_DEBUG_PERIOD_MS
 * @brief Chu kỳ phát chuỗi dữ liệu telemetry định kỳ qua UART.
 * @details
 * Khoảng thời gian định kỳ giữa hai lần hệ thống đóng gói và phát bản tin giám sát toàn diện
 * (bao gồm điện áp pin, vận tốc 2 bánh, khoảng cách 3 cảm biến siêu âm và cờ trạng thái lỗi).
 *
 * @par Đơn vị:
 * Milliseconds (ms).
 *
 * @par Tần số làm tươi:
 * @f[f_{\text{telemetry}} = \frac{1000}{\text{AMR\_DEBUG\_PERIOD\_MS}} = \frac{1000}{200} = 5\text{ Hz}@f]
 *
 * @note Không nên cấu hình chu kỳ nhỏ hơn 20 ms để tránh làm đầy hàng đợi (overflow buffer) cổng UART ở tốc độ 115200 bps.
 */
#define AMR_DEBUG_PERIOD_MS             200U
/** @} */

/**
 * @name 1. Cấu hình Động cơ và PWM ban đầu
 * Thiết lập các thông số điều chế độ rộng xung (PWM) Timer 2 và thuật toán điều khiển động cơ.
 * @{
 */

/**
 * @def PWM_ARR_PERIOD
 * @brief Giá trị nạp lại tự động (Auto-Reload Register - ARR) của Timer 2 tạo chu kỳ PWM 1 kHz.
 * @details
 * Xác định chu kỳ xung điều chế PWM đưa vào các chân Enable của IC cầu H L298N (kênh TIM2_CH1 và TIM2_CH2).
 * Tần số PWM được cấu hình ở mức 1 kHz để tối ưu hóa đặc tính đáp ứng của mạch công suất L298N
 * và giảm thiểu tổn hao đóng ngắt (switching loss).
 *
 * @par Công thức tính toán phần cứng:
 * Với nguồn cấp xung nhịp nội cho Timer 2 từ bus APB1 qua Clock Tree x2 là @f$f_{\text{CLK}} = 72\text{ MHz}@f$,
 * hệ số chia tần số Prescaler @f$\text{PSC} = 71@f$:
 * @f[f_{\text{cnt}} = \frac{f_{\text{CLK}}}{\text{PSC} + 1} = \frac{72\text{ MHz}}{71 + 1} = 1\text{ MHz}@f]
 * Để đạt tần số ngõ ra PWM mong muốn @f$f_{\text{PWM}} = 1\text{ kHz}@f$:
 * @f[\text{ARR} = \frac{f_{\text{cnt}}}{f_{\text{PWM}}} - 1 = \frac{1\text{ MHz}}{1\text{ kHz}} - 1 = 999@f]
 *
 * @par Độ phân giải:
 * 1000 bước (từ 0 đến 999), tương ứng mỗi đơn vị CCR tăng/giảm 0.1% Duty Cycle.
 *
 * @par Đơn vị:
 * Ticks (Đếm Timer).
 */
#define PWM_ARR_PERIOD                  999U

/**
 * @def PWM_DUTY_MAX_LIMIT
 * @brief Giới hạn tối đa của giá trị thanh ghi Capture/Compare (CCR) cho các kênh PWM động cơ.
 * @details
 * Khống chế chu kỳ làm việc (Duty Cycle) của xung PWM không vượt quá 55% (550 / 1000).
 * Hệ thống sử dụng khối nguồn pin Li-ion 4S có dải điện áp từ 12.0 V đến 16.8 V (danh định 14.8 V).
 * Trong khi đó, cụm động cơ DC giảm tốc trên xe có điện áp làm việc định mức từ 6 V đến 8 V.
 * Việc giới hạn Duty Cycle ở 55% đảm bảo điện áp hiệu dụng (RMS) đặt lên cuộn dây động cơ luôn nằm trong ngưỡng an toàn:
 * @f[V_{\text{rms\_max}} \approx V_{\text{bat\_nom}} \times \text{Duty}_{\text{max}} = 14.8\text{ V} \times 0.55 \approx 8.14\text{ V}@f]
 *
 * @par Đơn vị:
 * Ticks CCR (Tương ứng 55.0% Duty Cycle).
 *
 * @warning Tuyệt đối không nâng giá trị này vượt quá 600 khi dùng pin 4S, vì sẽ gây quá điện áp
 *          làm cháy cuộn dây phần ứng động cơ hoặc đánh thủng tầng công suất BJT của IC L298N do quá nhiệt.
 */
#define PWM_DUTY_MAX_LIMIT              550

/**
 * @def PWM_RAMP_STEP
 * @brief Bước tăng/giảm giá trị CCR tối đa trong mỗi chu kỳ làm mượt gia tốc (Slew-rate Limiter).
 * @details
 * Bộ giới hạn tốc độ biến thiên xung PWM (Software Slew Rate Limiter). Khi nhận lệnh thay đổi tốc độ đột ngột,
 * giá trị ghi vào thanh ghi CCR sẽ được nội suy tăng hoặc giảm từng bước tối đa `PWM_RAMP_STEP` ticks
 * sau mỗi chu kỳ `PWM_RAMP_PERIOD_MS`.
 *
 * Mục đích kỹ thuật:
 * - Triệt tiêu dòng khởi động cực đại (Inrush Stalling Current) của động cơ DC, tránh sụt áp nguồn hệ thống.
 * - Ngăn hiện tượng trượt bánh dẫn động (wheel slip) trên bề mặt nền xưởng nhẵn.
 * - Bảo vệ cụm bánh răng hộp giảm tốc khỏi hiện tượng sốc cơ học và ứng suất xoắn tức thời.
 *
 * @par Đơn vị:
 * Ticks CCR / chu kỳ nội suy.
 */
#define PWM_RAMP_STEP                   25

/**
 * @def PWM_RAMP_PERIOD_MS
 * @brief Chu kỳ thời gian cập nhật thuật toán làm mượt độ dốc gia tốc PWM.
 * @details
 * Khoảng thời gian kích hoạt lại hàm tính toán nội suy dốc CCR cho 2 kênh động cơ.
 * Với `PWM_RAMP_STEP = 25` và `PWM_RAMP_PERIOD_MS = 10U`, tốc độ tăng dốc là 2500 ticks/giây.
 * Thời gian để xe tăng tốc từ trạng thái dừng hoàn toàn (CCR = 0) lên tốc độ tối đa (CCR = 550) là:
 * @f[t_{\text{ramp}} = \frac{550}{25} \times 10\text{ ms} = 220\text{ ms}@f]
 * Độ trễ 220 ms đảm bảo xe chuyển động êm ái nhưng vẫn bám sát tín hiệu điều khiển từ bộ thuật toán dẫn đường.
 *
 * @par Đơn vị:
 * Milliseconds (ms).
 */
#define PWM_RAMP_PERIOD_MS              10U

/**
 * @def MOTOR_CMD_MAX
 * @brief Giá trị chuẩn hóa biên độ lệnh vận tốc đầu vào từ tầng điều khiển cấp cao.
 * @details
 * Quy chuẩn thang đo đầu vào của hàm điều khiển động cơ trong khoảng đối xứng:
 * `[-MOTOR_CMD_MAX, +MOTOR_CMD_MAX]` tức từ `-100` đến `+100`.
 * - `+100`: Lệnh chạy tiến với vận tốc cực đại (tương ứng `PWM_DUTY_MAX_LIMIT`).
 * - `-100`: Lệnh chạy lùi với vận tốc cực đại (tương ứng `PWM_DUTY_MAX_LIMIT`).
 * - `0`: Lệnh dừng động cơ (CCR = 0).
 *
 * @par Đơn vị:
 * Phần trăm quy ước (%).
 */
#define MOTOR_CMD_MAX                   100

/**
 * @def MOTOR_BRAKE_HOLD_MS
 * @brief Thời gian duy trì trạng thái phanh điện động học (Dynamic Braking) trước khi thả trôi tự do.
 * @details
 * Cơ chế phanh điện được kích hoạt bằng cách đưa cả 2 chân điều khiển logic của cầu H
 * (IN1=HIGH, IN2=HIGH hoặc IN3=HIGH, IN4=HIGH) lên mức tích cực.
 * Hành động này nối tắt 2 cực cuộn dây phần ứng động cơ qua các transistor công suất xuống GND,
 * biến động cơ thành máy phát tự tiêu tán động năng quán tính thành nhiệt lượng trên điện trở cuộn dây,
 * giúp xe dừng lại tức thì mà không bị trôi tự do.
 *
 * Sau khoảng thời gian 300 ms, xe đã triệt tiêu hoàn toàn vận tốc quán tính và dừng hẳn.
 * Hệ thống sẽ tự động chuyển sang chế độ thả trôi tự do (Coast mode: IN1=LOW, IN2=LOW) nhằm ngắt dòng điện quẩn,
 * ngăn chặn cuộn dây động cơ và IC L298N bị nung nóng không cần thiết.
 *
 * @par Đơn vị:
 * Milliseconds (ms).
 */
#define MOTOR_BRAKE_HOLD_MS             300U

/**
 * @def MOTOR_LEFT_INVERT
 * @brief Cấu hình đảo chiều quay logic cho cụm động cơ bánh xe bên trái.
 * @details
 * Cho phép đảo ngược logic điều khiển chiều quay của động cơ trái bằng phần mềm:
 * - `0`: Giữ nguyên chiều quay phần cứng mặc định (Lệnh dương làm quay bánh xe tiến).
 * - `1`: Đảo ngược trạng thái các chân logic điều khiển IN1 và IN2.
 *
 * @note Tính năng này giúp kỹ sư sửa đổi chiều quay khi phát hiện đấu chéo cực tính dây động cơ
 *       trong quá trình lắp ráp phần cứng mà không cần phải tháo dỡ giắc cắm hoặc hàn lại dây.
 */
#define MOTOR_LEFT_INVERT               0

/**
 * @def MOTOR_RIGHT_INVERT
 * @brief Cấu hình đảo chiều quay logic cho cụm động cơ bánh xe bên phải.
 * @details
 * Cho phép đảo ngược logic điều khiển chiều quay của động cơ phải bằng phần mềm:
 * - `0`: Giữ nguyên chiều quay phần cứng mặc định (Lệnh dương làm quay bánh xe tiến).
 * - `1`: Đảo ngược trạng thái các chân logic điều khiển IN3 và IN4.
 *
 * @note Hỗ trợ cân chỉnh đồng pha hướng chuyển động giữa hai bánh xe độc lập.
 */
#define MOTOR_RIGHT_INVERT              0
/** @} */

/**
 * @name 2. Cấu hình Encoder quang LM393
 * Thiết lập các thông số đo đạc vận tốc vòng quay bánh xe và cơ chế bảo vệ kẹt động cơ (Stall Protection).
 * @{
 */

/**
 * @def ENCODER_PPR
 * @brief Số lượng xung tạo ra trên một vòng quay hoàn chỉnh của bánh xe dẫn động (Pulses Per Revolution).
 * @details
 * Hệ thống sử dụng đĩa mã quang học (Slotted Encoder Disk) gồm 20 khe xuyên tâm gắn đồng trục với bánh xe dẫn động.
 * Cứ mỗi khi một khe đi qua khe thu phát hồng ngoại của module LM393, mạch so sánh sẽ kích hoạt một cạnh xung mức logic
 * đưa vào chân ngắt ngoài EXTI của vi điều khiển STM32.
 *
 * @par Đơn vị:
 * Xung / Vòng (Pulses/rev).
 */
#define ENCODER_PPR                     20U

/**
 * @def WHEEL_DIAMETER_MM
 * @brief Đường kính ngoài danh định của bánh xe dẫn động cao su.
 * @details
 * Thông số hình học cốt lõi phục vụ mô hình động học vi sai (Differential Drive Kinematics) và bài toán Odometry.
 * Được sử dụng để tính chu vi lăn và quy đổi số xung encoder đếm được thành khoảng cách di chuyển thực tế
 * cũng như vận tốc dài của robot:
 * @f[C_{\text{wheel}} = \pi \times D_{\text{wheel}} = \pi \times 65\text{ mm} \approx 204.2\text{ mm}@f]
 * Vận tốc dài tức thời được tính theo công thức:
 * @f[v = \frac{\pi \times D_{\text{wheel}} \times \Delta\text{pulses}}{\text{PPR} \times \Delta t}@f]
 *
 * @par Đơn vị:
 * Millimeters (mm).
 */
#define WHEEL_DIAMETER_MM               65U

/**
 * @def ENCODER_GLITCH_US
 * @brief Cửa sổ thời gian tối thiểu lọc nhiễu nảy cơ học và xung giả của cảm biến quang LM393.
 * @details
 * Bộ lọc khử nhiễu phần mềm trong ngắt ngoài (Software Glitch Filter ISR). Khi có cạnh xung ngắt EXTI kích hoạt,
 * vi điều khiển kiểm tra thời gian trôi qua từ cạnh xung hợp lệ trước đó. Nếu khoảng thời gian @f$\Delta t < 1500\,\mu\text{s}@f$,
 * xung này bị coi là xung nhiễu quang ngẫu nhiên, rung chấn cơ học hoặc tia lửa điện chổi than và sẽ bị bỏ qua.
 *
 * Phân tích giới hạn tần số:
 * - Tần số xung tối đa mà bộ lọc chấp nhận:
 *   @f[f_{\text{pulse\_max}} = \frac{1}{1500\,\mu\text{s}} \approx 666.7\text{ Hz}@f]
 * - Tốc độ vòng quay tối đa của bánh xe tương ứng:
 *   @f[N_{\text{max}} = \frac{666.7\text{ Hz}}{20\text{ PPR}} \times 60 \approx 2000\text{ RPM}@f]
 *
 * Vì tốc độ thực tế của động cơ xe sau hộp số chỉ dao động dưới 250 RPM (< 85 Hz xung),
 * ngưỡng 1500 us hoàn toàn không gây hiện tượng mất xung thực tế của bánh xe.
 *
 * @par Đơn vị:
 * Microseconds (us).
 */
#define ENCODER_GLITCH_US               1500U

/**
 * @def ENCODER_SPEED_WINDOW_MS
 * @brief Cửa sổ thời gian tích lũy xung để tính toán vận tốc bánh xe định kỳ.
 * @details
 * Khoảng thời gian giữa các chu kỳ đọc bộ đếm xung encoder để cập nhật vận tốc dài và tốc độ góc của từng bánh xe.
 * Cứ sau mỗi 100 ms, hàm giám sát sẽ đọc số xung tích lũy trong khoảng thời gian @f$\Delta t@f$,
 * tính toán ra vận tốc @f$v@f$ (mm/s), sau đó xóa/cập nhật lại bộ tích lũy cho chu kỳ kế tiếp.
 *
 * @par Đơn vị:
 * Milliseconds (ms).
 *
 * @par Tần số làm tươi:
 * 10 Hz (1000 / 100).
 */
#define ENCODER_SPEED_WINDOW_MS         100U

/**
 * @def STALL_CCR_THRESHOLD
 * @brief Ngưỡng PWM tối thiểu bắt đầu kích hoạt cơ chế phát hiện kẹt bánh xe (Stall Detection).
 * @details
 * Quy định mức công suất tối thiểu đưa tới động cơ để bộ giám sát bắt đầu theo dõi hiện tượng kẹt cơ khí.
 * Khi giá trị CCR điều khiển lớn hơn hoặc bằng ngưỡng này (động cơ đã được cấp đủ điện áp và mô-men xoắn
 * để thắng ma sát tĩnh lăn bánh bình thường), nhưng bộ đếm xung encoder lại không thu nhận được bất kỳ xung
 * chuyển động nào trong một khoảng thời gian, hệ thống sẽ xác định động cơ đang bị kẹt cứng trục.
 *
 * Ngưỡng này ngăn việc báo lỗi kẹt giả khi xe đang chạy ở tốc độ cực thấp hoặc trong vùng chết (dead-band)
 * ma sát tĩnh của hộp số.
 *
 * @par Đơn vị:
 * Ticks CCR (Tương ứng 20% Duty Cycle).
 */
#define STALL_CCR_THRESHOLD             200

/**
 * @def STALL_TIMEOUT_MS
 * @brief Thời gian kẹt trục liên tục tối đa cho phép trước khi thực hiện ngắt khẩn cấp động cơ.
 * @details
 * Khoảng thời gian duy trì điều kiện kẹt bánh (Duty @f$\ge \text{STALL\_CCR\_THRESHOLD}@f$ và vận tốc encoder @f$= 0@f$)
 * để hệ thống chính thức kích hoạt cơ chế bảo vệ an toàn:
 * - Bật cờ sự cố @ref AMR_FLAG_STALL_LEFT hoặc @ref AMR_FLAG_STALL_RIGHT.
 * - Lập tức ngắt toàn bộ xung PWM (đưa CCR về 0) và kích hoạt phanh dừng để bảo vệ cuộn dây động cơ
 *   không bị nung nóng do dòng điện hãm (Stall Current) tăng vọt.
 * - Bảo vệ tầng công suất IC L298N không bị phá hủy vì quá nhiệt.
 * - Kích hoạt còi Buzzer phát âm thanh cảnh báo lỗi liên tục cho người vận hành.
 *
 * @par Đơn vị:
 * Milliseconds (ms).
 *
 * @warning Không nên đặt giá trị này quá lớn (> 1000 ms), vì dòng điện kẹt của động cơ khi bị chặn đứng trục
 *          rất lớn, có thể gây sụt áp nguồn nghiêm trọng và làm nóng chảy cách điện cuộn dây phần ứng.
 */
#define STALL_TIMEOUT_MS                500U
/** @} */

/**
 * @name 3. Cấu hình cảm biến siêu âm HC-SR04
 * Quản lý thuật toán quét ghép kênh phân chia thời gian (TDM) và cơ chế phanh dừng khẩn cấp tránh va chạm.
 * @{
 */

/**
 * @def SONAR_COUNT
 * @brief Tổng số lượng cảm biến đo khoảng cách siêu âm HC-SR04 trang bị trên thân xe.
 * @details
 * Xe được bố trí 3 module cảm biến siêu âm hướng về phía trước để bao quát toàn diện vùng không gian di chuyển:
 * - Kênh 0: Cảm biến góc quét bên trái (Trig: PB12, Echo: PB13).
 * - Kênh 1: Cảm biến hướng tâm chính giữa (Trig: PB14, Echo: PB15).
 * - Kênh 2: Cảm biến góc quét bên phải (Trig: PB8, Echo: PB9).
 *
 * @par Đơn vị:
 * Cảm biến (Sensors).
 */
#define SONAR_COUNT                     3U

/**
 * @def SONAR_SLOT_MS
 * @brief Khe thời gian quét cho từng cảm biến theo cơ chế phân chia thời gian (Time-Division Multiplexing - TDM).
 * @details
 * Để triệt tiêu hoàn toàn hiện tượng phản xạ chéo (Acoustic Crosstalk - sóng âm của cảm biến này dội ngược
 * vào đầu thu của cảm biến khác gây sai lệch khoảng cách), hệ thống kích hoạt tuần tự từng cảm biến riêng biệt.
 * Mỗi cảm biến được phân bổ một khe thời gian cố định là 25 ms để kích xung và chờ sóng âm phản xạ quay về
 * trước khi kích hoạt cảm biến tiếp theo.
 *
 * Tổng chu kỳ làm tươi toàn bộ 3 cảm biến:
 * @f[T_{\text{scan}} = \text{SONAR\_COUNT} \times \text{SONAR\_SLOT\_MS} = 3 \times 25\text{ ms} = 75\text{ ms}@f]
 * Tương ứng với tần số cập nhật dữ liệu khoảng cách toàn hệ thống là:
 * @f[f_{\text{scan}} = \frac{1000}{75} \approx 13.33\text{ Hz}@f]
 *
 * @par Đơn vị:
 * Milliseconds (ms).
 */
#define SONAR_SLOT_MS                   25U

/**
 * @def SONAR_TRIG_PULSE_US
 * @brief Độ rộng xung mức HIGH phát ra trên chân Trigger để kích hoạt cảm biến HC-SR04.
 * @details
 * Đặc tính kỹ thuật của module HC-SR04 yêu cầu một xung kích mức logic 1 có độ rộng tối thiểu là @f$10\,\mu\text{s}@f$
 * trên chân Trig để vi xử lý trên module phát ra chùm 8 chu kỳ xung siêu âm tần số 40 kHz.
 * Giá trị 12 us đảm bảo biên an toàn dự phòng độ trễ chuyển mạch GPIO của vi điều khiển.
 *
 * @par Đơn vị:
 * Microseconds (us).
 */
#define SONAR_TRIG_PULSE_US             12U

/**
 * @def SONAR_RISE_TIMEOUT_US
 * @brief Thời gian chờ tối đa cho cạnh lên của tín hiệu Echo phản hồi từ module siêu âm.
 * @details
 * Sau khi STM32 kết thúc xung kích Trig, vi xử lý trên cảm biến HC-SR04 sẽ phát chùm sóng âm và kéo chân Echo lên mức HIGH.
 * Nếu sau 6000 us (6 ms) mà chân Echo vẫn giữ ở mức LOW, hệ thống xác định cảm biến đã bị treo cứng phần cứng,
 * mất kết nối hoặc lỏng giắc cắm tín hiệu.
 *
 * @par Đơn vị:
 * Microseconds (us).
 */
#define SONAR_RISE_TIMEOUT_US           6000U

/**
 * @def SONAR_ECHO_MAX_US
 * @brief Độ rộng xung Echo tối đa tương ứng với dải đo giới hạn an toàn của cảm biến.
 * @details
 * Vận tốc truyền sóng âm trong không khí ở nhiệt độ @f$25^\circ\text{C}@f$ xấp xỉ @f$v_s = 343\text{ m/s} = 0.343\text{ mm/}\mu\text{s}@f$.
 * Khoảng cách vật lý đo được xác định theo công thức:
 * @f[d = \frac{t_{\text{echo}} \times 0.343}{2}@f]
 * Với thời gian @f$t_{\text{echo}} = 24000\,\mu\text{s}@f$, khoảng cách tính toán tương đương:
 * @f[d_{\text{calc}} \approx \frac{24000 \times 0.343}{2} \approx 4116\text{ mm} \approx 4.1\text{ m}@f]
 * Nếu độ rộng xung Echo vượt quá 24000 us hoặc chân Echo không kéo xuống LOW sau thời gian này,
 * hệ thống sẽ tự động bão hòa khoảng cách đo về giá trị tối đa (coi như không gian hoàn toàn thông thoáng).
 *
 * @par Đơn vị:
 * Microseconds (us).
 */
#define SONAR_ECHO_MAX_US               24000U

/**
 * @def SONAR_MIN_MM
 * @brief Giới hạn khoảng cách vật lý tối thiểu có thể đo chính xác (Vùng mù gần của cảm biến).
 * @details
 * Module cảm biến siêu âm HC-SR04 có đặc tính vùng mù (blind zone) vật lý đối với các vật thể nằm gần hơn 20 mm (2 cm).
 * Mọi giá trị đo được trả về nhỏ hơn 20 mm sẽ được xử lý quy về 20 mm hoặc đánh giá là vật cản đã tiếp cận sát thân xe.
 *
 * @par Đơn vị:
 * Millimeters (mm).
 */
#define SONAR_MIN_MM                    20U

/**
 * @def SONAR_MAX_MM
 * @brief Giới hạn khoảng cách vật lý tối đa tin cậy của cảm biến siêu âm.
 * @details
 * Khoảng cách đo xa tối đa đáng tin cậy của cảm biến HC-SR04 trong môi trường nhà xưởng công nghiệp (4000 mm = 4.0 m).
 * Mọi kết quả đo khoảng cách lớn hơn ngưỡng này sẽ được phần mềm kẹp (clamp) về 4000 mm
 * để ngăn chặn sai số quá dải trong các thuật toán điều hướng.
 *
 * @par Đơn vị:
 * Millimeters (mm).
 */
#define SONAR_MAX_MM                    4000U

/**
 * @def SONAR_FAIL_LIMIT
 * @brief Số chu kỳ quét rớt tín hiệu liên tiếp để xác nhận cảm biến bị lỗi phần cứng (Fault Filter).
 * @details
 * Bộ lọc số chống báo lỗi cảm biến giả do sóng âm bị suy giảm khi bề mặt vật cản nghiêng hoặc hấp thụ âm thanh.
 * Chỉ khi một cảm biến liên tục bị quá thời gian chờ cạnh lên (rise timeout) hoặc mất tín hiệu Echo
 * quá 5 chu kỳ quét liên tiếp (@f$5 \times 75\text{ ms} = 375\text{ ms}@f$), firmware mới bật cờ lỗi phần cứng tương ứng
 * (`AMR_FLAG_SONAR_FAULT_L`, `AMR_FLAG_SONAR_FAULT_M` hoặc `AMR_FLAG_SONAR_FAULT_R`).
 *
 * @par Đơn vị:
 * Chu kỳ quét lỗi liên tiếp.
 */
#define SONAR_FAIL_LIMIT                5U

/**
 * @def OBSTACLE_STOP_MM_DEFAULT
 * @brief Ngưỡng khoảng cách phát hiện vật cản phía trước để kích hoạt phanh dừng khẩn cấp an toàn.
 * @details
 * Khi cảm biến siêu âm hướng tâm (hoặc góc quét phía trước) phát hiện khoảng cách tới vật thể lạ @f$\le 250\text{ mm}@f$ (25 cm):
 * - Kích hoạt ngay lập tức cờ trạng thái @ref AMR_FLAG_OBSTACLE.
 * - Bộ điều phối an toàn (Supervisor) sẽ cưỡng bức ngắt lệnh tiến của động cơ, kích hoạt phanh điện hãm tốc dừng xe lập tức
 *   nhằm ngăn chặn va chạm cơ học.
 * - Cho phép xe nhận lệnh lùi hoặc quay góc để thoát khỏi vùng vật cản.
 *
 * @par Đơn vị:
 * Millimeters (mm).
 */
#define OBSTACLE_STOP_MM_DEFAULT        250U

/**
 * @def OBSTACLE_HYSTERESIS_MM
 * @brief Dải khoảng cách trễ hồi tiếp (Hysteresis Band) để giải phóng trạng thái phanh dừng vật cản.
 * @details
 * Ngăn ngừa hiện tượng dao động bật/tắt liên tục (Chattering) xung quanh ngưỡng khoảng cách dừng
 * khi vật cản đứng mấp mé ở ranh giới 250 mm.
 * Xe chỉ tự động giải phóng cờ @ref AMR_FLAG_OBSTACLE và cho phép tiếp tục hành trình khi khoảng cách đo được
 * từ cảm biến vượt qua ngưỡng giải phóng:
 * @f[d_{\text{clear}} \ge \text{OBSTACLE\_STOP\_MM\_DEFAULT} + \text{OBSTACLE\_HYSTERESIS\_MM} = 250\text{ mm} + 50\text{ mm} = 300\text{ mm}@f]
 *
 * @par Đơn vị:
 * Millimeters (mm).
 */
#define OBSTACLE_HYSTERESIS_MM          50U

/**
 * @def OBSTACLE_BLOCK_ON_SONAR_FAULT
 * @brief Cơ chế ứng xử an toàn (Fail-Safe Strategy) khi cảm biến siêu âm chính giữa gặp sự cố phần cứng.
 * @details
 * Xác định hành vi xử lý của robot khi phát hiện cảm biến siêu âm trung tâm bị đứt dây hoặc mất tín hiệu:
 * - `1`: Chế độ an toàn công nghiệp tuyệt đối (Fail-Safe Active). Nếu cảm biến giữa bị lỗi, firmware tự động
 *        kích hoạt cờ vật cản @ref AMR_FLAG_OBSTACLE và khóa cứng chuyển động tiến của xe để loại trừ nguy cơ va chạm do mù cảm biến.
 * - `0`: Chế độ cho phép di chuyển (Bypass Fault). Bỏ qua lỗi mất cảm biến, chỉ áp dụng trong môi trường thí nghiệm
 *        trên bàn test khi chưa lắp ráp đủ module cảm biến.
 *
 * @warning Phải luôn đặt bằng `1` khi triển khai robot chạy thực tế trên sàn nhà xưởng.
 */
#define OBSTACLE_BLOCK_ON_SONAR_FAULT   1
/** @} */

/**
 * @name 4. Cấu hình đo áp pin Li-ion 4S (PA6 / ADC1_IN6)
 * Giám sát điện áp nguồn pin, lọc số khử nhiễu IIR và các ngưỡng bảo vệ xả cạn an toàn.
 * @{
 */

/**
 * @def BATTERY_MV_NUM
 * @brief Tử số của tỷ số phân áp chuyển đổi giá trị số ADC 12-bit thô (raw) sang điện áp pin (mV).
 * @details
 * Mạch đo áp pin trên bo mạch sử dụng cầu phân áp điện trở gồm điện trở nhánh trên @f$R_1 = 100\text{ k}\Omega@f$
 * và nhánh dưới @f$R_2 = 22\text{ k}\Omega@f$ nối tới chân PA6 (kênh ADC1_IN6).
 * Điện áp tham chiếu @f$V_{\text{ref}} = 3300\text{ mV}@f$, độ phân giải ADC 12-bit (@f$2^{12} - 1 = 4095@f$).
 *
 * Công thức chuyển đổi điện áp lý thuyết:
 * @f[V_{\text{bat}} = \text{ADC}_{\text{raw}} \times \frac{V_{\text{ref}}}{4095} \times \frac{R_1 + R_2}{R_2} = \text{ADC}_{\text{raw}} \times \frac{3300}{4095} \times \frac{100 + 22}{22} \approx \text{ADC}_{\text{raw}} \times 4.4727\text{ mV}@f]
 *
 * Để tối ưu hóa hiệu năng tính toán trên nhân ARM Cortex-M3 (không có bộ đồng xử lý dấu phẩy động FPU),
 * tỷ số thực tế được quy đổi sang dạng phân số số nguyên lớn với tử số `BATTERY_MV_NUM` và mẫu số `BATTERY_MV_DEN` (Fixed-point arithmetic).
 *
 * @par Đơn vị:
 * Hằng số nguyên tính toán.
 */
#define BATTERY_MV_NUM                  702900UL

/**
 * @def BATTERY_MV_DEN
 * @brief Mẫu số của tỷ số phân áp chuyển đổi giá trị ADC thô sang millivolt (mV).
 * @details
 * Mẫu số trong phép chia số nguyên chuyển đổi điện áp:
 * @f[V_{\text{bat\_raw}}[\text{mV}] = \frac{\text{ADC}_{\text{raw}} \times \text{BATTERY\_MV\_NUM}}{\text{BATTERY\_MV\_DEN}}@f]
 * Phép toán này đảm bảo độ chính xác tính toán điện áp với sai số dưới 1 mV mà không tiêu tốn chu kỳ lệnh
 * cho thư viện phần mềm mô phỏng số thực (Software Floating-Point Emulation).
 *
 * @par Đơn vị:
 * Hằng số nguyên tính toán.
 */
#define BATTERY_MV_DEN                  135135UL

/**
 * @def BATTERY_CAL_X10000
 * @brief Hệ số cân chỉnh thực tế đơn điểm (Single-point Calibration Factor) nhân tỷ lệ phóng đại 10000.
 * @details
 * Bù trừ sai số linh kiện thực tế của mạch phần cứng, bao gồm sai số dung sai điện trở phân áp (@f$\pm 1\%@f$)
 * và sai số trôi điện áp của IC ổn áp LDO 3.3 V cấp nguồn tham chiếu cho MCU.
 * - Giá trị mặc định: `10000` (tương ứng với hệ số hiệu chỉnh @f$1.0000@f$).
 * - Công thức tính toán hệ số cân chỉnh thực tế bằng đồng hồ vạn năng VOM chuẩn:
 *   @f[\text{BATTERY\_CAL\_X10000} = \frac{V_{\text{VOM\_do\_thuc\_te}}}{V_{\text{hien\_thi\_chua\_cal}}} \times 10000@f]
 *
 * @par Đơn vị:
 * Hệ số phóng đại x10000.
 */
#define BATTERY_CAL_X10000              10000UL

/**
 * @def BATTERY_SAMPLE_MS
 * @brief Chu kỳ thời gian đọc mẫu bộ chuyển đổi tương tự-số (ADC) đo điện áp pin.
 * @details
 * Khoảng thời gian định kỳ khởi động chuyển đổi ADC và kích hoạt bộ lọc số đo pin.
 * Giá trị 50 ms tương ứng với tần số lấy mẫu 20 Hz, đảm bảo khả năng phản ứng kịp thời với các đợt sụt áp
 * do động cơ khởi động đồng thời không gây hao tổn tài nguyên xử lý ngắt CPU.
 *
 * @par Đơn vị:
 * Milliseconds (ms).
 */
#define BATTERY_SAMPLE_MS               50U

/**
 * @def BATTERY_IIR_SHIFT
 * @brief Hệ số dịch bit của bộ lọc số thông thấp đệ quy bậc 1 (Single-pole Low-Pass IIR Filter).
 * @details
 * Bộ lọc số khử nhiễu gai điện áp và sụt áp tức thời do đóng cắt xung PWM động cơ gây ra trên đường bus nguồn pin.
 * Thuật toán lọc số nguyên dịch bit cực kỳ tối ưu:
 * @f[V_{\text{filtered}}[k] = V_{\text{filtered}}[k-1] + \frac{V_{\text{raw}}[k] - V_{\text{filtered}}[k-1]}{2^{\text{BATTERY\_IIR\_SHIFT}}}@f]
 * Với @f$\text{SHIFT} = 3 \implies \alpha = \frac{1}{2^3} = \frac{1}{8} = 0.125@f$.
 *
 * Hằng số thời gian đáp ứng (Time Constant) của bộ lọc:
 * @f[\tau \approx \frac{\text{BATTERY\_SAMPLE\_MS}}{\alpha} = \frac{50\text{ ms}}{0.125} = 400\text{ ms}@f]
 * Bộ lọc làm phẳng tín hiệu đo điện áp trong khoảng 0.4 giây, loại trừ hiện tượng báo pin ảo.
 *
 * @par Đơn vị:
 * Số bit dịch (Shift bits).
 */
#define BATTERY_IIR_SHIFT               3U

/**
 * @def BATTERY_WARN_MV
 * @brief Ngưỡng điện áp kích hoạt trạng thái cảnh báo pin yếu (Battery Low Warning).
 * @details
 * Đối với khối pin Li-ion 4S (4 cell mắc nối tiếp), ngưỡng cảnh báo tương ứng với mức điện áp 13.6 V
 * (3.4 V / cell, dung lượng còn lại khoảng 15% - 20%).
 * Khi điện áp pin đo được sụt xuống dưới ngưỡng này:
 * - Bật cờ trạng thái @ref AMR_FLAG_BAT_LOW.
 * - Gửi cảnh báo qua bản tin Telemetry để Raspberry Pi lên kế hoạch dẫn đường đưa xe về trạm sạc tự động.
 * - Kích hoạt còi Buzzer phát âm thanh cảnh báo ngắt quãng.
 *
 * @par Đơn vị:
 * Millivolts (mV).
 */
#define BATTERY_WARN_MV                 13600U

/**
 * @def BATTERY_WARN_RELEASE_MV
 * @brief Ngưỡng điện áp giải phóng trạng thái cảnh báo pin yếu (Warning Clear Hysteresis).
 * @details
 * Độ trễ điện áp chống hiện tượng bật tắt cảnh báo chập chờn khi pin được giảm tải.
 * Khi xe dừng động cơ, điện áp pin sẽ tự hồi phục nhẹ lên trên 13.6 V.
 * Hệ thống chỉ xóa cờ @ref AMR_FLAG_BAT_LOW khi điện áp pin sạc hoặc phục hồi đạt ngưỡng @f$\ge 13.9\text{ V}@f$ (3.475 V / cell).
 *
 * @par Đơn vị:
 * Millivolts (mV).
 */
#define BATTERY_WARN_RELEASE_MV         13900U

/**
 * @def BATTERY_CUTOFF_MV
 * @brief Ngưỡng điện áp ngắt tải khẩn cấp bảo vệ chống xả cạn kiệt khối pin (Critical Low Voltage).
 * @details
 * Điện áp giới hạn xả an toàn cực đại của khối pin Li-ion 4S (12.0 V, tương đương 3.0 V / cell, dung lượng xấp xỉ 0%).
 * Nếu tiếp tục xả pin dưới ngưỡng 3.0 V/cell, các phản ứng hóa học không thuận nghịch sẽ phá hủy cấu trúc
 * điện cực tế bào pin, gây chai phù pin vĩnh viễn hoặc mất an toàn cháy nổ khi sạc lại.
 *
 * Khi điện áp chạm ngưỡng này liên tục quá `BATTERY_CUTOFF_HOLD_MS`:
 * - Kích hoạt cờ ngắt khẩn cấp @ref AMR_FLAG_BAT_CRITICAL.
 * - Cắt toàn bộ công suất động cơ, cô lập tải.
 * - Bắt đầu tiến trình shutdown hệ thống và cắt nguồn chính.
 *
 * @par Đơn vị:
 * Millivolts (mV).
 *
 * @warning Tuyệt đối không hạ ngưỡng này xuống dưới 12000 mV để bảo vệ tuổi thọ khối pin Li-ion.
 */
#define BATTERY_CUTOFF_MV               12000U

/**
 * @def BATTERY_CUTOFF_HOLD_MS
 * @brief Thời gian duy trì liên tục dưới ngưỡng cắt xả trước khi chính thức ngắt nguồn (Cutoff Hold Debounce).
 * @details
 * Bộ lọc trễ thời gian chống ngắt nguồn nhầm lẫn do sụt áp đột biến (Voltage Sag) sinh ra khi 2 động cơ
 * đồng thời khởi động hoặc leo dốc mang tải nặng. Hệ thống chỉ ra lệnh ngắt nguồn khi điện áp pin sụt dưới
 * `BATTERY_CUTOFF_MV` và duy trì liên tục kéo dài quá 3000 ms (3 giây).
 *
 * @par Đơn vị:
 * Milliseconds (ms).
 */
#define BATTERY_CUTOFF_HOLD_MS          3000U

/**
 * @def BATTERY_ABSENT_MV
 * @brief Ngưỡng nhận diện không kết nối khối pin vật lý (Battery Disconnected Detection).
 * @details
 * Phát hiện tình huống cấp nguồn phục vụ nạp chương trình hoặc debug trên bàn làm việc qua cổng ST-Link
 * hoặc cổng Micro-USB 5V (không cắm giắc pin chính 4S).
 * Khi không có pin, điện áp đo được tại cầu phân áp chỉ ở mức nhiễu trôi (< 6.0 V):
 * - Hệ thống tự động kích hoạt cờ @ref AMR_FLAG_BAT_ABSENT.
 * - Tạm thời vô hiệu hóa cơ chế bảo vệ cắt nguồn pin để tránh việc vi điều khiển tự động kích hoạt
 *   tiến trình ngắt nguồn/tắt máy khi kỹ sư đang nạp code và debug.
 *
 * @par Đơn vị:
 * Millivolts (mV).
 */
#define BATTERY_ABSENT_MV               6000U
/** @} */

/**
 * @name 5. Cấu hình quản lý nguồn (Smart Power Latch PB0 / PB1 / PB10)
 * Mạch tự giữ nguồn mềm (Power Latch) và giao thức phối hợp tắt máy an toàn với Raspberry Pi.
 * @{
 */

/**
 * @def PWR_BTN_DEBOUNCE_MS
 * @brief Thời gian lọc chống rung tiếp điểm cơ khí cho nút nhấn nguồn (Power Button Debounce).
 * @details
 * Nút nhấn kích hoạt nguồn vật lý trên thân vỏ xe được nối tới chân PB1. Bộ lọc phần mềm yêu cầu mức logic
 * của nút phải giữ ổn định liên tục quá 50 ms mới công nhận sự kiện nhấn hoặc nhả, loại bỏ triệt để các xung nảy cơ khí.
 *
 * @par Đơn vị:
 * Milliseconds (ms).
 */
#define PWR_BTN_DEBOUNCE_MS             50U

/**
 * @def PWR_BTN_SHORT_MAX_MS
 * @brief Giới hạn thời gian tối đa để xác nhận thao tác nhấn nhả nhanh nút nguồn (Short Press).
 * @details
 * Quy ước cử chỉ thao tác của người vận hành:
 * Nếu nút nguồn được nhấn và nhả ra trong khoảng thời gian @f$50\text{ ms} \le t_{\text{press}} < 800\text{ ms}@f$
 * khi hệ thống đang hoạt động bình thường, firmware sẽ giải mã là lệnh yêu cầu kích hoạt nhanh chế độ tự kiểm tra phần cứng (Self-Test mode).
 *
 * @par Đơn vị:
 * Milliseconds (ms).
 */
#define PWR_BTN_SHORT_MAX_MS            800U

/**
 * @def PWR_BTN_LONG_MS
 * @brief Thời gian nhấn giữ tối thiểu nút nguồn để yêu cầu tắt máy chủ động (Long Press Power-Off).
 * @details
 * Nếu người dùng nhấn và giữ nút nguồn liên tục trong thời gian @f$t_{\text{press}} \ge 1500\text{ ms}@f$ (1.5 giây),
 * hệ thống xác nhận người dùng muốn tắt nguồn xe an toàn. STM32 sẽ phát tín hiệu ngắt báo hiệu cho Raspberry Pi
 * và bắt đầu chu trình đếm ngược shutdown hệ thống.
 *
 * @par Đơn vị:
 * Milliseconds (ms).
 */
#define PWR_BTN_LONG_MS                 1500U

/**
 * @def PI_HALT_DEBOUNCE_MS
 * @brief Thời gian lọc ổn định tín hiệu xác nhận tắt máy (GPIO26 Halt Signal) từ Raspberry Pi.
 * @details
 * Raspberry Pi được cấu hình overlay hạt nhân Linux (`dtoverlay=gpio-poweroff,gpiopin=26,active_low=0`)
 * sẽ kéo chân GPIO26 lên mức logic 1 (HIGH) khi và chỉ khi hệ điều hành hoàn tất việc dừng mọi dịch vụ
 * và ngắt tiến trình hạt nhân an toàn.
 * Chân PB10 của STM32 giám sát tín hiệu này và yêu cầu mức HIGH phải duy trì ổn định liên tục 200 ms
 * để tránh nhận nhầm xung nhiễu điện từ.
 *
 * @par Đơn vị:
 * Milliseconds (ms).
 */
#define PI_HALT_DEBOUNCE_MS             200U

/**
 * @def PI_HALT_SETTLE_MS
 * @brief Thời gian trễ chờ Linux và thẻ nhớ MicroSD hoàn tất đồng bộ trước khi cắt nguồn chính.
 * @details
 * Sau khi nhận được tín hiệu GPIO26 báo Pi Halt hợp lệ, STM32 chờ thêm 800 ms trước khi kéo chân giữ nguồn PB0
 * xuống LOW để ngắt relay. Khoảng thời gian này đảm bảo chip điều khiển thẻ nhớ MicroSD trên Raspberry Pi hoàn tất việc
 * xả sạch bộ đệm ghi (flush write cache) vào chip nhớ flash, chống hỏng phân vùng dữ liệu Linux (Filesystem Corruption).
 *
 * @par Đơn vị:
 * Milliseconds (ms).
 */
#define PI_HALT_SETTLE_MS               800U

/**
 * @def PI_HALT_TIMEOUT_MS
 * @brief Thời gian chờ tối đa cho Raspberry Pi shutdown trước khi cưỡng bức ngắt nguồn (Power-Off Watchdog).
 * @details
 * Cơ chế bảo vệ quá thời gian (Fail-Safe Shutdown Watchdog). Nếu sau khi phát lệnh tắt máy mà Raspberry Pi
 * bị treo cứng hạt nhân (Kernel Panic) hoặc dịch vụ hệ thống bị khóa khiến chân GPIO26 không bao giờ lên mức HIGH,
 * sau 45 giây watchdog nguồn sẽ kích hoạt, cưỡng bức kéo chân PB0 về LOW để ngắt rơ-le nguồn chính, bảo vệ pin không bị xả kiệt.
 *
 * @par Đơn vị:
 * Milliseconds (ms).
 */
#define PI_HALT_TIMEOUT_MS              45000U
/** @} */

/**
 * @name 6. Cờ trạng thái hệ thống
 * Mặt nạ bit (Bitmask) biểu diễn trạng thái và sự cố toàn cục trong thanh ghi `amr_flags`.
 * @{
 */

/**
 * @def AMR_FLAG_OBSTACLE
 * @brief Bit 0: Cờ phát hiện vật cản phía trước trong dải dừng an toàn khẩn cấp.
 * @details
 * Được kích hoạt khi bất kỳ cảm biến siêu âm nào ghi nhận khoảng cách @f$\le \text{OBSTACLE\_STOP\_MM\_DEFAULT}@f$.
 * Khi cờ này tích cực, hệ thống khóa chiều chuyển động tiến của xe và kích hoạt phanh điện.
 */
#define AMR_FLAG_OBSTACLE               (1UL << 0)

/**
 * @def AMR_FLAG_STALL_LEFT
 * @brief Bit 1: Cờ báo động sự cố kẹt cơ khí bánh xe dẫn động bên trái.
 * @details
 * Được kích hoạt khi động cơ trái nhận lệnh công suất lớn (CCR @f$\ge \text{STALL\_CCR\_THRESHOLD}@f$)
 * nhưng encoder không phản hồi xung trong thời gian quá `STALL_TIMEOUT_MS`. Lập tức cắt ga và báo động.
 */
#define AMR_FLAG_STALL_LEFT             (1UL << 1)

/**
 * @def AMR_FLAG_STALL_RIGHT
 * @brief Bit 2: Cờ báo động sự cố kẹt cơ khí bánh xe dẫn động bên phải.
 * @details
 * Được kích hoạt khi động cơ phải nhận lệnh công suất lớn (CCR @f$\ge \text{STALL\_CCR\_THRESHOLD}@f$)
 * nhưng encoder không phản hồi xung trong thời gian quá `STALL_TIMEOUT_MS`. Lập tức cắt ga và báo động.
 */
#define AMR_FLAG_STALL_RIGHT            (1UL << 2)

/**
 * @def AMR_FLAG_BAT_LOW
 * @brief Bit 3: Cờ cảnh báo mức điện áp pin Li-ion yếu.
 * @details
 * Được kích hoạt khi điện áp pin sụt xuống dưới ngưỡng cảnh báo `BATTERY_WARN_MV` (13.6 V).
 * Kích hoạt còi báo chu kỳ và truyền telemetry để tầng điều khiển cấp cao đưa robot về trạm sạc.
 */
#define AMR_FLAG_BAT_LOW                (1UL << 3)

/**
 * @def AMR_FLAG_BAT_CRITICAL
 * @brief Bit 4: Cờ ngắt bảo vệ khẩn cấp do điện áp pin chạm ngưỡng xả cạn nguy hiểm.
 * @details
 * Được kích hoạt khi điện áp pin sụt xuống dưới `BATTERY_CUTOFF_MV` (12.0 V) liên tục quá `BATTERY_CUTOFF_HOLD_MS`.
 * Hệ thống lập tức ngắt toàn bộ tải động cơ và tiến hành tắt máy để bảo vệ các cell pin Li-ion.
 */
#define AMR_FLAG_BAT_CRITICAL           (1UL << 4)

/**
 * @def AMR_FLAG_BAT_ABSENT
 * @brief Bit 5: Cờ nhận diện không có kết nối khối pin vật lý.
 * @details
 * Được kích hoạt khi điện áp đo được tại cầu phân áp @f$< \text{BATTERY\_ABSENT\_MV}@f$ (6.0 V),
 * nhận biết hệ thống đang cấp nguồn debug qua ST-Link/USB. Tự động vô hiệu hóa bảo vệ xả pin.
 */
#define AMR_FLAG_BAT_ABSENT             (1UL << 5)

/**
 * @def AMR_FLAG_SONAR_FAULT_L
 * @brief Bit 6: Cờ báo lỗi phần cứng mất tín hiệu cảm biến siêu âm bên trái.
 * @details
 * Được kích hoạt khi cảm biến siêu âm trái bị mất tín hiệu Echo hoặc timeout cạnh lên liên tục quá `SONAR_FAIL_LIMIT` lần.
 */
#define AMR_FLAG_SONAR_FAULT_L          (1UL << 6)

/**
 * @def AMR_FLAG_SONAR_FAULT_M
 * @brief Bit 7: Cờ báo lỗi phần cứng mất tín hiệu cảm biến siêu âm chính giữa.
 * @details
 * Được kích hoạt khi cảm biến siêu âm giữa bị mất tín hiệu liên tục quá `SONAR_FAIL_LIMIT` lần.
 * Nếu `OBSTACLE_BLOCK_ON_SONAR_FAULT = 1`, xe sẽ tự động dừng phanh khẩn cấp để đảm bảo an toàn.
 */
#define AMR_FLAG_SONAR_FAULT_M          (1UL << 7)

/**
 * @def AMR_FLAG_SONAR_FAULT_R
 * @brief Bit 8: Cờ báo lỗi phần cứng mất tín hiệu cảm biến siêu âm bên phải.
 * @details
 * Được kích hoạt khi cảm biến siêu âm phải bị mất tín hiệu Echo hoặc timeout cạnh lên liên tục quá `SONAR_FAIL_LIMIT` lần.
 */
#define AMR_FLAG_SONAR_FAULT_R          (1UL << 8)

/**
 * @def AMR_FLAG_SHUTDOWN_PENDING
 * @brief Bit 9: Cờ báo hệ thống đang trong tiến trình đếm ngược tắt máy và cắt nguồn.
 * @details
 * Được kích hoạt khi nhận lệnh tắt máy hợp lệ từ nút nhấn hoặc cảnh báo pin cạn.
 * Khi cờ này bật, toàn bộ các lệnh vận tốc mới từ Raspberry Pi sẽ bị từ chối để chuẩn bị cắt nguồn an toàn.
 */
#define AMR_FLAG_SHUTDOWN_PENDING       (1UL << 9)
/** @} */

#endif /* INC_AMR_CONFIG_H_ */
