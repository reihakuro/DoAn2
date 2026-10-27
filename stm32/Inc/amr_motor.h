/**
 ******************************************************************************
 * @file    amr_motor.h
 * @brief   Module điều khiển công suất 2 cụm động cơ dẫn động vi sai qua mạch cầu H L298N.
 * @details
 * Cung cấp giải thuật điều khiển chuyển động cho robot di động 4 bánh vi sai (Differential Drive):
 * - Ngoại vi băm xung PWM: Timer 2 cấu hình tần số 1 kHz (kênh TIM2_CH1 / chân PA0 nối chân ENA cầu trái,
 *   kênh TIM2_CH4 / chân PA3 nối chân ENB cầu phải).
 * - Chân điều khiển logic chiều quay:
 *   - Bánh trái: Chân PA1 (IN1) và PA2 (IN2).
 *   - Bánh phải: Chân PA4 (IN3) và PA5 (IN4).
 * - Khống chế điện áp an toàn: Giới hạn trần thanh ghi CCR không vượt quá @ref PWM_DUTY_MAX_LIMIT (550 = 55.0% Duty Cycle)
 *   nhằm tương thích với dải điện áp 6 V - 8 V của động cơ khi cấp nguồn từ pin Li-ion 4S (14.8 V).
 * - Làm mượt gia tốc (Software Ramp Limiter): Tăng/giảm độ dốc xung từng bước @ref PWM_RAMP_STEP (25 ticks)
 *   sau mỗi chu kỳ @ref PWM_RAMP_PERIOD_MS (10 ms), triệt tiêu dòng khởi động và chống trượt bánh.
 * - Cơ chế phanh điện động học (Dynamic Braking): Nối tắt 2 đầu cuộn dây động cơ trong thời gian @ref MOTOR_BRAKE_HOLD_MS (300 ms)
 *   để dừng khẩn cấp tức thì, sau đó tự động chuyển sang chế độ thả trôi (Coast Mode) để bảo vệ driver.
 * - Khóa an toàn phân vùng: Hỗ trợ khóa toàn bộ chuyển động (@ref AMR_Motor_SetInhibit) hoặc chỉ chặn riêng chiều tiến
 *   khi có vật cản phía trước (@ref AMR_Motor_SetForwardBlocked).
 *
 * @version 2.1
 * @date    2026
 ******************************************************************************
 */

#ifndef INC_AMR_MOTOR_H_
#define INC_AMR_MOTOR_H_

#include "amr_config.h"

/**
 * @brief Khởi tạo phần cứng PWM Timer 2 và các chân GPIO điều khiển hướng động cơ.
 * @details Kích hoạt kênh TIM2_CH1 và TIM2_CH4, thiết lập các chân logic IN1..IN4 về mức LOW (trạng thái thả trôi).
 *
 * @note Hàm này chỉ cần gọi một lần duy nhất tại @ref AMR_App_Init.
 */
void AMR_Motor_Init(void);

/**
 * @brief Đặt giá trị vận tốc mục tiêu cho hai cụm động cơ trái và phải.
 * @details
 * Nhận lệnh vận tốc chuẩn hóa trong thang đối xứng `[-MOTOR_CMD_MAX, +MOTOR_CMD_MAX]` (tức từ `-100` đến `+100`):
 * - Các giá trị vượt ngoài dải sẽ tự động được kẹp (clamp) về biên an toàn.
 * - Giá trị thanh ghi CCR thực tế xuất xuống động cơ sẽ không nhảy bậc tức thời mà được nội suy
 *   tăng/giảm dần dần theo thuật toán gia tốc độ dốc (Slew-rate Ramp).
 *
 * @param[in] left  Lệnh vận tốc mong muốn cho cụm động cơ trái (`-100` đến `+100`).
 * @param[in] right Lệnh vận tốc mong muốn cho cụm động cơ phải (`-100` đến `+100`).
 */
void AMR_Motor_SetSpeed(int16_t left, int16_t right);

/**
 * @brief Hàm cập nhật thuật toán làm mượt gia tốc dốc PWM định kỳ.
 * @details
 * Được gọi định kỳ mỗi @ref PWM_RAMP_PERIOD_MS (10 ms) từ vòng lặp chính của Supervisor.
 * Thực hiện:
 * 1. Kiểm tra trạng thái phanh điện động học, tự động chuyển về thả trôi sau khi hết thời gian 300 ms.
 * 2. So sánh giá trị CCR hiện tại với giá trị CCR mục tiêu tương ứng với lệnh điều khiển.
 * 3. Tăng hoặc giảm giá trị CCR từng bước tối đa @ref PWM_RAMP_STEP ticks.
 * 4. Xuất giá trị thanh ghi CCR và mức logic IN1..IN4 xuống phần cứng cầu H.
 */
void AMR_Motor_Update(void);

/**
 * @brief Kích hoạt quy trình phanh dừng khẩn cấp (Emergency Stop).
 * @details
 * Lập tức bỏ qua bộ làm mượt gia tốc, đưa cả hai đầu cuộn dây động cơ lên mức logic HIGH (Dynamic Braking)
 * để sinh sức phản điện động hãm cứng trục động cơ trong thời gian @ref MOTOR_BRAKE_HOLD_MS (300 ms).
 * Lệnh điều khiển gốc của người dùng vẫn được lưu giữ; sau khi phanh dừng hoàn tất, động cơ chỉ khởi động lại
 * (có dốc tăng tốc êm) nếu không còn bất kỳ điều kiện khóa an toàn nào tồn tại.
 *
 * @note An toàn tuyệt đối khi gọi lặp lại liên tục từ các cơ chế giám sát an toàn.
 */
void AMR_Motor_EmergencyStop(void);

/**
 * @brief Cấm toàn bộ mọi chuyển động của xe (Toàn quyền khóa ga).
 * @details
 * Áp dụng khi hệ thống xảy ra các sự cố nghiêm trọng: kẹt cơ khí bánh xe (Stall),
 * hệ thống đang đếm ngược tắt máy (Shutdown Pending), hoặc mất kết nối truyền thông.
 * Khi bị khóa, toàn bộ giá trị PWM xuất ra động cơ bị cưỡng bức về 0 và chuyển sang phanh/thả trôi.
 *
 * @param[in] inhibit Trạng thái khóa:
 *                    - `true`: Khóa toàn bộ chuyển động.
 *                    - `false`: Giải phóng khóa, cho phép điều khiển bình thường.
 */
void AMR_Motor_SetInhibit(bool inhibit);

/**
 * @brief Khóa riêng chiều chuyển động TIẾN của robot (Chống va chạm phía trước).
 * @details
 * Được kích hoạt khi cảm biến siêu âm phát hiện vật cản nằm trong khoảng cách nguy hiểm (@ref AMR_FLAG_OBSTACLE).
 * Cơ chế này ngăn chặn xe tiếp tục đâm về phía trước, nhưng vẫn cho phép xe nhận lệnh LÙI hoặc QUAY TẠI CHỖ
 * (điều kiện tổng đại số vận tốc: @f$\text{left} + \text{right} \le 0@f$) để chủ động thoát khỏi vùng vật cản.
 *
 * @param[in] blocked Trạng thái chặn chiều tiến:
 *                    - `true`: Chặn chuyển động tiến.
 *                    - `false`: Cho phép chạy tiến bình thường.
 */
void AMR_Motor_SetForwardBlocked(bool blocked);

/**
 * @brief Đọc giá trị PWM CCR có dấu đang xuất thực tế xuống driver cầu H cho bánh trái.
 * @return Giá trị CCR có dấu trong dải `-550` đến `+550`.
 *         Dấu dương thể hiện quay tiến, dấu âm thể hiện quay lùi.
 */
int16_t AMR_Motor_GetPwmLeft(void);

/**
 * @brief Đọc giá trị PWM CCR có dấu đang xuất thực tế xuống driver cầu H cho bánh phải.
 * @return Giá trị CCR có dấu trong dải `-550` đến `+550`.
 *         Dấu dương thể hiện quay tiến, dấu âm thể hiện quay lùi.
 */
int16_t AMR_Motor_GetPwmRight(void);

/**
 * @brief Đọc giá trị lệnh vận tốc mục tiêu hiện tại của bánh xe bên trái.
 * @return Lệnh vận tốc trong dải `-100` đến `+100`.
 */
int16_t AMR_Motor_GetCmdLeft(void);

/**
 * @brief Đọc giá trị lệnh vận tốc mục tiêu hiện tại của bánh xe bên phải.
 * @return Lệnh vận tốc trong dải `-100` đến `+100`.
 */
int16_t AMR_Motor_GetCmdRight(void);

/**
 * @brief Kiểm tra xem hệ thống có đang trong giai đoạn duy trì phanh điện động học hay không.
 * @return Trạng thái phanh:
 *         - `true`: Đang trong khoảng thời gian nối tắt phanh điện 300 ms.
 *         - `false`: Đã kết thúc phanh hoặc đang vận hành bình thường.
 */
bool AMR_Motor_IsBraking(void);

#endif /* INC_AMR_MOTOR_H_ */
