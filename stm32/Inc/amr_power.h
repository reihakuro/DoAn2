/**
 ******************************************************************************
 * @file    amr_power.h
 * @brief   Module quản lý chốt nguồn thông minh (Smart Power Latch) và tắt máy an toàn với Raspberry Pi.
 * @details
 * Cung cấp giải pháp bật/tắt nguồn mềm và cơ chế phối hợp ngắt nguồn an toàn phần cứng:
 * - **Sơ đồ chân phần cứng**:
 *   - Chân PB0 (`POWER_HOLD`): Ngõ ra điều khiển cực cổng P-MOSFET (Q2). Mức HIGH giữ nguồn mở toàn mạch;
 *     khi kéo xuống mức LOW, transistor ngắt hoàn toàn nguồn cấp từ pin, dòng rò nghỉ xấp xỉ @f$\approx 0\,\mu\text{A}@f$.
 *   - Chân PB1 (`PWR_BTN`): Ngõ vào giám sát nút nhấn vật lý SW1 (kéo lên nội dung Pull-up, tích cực mức LOW qua diode D5).
 *   - Chân PB10 (`PI_HALT`): Ngõ vào giám sát tín hiệu GPIO26 từ Raspberry Pi (tích cực mức HIGH khi Linux halt xong).
 *
 * ### Quy trình tắt máy an toàn (Graceful Shutdown Workflow):
 * 1. **Khởi phát**: Nhấn giữ nút nguồn >= 1.5 giây (@ref PWR_BTN_LONG_MS), hoặc pin cạn xả sâu (@ref BATTERY_CUTOFF_MV),
 *    hoặc nhận lệnh shutdown từ UART. Trạng thái chuyển sang `PWR_STATE_SHUTDOWN_REQUESTED`.
 * 2. **Hạ tải & Chờ hệ điều hành**: Dừng toàn bộ động cơ, phát tín hiệu cho Raspberry Pi đóng các dịch vụ Linux.
 *    MCU chờ chân PB10 lên mức HIGH ổn định liên tục 200 ms (thời gian chờ tối đa 45 giây qua Watchdog @ref PI_HALT_TIMEOUT_MS).
 * 3. **Đồng bộ thẻ nhớ**: Sau khi PB10 lên HIGH, chuyển sang `PWR_STATE_HALT_SETTLE`, chờ thêm 800 ms (@ref PI_HALT_SETTLE_MS)
 *    để bộ điều khiển flash trên thẻ MicroSD xả sạch bộ đệm ghi (cache flush), chống hỏng hệ điều hành.
 * 4. **Ngắt nguồn vật lý**: Chuyển sang `PWR_STATE_POWER_OFF`, kéo chân PB0 về mức LOW để ngắt rơ-le / MOSFET chính.
 *
 * @version 2.1
 * @date    2026
 ******************************************************************************
 */

#ifndef INC_AMR_POWER_H_
#define INC_AMR_POWER_H_

#include "amr_config.h"

/**
 * @enum PowerState_t
 * @brief Định nghĩa các giai đoạn trong máy trạng thái quản lý nguồn thông minh.
 */
typedef enum
{
    /** @brief Hệ thống đang giữ chân PB0 ở mức HIGH và vận hành bình thường. */
    PWR_STATE_RUNNING = 0,

    /** @brief Đã phát lệnh yêu cầu tắt máy, đang chờ Raspberry Pi hoàn tất dừng hệ điều hành. */
    PWR_STATE_SHUTDOWN_REQUESTED,

    /** @brief Đã nhận diện tín hiệu GPIO26 Halt từ Pi, đang chờ thêm thời gian trễ đồng bộ thẻ nhớ SD. */
    PWR_STATE_HALT_SETTLE,

    /** @brief Đã nhả chân giữ nguồn PB0 xuống LOW, ngắt hoàn toàn nguồn điện hệ thống. */
    PWR_STATE_POWER_OFF
} PowerState_t;

/**
 * @enum PowerReason_t
 * @brief Định danh nguyên nhân kích hoạt quy trình tắt nguồn robot.
 */
typedef enum
{
    /** @brief Chưa có sự kiện tắt nguồn nào được kích hoạt. */
    PWR_REASON_NONE = 0,

    /** @brief Người dùng chủ động nhấn và giữ nút nguồn vật lý SW1 >= 1.5 giây. */
    PWR_REASON_BUTTON,

    /** @brief Khối pin Li-ion sụt dưới ngưỡng ngắt an toàn 12.0 V duy trì liên tục quá 3 giây. */
    PWR_REASON_BATTERY,

    /** @brief Raspberry Pi chủ động phát tín hiệu GPIO26 Halt trước (người dùng tắt máy từ giao diện web/SSH). */
    PWR_REASON_PI_HALTED,

    /** @brief Nhận gói tin lệnh yêu cầu tắt máy từ xa qua kênh truyền thông UART. */
    PWR_REASON_REMOTE
} PowerReason_t;

/**
 * @enum ButtonEvent_t
 * @brief Định nghĩa các sự kiện thao tác nút nhấn nguồn vật lý sau khi lọc chống rung.
 */
typedef enum
{
    /** @brief Không có sự kiện nhấn nhả nút nào xảy ra. */
    BTN_EVENT_NONE = 0,

    /** @brief Thao tác nhấn nhả nhanh nút nguồn (50 ms <= t_press < 800 ms). */
    BTN_EVENT_SHORT,

    /** @brief Thao tác nhấn giữ nút nguồn liên tục (t_press >= 1.5 s). */
    BTN_EVENT_LONG
} ButtonEvent_t;

/**
 * @brief Khởi tạo phần cứng mạch giữ nguồn và cấu hình các chân GPIO quản lý nguồn.
 * @details
 * Cấu hình chân PB0 (Output Push-pull) giữ mức HIGH để dẫn MOSFET nguồn,
 * cấu hình chân PB1 (Input Pull-up) kết nối nút nguồn, và PB10 (Input Pull-down) giám sát Pi Halt.
 *
 * @note Hàm này chỉ cần gọi một lần duy nhất tại @ref AMR_App_Init.
 */
void AMR_Power_Init(void);

/**
 * @brief Hàm cập nhật máy trạng thái quản lý nguồn và giải mã nút nhấn định kỳ.
 * @details
 * Được gọi định kỳ mỗi 10 ms từ vòng lặp chính của Supervisor. Thực hiện:
 * 1. Chống rung tiếp điểm cơ khí cho nút nhấn PB1 (ngưỡng 50 ms).
 * 2. Đo thời lượng nhấn để giải mã sự kiện nhấn ngắn (@ref BTN_EVENT_SHORT) hoặc nhấn dài (@ref BTN_EVENT_LONG).
 * 3. Chống rung tín hiệu chân PB10 giám sát trạng thái Raspberry Pi Halt (ngưỡng 200 ms).
 * 4. Tiến hành chuyển đổi tuần tự giữa các trạng thái tắt nguồn an toàn.
 *
 * @return Sự kiện nút nhấn phát hiện được tại chu kỳ hiện tại (@ref ButtonEvent_t).
 */
ButtonEvent_t AMR_Power_Update(void);

/**
 * @brief Kích hoạt chu trình tắt nguồn xe có kiểm soát.
 * @details
 * Ghi nhận lý do tắt nguồn và bắt đầu quy trình phối hợp shutdown an toàn.
 * Lệnh này sẽ tự động bị bỏ qua nếu hệ thống vốn đã đang nằm trong tiến trình tắt nguồn trước đó.
 *
 * @param[in] reason Nguyên nhân yêu cầu tắt máy theo kiểu liệt kê @ref PowerReason_t.
 */
void AMR_Power_RequestShutdown(PowerReason_t reason);

/**
 * @brief Lấy trạng thái hiện tại của máy trạng thái quản lý nguồn.
 * @return Trạng thái nguồn hiện tại theo kiểu liệt kê @ref PowerState_t.
 */
PowerState_t AMR_Power_GetState(void);

/**
 * @brief Lấy nguyên nhân dẫn đến quyết định tắt nguồn hệ thống.
 * @return Lý do tắt máy theo kiểu liệt kê @ref PowerReason_t.
 */
PowerReason_t AMR_Power_GetReason(void);

/**
 * @brief Kiểm tra xem hệ thống có đang trong tiến trình đếm ngược tắt máy hay không.
 * @return Kết quả kiểm tra:
 *         - `true`: Đang trong tiến trình tắt máy (động cơ bị khóa, chuẩn bị cắt nguồn).
 *         - `false`: Hệ thống đang vận hành bình thường.
 */
bool AMR_Power_IsShuttingDown(void);

/**
 * @brief Kiểm tra mức logic hợp lệ đã khử rung của chân giám sát Raspberry Pi Halt (PB10).
 * @return Kết quả kiểm tra:
 *         - `true`: Raspberry Pi đã hoàn tất shutdown an toàn (chân GPIO26 đạt mức HIGH ổn định).
 *         - `false`: Raspberry Pi đang hoạt động hoặc chưa khởi động xong.
 */
bool AMR_Power_IsPiHalted(void);

#endif /* INC_AMR_POWER_H_ */
