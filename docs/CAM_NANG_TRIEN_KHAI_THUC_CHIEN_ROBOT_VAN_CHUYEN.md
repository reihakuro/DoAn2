# CẨM NANG TRIỂN KHAI THỰC CHIẾN: ROBOT VẬN CHUYỂN LINH KIỆN NỘI BỘ
**Dành cho:** Kỹ sư Chống Định Khang & Phạm Trung Kiên  
**Đề tài:** Thiết kế xe tự hành vận chuyển linh kiện (Đồ án 2 - HCMUTE)  
**Giảng viên hướng dẫn:** PGS. TS. Phan Văn Ca  
**Phiên bản:** 2.0 (Bản kỹ thuật thực chiến sau khi đề cương được phê duyệt)

---

## MỤC LỤC CHI TIẾT
1. [Bảng BOM Mua sắm Linh kiện Thực tế & Nơi mua](#1-bảng-bom-mua-sắm-linh-kiện-thực-tế--nơi-mua)
2. [Sơ đồ Nguyên lý & Bản vẽ Đi dây Pin-to-Pin](#2-sơ-đồ-nguyên-lý--bản-vẽ-đi-dây-pin-to-pin)
3. [Quy tắc Đi dây Mass Hình Sao & Chống Nhiễu](#3-quy-tắc-đi-dây-mass-hình-sao--chống-nhiễu)
4. [Bản vẽ Phân bổ Bố trí Vật lý 2 Tầng Xe](#4-bản-vẽ-phân-bổ-bố-trí-vật-lý-2-tầng-xe)
5. [Lộ trình Triển khai 6 Giai đoạn (Cầm tay chỉ việc)](#5-lộ-trình-triển-khai-6-giai-đoạn-cầm-tay-chỉ-việc)
   * [Giai đoạn 1: Cơ khí, Nguồn điện & Mạch bảo vệ](#giai-đoạn-1-cơ-khí-nguồn-điện--mạch-bảo-vệ)
   * [Giai đoạn 2: Lập trình Firmware STM32 (Unit Test từng khối)](#giai-đoạn-2-lập-trình-firmware-stm32-unit-test-từng-khối)
   * [Giai đoạn 3: Driver Giao tiếp UART Nhị phân & Watchdog](#giai-đoạn-3-driver-giao-tiếp-uart-nhị-phân--watchdog)
   * [Giai đoạn 4: Thuật toán Thị giác Máy tính ArUco & Bộ điều khiển PD](#giai-đoạn-4-thuật-toán-thị-giác-máy-tính-aruco--bộ-điều-khiển-pd)
   * [Giai đoạn 5: Tích hợp Máy Trạng Thái FSM & Web Server Flask](#giai-đoạn-5-tích-hợp-máy-trạng-thái-fsm--web-server-flask)
   * [Giai đoạn 6: Đo đạc Thực nghiệm để Đạt Điểm Tuyệt Đối](#giai-đoạn-6-đo-đạc-thực-nghiệm-để-đạt-điểm-tuyệt-đối)

---

# 1. BẢNG BOM MUA SẮM LINH KIỆN THỰC TẾ & NƠI MUA

### A. Danh mục Thiết bị có sẵn (Chi phí: 0đ)
| STT | Thiết bị | Tình trạng | Vai trò hệ thống |
|:---:|---|:---:|---|
| 1 | **Raspberry Pi 4 Model B (Bản 8GB RAM)** | Đã có sẵn | Bộ não xử lý Master: OpenCV, Web Flask, FSM |
| 2 | **Webcam USB Logitech C270 HD (720p)** | Đã có sẵn | Mắt thần thu nhận hình ảnh mốc ArUco qua USB |
| 3 | **Mạch nạp USB-TTL UART (Chip CP2102)** | Đã có sẵn | Cầu nối UART phần cứng ổn định giữa Pi 4 và STM32 |
| 4 | **Thẻ nhớ MicroSD 32GB/64GB Class 10** | Đã có sẵn | Cài đặt hệ điều hành Raspberry Pi OS |

---

### B. Danh mục Mua mới (Dự toán chính xác 100%: 597.000đ)
*(Có thể đặt hàng tại Shopee hoặc ghé trực tiếp các cửa hàng linh kiện điện tử tại TP.HCM như Hshop.vn, Nshopvn, IC Đây Rồi).*

| STT | Tên linh kiện | Thông số kỹ thuật chi tiết | SL | Đơn giá | Thành tiền | Nơi mua gợi ý |
|:---:|---|---|:---:|:---:|:---:|---|
| 1 | **STM32F103C8T6 Blue Pill** | Bản cổng Type-C, chip ARM Cortex-M3 72MHz | 1 | 48.000đ | 48.000đ | Hshop / Shopee |
| 2 | **Module đĩa quang LM393** | Cảm biến khe chữ U 5mm, ngõ ra số D0 | 2 | 8.000đ | 16.000đ | Hshop / Shopee |
| 3 | **Cảm biến siêu âm HC-SR04** | Dải đo 2cm - 400cm, điện áp nuôi 5V chuẩn | 3 | 18.000đ | 54.000đ | Hshop / Shopee |
| 4 | **Màn hình OLED 0.91" I2C** | 128x32 pixel, driver SSD1306, cắm I2C Pi 4 | 1 | 28.000đ | 28.000đ | Hshop / Shopee |
| 5 | **Bộ khung xe 4 bánh mica** | Khung chữ nhật 2 tầng, kèm 4 motor TT & 4 đĩa encoder 20 nan | 1 bộ | 115.000đ | 115.000đ | Hshop / Shopee |
| 6 | **Driver cầu H L298N** | Bản có cánh tản nhiệt nhôm lớn, 2A liên tục/kênh | 1 | 32.000đ | 32.000đ | Hshop / Shopee |
| 7 | **Nút nhấn dừng khẩn cấp (E-Stop)** | Nút nhấn nhả to đường kính 12-16mm hoặc nút nấm | 1 | 5.000đ | 5.000đ | Cửa hàng điện tử |
| 8 | **Active Buzzer 5V + Đệm** | Còi chíp 5V + Transistor 2N2222 + Trở 1kΩ | 1 bộ | 4.000đ | 4.000đ | Cửa hàng điện tử |
| 9 | **Hộp nhựa khay chở hàng** | Hộp nhựa mini trong suốt có nắp | 1 | 10.000đ | 10.000đ | Nhà sách / Đồ gia dụng |
| 10 | **Cell pin Li-ion 18650** | Dòng xả $\ge 10\text{A}$ (Sony VTC4/VTC5 hoặc Lishen 2500mAh) | 5 viên | 32.000đ | 160.000đ | Shopee Pin Chuyên Nghiệp |
| 11 | **Khay 3 pin 18650 (3S)** | Khay nối tiếp có sẵn dây đỏ đen cấp 11.1V | 1 cái | 12.000đ | 12.000đ | Hshop / Shopee |
| 12 | **Khay 2 pin 18650 (2S)** | Khay nối tiếp có sẵn dây đỏ đen cấp 7.4V | 1 cái | 10.000đ | 10.000đ | Hshop / Shopee |
| 13 | **Mạch hạ áp Buck XL4015** | Điện áp vào 8-36V, ra 1.25-32V, dòng tải 5A có tản nhiệt | 1 | 28.000đ | 28.000đ | Hshop / Shopee |
| 14 | **Gói linh kiện bảo vệ & lọc nguồn** | • 1 tụ hóa $2200\mu\text{F}/16\text{V}$<br>• 3 tụ hóa $100\mu\text{F} + 3$ tụ gốm $100\text{nF}$ (siêu âm)<br>• Trở phân áp $10\text{k}\Omega + 3.3\text{k}\Omega$ (sai số 1%)<br>• 1 Diode TVS 5V (SMAJ5.0A) + 1 Polyfuse 3A | 1 gói | 6.000đ | 6.000đ | Cửa hàng linh kiện |
| 15 | **Công tắc nguồn bập bênh** | Loại 2 chân ON/OFF nhỏ KCD11 | 2 cái | 3.000đ | 6.000đ | Cửa hàng linh kiện |
| 16 | **Đầu giắc đực USB Type-C hàn dây** | Bo hàn 2 chân VBUS và GND | 1 cái | 5.000đ | 5.000đ | Hshop / Shopee |
| 17 | **Đốc sạc đôi pin 18650 rời** | Đốc cắm điện lưới 220V tự ngắt khi đầy 4.2V | 1 cái | 28.000đ | 28.000đ | Hshop / Shopee |
| 18 | **Dây nối & Ốc trụ đồng M3** | • 1 tép dây bus Dupont cái-cái 20cm (40 sợi)<br>• Bộ trụ đồng M3 (10mm, 20mm, 30mm) kèm ốc tán | 1 gói | 30.000đ | 30.000đ | Hshop / Shopee |
| **TỔNG** | **TỔNG CHI PHÍ MUA MỚI:** | **Kiểm toán chính xác 100% không phát sinh** | | | **597.000đ** | |

---

# 2. SƠ ĐỒ NGUYÊN LÝ & BẢN VẼ ĐI DÂY PIN-TO-PIN

```
========================================================================================================================
                                      SƠ ĐỒ NGUYÊN LÝ NỐI DÂY TOÀN DIỆN HỆ THỐNG
========================================================================================================================

[ KHAY PIN 3S (11.1V danh định) ]
   (+) Cực Dương ────[Công tắc A]──► Chân IN+ của Mạch Buck XL4015 (5A)
   (-) Cực Âm ─────────────────────► Chân IN- của Mạch Buck XL4015
         │
         ├───► [Điện trở 10kΩ] ───┬───► Chân PA6 của STM32 (ADC đo điện áp pin 3S)
                                  │
                               [3.3kΩ]
                                  │
                                 GND

[ MẠCH BUCK XL4015 (Chỉnh đúng 5.15V, khóa keo biến trở) ]
   Ngõ ra OUT+ (5.15V DC) ─────┬───► [Polyfuse 3A] ──► [TVS 5V] ──► Chân VBUS Giắc Type-C đực ──► Cổng Type-C Pi 4
                               ├───► Chân 5V của STM32 Blue Pill (LDO onboard hạ áp 3.3V nuôi MCU)
                               ├───► Chân VCC của 3 Cảm biến Siêu âm HC-SR04 (Lọc tụ 100uF + 100nF từng con)
                               └───► (+) Tụ hóa 2200uF/16V mắc song song tụ gốm 100nF dập xung transient
   Ngõ ra OUT- (GND) ──────────┴───► [ĐIỂM NỐI MASS TRUNG TÂM - STAR GROUND]

[ RASPBERRY PI 4 MODEL B ]
   • Cổng USB 2.0 (Đen 1) ────────► Cắm Cáp Webcam Logitech C270 HD (720p)
   • Cổng USB 2.0 (Đen 2) ────────► Cắm Mạch nạp USB-TTL (CP2102)
   • Cổng GPIO Header:
       - Pin 1 (3.3V) ────────────► Chân VCC Màn hình OLED 0.91" I2C
       - Pin 9 (GND) ─────────────► Chân GND Màn hình OLED 0.91" I2C
       - Pin 3 (GPIO 2 - SDA) ────► Chân SDA Màn hình OLED 0.91" I2C
       - Pin 5 (GPIO 3 - SCL) ────► Chân SCL Màn hình OLED 0.91" I2C

[ MẠCH NẠP USB-TTL CP2102 (Cắm cổng USB Pi 4, lấy nguồn trực tiếp từ USB) ]
   • Chân TX của CP2102 ──────────► Chân PA10 của STM32 (USART1_RX)
   • Chân RX của CP2102 ──────────► Chân PA9 của STM32 (USART1_TX)
   • Chân GND của CP2102 ─────────► Nối chung Mass với STM32

[ KHAY PIN 2S (7.4V danh định) ]
   (+) Cực Dương ────[Công tắc B]──► [Nút E-Stop Cơ Khí] ──► Chân 12V/VM của Driver L298N
   (-) Cực Âm ──────────────────────────────────────────► Chân GND của Driver L298N ──► [STAR GROUND]

[ DRIVER CẦU H L298N (Tháo bỏ jumper 5V-EN nếu có để cô lập nguồn logic) ]
   • Chân ENA ────────────────────► PA0 của STM32 (Timer 2 Ch1 - PWM bánh Trái)
   • Chân IN1 ────────────────────► PA1 của STM32 (Chiều 1 bánh Trái)
   • Chân IN2 ────────────────────► PA2 của STM32 (Chiều 2 bánh Trái)
   • Chân ENB ────────────────────► PA3 của STM32 (Timer 2 Ch4 - PWM bánh Phải)
   • Chân IN3 ────────────────────► PA4 của STM32 (Chiều 1 bánh Phải)
   • Chân IN4 ────────────────────► PA5 của STM32 (Chiều 2 bánh Phải)
   • Cụm OUT1 & OUT2 ─────────────► Đấu song song 2 Động cơ TT Bánh Trái (Trước & Sau)
   • Cụm OUT3 & OUT4 ─────────────► Đấu song song 2 Động cơ TT Bánh Phải (Trước & Sau)

[ 2 MODULE CẢM BIẾN ĐĨA QUANG ENCODER LM393 ]
   • VCC của 2 Encoder ───────────► Chân 3.3V của STM32
   • GND của 2 Encoder ───────────► Chân GND của STM32
   • D0 Encoder Bánh Trái ────────► Chân PB6 của STM32 (Ngắt ngoài EXTI6)
   • D0 Encoder Bánh Phải ────────► Chân PB7 của STM32 (Ngắt ngoài EXTI7)

[ 3 CẢM BIẾN SIÊU ÂM HC-SR04 ]
   • Chân Trig Trái ──────────────► PB12 của STM32
   • Chân Echo Trái (5V) ─────────► PB13 của STM32 (Chân chịu áp 5V-Tolerant)
   • Chân Trig Giữa ──────────────► PB14 của STM32
   • Chân Echo Giữa (5V) ─────────► PB15 của STM32 (Chân chịu áp 5V-Tolerant)
   • Chân Trig Phải ──────────────► PB8 của STM32
   • Chân Echo Phải (5V) ─────────► PB9 của STM32 (Chân chịu áp 5V-Tolerant)

[ CÒI BÁO ĐỘNG BUZZER 5V ]
   • Chân PB5 của STM32 ──────────► Điện trở 1kΩ ──► Cực Base Transistor 2N2222
   • Cực Emitter của 2N2222 ──────► Nối Mass GND
   • Cực Collector của 2N2222 ────► Cực Âm (-) của Còi Buzzer
   • Cực Dương (+) của Còi Buzzer ─► Đường nguồn 5.15V từ XL4015
========================================================================================================================
```

---

# 3. QUY TẮC ĐI DÂY MASS HÌNH SAO & CHỐNG NHIỄU

Một sai lầm kinh điển của sinh viên làm robot xe chạy STM32 + Raspberry Pi là **nối mass hình chuỗi (Daisy-Chain Grounding)**: Nối cực âm pin vào Driver, rồi từ Driver câu dây mass sang STM32, rồi từ STM32 câu tiếp sang Pi 4.
* **Hậu quả:** Khi 4 động cơ TT đảo chiều hoặc khởi động, dòng điện hàng ampe chạy qua đoạn dây mass sinh ra sụt áp tức thời $\Delta V = I \cdot R_{dây} + L \frac{di}{dt}$. Hiện tượng này làm điện thế điểm mass của STM32 nhảy vọt lên $\approx 0.5\text{V} - 1.0\text{V}$ so với mass thực tế của pin, gây hiện tượng:
  1. Bộ chuyển đổi ADC PA6 đọc sai điện áp pin, kích hoạt cảnh báo brownout giả.
  2. Xung phản hồi Echo của siêu âm bị nhiễu kích hoạt phanh ảo.
  3. Vi điều khiển STM32 hoặc Pi 4 bị treo ngẫu nhiên (*Hard Fault / Reset*).

### Quy tắc Đi dây Mass Hình Sao Thực chiến:
1. **Thiết lập 1 điểm Mass Trung tâm (Star Point):** Dùng một cầu đấu domino hoặc hàn một cụm 4-6 chân trên bo mạch đục lỗ làm "Mass gốc".
2. **Kéo dây riêng biệt từ từng khối về điểm Mass gốc:**
   * Dây 1: Cực âm khay pin 3S.
   * Dây 2: Cực âm khay pin 2S.
   * Dây 3: Chân OUT- của module Buck XL4015.
   * Dây 4: Chân GND của Driver L298N (dùng dây lớn $\ge 0.5\text{mm}^2$ hoặc AWG 20).
   * Dây 5: Chân GND của STM32.
   * Dây 6: Chân GND của Raspberry Pi 4.
3. Tuyệt đối không để dòng điện trở về của động cơ chạy xuyên qua bo mạch của STM32 hoặc Pi 4!

---

# 4. BẢN VẼ PHÂN BỔ BỐ TRÍ VẬT LÝ 2 TẦNG XE

```
========================================================================================================================
                                     BẢN VẼ BỐ TRÍ VẬT LÝ KHUNG XE MICA 2 TẦNG
========================================================================================================================

                                  [ ĐẦU XE (MŨI XE HƯỚNG TỚI) ]
        ┌────────────────────────────────────────────────────────────────────────┐
        │       [Siêu âm Trái 45°]      [Siêu âm Giữa 0°]      [Siêu âm Phải -45°]│
        │                                                                        │
        │                       ┌─────────────────────────┐                      │
        │                       │  WEBCAM LOGITECH C270   │                      │
        │                       │ (Bắt vít cố định cứng,  │                      │
        │                       │  cao 20cm, nghiêng 6°)  │                      │
        │                       └─────────────────────────┘                      │
        │                                                                        │
        │   ┌──────────────────────────┐         ┌──────────────────────────┐    │
        │   │   RASPBERRY PI 4 MODEL B │         │  BO MẠCH STM32 BLUE PILL │    │
        │   │  (Có tản nhiệt nhôm lớn) │         │ (Kèm perfboard đệm còi & │    │
        │   │  • Cắm USB Webcam C270   │         │  trở phân áp đo pin ADC) │    │
        │   │  • Cắm USB-TTL CP2102    │         └──────────────────────────┘    │
        │   └──────────────────────────┘                       │                 │
        │                                             [OLED 0.91" I2C]           │
        │                                                                        │
        │                 ┌──────────────────────────────────────┐               │
        │                 │       KHOANG CHỨA KHAY LINH KIỆN     │               │
        │                 │  (Hộp nhựa trong suốt có nắp chốt)   │               │
        │                 └──────────────────────────────────────┘               │
        │                                                                        │
        │       [NÚT E-STOP KHẨN CẤP]                  [CÔNG TẮC NGUỒN A & B]    │
        └────────────────────────────────────────────────────────────────────────┘
                                 [ ĐUÔI XE (PHÍA SAU) ]

========================================================================================================================
TẦNG ĐÁY (GẦM XE):
• 4 Động cơ giảm tốc TT bắt vào 4 góc khung xe.
• 2 Đĩa encoder 20 nan kèm 2 cảm biến quang LM393 bố trí ở 2 bánh sau (tránh va đập).
• Khay pin 3S (bên trái) và Khay pin 2S (bên phải) bắt đối xứng sát mép để hạ trọng tâm xe.
• Driver L298N bắt ở trung tâm tầng đáy, hướng cánh tản nhiệt nhôm ra khoảng thoáng để gió làm mát.
• Mạch Buck XL4015 gắn ở góc sau tầng đáy, thuận tiện luồn dây Type-C lên tầng trên cấp cho Pi 4.
========================================================================================================================
```

---

# 5. LỘ TRÌNH TRIỂN KHAI 6 GIAI ĐOẠN (CẦM TAY CHỈ VIỆC)

## GIAI ĐOẠN 1: CƠ KHÍ, NGUỒN ĐIỆN & MẠCH BẢO VỆ (1 - 2 ngày)

### Mục tiêu: Khung xe lăn bánh trơn tru, nguồn điện ổn định, đo kiểm an toàn 100% trước khi cắm Pi 4.

1. **Lắp ráp cơ khí:**
   * Bắt 4 động cơ TT vào tầng đáy bằng bu-lông dài đi kèm bộ khung. Chú ý quay trục động cơ ra ngoài, 2 bánh sau lắp thêm đĩa encoder đen 20 nan sát vào thân motor.
   * Cố định 2 module đĩa quang LM393 sao cho đĩa 20 nan nằm lọt chính giữa khe chữ U của cảm biến, không bị cạ quẹt khi bánh xe quay.
   * Lắp 4 bánh cao su 65mm. Đặt xe xuống sàn đẩy thử bằng tay xem có bị kẹt hay lệch bánh không.
2. **Cân chỉnh mạch hạ áp XL4015 (Cực kỳ quan trọng - Tránh cháy Pi 4):**
   * Lắp 3 viên pin 18650 vào khay 3S (đo kiểm điện áp hai đầu dây ra đạt $11.1\text{V} - 12.6\text{V}$).
   * Nối cực dương pin 3S qua Công tắc A vào chân `IN+` của XL4015, cực âm vào `IN-`.
   * **Chưa cắm bất kỳ vi điều khiển hay Pi 4 nào!** Bật Công tắc A.
   * Dùng đồng hồ VOM (thang đo DC 20V), chọc que đo vào 2 chân `OUT+` và `OUT-`. Dùng tuốc nơ vít dẹp nhỏ vặn biến trở đa vòng ngược chiều kim đồng hồ cho đến khi điện áp hiển thị **chính xác 5.15V** (sai số cho phép: $5.12\text{V} - 5.18\text{V}$).
   * Sau khi chỉnh chuẩn, **nhỏ 1 giọt keo nến hoặc sơn móng tay lên ốc biến trở** để cố định không cho trôi áp khi xe rung lắc.
3. **Lắp ráp cụm bảo vệ ngõ ra:**
   * Hàn nối tiếp cầu chì Polyfuse 3A trên đường dương $5.15\text{V}$.
   * Hàn diode TVS SMAJ5.0A mắc song song giữa đường dương và mass (vạch trắng catot nối dương).
   * Hàn dây dương vào chân VBUS và dây âm vào chân GND của giắc Type-C đực. Bọc ống gen co nhiệt cẩn thận.
4. **Hàn bo đệm chia nguồn & phân áp ADC:**
   * Dùng một miếng phíp đồng đục lỗ (perfboard) nhỏ $3\text{cm} \times 5\text{cm}$:
     * Tạo đường bus 5.15V và Mass hình sao.
     * Hàn cầu phân áp gồm điện trở $10\text{k}\Omega$ (nối với cực dương pin 3S) và $3.3\text{k}\Omega$ (nối mass). Điểm giữa hàn ra một cọng dây bus cắm vào PA6 của STM32.
     * Hàn cụm còi Buzzer: chân dương còi nối 5.15V, chân âm nối Collector 2N2222, Emitter nối Mass, Base qua trở $1\text{k}\Omega$ ra chân PB5 STM32.

---

## GIAI ĐOẠN 2: LẬP TRÌNH FIRMWARE STM32 (UNIT TEST TỪNG KHỐI) (2 - 3 ngày)

### Mục tiêu: STM32 làm chủ hoàn toàn chuyển động cơ khí, đọc cảm biến và đo áp độc lập.

Sử dụng phần mềm **STM32CubeIDE** tạo project với chip `STM32F103C8Tx`:

### 1. Cấu hình Timer PWM Điều khiển 4 Động cơ:
* Cấu hình **TIM2**:
  * Channel 1 (`PA0`): PWM Generation (Bánh Trái - chân ENA).
  * Channel 4 (`PA3`): PWM Generation (Bánh Phải - chân ENB).
  * Prescaler: `71` $\rightarrow$ Tần số đếm timer: $72\text{MHz} / (71 + 1) = 1\text{MHz}$.
  * AutoReload Register (ARR): `999` $\rightarrow$ Tần số xung PWM: $1\text{MHz} / 1000 = 1\text{kHz}$ (tần số 1kHz giúp động cơ chạy êm, không bị rít tiếng kêu tần số âm thanh).
  * Duty cycle từ $0 - 100\%$ tương ứng giá trị CCR từ $0 - 999$.
* Cấu hình 4 chân GPIO Output cho chiều quay:
  * `PA1` (IN1), `PA2` (IN2): Chiều bánh Trái.
  * `PA4` (IN3), `PA5` (IN4): Chiều bánh Phải.
* **Viết hàm test động lực:**
  ```c
  void Motor_SetSpeed(int16_t left_speed, int16_t right_speed) {
      // left_speed, right_speed từ -100 đến +100
      // Khởi động mềm qua độ dốc gia tốc
      // Điều khiển chiều qua IN1..IN4 và độ rộng xung qua CCR1, CCR4
  }
  ```

### 2. Cấu hình Ngắt Ngoài Đếm Xung Encoder LM393:
* Cấu hình chân `PB6` và `PB7` ở chế độ **GPIO_EXTI**:
  * Trigger: `Rising Edge` (Sườn lên).
  * Pull-up: Enabled.
  * Trong NVIC Settings: Kích hoạt ngắt `EXTI line[9:5] interrupts`.
* Viết hàm đếm xung:
  ```c
  volatile uint32_t pulse_count_left = 0;
  volatile uint32_t pulse_count_right = 0;

  void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
      if (GPIO_Pin == GPIO_PIN_6) {
          pulse_count_left++;
      } else if (GPIO_Pin == GPIO_PIN_7) {
          pulse_count_right++;
      }
  }
  ```

### 3. Cấu hình Quét Siêu Âm Tuần Tự TDM (Chống nhiễu chéo):
* Khởi tạo một Timer định thời chu kỳ **$25\text{ms}$** (ví dụ TIM3):
  * State 0 ($0\text{ms}$): Kích xung Trig Trái (`PB12`) $10\mu\text{s}$, đọc thời gian Echo (`PB13`).
  * State 1 ($25\text{ms}$): Kích xung Trig Giữa (`PB14`) $10\mu\text{s}$, đọc thời gian Echo (`PB15`).
  * State 2 ($50\text{ms}$): Kích xung Trig Phải (`PB8`) $10\mu\text{s}$, đọc thời gian Echo (`PB9`).
* Áp dụng bộ lọc trung vị (Median Filter 3 mẫu) để loại trừ các giá trị đo đột biến do sóng âm phản xạ trượt.

### 4. Cấu hình ADC Đo Điện Áp Pin 3S:
* Cấu hình **ADC1 Channel 6 (`PA6`)**, độ phân giải 12-bit (giá trị $0 - 4095$ ứng với $0 - 3.3\text{V}$).
* Đọc ADC chu kỳ $500\text{ms}$, tính toán:
  $$V_{bat} = \frac{ADC\_RAW \times 3.3\text{V}}{4095} \times \frac{10\text{k}\Omega + 3.3\text{k}\Omega}{3.3\text{k}\Omega} \approx \frac{ADC\_RAW \times 3.3}{4095 \times 0.2481} \text{ (Volt)}$$
* Nếu $V_{bat} \le 10.2\text{V}$: Bật cờ cảnh báo sớm.
* Nếu $V_{bat} \le 9.6\text{V}$: Bật còi và gửi lệnh cưỡng bức shutdown sang Pi 4.

---

## GIAI ĐOẠN 3: DRIVER GIAO TIẾP UART NHỊ PHÂN & WATCHDOG (1 - 2 ngày)

### Mục tiêu: Pi 4 gửi lệnh điều khiển vận tốc mượt mà, STM32 phản hồi dữ liệu cảm biến; nếu mất kết nối 300ms xe tự dừng an toàn.

### 1. Phía STM32 (C/C++):
* Cấu hình **USART1** (`PA9` TX, `PA10` RX) ở tốc độ **115200 baud**, 8 data bits, no parity, 1 stop bit.
* Bật ngắt nhận byte `HAL_UART_Receive_IT(&huart1, &rx_byte, 1)`.
* Xây dựng máy trạng thái giải mã gói tin (Parser State Machine):
  * `WAIT_HEADER_1 (0xAA)` $\rightarrow$ `WAIT_HEADER_2 (0x55)` $\rightarrow$ `READ_LEN` $\rightarrow$ `READ_MSG_TYPE` $\rightarrow$ `READ_PAYLOAD` $\rightarrow$ `VERIFY_CHECKSUM`.
* **Cài đặt Watchdog Timer 300ms:**
  ```c
  volatile uint32_t uart_watchdog_counter = 0;

  // Trong ngắt SysTick (1ms gọi 1 lần):
  void SysTick_Handler(void) {
      HAL_IncTick();
      uart_watchdog_counter++;
      if (uart_watchdog_counter >= 300) { // Quá 300ms không nhận gói hợp lệ
          Motor_SetSpeed(0, 0);          // Cắt PWM khẩn cấp
          Buzzer_SetState(BUZZER_BEEP_ALARM); // Hú còi cảnh báo Pi treo
      }
  }

  // Khi parser xác nhận gói tin hợp lệ:
  void On_Valid_Packet_Received(uint8_t msg_type, uint8_t *payload) {
      uart_watchdog_counter = 0; // Reset watchdog
      if (msg_type == 0x01) {    // Lệnh vận tốc
          int16_t v_L = (payload[0] << 8) | payload[1];
          int16_t v_R = (payload[2] << 8) | payload[3];
          Motor_SetSpeed(v_L, v_R);
      }
  }
  ```

### 2. Phía Raspberry Pi 4 (Python):
* Cài đặt thư viện: `pip3 install pyserial`.
* Viết module giao tiếp `uart_bridge.py`:
  ```python
  import serial, struct, time

  class STM32Bridge:
      def __init__(self, port='/dev/ttyUSB0', baudrate=115200):
          self.ser = serial.Serial(port, baudrate, timeout=0.05)
          time.sleep(1.0) # Chờ DTR ổn định

      def send_velocity(self, v_left, v_right):
          # v_left, v_right: -100 đến 100
          msg_type = 0x01
          payload = struct.pack('>hh', int(v_left), int(v_right))
          length = len(payload) + 1 # Bao gồm msg_type
          
          # Tính XOR Checksum
          chk = length ^ msg_type
          for b in payload:
              chk ^= b
              
          frame = bytearray([0xAA, 0x55, length, msg_type]) + payload + bytearray([chk])
          self.ser.write(frame)

      def send_heartbeat(self):
          # Gửi khi xe dừng để reset watchdog
          frame = bytearray([0xAA, 0x55, 0x01, 0x02, 0x01 ^ 0x02])
          self.ser.write(frame)
  ```

---

## GIAI ĐOẠN 4: THUẬT TOÁN THỊ GIÁC MÁY TÍNH ARUCO & BỘ ĐIỀU KHIỂN PD (2 ngày)

### Mục tiêu: Camera nhận diện mốc 18cm mượt mà, tính sai số và xuất tín hiệu lái ổn định.

1. **Tối ưu hóa thu luồng ảnh Webcam Logitech C270:**
   * Sử dụng backend `cv2.CAP_V4L2` và FourCC `MJPG` để đạt tốc độ khung hình 15 - 25 FPS trên Pi 4:
     ```python
     import cv2

     cap = cv2.VideoCapture(0, cv2.CAP_V4L2)
     cap.set(cv2.CAP_PROP_FOURCC, cv2.VideoWriter_fourcc('M','J','P','G'))
     cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
     cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)
     cap.set(cv2.CAP_PROP_FPS, 30)
     ```
2. **Nhận diện Mốc ArUco DICT_4X4_50:**
   * Tạo từ điển: `aruco_dict = cv2.aruco.getPredefinedDictionary(cv2.aruco.DICT_4X4_50)`.
   * Chuyển ảnh xám `gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)`.
   * Phát hiện góc mốc: `corners, ids, rejected = cv2.aruco.detectMarkers(gray, aruco_dict)`.
   * Nếu phát hiện đúng ID yêu cầu:
     * Tính tâm mốc: $C_x = \frac{x_0 + x_1 + x_2 + x_3}{4}$.
     * Tính bề rộng pixel: $W_{px} = \max(|x_1 - x_0|, |x_2 - x_3|)$.
     * Sai số chuẩn hóa: $e_{norm} = \frac{C_x - 320}{320} \in [-1.0, 1.0]$.
3. **Bộ điều khiển PD Chuẩn hóa có Lọc thông thấp:**
   ```python
   class PDController:
       def __init__(self, Kp=0.45, Kd=0.08, alpha=0.25):
           self.Kp = Kp
           self.Kd = Kd
           self.alpha = alpha
           self.last_e = 0.0
           self.filtered_de = 0.0
           self.last_time = time.perf_counter()

       def compute(self, e_norm):
           now = time.perf_counter()
           dt = now - self.last_time
           if dt <= 0.001: dt = 0.033 # Mặc định nếu dt quá nhỏ

           # Đạo hàm và lọc nhiễu jitter
           raw_de = (e_norm - self.last_e) / dt
           self.filtered_de = self.alpha * raw_de + (1 - self.alpha) * self.filtered_de

           # Tín hiệu điều khiển vi sai
           u = self.Kp * e_norm + self.Kd * self.filtered_de
           u_sat = max(-0.5, min(0.5, u)) # Bão hòa giới hạn lái

           self.last_e = e_norm
           self.last_time = now

           # Phân bổ PWM
           PWM_0 = 50.0      # Vận tốc hành trình 50%
           PWM_SCALE = 30.0  # Độ nhạy vi sai
           pwm_l = max(35.0, min(80.0, PWM_0 - u_sat * PWM_SCALE))
           pwm_r = max(35.0, min(80.0, PWM_0 + u_sat * PWM_SCALE))
           return pwm_l, pwm_r
   ```

---

## GIAI ĐOẠN 5: TÍCH HỢP MÁY TRẠNG THÁI FSM & WEB SERVER FLASK (2 - 3 ngày)

### Mục tiêu: Hệ thống vận hành tự động toàn chu trình, điều phối và giám sát video trực tiếp qua điện thoại/máy tính.

1. **Lập trình FSM hoàn chỉnh trên Pi 4 (`robot_fsm.py`):**
   * **State `IDLE`:** Đứng yên, gửi heartbeat, chờ nút bấm chọn trạm trên Web.
   * **State `SEARCH`:** Xe nhích xoay vi sai từng góc $15^\circ$ $\rightarrow$ dừng $100\text{ms}$ chụp ảnh. Nếu thấy đúng ID $\rightarrow$ `ALIGN`; nếu xoay quá 8s $\rightarrow$ `TIMEOUT`.
   * **State `ALIGN`:** Nhích nhẹ thân xe tại chỗ cho tới khi $|e_{norm}| < 0.1$ $\rightarrow$ `NAVIGATE`.
   * **State `NAVIGATE`:** Chạy PD bám mốc. Nếu siêu âm bất kỳ $< 30\text{cm}$ $\rightarrow$ `OBSTACLE`. Khi $W_{marker} \ge 180\text{px}$ $\rightarrow$ `APPROACH_DOCKING`.
   * **State `APPROACH_DOCKING`:** Giảm tốc bò $PWM \approx 40\%$. Chỉ đọc siêu âm giữa. Nếu siêu âm giữa $< 15\text{cm}$ mà $W_{marker} < 350\text{px}$ $\rightarrow$ `OBSTACLE` (vật lạ). Khi siêu âm giữa $\le 10\text{cm}$ VÀ $W_{marker} \ge 350\text{px}$ $\rightarrow$ `DOCKING`. Nếu đi quá $35\text{cm}$ encoder mà không cập trạm $\rightarrow$ `OVERSHOOT`.
   * **State `DOCKING`:** Phanh dừng, phát còi dài, thông báo Web lấy hàng, trở về `IDLE`.
   * **State `OVERSHOOT`:** Lùi chậm $3\text{cm}$ tìm lại mốc. Nếu thất bại $\rightarrow$ Dừng hẳn báo lỗi.
   * **State `OBSTACLE`:** Dừng xe, hú còi. Khi cự ly thoáng $> 35\text{cm}$, giữ trễ thêm $1.0\text{s}$ rồi chạy tiếp. Nếu kẹt quá $5.0\text{s}$ $\rightarrow$ Báo Web.
2. **Máy chủ Web Flask & Giao diện Điều phối:**
   * Truyền phát video MJPEG: `yield (b'--frame\r\n' b'Content-Type: image/jpeg\r\n\r\n' + frame_bytes + b'\r\n')`.
   * Giao diện HTML5 responsive: 3 nút bấm lớn "Trạm 1", "Trạm 2", "Trạm 3", nút đỏ to "DỪNG KHẨN CẤP", thanh hiển thị điện áp pin và khoảng cách siêu âm thời gian thực.
3. **Cài đặt Tự khởi động cùng Hệ điều hành (systemd service):**
   * Tạo file `/etc/systemd/system/amr_robot.service` để robot tự động khởi chạy chương trình ngay sau khi Pi 4 khởi động mà không cần cắm màn hình bàn phím.

---

## GIAI ĐOẠN 6: ĐO ĐẠC THỰC NGHIỆM ĐỂ ĐẠT ĐIỂM TUYỆT ĐỐI (2 ngày)

### Mục tiêu: Thu thập số liệu định lượng bảo vệ thuyết phục trước Hội đồng Thầy Phan Văn Ca.

1. **Khảo sát Vùng chết Động cơ ($PWM_{min}$):**
   * Đặt xe trên mặt sàn lab. Tăng dần PWM từ $20\%$ lên từng bước $2\%$. Ghi lại giá trị PWM mà xe bắt đầu nhúc nhích. Lập bảng và vẽ đồ thị vùng chết.
2. **Đo dòng tiêu thụ thực tế:**
   * Dùng đồng hồ VOM thang đo 10A DC nối tiếp vào cực dương nguồn motor.
   * Ghi nhận 3 giá trị: Dòng không tải nhấc bánh ($\approx 0.3\text{A}$), Dòng có tải chạy sàn ($\approx 0.8\text{A}$), Dòng stall giữ bánh ($\approx 2.4\text{A}$). Chứng minh dòng làm việc thực tế nằm trong ngưỡng an toàn 2A của L298N.
3. **Đồ thị Thực nghiệm Đáp ứng P vs PD:**
   * Cho xe chạy bám mốc ở cự ly $2.0\text{m}$. Lưu mảng giá trị `(timestamp, e_norm)` vào file CSV.
   * Vẽ 2 đường đồ thị trên cùng 1 biểu đồ:
     * Đường 1 ($K_p=0.45, K_d=0$): Sai số dao động điều hòa quanh số 0 (xe lắc mũi).
     * Đường 2 ($K_p=0.45, K_d=0.08$ có lọc $\alpha$): Sai số hội tụ mượt mà về 0 trong vòng $1.2\text{s}$.
4. **Đo sai số cập trạm (Docking Accuracy):**
   * Cho xe cập trạm 30 lần. Dùng thước lá đo sai số cự ly dừng ($\pm \text{mm}$) và sai số lệch tâm ngang ($\pm \text{mm}$). Lập bảng thống kê độ lệch chuẩn $\sigma$ và tỷ lệ hoàn thành nhiệm vụ thành công ($\ge 95\%$).
