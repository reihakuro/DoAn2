/**
 ******************************************************************************
 * @file    amr_sonar.h
 * @brief   Module quản lý cụm 3 cảm biến khoảng cách siêu âm HC-SR04 ghép kênh TDM.
 * @details
 * Cung cấp giải thuật quét khoảng cách tránh va chạm thời gian thực không gây nghẽn CPU:
 * - **Sơ đồ phân bổ phần cứng**:
 *   - Cảm biến Trái (@ref SONAR_LEFT): Chân Trig PB12, chân Echo PB13.
 *   - Cảm biến Giữa (@ref SONAR_MID): Chân Trig PB14, chân Echo PB15.
 *   - Cảm biến Phải (@ref SONAR_RIGHT): Chân Trig PB8, chân Echo PB9.
 *
 * ### Cơ chế ghép kênh phân chia thời gian (Time-Division Multiplexing - TDM):
 * Để triệt tiêu hiện tượng dội sóng chéo (Acoustic Crosstalk) giữa các đầu dò siêu âm đặt cạnh nhau,
 * hệ thống chỉ kích hoạt duy nhất một cảm biến tại một thời điểm:
 * - Mỗi cảm biến được phân bổ một khe thời gian (@ref SONAR_SLOT_MS = 25 ms) để kích xung và chờ sóng phản xạ.
 * - Tổng chu kỳ quét hoàn tất 3 cảm biến là 3 x 25 ms = 75 ms (tần số quét ~13.33 Hz).
 *
 * ### Bộ lọc số và chuẩn đoán cảm biến:
 * - **Lọc trung vị 3 mẫu (Median Filter-of-3)**: Loại bỏ các mẫu đo bất thường (outliers) do sóng âm bị phản xạ phân tán.
 * - **Chuẩn đoán lỗi phần cứng**: Tự động nhận diện mất kết nối hoặc hỏng cảm biến khi không có xung phản xạ
 *   quá @ref SONAR_FAIL_LIMIT (5 lần liên tiếp).
 *
 * @version 2.1
 * @date    2026
 ******************************************************************************
 */

#ifndef INC_AMR_SONAR_H_
#define INC_AMR_SONAR_H_

#include "amr_config.h"

/**
 * @enum SonarId_t
 * @brief Định danh vị trí hình học của các module cảm biến siêu âm trên thân xe.
 */
typedef enum
{
    /** @brief Cảm biến siêu âm góc quét bên trái (PB12/PB13). */
    SONAR_LEFT  = 0,

    /** @brief Cảm biến siêu âm hướng tâm chính giữa (PB14/PB15). */
    SONAR_MID   = 1,

    /** @brief Cảm biến siêu âm góc quét bên phải (PB8/PB9). */
    SONAR_RIGHT = 2
} SonarId_t;

/**
 * @def SONAR_DIST_FAR_MM
 * @brief Hằng số biểu diễn khoảng cách vượt ngoài dải đo (vùng không gian thông thoáng).
 * @details Giá trị quy ước `0xFFFF` (65535 mm) được trả về khi không có vật cản trong tầm đo (> 4.0 m)
 *          hoặc khi cảm biến bị lỗi mất phản hồi xung Echo.
 */
#define SONAR_DIST_FAR_MM   0xFFFFU

/**
 * @brief Khởi tạo phần cứng và bộ đệm dữ liệu của cụm cảm biến siêu âm.
 * @details Cấu hình các chân Trig ở mức LOW, khởi tạo giá trị đo ban đầu bằng @ref SONAR_DIST_FAR_MM.
 *
 * @note Hàm này chỉ cần gọi một lần duy nhất tại @ref AMR_App_Init.
 */
void AMR_Sonar_Init(void);

/**
 * @brief Máy trạng thái đo đạc và xử lý tín hiệu cảm biến siêu âm theo thời gian thực.
 * @details
 * Cần được gọi liên tục trong mỗi chu kỳ của vòng lặp chính (`main super-loop`).
 * Hàm thực hiện lấy mẫu cạnh lên/cạnh xuống của chân Echo thông qua bộ đếm microsecond TIM4:
 * - Kích xung chân Trig @ref SONAR_TRIG_PULSE_US (12 µs).
 * - Chờ cạnh lên của tín hiệu Echo (quá thời gian @ref SONAR_RISE_TIMEOUT_US xem như mất cảm biến).
 * - Đo độ rộng xung Echo và quy đổi sang khoảng cách vật lý theo vận tốc âm thanh 343 m/s.
 * - Đẩy kết quả vào bộ lọc trung vị 3 mẫu.
 *
 * @warning Vòng lặp chính tuyệt đối KHÔNG được chứa các hàm delay chặn dài (như `HAL_Delay`)
 *          để tránh làm sai lệch độ phân giải thời gian đo độ rộng xung Echo.
 */
void AMR_Sonar_Process(void);

/**
 * @brief Lấy khoảng cách đo được hiện tại của cảm biến sau khi đã qua bộ lọc trung vị 3 mẫu.
 * @param[in] id Định danh vị trí cảm biến (@ref SONAR_LEFT, @ref SONAR_MID, hoặc @ref SONAR_RIGHT).
 * @return Khoảng cách đo được tính bằng milimét (mm), hoặc @ref SONAR_DIST_FAR_MM nếu ngoài tầm đo.
 * @par Đơn vị:
 * Millimeters (mm).
 */
uint16_t AMR_Sonar_GetDistanceMm(SonarId_t id);

/**
 * @brief Lấy giá trị khoảng cách đo được thô (raw) ở chu kỳ lấy mẫu gần nhất chưa qua lọc.
 * @param[in] id Định danh vị trí cảm biến (@ref SONAR_LEFT, @ref SONAR_MID, hoặc @ref SONAR_RIGHT).
 * @return Khoảng cách thô gần nhất (dùng cho chẩn đoán kiểm thử phần cứng).
 * @par Đơn vị:
 * Millimeters (mm).
 */
uint16_t AMR_Sonar_GetRawMm(SonarId_t id);

/**
 * @brief Kiểm tra xem cảm biến siêu âm có đang gặp sự cố phần cứng hay không.
 * @param[in] id Định danh vị trí cảm biến (@ref SONAR_LEFT, @ref SONAR_MID, hoặc @ref SONAR_RIGHT).
 * @return Kết quả chuẩn đoán:
 *         - `true`: Cảm biến bị mất tín hiệu Echo liên tục quá @ref SONAR_FAIL_LIMIT lần (lỏng dây, hỏng cảm biến).
 *         - `false`: Cảm biến hoạt động bình thường và phản hồi xung đều đặn.
 */
bool AMR_Sonar_IsFaulty(SonarId_t id);

/**
 * @brief Lấy tổng số chu kỳ quét hoàn tất toàn bộ 3 cảm biến kể từ khi khởi động.
 * @return Số chu kỳ quét hoàn thành (mỗi chu kỳ tương ứng 75 ms).
 */
uint32_t AMR_Sonar_GetCycleCount(void);

#endif /* INC_AMR_SONAR_H_ */
