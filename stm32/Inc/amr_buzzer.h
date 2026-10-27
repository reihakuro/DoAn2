/**
 ******************************************************************************
 * @file    amr_buzzer.h
 * @brief   Module điều khiển còi báo hiệu âm thanh (Active Buzzer 5V).
 * @details
 * Cung cấp chức năng phát các mẫu âm thanh (auditory patterns) phản hồi trạng thái,
 * cảnh báo lỗi và báo động khẩn cấp mà không làm nghẽn CPU (Non-blocking):
 * - Phần cứng: Chân PB5 điều khiển transistor NPN (2N2222) đóng cắt nguồn cấp 5V cho còi Active Buzzer.
 * - Cơ chế định thời: Sử dụng máy trạng thái thời gian thực được cập nhật định kỳ mỗi 10 ms từ vòng lặp chính.
 * - Ưu tiên âm học: Chế độ báo động liên tục (@ref BUZZ_ALARM) có mức ưu tiên cao nhất,
 *   tự động ghi đè lên các mẫu âm thanh thông thường.
 *
 * @version 2.1
 * @date    2026
 ******************************************************************************
 */

#ifndef INC_AMR_BUZZER_H_
#define INC_AMR_BUZZER_H_

#include "amr_config.h"

/**
 * @enum BuzzerPattern_t
 * @brief Định nghĩa các mẫu âm thanh cảnh báo và phản hồi của còi Buzzer.
 */
typedef enum
{
    /**
     * @brief Tắt còi hoàn toàn (mức logic LOW trên chân PB5).
     */
    BUZZ_OFF = 0,

    /**
     * @brief Một tiếng bíp ngắn (thời lượng 100 ms).
     * @details Dùng để phát âm thanh phản hồi khi khởi động xong hệ thống hoặc khi nhận lệnh điều khiển hợp lệ.
     */
    BUZZ_SHORT,

    /**
     * @brief Hai tiếng bíp kép ngắn (mỗi tiếng 150 ms, ngắt quãng 100 ms).
     * @details Dùng để cảnh báo mức pin yếu (@ref AMR_FLAG_BAT_LOW), phát lặp lại định kỳ mỗi 5 giây.
     */
    BUZZ_DOUBLE,

    /**
     * @brief Một tiếng bíp dài duy trì liên tục 1000 ms (1 giây).
     * @details Dùng khi robot hoàn tất cập trạm sạc hoặc phát tín hiệu bắt đầu quy trình tắt máy an toàn.
     */
    BUZZ_LONG,

    /**
     * @brief Báo động bíp dồn dập chu kỳ liên tục (bật 100 ms, tắt 100 ms tuần hoàn).
     * @details Kích hoạt khi có các sự cố an toàn khẩn cấp: phát hiện vật cản phía trước, kẹt bánh xe hoặc mất kết nối.
     */
    BUZZ_ALARM
} BuzzerPattern_t;

/**
 * @brief Khởi tạo phần cứng chân điều khiển còi Buzzer.
 * @details Cấu hình chân PB5 ở chế độ ngõ ra đẩy-kéo (Push-Pull, tốc độ thấp) và kéo về mức logic LOW.
 *
 * @note Hàm này chỉ cần gọi một lần duy nhất tại @ref AMR_App_Init.
 */
void AMR_Buzzer_Init(void);

/**
 * @brief Kích hoạt phát một mẫu âm thanh đơn lẻ một lần (Single-shot Pattern).
 * @details
 * Hỗ trợ các mẫu @ref BUZZ_SHORT, @ref BUZZ_DOUBLE, @ref BUZZ_LONG.
 * Nếu có một mẫu âm thanh khác đang phát dở, mẫu âm mới sẽ lập tức ghi đè và thực thi lại từ đầu.
 *
 * @param[in] pattern Mẫu âm thanh cần phát theo kiểu liệt kê @ref BuzzerPattern_t.
 *
 * @note Không có tác dụng nếu còi đang bị khóa trong chế độ báo động khẩn cấp @ref BUZZ_ALARM.
 */
void AMR_Buzzer_Play(BuzzerPattern_t pattern);

/**
 * @brief Bật hoặc tắt chế độ còi báo động khẩn cấp liên tục (Alarm Mode).
 * @details
 * Chế độ này có mức ưu tiên tuyệt đối cao nhất. Khi được bật, còi sẽ liên tục phát âm thanh ngắt quãng
 * chu kỳ ngắn để thu hút sự chú ý của người vận hành nhà xưởng cho tới khi cờ lỗi được xóa hoàn toàn.
 *
 * @param[in] on Trạng thái kích hoạt:
 *               - `true`: Bật chế độ báo động khẩn cấp.
 *               - `false`: Hủy bỏ chế độ báo động, đưa còi về trạng thái nghỉ.
 */
void AMR_Buzzer_SetAlarm(bool on);

/**
 * @brief Hàm cập nhật máy trạng thái bộ điều khiển còi Buzzer theo thời gian thực.
 * @details
 * Được gọi định kỳ mỗi 10 ms từ vòng lặp chính của Supervisor. Hàm kiểm tra biến đếm thời gian
 * và thực hiện đóng/ngắt mức logic chân PB5 tương ứng với từng giai đoạn trong mẫu âm thanh.
 *
 * @note Hàm được thiết kế hoàn toàn không khóa (Non-blocking), không sử dụng delay phần cứng.
 */
void AMR_Buzzer_Update(void);

#endif /* INC_AMR_BUZZER_H_ */
