/**
 ******************************************************************************
 * @file    amr_battery.h
 * @brief   Module giám sát và đo lường điện áp khối pin Li-ion 4S qua bộ biến đổi ADC1.
 * @details
 * Cung cấp chức năng đọc điện áp nguồn cấp động lực, áp dụng bộ lọc số IIR thông thấp
 * và phân loại các cấp trạng thái pin để phục vụ cảnh báo sớm và bảo vệ ngắt tải an toàn:
 * - Kênh phần cứng: Chân PA6 cấu hình kênh tương tự ADC1_IN6.
 * - Mạch phân áp ngoài: R1 = 100 kΩ, R2 = 22 kΩ.
 * - Thuật toán lọc: Bộ lọc đệ quy IIR bậc 1 dịch bit với alpha = 1/8, triệt tiêu nhiễu đóng ngắt PWM.
 * - Phân cấp trạng thái: 4 cấp độ tương ứng với dung lượng thực tế của khối pin 4S.
 *
 * @version 2.1
 * @date    2026
 ******************************************************************************
 */

#ifndef INC_AMR_BATTERY_H_
#define INC_AMR_BATTERY_H_

#include "amr_config.h"

/**
 * @enum BatteryState_t
 * @brief Định nghĩa các cấp độ trạng thái hoạt động của khối pin Li-ion 4S.
 */
typedef enum
{
    /**
     * @brief Cấp 0: Không phát hiện kết nối khối pin vật lý (@f$V_{\text{bat}} < 6.0\text{ V}@f$).
     * @details
     * Nhận diện hệ thống đang được cấp nguồn thử nghiệm trên bàn test thông qua mạch nạp ST-Link
     * hoặc cổng USB (5 V). Hệ thống tự động vô hiệu hóa các cơ chế bảo vệ cắt nguồn pin.
     */
    BAT_STATE_ABSENT = 0,

    /**
     * @brief Cấp 1: Điện áp pin ở mức an toàn, hoạt động bình thường (@f$V_{\text{bat}} > 13.6\text{ V}@f$).
     * @details
     * Tương ứng điện áp mỗi cell @f$> 3.4\text{ V}@f$, dung lượng pin dồi dào, cho phép xe vận hành đầy tải.
     */
    BAT_STATE_OK,

    /**
     * @brief Cấp 2: Cảnh báo mức pin yếu (@f$V_{\text{bat}} \le 13.6\text{ V}@f$).
     * @details
     * Tương ứng điện áp mỗi cell @f$\le 3.4\text{ V}@f$ (dung lượng còn lại khoảng 15% - 20%).
     * Kích hoạt còi báo chu kỳ và cờ cảnh báo telemetry để tầng điều khiển đưa xe về trạm sạc.
     * Cảnh báo chỉ tự động xóa khi pin phục hồi hoặc được sạc vượt quá ngưỡng @ref BATTERY_WARN_RELEASE_MV (13.9 V).
     */
    BAT_STATE_LOW,

    /**
     * @brief Cấp 3: Mức pin chạm ngưỡng xả cạn nguy hiểm (@f$V_{\text{bat}} \le 12.0\text{ V}@f$).
     * @details
     * Tương ứng điện áp mỗi cell @f$\le 3.0\text{ V}@f$ (dung lượng xấp xỉ 0%).
     * Nếu điện áp duy trì dưới ngưỡng này liên tục quá @ref BATTERY_CUTOFF_HOLD_MS (3 giây),
     * hệ thống sẽ lập tức khóa động cơ và yêu cầu ngắt relay cắt nguồn chính để bảo vệ cell pin.
     */
    BAT_STATE_CRITICAL
} BatteryState_t;

/**
 * @brief Khởi tạo phần cứng bộ biến đổi ADC1 và bộ lọc số giám sát pin.
 * @details
 * Tiến hành hiệu chuẩn bộ chuyển đổi tương tự-số (ADC Calibration), nạp cấu hình kênh chuyển đổi ADC1_IN6
 * và lấy mẫu ban đầu để lấp đầy giá trị khởi tạo cho bộ lọc IIR.
 *
 * @note Hàm này chỉ cần gọi một lần duy nhất tại @ref AMR_App_Init.
 */
void AMR_Battery_Init(void);

/**
 * @brief Hàm cập nhật thuật toán lấy mẫu và lọc số điện áp pin định kỳ.
 * @details
 * Được gọi tuần hoàn từ vòng lặp chính của Supervisor. Hàm tự động kiểm tra thời gian trôi qua,
 * cứ mỗi chu kỳ @ref BATTERY_SAMPLE_MS (50 ms) sẽ:
 * 1. Khởi động chuyển đổi ADC và đọc giá trị số thô (ADC raw).
 * 2. Quy đổi sang đơn vị millivolt (mV) thông qua tỷ số nguyên @ref BATTERY_MV_NUM / @ref BATTERY_MV_DEN.
 * 3. Áp dụng hệ số cân chỉnh VOM thực tế @ref BATTERY_CAL_X10000.
 * 4. Đưa qua bộ lọc số thông thấp IIR đệ quy với hệ số dịch bit @ref BATTERY_IIR_SHIFT.
 * 5. Cập nhật máy trạng thái @ref BatteryState_t và đếm thời gian cho cơ chế ngắt nguồn khẩn cấp.
 *
 * @note Hàm được thiết kế không khóa (Non-blocking).
 */
void AMR_Battery_Update(void);

/**
 * @brief Lấy giá trị điện áp pin hiện tại sau khi đã lọc nhiễu IIR và cân chỉnh thực tế.
 * @return Điện áp pin tức thời đã xử lý.
 * @par Đơn vị:
 * Millivolts (mV).
 */
uint16_t AMR_Battery_GetMillivolts(void);

/**
 * @brief Đọc giá trị số thô trực tiếp từ bộ chuyển đổi ADC1 kênh 6.
 * @return Giá trị ADC 12-bit chưa qua xử lý lọc số.
 * @par Dải giá trị:
 * 0 đến 4095.
 */
uint16_t AMR_Battery_GetRawAdc(void);

/**
 * @brief Đọc trạng thái đánh giá an toàn mức pin hiện tại.
 * @return Trạng thái pin theo kiểu liệt kê @ref BatteryState_t.
 */
BatteryState_t AMR_Battery_GetState(void);

#endif /* INC_AMR_BATTERY_H_ */
