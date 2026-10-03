# HƯỚNG DẪN KỸ THUẬT: XE ROBOT THÔNG MINH PHÂN TẦNG (PI 4 + STM32 BLUE PILL TYPE-C)
## KIẾN TRÚC MASTER - SLAVE — BOM LINH KIỆN — SƠ ĐỒ ĐẤU DÂY & GIAO THỨC TRUYỀN THÔNG

---

## 1. ĐÁNH GIÁ KỸ THUẬT: ĐỔI SANG STM32F103C8T6 CÓ LỢI GÌ?

Việc bạn chuyển từ ESP32 sang **STM32F103C8T6 (Blue Pill cổng Type-C)** là một lựa chọn **cực kỳ được lòng các giảng viên kỹ thuật**:
1. **Chuẩn chất vi điều khiển công nghiệp:** STM32 dùng lõi ARM Cortex-M3 (72 MHz), có hệ thống Timer phần cứng cực mạnh (TIM1, TIM2, TIM3, TIM4) chuyên dụng để tạo xung PWM điều tốc động cơ và đo thời gian xung siêu âm chính xác đến từng micro-giây.
2. **Chân chịu được điện áp 5V (5V-Tolerant):** Rất nhiều chân GPIO của STM32 chịu được mức 5V (các chân ký hiệu FT - Five-volt Tolerant). Nhờ đó, bạn có thể cắm trực tiếp chân `ECHO` (5V) của cảm biến siêu âm HC-SR04 vào STM32 mà **không sợ cháy vi điều khiển** và **không cần dùng cầu phân áp**.
3. **Cổng Type-C tiện lợi:** Bản Blue Pill Type-C mới giải quyết hoàn toàn nhược điểm cổng Micro-USB lỏng lẻo dễ gãy của bản cũ, cắm nối với Raspberry Pi 4 rất chắc chắn.

---

## 2. BẢNG LINH KIỆN CHI TIẾT (BOM — Bill of Materials)

| STT | Tên linh kiện | Model / Thông số kỹ thuật | SL | Chức năng chi tiết trong hệ thống | Giá mua tại VN |
|:---:|---|---|:---:|---|:---:|
| 1 | **Bo xử lý cấp cao (Master)** | Raspberry Pi 4 Model B (8GB RAM) | 1 | Chạy Web Server, truyền video camera FPV qua Wi-Fi | *Đã có sẵn* |
| 2 | **Vi điều khiển thời gian thực (Slave)** | **STM32F103C8T6 Blue Pill (Cổng Type-C)** | 1 | Lõi ARM Cortex-M3 (72MHz), đọc 3 cảm biến siêu âm, lái động cơ | ~45k – 55k |
| 3 | **Mạch nạp STM32** | **ST-Link V2 mini** | 1 | Dùng nạp code cho STM32 qua 4 chân SWD (SWDIO, SWCLK, 3.3V, GND) | ~35k – 45k |
| 4 | **Cụm cảm biến khoảng cách** | **HC-SR04** (hoặc HC-SR04P) | 3 | Bố trí 3 hướng (Trái, Giữa, Phải), quét góc rộng $>120^\circ$ | ~18k x 3 = 54k |
| 5 | **Khung xe robot** | Khung mica 2 tầng chữ nhật + 4 bánh TT | 1 bộ | Khung gầm 2 tầng chứa pin, bo mạch và động cơ | ~100k – 120k |
| 6 | **Động cơ di chuyển** | Động cơ giảm tốc vàng TT Motor (3V–6V) | 4 | Dẫn động 4 bánh xe (nối song song 2 bên thành 2 kênh) | Đi kèm khung |
| 7 | **Mạch lái động cơ** | **TB6612FNG** (hoặc L298N có tản nhiệt) | 1 | Lái 2 kênh động cơ bằng MOSFET (mát, hiệu suất cao) | ~35k – 45k |
| 8 | **Camera giám sát** | **Pi Camera V2 (CSI)** hoặc Webcam USB | 1 | Gắn đầu xe truyền video FPV thời gian thực về Web | ~180k – 250k |
| 9 | **Màn hình hiển thị tại chỗ** | **OLED 0.91 inch I2C (SSD1306)** | 1 | Nối vào Pi 4 hiển thị địa chỉ IP Wi-Fi và chế độ | ~25k – 35k |
| 10 | **Tản nhiệt + Quạt** | Bộ tản nhiệt nhôm + Quạt mini 5V cho Pi 4 | 1 | Làm mát CPU Pi 4 chạy ổn định không lo nghẽn nhiệt | ~30k – 40k |
| 11 | **Cáp kết nối Pi 4 – STM32** | Cáp Type-C to Type-C hoặc USB-A to Type-C (ngắn) | 1 | Cắm từ cổng USB của Pi 4 sang Type-C của STM32 | ~20k |
| 12 | **Nguồn nuôi Pi 4 + STM32** | Pin sạc dự phòng 5V – 3A (Cổng Type-C) | 1 | Cấp nguồn cho Pi 4 (Pi cấp tiếp 5V qua cáp nuôi STM32) | ~150k – 200k |
| 13 | **Nguồn nuôi Động cơ** | Khay 2 pin 18650 có công tắc + 2 cell pin 18650 | 1 bộ | Cấp 7.4V – 8.4V riêng cho mạch cầu H và động cơ (xả $\ge 10\text{A}$) | ~80k – 120k |
| 14 | **Dây nối & Trụ đồng** | Dây Dupont đực–cái, cái–cái + ốc M3 | 1 bó | Đấu nối mạch và bắt cố định linh kiện | ~25k |

> 💰 **Tổng chi phí mua thêm dự kiến:** Khoảng **750.000đ – 950.000đ** (rất tiết kiệm, combo Pi 4 + STM32 cực kỳ uy tín trước hội đồng).

---

## 3. SƠ ĐỒ ĐẤU NỐI CHÂN CHI TIẾT (PINOUT STM32F103C8T6)

### 3.1. Phân bổ chân trên STM32 Blue Pill

```
                 STM32F103C8T6 BLUE PILL (TYPE-C)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Chân STM32      │ Nối đến linh kiện        │ Chức năng & Ghi chú
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
5V & G (GND)    │ Nhận nguồn từ cáp Type-C │ Nhận 5V từ cổng USB của Pi 4
PA0 (TIM2_CH1)  │ TB6612FNG — PWMA         │ Xuất xung Hardware PWM motor Trái
PA1 (TIM2_CH2)  │ TB6612FNG — PWMB         │ Xuất xung Hardware PWM motor Phải
PA2             │ TB6612FNG — AIN1         │ Chiều quay motor Trái
PA3             │ TB6612FNG — AIN2         │ Chiều quay motor Trái
PA4             │ TB6612FNG — BIN1         │ Chiều quay motor Phải
PA5             │ TB6612FNG — BIN2         │ Chiều quay motor Phải
PA6             │ TB6612FNG — STBY         │ Mở cầu H (kéo mức HIGH)
PB12            │ Siêu âm TRÁI — TRIG      │ Kích phát xung siêu âm Trái
PB13 (5V-Tol)   │ Siêu âm TRÁI — ECHO      │ Đọc xung phản hồi Trái (chịu được 5V)
PB14            │ Siêu âm GIỮA — TRIG      │ Kích phát xung siêu âm Giữa
PB15 (5V-Tol)   │ Siêu âm GIỮA — ECHO      │ Đọc xung phản hồi Giữa (chịu được 5V)
PB8             │ Siêu âm PHẢI — TRIG      │ Kích phát xung siêu âm Phải
PB9 (5V-Tol)    │ Siêu âm PHẢI — ECHO      │ Đọc xung phản hồi Phải (chịu được 5V)
PA9 (USART1_TX) │ Nối RX của Pi / USB CDC  │ Truyền dữ liệu khoảng cách lên Pi
PA10 (USART1_RX)│ Nối TX của Pi / USB CDC  │ Nhận lệnh điều khiển từ Pi
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
```

> [!TIP]
> **ĐIỂM CỘNG CỰC LỚN CỦA STM32:**
> Các chân `PB13`, `PB15`, `PB9` đều thuộc nhóm chân **5V-Tolerant (FT)** trên STM32F103. Tín hiệu xung phản hồi `ECHO` (5V) từ cảm biến siêu âm HC-SR04 cắm thẳng trực tiếp vào các chân này mà **không cần hàn trở chia áp**, mạch cực kỳ gọn gàng và không lo nhiễu!

---

### 3.2. Phân bổ chân trên Raspberry Pi 4 (Master)

```
                            RASPBERRY PI 4
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Chân Pi 4       │ Nối đến linh kiện        │ Chức năng
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Cổng CSI        │ Cáp dẹp Pi Camera V2     │ Luồng video 30 FPS góc nhìn FPV
Cổng USB 3.0    │ Cáp Type-C sang STM32    │ Cấp nguồn STM32 + Giao tiếp Serial (`/dev/ttyACM0` hoặc `/dev/ttyUSB0`)
Pin 4 (5V)      │ Chân (+) Quạt mini 5V    │ Làm mát CPU Pi 4
Pin 6 (GND)     │ Chân (-) Quạt mini 5V    │ Mass quạt
Pin 1 (3.3V)    │ OLED VCC                 │ Nguồn màn hình
Pin 3 (GPIO 2)  │ OLED SDA                 │ I2C Data
Pin 5 (GPIO 3)  │ OLED SCL                 │ I2C Clock
Pin 9 (GND)     │ OLED GND                 │ Mass màn hình
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
```

---

## 4. SƠ ĐỒ ĐẤU NỐI NGUỒN VÀ MASS CHUNG

```
   [ Pin Dự Phòng 5V - 3A ] ══════(Cáp Type-C)══════> [ Cổng Nguồn Pi 4 ]
                                                            │
                                                     (Cổng USB của Pi)
                                                            │
                                                   (Cáp USB sang Type-C)
                                                            │
                                                            ▼
                                                [ Cổng Type-C STM32 ]
                                                            │
   [ Khay 2 Pin 18650 (7.4V) ]                              │ Chân Logic 3.3V / Chân G
        ├─── Dương (+) ─────────> TB6612FNG: VM (Nguồn Motor)│
        └─── Âm (-) ────┬───────> TB6612FNG: GND (Động cơ)   │
                        │                                   │
                        └───────> NỐI CHUNG GND <───────────┘ (Chân G của STM32)
                                  (BẮT BUỘC NỐI CHUNG MASS)
                                        │
                                        ▼
                                 [ Mạch TB6612FNG ]
                                 ├── A01, A02 ───> [ 2 Động cơ Trái (Song song) ]
                                 └── B01, B02 ───> [ 2 Động cơ Phải (Song song) ]
```

---

## 5. GIAO THỨC TRUYỀN THÔNG SERIAL (PI 4 $\leftrightarrow$ STM32)

Hai bên giao tiếp qua cổng nối tiếp UART (115200 baud, 8-N-1):

### A. Pi 4 gửi xuống STM32:
* `MODE:AUTO\n` : Bật chế độ xe tự động né vật cản 3 hướng.
* `MODE:MANUAL\n` : Chuyển sang chế độ lái tay từ giao diện Web.
* Khi ở chế độ lái tay:
  * `CMD:FWD\n` : Tiến thẳng
  * `CMD:REV\n` : Lùi xe
  * `CMD:LEFT\n` : Rẽ trái
  * `CMD:RIGHT\n` : Rẽ phải
  * `CMD:STOP\n` : Dừng xe

### B. STM32 gửi ngược lên Pi 4:
* Chu kỳ gửi định kỳ mỗi 50ms:  
  `DIST:<Trái>,<Giữa>,<Phải>\n`  
  *Ví dụ:* `DIST:45,110,85\n` (Khoảng cách Trái 45cm, Giữa 110cm, Phải 85cm).
* Pi 4 đọc chuỗi này hiển thị trực tiếp lên giao diện Web để người dùng nhìn thấy thanh radar đo khoảng cách 3 hướng theo thời gian thực.

---

## 6. THUẬT TOÁN ĐIỀU KHIỂN NÉ VẬT CẢN TRÊN STM32

STM32 chạy vòng lặp đọc liên tục 3 cảm biến siêu âm:

```cpp
// Logic né vật cản 3 hướng trên STM32 (Lập trình bằng Arduino IDE STM32duino hoặc Keil C)
int dL = readUltrasonic(PB12, PB13);
int dC = readUltrasonic(PB14, PB15);
int dR = readUltrasonic(PB8, PB9);

if (dC > 30 && dL > 20 && dR > 20) {
    // Phía trước và hai bên đều thoáng -> Tiến thẳng
    motorForward(180);
}
else if (dC <= 30 && dC > 15) {
    // Vật cản ở cự ly cảnh báo phía trước (15-30cm) -> So sánh hai bên để bẻ lái
    if (dL > dR) {
        motorTurnLeft(160);   // Bên trái thoáng hơn -> Rẽ trái
    } else {
        motorTurnRight(160);  // Bên phải thoáng hơn -> Rẽ phải
    }
}
else if (dC <= 15 || (dL <= 15 && dR <= 15)) {
    // Vật cản quá gần (<15cm) hoặc cả 3 hướng đều vướng -> Lùi khẩn cấp rồi quay đầu
    motorBackward(170);
    delay(400);
    if (dL > dR) motorTurnLeft(170); else motorTurnRight(170);
    delay(450);
}
```

---

## 7. MẸO LẬP TRÌNH CHO BẠN (CỰC KỲ DỄ THỰC HIỆN)

Bạn không nhất thiết phải viết STM32 bằng thanh ghi (Register) hay Keil C phức tạp nếu chưa quen:
* **Dùng Arduino IDE với gói "STM32 MCU Based Boards":** Cài đặt gói bo mạch STM32 của hãng STMicroelectronics trên Arduino IDE. Bạn có thể code bằng các hàm quen thuộc như `digitalWrite`, `analogWrite`, `Serial.print` hệt như làm trên Arduino/ESP32, nhưng code được biên dịch tối ưu chạy mượt mà trên lõi ARM Cortex-M3 của STM32!
* **Nạp code qua ST-Link V2:** Nối 4 dây từ ST-Link vào 4 chân cọc nhô ra ở đuôi bo Blue Pill (`3.3V`, `GND`, `SWDIO`, `SWCLK`), bấm nút Upload trên Arduino IDE là nạp xong trong 2 giây.
