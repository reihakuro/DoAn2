/**
 ******************************************************************************
 * @file    amr_debug.h
 * @brief   Module xuất nhật ký gỡ lỗi (Debug Logging) và Telemetry qua USART1.
 * @details
 * Cung cấp giải pháp in chuỗi ký tự ASCII và định dạng dữ liệu telemetry không gây khóa CPU (Non-blocking):
 * - Kênh phần cứng: Ngoại vi USART1 (TX: PA9, RX: PA10), Baudrate 115200 bps, kết nối module USB-UART CP2102.
 * - Cấu trúc hàng đợi: Hàng đệm vòng tròn (Ring Buffer) lưu trữ chuỗi văn bản tạm thời.
 * - Cơ chế bơm dữ liệu (@ref AMR_Debug_Pump): Kiểm tra cờ trống thanh ghi truyền `USART_SR_TXE`
 *   để đẩy từng byte dữ liệu ra phần cứng mỗi chu kỳ vòng lặp chính mà không cần bật ngắt ngắt truyền TXE ISR,
 *   giảm thiểu tối đa phụ tải xử lý ngắt cho CPU.
 * - Xử lý tràn bộ đệm: Khi hàng đợi đầy, dữ liệu mới sẽ tự động bị cắt tỉa (drop) thay vì chờ đợi,
 *   đảm bảo vòng lặp thời gian thực không bao giờ bị trễ.
 *
 * @note Module này chỉ hoạt động khi macro @ref AMR_DEBUG_UART_ENABLE được đặt bằng `1`.
 *
 * @version 2.1
 * @date    2026
 ******************************************************************************
 */

#ifndef INC_AMR_DEBUG_H_
#define INC_AMR_DEBUG_H_

#include "amr_config.h"

/**
 * @brief Khởi tạo phần cứng và bộ đệm hàng đợi truyền thông USART1.
 * @details Xóa rỗng con trỏ ghi/đọc của Ring Buffer và cấu hình thanh ghi truyền USART1.
 *
 * @note Hàm này chỉ cần gọi một lần duy nhất tại @ref AMR_App_Init.
 */
void AMR_Debug_Init(void);

/**
 * @brief Đưa một chuỗi ký tự (String) kết thúc bằng null vào hàng đợi truyền.
 * @details
 * Sao chép tuần tự từng ký tự của chuỗi `s` vào bộ đệm vòng tròn (Ring Buffer).
 * Nếu bộ đệm bị đầy trong quá trình ghi, các ký tự vượt quá giới hạn sẽ tự động bị bỏ qua
 * để đảm bảo hàm luôn thoát ra ngay lập tức và không bao giờ gây khóa hệ thống.
 *
 * @param[in] s Con trỏ hằng trỏ tới chuỗi ký tự ASCII cần truyền.
 */
void AMR_Debug_Write(const char *s);

/**
 * @brief Định dạng và xuất chuỗi văn bản theo cú pháp rút gọn tương tự printf tiêu chuẩn.
 * @details
 * Bộ phân tích chuỗi định dạng siêu nhẹ (Lightweight Integer Printf) dành riêng cho hệ thống nhúng:
 * - Hỗ trợ các định dạng số nguyên cơ bản: `%d`, `%i`, `%u`, `%x`, `%X`, `%c`, `%s`.
 * - Không hỗ trợ số thực dấu phẩy động (`%f`) nhằm tiết kiệm dung lượng Flash, giảm phân mảnh bộ nhớ Stack
 *   và loại trừ thư viện phần mềm mô phỏng số thực nặng nề trên ARM Cortex-M3.
 *
 * @param[in] fmt Chuỗi định dạng văn bản chứa các chỉ dẫn chuyển đổi.
 * @param[in] ... Danh sách tham số biến thiên (Variadic Arguments) tương ứng với chuỗi định dạng.
 */
void AMR_Debug_Printf(const char *fmt, ...);

/**
 * @brief Bơm dữ liệu từ hàng đợi vòng tròn ra thanh ghi truyền dữ liệu của phần cứng USART1.
 * @details
 * Được gọi liên tục trong mỗi chu kỳ của vòng lặp chính (`main super-loop`).
 * Hàm kiểm tra cờ trạng thái `USART_FLAG_TXE` (Transmit Data Register Empty). Nếu thanh ghi truyền rỗng
 * và trong Ring Buffer còn dữ liệu, hàm sẽ đẩy một lượng byte nhất định vào thanh ghi `USART1->DR`,
 * sau đó giải phóng quyền điều khiển ngay lập tức để CPU thực thi các tác vụ khác.
 *
 * @note Hàm được thiết kế tối ưu hóa thời gian thực, không sử dụng vòng lặp chờ cờ phần cứng.
 */
void AMR_Debug_Pump(void);

#endif /* INC_AMR_DEBUG_H_ */
