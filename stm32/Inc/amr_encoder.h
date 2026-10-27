/**
 ******************************************************************************
 * @file    amr_encoder.h
 * @brief   Module đếm xung Encoder quang học LM393, tính toán vận tốc và phát hiện kẹt bánh xe.
 * @details
 * Cung cấp chức năng thu thập xung ngắt ngoài từ 2 cảm biến đĩa quang gắn trên bánh xe robot:
 * - Kênh phần cứng: Chân PB6 (Bánh trái) và PB7 (Bánh phải) cấu hình ngắt ngoài EXTI cạnh lên (Rising Edge).
 * - Lọc chống nảy xung (Glitch Filter): Bỏ qua các xung ngắt có khoảng cách @f$< 1500\,\mu\text{s}@f$ (@ref ENCODER_GLITCH_US).
 * - Suy diễn chiều quay: Do encoder quang 1 kênh không có kênh pha B lệch pha 90 độ (như encoder vuông pha quadrature),
 *   chiều quay dấu (+/-) được suy diễn từ dấu của giá trị PWM CCR điều khiển đang xuất xuống driver tại thời điểm đó.
 * - Giám sát kẹt cơ khí (Stall Protection): Khi động cơ nhận lệnh điều khiển lớn hơn @ref STALL_CCR_THRESHOLD
 *   mà không có xung encoder phản hồi trong thời gian quá @ref STALL_TIMEOUT_MS, hệ thống sẽ chốt cờ báo kẹt.
 *
 * @version 2.1
 * @date    2026
 ******************************************************************************
 */

#ifndef INC_AMR_ENCODER_H_
#define INC_AMR_ENCODER_H_

#include "amr_config.h"

/**
 * @enum EncoderSide_t
 * @brief Định danh kênh cảm biến encoder cho từng phía bánh xe dẫn động.
 */
typedef enum
{
    /** @brief Kênh encoder bánh xe bên trái (gắn chân PB6, EXTI line 6). */
    ENC_LEFT  = 0,

    /** @brief Kênh encoder bánh xe bên phải (gắn chân PB7, EXTI line 7). */
    ENC_RIGHT = 1,

    /** @brief Tổng số lượng kênh encoder độc lập trang bị trên xe. */
    ENC_COUNT = 2
} EncoderSide_t;

/**
 * @brief Khởi tạo phần cứng ngắt ngoài EXTI và cấu trúc dữ liệu của bộ đếm encoder.
 * @details Cấu hình chân PB6 và PB7 ở chế độ ngõ vào kéo xuống (Pull-down), kích hoạt ngắt trên cạnh lên trong NVIC.
 *
 * @note Hàm này chỉ cần gọi một lần duy nhất tại @ref AMR_App_Init.
 */
void AMR_Encoder_Init(void);

/**
 * @brief Hàm cập nhật thuật toán tính toán vận tốc và theo dõi kẹt cơ khí định kỳ.
 * @details
 * Được gọi định kỳ mỗi 10 ms từ vòng lặp chính của Supervisor. Cứ sau mỗi cửa sổ thời gian
 * @ref ENCODER_SPEED_WINDOW_MS (100 ms), hàm sẽ:
 * 1. Đọc số lượng xung ngắt ngoài đã đếm được trong khoảng thời gian cửa sổ.
 * 2. Tính toán vận tốc góc quy đổi ra xung/giây (pps) và vận tốc dài (mm/s).
 * 3. Tích lũy quãng đường di chuyển có dấu theo chiều quay của giá trị PWM đang phát.
 * 4. Kiểm tra điều kiện kẹt trục động cơ: Nếu giá trị độ lớn của `pwm_left` hoặc `pwm_right`
 *    lớn hơn @ref STALL_CCR_THRESHOLD nhưng không có xung encoder nào xuất hiện, bộ đếm thời gian kẹt sẽ tăng dần.
 *    Khi vượt quá @ref STALL_TIMEOUT_MS (500 ms), hàm sẽ kích hoạt cờ chốt kẹt động cơ.
 *
 * @param[in] pwm_left  Giá trị PWM CCR có dấu đang xuất ra động cơ trái (dải `-550` đến `+550`).
 * @param[in] pwm_right Giá trị PWM CCR có dấu đang xuất ra động cơ phải (dải `-550` đến `+550`).
 */
void AMR_Encoder_Update(int16_t pwm_left, int16_t pwm_right);

/**
 * @brief Lấy tổng số lượng xung ngắt ngoài tuyệt đối đếm được kể từ khi khởi động hệ thống.
 * @param[in] side Kênh bánh xe cần đọc (@ref ENC_LEFT hoặc @ref ENC_RIGHT).
 * @return Tổng số xung tuyệt đối (không dấu, tăng dần liên tục).
 * @par Đơn vị:
 * Xung (Pulses).
 * @note Dùng để kiểm tra tỷ số truyền và độ chính xác của đĩa mã (20 xung/vòng).
 */
uint32_t AMR_Encoder_GetTotal(EncoderSide_t side);

/**
 * @brief Lấy giá trị quãng đường tích lũy có dấu biểu diễn theo số lượng xung encoder.
 * @param[in] side Kênh bánh xe cần đọc (@ref ENC_LEFT hoặc @ref ENC_RIGHT).
 * @return Số xung có dấu (chạy tiến tăng xung dương, chạy lùi giảm xung âm).
 */
int32_t AMR_Encoder_GetOdometry(EncoderSide_t side);

/**
 * @brief Lấy vận tốc góc tức thời có dấu của bánh xe trong cửa sổ đo lường gần nhất.
 * @param[in] side Kênh bánh xe cần đọc (@ref ENC_LEFT hoặc @ref ENC_RIGHT).
 * @return Vận tốc đo được tính bằng số xung trên giây (pps).
 * @par Đơn vị:
 * Xung / giây (Pulses Per Second - pps).
 */
int16_t AMR_Encoder_GetSpeedPps(EncoderSide_t side);

/**
 * @brief Lấy quãng đường di chuyển tịnh tiến tích lũy có dấu của bánh xe theo đơn vị vật lý.
 * @param[in] side Kênh bánh xe cần đọc (@ref ENC_LEFT hoặc @ref ENC_RIGHT).
 * @return Quãng đường di chuyển tích lũy tính bằng milimét (mm).
 * @par Công thức quy đổi:
 * @f[S = \frac{\text{Odometry} \times \pi \times \text{WHEEL\_DIAMETER\_MM}}{\text{ENCODER\_PPR}}@f]
 * @par Đơn vị:
 * Millimeters (mm).
 */
int32_t AMR_Encoder_GetDistanceMm(EncoderSide_t side);

/**
 * @brief Kiểm tra trạng thái chốt lỗi kẹt bánh xe cơ khí (Stall Status).
 * @param[in] side Kênh bánh xe cần kiểm tra (@ref ENC_LEFT hoặc @ref ENC_RIGHT).
 * @return Trạng thái kẹt bánh:
 *         - `true`: Động cơ đang bị kẹt cơ khí, đã chốt khóa an toàn.
 *         - `false`: Động cơ quay bình thường hoặc đang dừng lệnh.
 */
bool AMR_Encoder_IsStalled(EncoderSide_t side);

/**
 * @brief Xóa cờ chốt khóa sự cố kẹt cơ khí bánh xe.
 * @details
 * Giải phóng trạng thái khóa an toàn kẹt trục, đưa biến đếm thời gian kẹt về 0.
 * Chỉ nên gọi hàm này sau khi lệnh công suất động cơ đã được đưa về 0 và người vận hành đã giải phóng kẹt cơ khí.
 */
void AMR_Encoder_ClearStall(void);

#endif /* INC_AMR_ENCODER_H_ */
