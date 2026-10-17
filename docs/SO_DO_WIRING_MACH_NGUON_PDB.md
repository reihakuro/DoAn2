# SƠ ĐỒ WIRING, NETLIST & BẢNG PHÂN BỔ CHÂN (PINOUT) HỆ THỐNG AGV
## TÀI LIỆU KỸ THUẬT PHỤC VỤ THI CÔNG, KIỂM TRA ĐI DÂY & PHÂN TÍCH SCHEMATIC
**Chuẩn hóa 100% theo bản vẽ KiCad: `power-source-board.kicad_sch` (Rev 2.0 - Đồ án 2 HCM-UTE)**

---

## 1. TỔNG QUAN KIẾN TRÚC PHẦN CỨNG & CÁC CẢI TIẾN TRÊN BẢN VẼ SCHEMATIC

Bản vẽ nguyên lý chuẩn hóa `power-source-board.kicad_sch` tích hợp toàn bộ các khối chức năng từ phân phối nguồn, chống brownout, mạch điều khiển vi sai động cơ, đọc cảm biến và bảo vệ quá áp:
1. **Kiến trúc nguồn 2 nhánh cách ly hoàn toàn:**
   * **Nhánh A (Logic):** Khay 3S (`BT1, BT2, BT3` nối tiếp, $9.0\text{V} - 12.6\text{V}$) $\to$ Cọc `J14` $\to$ Công tắc A `J3` $\to$ Lọc nguồn `C13 (100uF) + C14 (100nF)` $\to$ Module Buck XL4015 `U3` hạ áp ra `+5V`.
   * **Nhánh B (Động lực Motor):** Khay 2S (`BT4, BT5` nối tiếp, $6.0\text{V} - 8.4\text{V}$) $\to$ Cọc `J15` $\to$ Công tắc B `J4` $\to$ Nút dừng khẩn cấp E-Stop `J5` $\to$ Đường nguồn `12V/8V4` $\to$ Cọc `J12` cấp L298N.
2. **Cơ cấu Star Ground & Tách Mass qua NetTie:**
   * Mạch phân định rạch ròi giữa Mass Logic (`GND`) và Mass Công suất Động cơ (`PGND`). Hai đường mass này chỉ chạm nhau tại **1 điểm duy nhất thông qua linh kiện `NetTie_2`** trên PCB, ngăn hoàn toàn dòng điện xung chổi than và hiện tượng sụt áp mass (ground bounce) lan truyền sang vi điều khiển và máy tính nhúng.
3. **Mạch bảo vệ ngõ ra Raspberry Pi 4 (Lớp Crowbar bảo vệ sống còn):**
   * Đường nguồn từ `+5V` đi qua cầu chì tự phục hồi `F1 (Polyfuse 3A)` để tạo thành đường `+5V/PI`.
   * Tại node `+5V/PI` mắc song song:
     * **Diode TVS `D1`:** Đã được tính toán nâng cấp từ SMAJ5.0A lên **SMAJ6.0A** (SMD) hoặc **1.5KE6.8A** (DIP) để thích ứng với điện áp Buck $5.15\text{V} - 5.20\text{V}$ mà không bị rò dòng/phát nhiệt.
     * **Cụm tụ bù sụt áp:** Tụ hóa Low-ESR cực lớn `C1 (2200uF/16V)` và tụ gốm `C2 (100nF)` triệt tiêu hoàn toàn cảnh báo sụt áp khi Pi 4 boost CPU.
4. **Giám sát kép điện áp 2 nhánh Pin (ADC PA6 & PA7):**
   * Đo dung lượng pin Logic 3S: `R1 (10k) + R2 (3.3k) + C9 (100nF)` đưa về chân **PA6**.
   * Đo dung lượng pin Động cơ 2S (hoặc pin 3S nếu nâng cấp): `R13 (10k) + R14 (3.3k) + C10 (100nF)` đưa về chân **PA7**.
5. **Đệm bảo vệ & Chuyển mức tín hiệu an toàn:**
   * 3 Cảm biến siêu âm HC-SR04: Đường phát xung Trig có trở đệm `R25, R26, R27 (330Ω)`. Đường phản hồi Echo được hạ áp qua cầu phân áp $5\text{V} \to 3.33\text{V}$ (`R7/R8`, `R9/R10`, `R11/R12`) về các chân PB13, PB15, PB9.
   * 2 Cảm biến đĩa quang Encoder LM393: Tích hợp mạch lọc thông thấp RC `R23 + C15` và `R24 + C16` ($330\Omega + 1\text{nF}$, tần số cắt $\approx 482\text{kHz}$) triệt xung nhiễu điện từ từ động cơ.
   * Cụm điều khiển Motor L298N: 6 trở đệm `R15 ÷ R20 (330Ω)` bảo vệ chân STM32, kèm 2 trở kéo xuống `R21, R22 (10k)` khóa động cơ an toàn khi vi điều khiển nạp code/reset.

---

## 2. BẢNG CHI TIẾT CÁC ĐẦU NỐI VÀ CONNECTOR (J1 ĐẾN J18)

Bảng dưới đây liệt kê chức năng chi tiết từng chân của toàn bộ 18 đầu nối/giắc cắm xuất hiện trên bản vẽ Schematic KiCad:

| Kí hiệu | Tên cổng / Khối chức năng | Kiểu chân (Footprint) | Số chân | Chi tiết định tuyến từng chân (Pin-to-Pin Assignment) |
|:---:|---|:---:|:---:|---|
| **J1** | **Encoder LM393 Trái** | Header 1x3P bước 2.54mm | 3P | • **Pin 1:** `+5V` (Nguồn nuôi Encoder)<br>• **Pin 2:** `GND` (Mass logic)<br>• **Pin 3:** Tín hiệu xung đĩa quang `D0/PB6` (đi qua trở $R23=330\Omega$ và tụ lọc $C15=1\text{nF}$ vào chân STM32 **PB6**) |
| **J2** | **Encoder LM393 Phải** | Header 1x3P bước 2.54mm | 3P | • **Pin 1:** `+5V` (Nguồn nuôi Encoder)<br>• **Pin 2:** `GND` (Mass logic)<br>• **Pin 3:** Tín hiệu xung đĩa quang `D0/PB7` (đi qua trở $R24=330\Omega$ và tụ lọc $C16=1\text{nF}$ vào chân STM32 **PB7**) |
| **J3** | **SWITCH A (Công tắc Logic)**| Header 1x2P hoặc KF301-2P | 2P | • **Pin 1:** Nhận điện áp dương `+12.6V` từ cọc `J14.1`<br>• **Pin 2:** Xuất điện áp sau công tắc `3S_V` sang module Buck XL4015 và cầu trở đo áp PA6 |
| **J4** | **SWITCH B (Công tắc Motor)**| Header 1x2P hoặc KF301-2P | 2P | • **Pin 1:** Nhận điện áp dương `+8.4V` từ cọc `J15.1`<br>• **Pin 2:** Xuất điện áp sang chân 1 của nút dừng khẩn cấp E-Stop `J5.1` |
| **J5** | **LATCHED BUTTON (Nút E-Stop)**| Header 1x2P hoặc KF301-2P | 2P | • **Pin 1:** Nhận điện áp từ chân `J4.2`<br>• **Pin 2:** Xuất điện áp động lực `12V/8V4` sang cọc nguồn L298N `J12.1`, cầu trở đo áp PA7 và LED báo nguồn D4 |
| **J6, J8**| **Socket Module Buck XL4015** | Header cái 1x2P bước 2.54mm | 4P | • **J6/J8 In Pin 1:** Nhận `3S_V` từ `J3.2`<br>• **J6/J8 In Pin 2:** Nối `GND`<br>• **J6/J8 Out Pin 1:** Xuất điện áp hạ áp `+5V` (chỉnh 5.15V)<br>• **J6/J8 Out Pin 2:** Nối `GND` |
| **J7** | **TTL-CP2102 (Nạp code & UART)**| Header 1x4P bước 2.54mm | 4P | • **Pin 1:** `+5V`<br>• **Pin 2:** TXD (qua trở đệm $R28=330\Omega$ vào chân STM32 **PA9 - TX1**)<br>• **Pin 3:** RXD (qua trở đệm $R29=330\Omega$ vào chân STM32 **PA10 - RX1**)<br>• **Pin 4:** `GND` |
| **J9** | **US1 (Siêu âm góc Trái)** | Header 1x4P bước 2.54mm | 4P | • **Pin 1:** `+5V` (Lọc bởi $C3=100\mu\text{F} + C4=100\text{nF}$)<br>• **Pin 2:** `Trig` (nhận xung từ chân STM32 **PB12** qua trở $R25=330\Omega$)<br>• **Pin 3:** `Echo` (nối vào cầu phân áp $R7=1\text{k}\Omega / R8=2\text{k}\Omega$ về chân STM32 **PB13**)<br>• **Pin 4:** `GND` |
| **J10** | **US2 (Siêu âm phía Trước Giữa)**| Header 1x4P bước 2.54mm | 4P | • **Pin 1:** `+5V` (Lọc bởi $C5=100\mu\text{F} + C6=100\text{nF}$)<br>• **Pin 2:** `Trig` (nhận xung từ chân STM32 **PB14** qua trở $R26=330\Omega$)<br>• **Pin 3:** `Echo` (nối vào cầu phân áp $R9=1\text{k}\Omega / R10=2\text{k}\Omega$ về chân STM32 **PB15**)<br>• **Pin 4:** `GND` |
| **J11** | **US3 (Siêu âm góc Phải)** | Header 1x4P bước 2.54mm | 4P | • **Pin 1:** `+5V` (Lọc bởi $C7=100\mu\text{F} + C8=100\text{nF}$)<br>• **Pin 2:** `Trig` (nhận xung từ chân STM32 **PB8** qua trở $R27=330\Omega$)<br>• **Pin 3:** `Echo` (nối vào cầu phân áp $R11=1\text{k}\Omega / R12=2\text{k}\Omega$ về chân STM32 **PB9**)<br>• **Pin 4:** `GND` |
| **J12** | **L298N-POWER (Cấp nguồn Driver)**| Domino 2P bước 5.08mm | 2P | • **Pin 1:** `12V/8V4` (Cực dương động lực sau nút E-Stop, lọc bởi $C11=470\mu\text{F} + C12=100\text{nF}$)<br>• **Pin 2:** `PGND` (Mass công suất động cơ) |
| **J13** | **L298N-INPUT (Điều khiển Driver)**| Header 1x6P bước 2.54mm | 6P | • **Pin 1:** `ENA/PA0` (PWM Bánh Trái - từ STM32 **PA0** qua trở $R20=330\Omega$, kèm trở kéo $R21=10\text{k}\Omega$ xuống GND)<br>• **Pin 2:** `IN1/PA1` (Chiều 1 Trái - từ STM32 **PA1** qua trở $R19=330\Omega$)<br>• **Pin 3:** `IN2/PA2` (Chiều 2 Trái - từ STM32 **PA2** qua trở $R18=330\Omega$)<br>• **Pin 4:** `IN3/PA4` (Chiều 1 Phải - từ STM32 **PA4** qua trở $R16=330\Omega$)<br>• **Pin 5:** `IN4/PA5` (Chiều 2 Phải - từ STM32 **PA5** qua trở $R15=330\Omega$)<br>• **Pin 6:** `ENB/PA3` (PWM Bánh Phải - từ STM32 **PA3** qua trở $R17=330\Omega$, kèm trở kéo $R22=10\text{k}\Omega$ xuống GND) |
| **J14** | **3S (Cọc nối Khay Pin 3S)** | Domino 2P bước 5.08mm | 2P | • **Pin 1:** `+12.6V` (Dây Đỏ cực dương từ khay pin 3S `BT1, BT2, BT3`)<br>• **Pin 2:** `GND` (Dây Đen cực âm khay pin 3S nối về Star Ground) |
| **J15** | **2S (Cọc nối Khay Pin 2S)** | Domino 2P bước 5.08mm | 2P | • **Pin 1:** `+8.4V` (Dây Đỏ cực dương từ khay pin 2S `BT4, BT5`)<br>• **Pin 2:** `PGND` (Dây Đen cực âm khay pin 2S nối về Mass công suất) |
| **J16** | **OUTPUT (Cọc ra 4 Động cơ TT)** | Domino 4P bước 5.08mm | 4P | • **Pin 1:** `OUT1` $\to$ Cực dương Motor Trái Trước (M1) và Trái Sau (M3)<br>• **Pin 2:** `OUT2` $\to$ Cực âm Motor Trái Trước (M1) và Trái Sau (M3)<br>• **Pin 3:** `OUT3` $\to$ Cực dương Motor Phải Trước (M2) và Phải Sau (M4)<br>• **Pin 4:** `OUT4` $\to$ Cực âm Motor Phải Trước (M2) và Phải Sau (M4) |
| **J17** | **Raspberry_Pi_4 (Giao tiếp Pi 4)**| Header / Cọc hàn cáp Type-C | 6P | • **Pin 2:** `5V` (Nối vào đường `+5V/PI` sau Polyfuse F1 và TVS D1)<br>• **Pin 6:** `GND` (Mass nối về Star Ground)<br>• **Pin 1:** `3V3` (Xuất nguồn `+3.3V/PI` từ Pi sang nuôi OLED J18)<br>• **Pin 3:** `SDA` (Tuyến dữ liệu I2C từ Pi sang OLED J18)<br>• **Pin 5:** `SCL` (Tuyến xung I2C từ Pi sang OLED J18)<br>• **Pin 8, 10:** GPIO14/15 UART (dự phòng) |
| **J18** | **OLED-SCREEN (Màn hình 0.91")** | Header 1x4P bước 2.54mm | 4P | • **Pin 1:** `+3.3V/PI` (Lấy từ Pin 1 của Raspberry Pi 4 qua `J17.1`)<br>• **Pin 2:** `SDA` (Tuyến dữ liệu I2C từ `J17.3`)<br>• **Pin 3:** `SCL` (Tuyến xung nhịp I2C từ `J17.5`)<br>• **Pin 4:** `GND` (Mass hệ thống) |

---

## 3. BẢNG PHÂN BỔ CHÂN VI ĐIỀU KHIỂN STM32F103C8T6 (BLUE PILL - U1)

| Tên chân STM32 | Số chân vật lý | Tên Net trên Schematic | Chế độ vi điều khiển | Chức năng kỹ thuật chi tiết & Mạch đệm phối hợp |
|:---:|:---:|---|---|---|
| **5V** | Pin 18 | `+5V` | Power In | Nhận nguồn $5.15\text{V}$ sạch từ ngõ ra module Buck XL4015 `U3` nuôi LDO 3.3V onboard |
| **GND** | Pin 19 | `GND` | Power Mass | Nối vào mặt phẳng mass chung Star Ground |
| **3V3** | Pin 20 | NC (Không nối) | Power Out | Để ngỏ (không cấp ngược vào hệ thống để tránh xung đột) |
| **PA0** | Pin 10 | `ENA/PA0` | TIM2_CH1 (PWM) | Điều tốc cụm bánh Trái qua trở đệm $R20=330\Omega$, có trở kéo xuống $R21=10\text{k}\Omega$ |
| **PA1** | Pin 11 | `IN1/PA1` | GPIO Output Push-Pull | Đảo chiều quay 1 cụm bánh Trái qua trở đệm $R19=330\Omega$ |
| **PA2** | Pin 12 | `IN2/PA2` | GPIO Output Push-Pull | Đảo chiều quay 2 cụm bánh Trái qua trở đệm $R18=330\Omega$ |
| **PA3** | Pin 13 | `ENB/PA3` | TIM2_CH4 (PWM) | Điều tốc cụm bánh Phải qua trở đệm $R17=330\Omega$, có trở kéo xuống $R22=10\text{k}\Omega$ |
| **PA4** | Pin 14 | `IN3/PA4` | GPIO Output Push-Pull | Đảo chiều quay 1 cụm bánh Phải qua trở đệm $R16=330\Omega$ |
| **PA5** | Pin 15 | `IN4/PA5` | GPIO Output Push-Pull | Đảo chiều quay 2 cụm bánh Phải qua trở đệm $R15=330\Omega$ |
| **PA6** | Pin 16 | `PA6` | ADC1_IN6 (Analog) | Đọc điện áp Pin 3S qua cầu chia áp $R1=10\text{k}\Omega / R2=3.3\text{k}\Omega$ và tụ lọc $C9=100\text{nF}$ |
| **PA7** | Pin 17 | `PA7` | ADC1_IN7 (Analog) | Đọc điện áp Pin 2S/Motor qua cầu chia áp $R13=10\text{k}\Omega / R14=3.3\text{k}\Omega$ và tụ lọc $C10=100\text{nF}$ |
| **PA9** | Pin 30 | `TXD/TX` | USART1_TX (AF-PP) | Truyền dữ liệu nối tiếp tốc độ 115200 baud sang CP2102 qua trở đệm $R28=330\Omega$ |
| **PA10**| Pin 31 | `RXD/RX` | USART1_RX (Input Pull-up)| Nhận dữ liệu lệnh vận tốc từ Pi 4 qua CP2102 qua trở đệm $R29=330\Omega$ |
| **PB5** | Pin 41 | `PB5` | GPIO Output | Lệnh kích còi báo động qua trở $R6=1\text{k}\Omega$ vào Base $Q1 (2N2222)$, có trở xả $R5=10\text{k}\Omega$ |
| **PB6** | Pin 42 | `D0/PB6` | EXTI Line 6 (Ngắt ngoài)| Đếm xung đĩa quang bánh Trái (nhận từ LM393 qua mạch lọc RC $R23=330\Omega + C15=1\text{nF}$) |
| **PB7** | Pin 43 | `D0/PB7` | EXTI Line 7 (Ngắt ngoài)| Đếm xung đĩa quang bánh Phải (nhận từ LM393 qua mạch lọc RC $R24=330\Omega + C16=1\text{nF}$) |
| **PB8** | Pin 45 | `Trig/PB8` | GPIO Output Push-Pull | Phát xung siêu âm góc Phải (US3) qua trở đệm $R27=330\Omega$ |
| **PB9** | Pin 46 | `Echo/PB9` | GPIO Input Floating | Nhận xung phản hồi siêu âm góc Phải từ cầu phân áp $R11=1\text{k}\Omega / R12=2\text{k}\Omega$ |
| **PB12**| Pin 25 | `Trig/PB12` | GPIO Output Push-Pull | Phát xung siêu âm góc Trái (US1) qua trở đệm $R25=330\Omega$ |
| **PB13**| Pin 26 | `Echo/PB13` | GPIO Input Floating | Nhận xung phản hồi siêu âm góc Trái từ cầu phân áp $R7=1\text{k}\Omega / R8=2\text{k}\Omega$ |
| **PB14**| Pin 27 | `Trig/PB14` | GPIO Output Push-Pull | Phát xung siêu âm phía Trước Giữa (US2) qua trở đệm $R26=330\Omega$ |
| **PB15**| Pin 28 | `Echo/PB15` | GPIO Input Floating | Nhận xung phản hồi siêu âm Trước Giữa từ cầu phân áp $R9=1\text{k}\Omega / R10=2\text{k}\Omega$ |

---

## 4. BẢNG PHÂN BỔ KẾT NỐI TRÊN RASPBERRY PI 4 MODEL B

| Cổng kết nối | Định danh trên Pi 4 | Thiết bị kết nối ngoại vi | Vai trò / Giao thức kỹ thuật |
|---|:---:|---|---|
| **Cổng USB Type-C** | Cổng cấp nguồn chính | Cáp từ cọc `J17` (`+5V/PI` & `GND`) | Nhận điện áp $5.15\text{V}$ ổn định từ sau Polyfuse F1 và TVS D1 |
| **Cổng USB 2.0 (Đen 1)** | `/dev/video0` | Webcam Logitech C270 HD | Luồng video MJPEG $640 \times 480$ tốc độ 30 FPS quét mã ArUco |
| **Cổng USB 2.0 (Đen 2)** | `/dev/ttyUSB0` | Mạch nạp/giao tiếp USB-TTL CP2102 | Cấp nguồn USB cho module CP2102 và truyền nhận gói tin nhị phân với STM32 |
| **Header Pin 1** | `3V3 Power` | Chân `VCC` màn hình OLED (`J18.1`) | Cấp nguồn $3.3\text{V}$ sạch từ LDO của Pi sang nuôi màn hình OLED |
| **Header Pin 3** | `GPIO 2 (SDA_I2C1)`| Chân `SDA` màn hình OLED (`J18.2`) | Đường truyền dữ liệu chuẩn I2C hiển thị thông số địa chỉ IP và trạng thái xe |
| **Header Pin 5** | `GPIO 3 (SCL_I2C1)`| Chân `SCL` màn hình OLED (`J18.3`) | Đường xung nhịp đồng bộ I2C tần số 400kHz |
| **Header Pin 6/9** | `Ground (GND)` | Chân `GND` màn hình OLED (`J18.4`) | Mass nối về mặt phẳng Star Ground |

---

## 5. BẢNG NETLIST TOÀN BỘ MẠCH IN PCB (CHI TIẾT TỪNG NET)

| Tên Net trên Mạch | Điện áp danh định | Danh sách các chân linh kiện kết nối vào Net | Đặc tính layout mạch in (PCB Design Rules) |
|---|:---:|---|---|
| **`+12.6V`** | $9.0\text{V} - 12.6\text{V}$ | • `J14.1` (Cực dương cọc pin 3S)<br>• `J3.1` (Chân vào Công tắc A) | Độ rộng đường đồng trace $\ge 1.5\text{mm}$, dòng định mức 3A |
| **`3S_V`** | $9.0\text{V} - 12.6\text{V}$ | • `J3.2` (Chân ra Công tắc A)<br>• `U3.IN+` (Chân vào Buck XL4015)<br>• `C13.(+)` (Tụ hóa 100uF)<br>• `C14.1` (Tụ gốm 100nF)<br>• `R1.1` (Chân 1 trở phân áp 10k đo áp pin 3S) | Độ rộng đường đồng $\ge 1.5\text{mm}$, bố trí tụ C13/C14 sát chân IN+ |
| **`+8.4V`** | $6.0\text{V} - 8.4\text{V}$ | • `J15.1` (Cực dương cọc pin 2S)<br>• `J4.1` (Chân vào Công tắc B) | Độ rộng đường đồng $\ge 2.5\text{mm}$, dòng đỉnh 5A |
| **`Net-(J4-Pad2)`** | $6.0\text{V} - 8.4\text{V}$ | • `J4.2` (Chân ra Công tắc B)<br>• `J5.1` (Chân vào nút dừng khẩn cấp E-Stop) | Độ rộng đường đồng $\ge 2.5\text{mm}$ |
| **`12V/8V4`** | $6.0\text{V} - 8.4\text{V}$ | • `J5.2` (Chân ra nút E-Stop)<br>• `J12.1` (Chân dương cấp Driver L298N)<br>• `C11.(+)` (Tụ hóa lớn 470uF)<br>• `C12.1` (Tụ gốm 100nF)<br>• `R4.1` (Trở hạn dòng LED báo nguồn Motor)<br>• `R13.1` (Trở phân áp 10k đo áp pin 2S/Motor) | Độ rộng đường đồng $\ge 3.0\text{mm}$ (hoặc phủ đồng riêng), cạo mass phủ chì hàn tăng cường |
| **`+5V`** | $5.15\text{V}$ | • `U3.OUT+` (Chân ra Buck XL4015)<br>• `F1.1` (Chân vào Cầu chì Polyfuse 3A)<br>• `U1.Pin18` (Chân 5V bo Blue Pill)<br>• `J1.1`, `J2.1` (VCC 2 Encoder LM393)<br>• `J9.1`, `J10.1`, `J11.1` (VCC 3 Siêu âm) + $C3, C5, C7.(+)$ + $C4, C6, C8.1$<br>• `BZ1.(+)` (Cực dương còi)<br>• `D2.Cathode` (Diode dập xung còi)<br>• `R3.1` (Trở LED báo động)<br>• `J7.1` (VCC cổng nạp/UART CP2102) | Phủ đồng Copper Pour toàn lớp mặt nguồn Logic, độ rộng tối thiểu $\ge 2.0\text{mm}$ |
| **`+5V/PI`** | $5.15\text{V}$ (bảo vệ) | • `F1.2` (Chân ra Cầu chì Polyfuse 3A)<br>• `D1.Cathode` (Diode TVS SMAJ6.0A hoặc 1.5KE6.8A)<br>• `C1.(+)` (Cực dương tụ hóa $2200\mu\text{F}$)<br>• `C2.1` (Tụ gốm 100nF)<br>• `J17.2` (Cọc 5V cấp cáp USB Type-C sang Pi 4) | Đường đồng $\ge 2.5\text{mm}$, đặt C1, C2 và TVS D1 sát cạnh chân ra J17 |
| **`+3.3V/PI`** | $3.3\text{V}$ | • `J17.1` (Chân lấy nguồn 3.3V từ Pi 4)<br>• `J18.1` (Chân VCC cấp màn hình OLED 0.91") | Đường mạch tín hiệu $0.4\text{mm}$ |
| **`PA6`** | $0 - 3.13\text{V}$ | • `R1.2` (Chân 2 trở 10k)<br>• `R2.1` (Chân 1 trở 3.3k)<br>• `C9.1` (Tụ gốm 100nF)<br>• `U1.PA6` (Chân ADC1_IN6 STM32) | Đường analog $0.3\text{mm}$, chạy cách ly xa đường công suất L298N |
| **`PA7`** | $0 - 2.08\text{V}$ | • `R13.2` (Chân 2 trở 10k)<br>• `R14.1` (Chân 1 trở 3.3k)<br>• `C10.1` (Tụ gốm 100nF)<br>• `U1.PA7` (Chân ADC1_IN7 STM32) | Đường analog $0.3\text{mm}$, bọc mass bảo vệ tín hiệu |
| **`Net-(D4-Pad2)`** | $\approx 2.0\text{V}$ | • `D4.Anode` $\longleftrightarrow$ `R4.2` | Đường mạch hiển thị LED $0.4\text{mm}$ |
| **`Net-(BZ1-Pad2)`** | $0 - 5\text{V}$ | • `BZ1.(-)` (Cực âm còi)<br>• `D2.Anode` (Diode 1N4148)<br>• `D3.Cathode` (LED báo động)<br>• `Q1.Collector` (Transistor 2N2222) | Đường mạch công suất còi $0.5\text{mm}$ |
| **`Net-(D3-Pad2)`** | $\approx 2.0\text{V}$ | • `D3.Anode` $\longleftrightarrow$ `R3.2` | Đường mạch tín hiệu $0.4\text{mm}$ |
| **`PB5`** | $0 / 3.3\text{V}$ | • `U1.PB5` (STM32 Output)<br>• `R6.1` (Chân 1 trở đệm $1\text{k}\Omega$) | Đường logic $0.3\text{mm}$ |
| **`Net-(Q1-Pad1)`** | $0 - 0.7\text{V}$ | • `R6.2` (Chân ra trở đệm 1k)<br>• `R5.1` (Trở xả 10k)<br>• `Q1.Base` (Transistor 2N2222) | Đặt sát chân transistor Q1 |
| **`D0/PB6`** | $0 / 5\text{V}$ | • `J1.3` (Tín hiệu D0 Encoder Trái)<br>• `R23.1` (Trở lọc $330\Omega$) | Đường tín hiệu $0.3\text{mm}$ |
| **`PB6`** | $0 / 5\text{V}$ (lọc) | • `R23.2`<br>• `C15.1` (Tụ lọc 1nF)<br>• `U1.PB6` (Chân EXTI6 STM32) | Đường ngắt ngoài $0.3\text{mm}$, bọc mass chống nhiễu chổi than |
| **`D0/PB7`** | $0 / 5\text{V}$ | • `J2.3` (Tín hiệu D0 Encoder Phải)<br>• `R24.1` (Trở lọc $330\Omega$) | Đường tín hiệu $0.3\text{mm}$ |
| **`PB7`** | $0 / 5\text{V}$ (lọc) | • `R24.2`<br>• `C16.1` (Tụ lọc 1nF)<br>• `U1.PB7` (Chân EXTI7 STM32) | Đường ngắt ngoài $0.3\text{mm}$, bọc mass chống nhiễu chổi than |
| **`Trig/PB12`** | $0 / 3.3\text{V}$ | • `U1.PB12`<br>• `R25.1` (Trở $330\Omega$) | Đường phát xung siêu âm $0.3\text{mm}$ |
| **`Echo/PB13`** | $0 / 3.33\text{V}$ | • `R7.2` (Chân 2 trở 1k)<br>• `R8.1` (Chân 1 trở 2k)<br>• `U1.PB13` (STM32 Input) | Đường nhận xung phản hồi $0.3\text{mm}$ |
| **`Trig/PB14`** | $0 / 3.3\text{V}$ | • `U1.PB14`<br>• `R26.1` (Trở $330\Omega$) | Đường phát xung siêu âm $0.3\text{mm}$ |
| **`Echo/PB15`** | $0 / 3.33\text{V}$ | • `R9.2` (Chân 2 trở 1k)<br>• `R10.1` (Chân 1 trở 2k)<br>• `U1.PB15` (STM32 Input) | Đường nhận xung phản hồi $0.3\text{mm}$ |
| **`Trig/PB8`** | $0 / 3.3\text{V}$ | • `U1.PB8`<br>• `R27.1` (Trở $330\Omega$) | Đường phát xung siêu âm $0.3\text{mm}$ |
| **`Echo/PB9`** | $0 / 3.33\text{V}$ | • `R11.2` (Chân 2 trở 1k)<br>• `R12.1` (Chân 1 trở 2k)<br>• `U1.PB9` (STM32 Input) | Đường nhận xung phản hồi $0.3\text{mm}$ |
| **`ENA/PA0`** | $0 / 3.3\text{V}$ | • `U1.PA0`<br>• `R20.1` (Trở $330\Omega$) | Tuyến điều tốc PWM Trái |
| **`IN1/PA1`** | $0 / 3.3\text{V}$ | • `U1.PA1`<br>• `R19.1` (Trở $330\Omega$) | Tuyến đảo chiều Motor Trái |
| **`IN2/PA2`** | $0 / 3.3\text{V}$ | • `U1.PA2`<br>• `R18.1` (Trở $330\Omega$) | Tuyến đảo chiều Motor Trái |
| **`ENB/PA3`** | $0 / 3.3\text{V}$ | • `U1.PA3`<br>• `R17.1` (Trở $330\Omega$) | Tuyến điều tốc PWM Phải |
| **`IN3/PA4`** | $0 / 3.3\text{V}$ | • `U1.PA4`<br>• `R16.1` (Trở $330\Omega$) | Tuyến đảo chiều Motor Phải |
| **`IN4/PA5`** | $0 / 3.3\text{V}$ | • `U1.PA5`<br>• `R15.1` (Trở $330\Omega$) | Tuyến đảo chiều Motor Phải |
| **`TXD/TX`** | $0 / 3.3\text{V}$ | • `U1.PA9`<br>• `R28.1` (Trở đệm $330\Omega$) | Đường UART TX1 sang CP2102 |
| **`RXD/RX`** | $0 / 3.3\text{V}$ | • `U1.PA10`<br>• `R29.1` (Trở đệm $330\Omega$) | Đường UART RX1 từ CP2102 |
| **`SDA`** | $0 / 3.3\text{V}$ | • `J17.3` (SDA Raspberry Pi 4)<br>• `J18.2` (SDA màn hình OLED) | Tuyến I2C Bus Data |
| **`SCL`** | $0 / 3.3\text{V}$ | • `J17.5` (SCL Raspberry Pi 4)<br>• `J18.3` (SCL màn hình OLED) | Tuyến I2C Bus Clock |
| **`OUT1` ÷ `OUT4`**| $0 - 8.4\text{V}$ | • `J16.1` $\to$ Cụm Motor M1, M3 (+)<br>• `J16.2` $\to$ Cụm Motor M1, M3 (-)<br>• `J16.3` $\to$ Cụm Motor M2, M4 (+)<br>• `J16.4` $\to$ Cụm Motor M2, M4 (-) | Dây công suất motor $\ge 1.5\text{mm}^2$, vặn chặt cọc domino |
| **`GND`** | $0\text{V}$ (Logic) | • `J14.2` (Cực âm Pin 3S)<br>• `U3.IN-` & `U3.OUT-`<br>• `D1.Anode` (TVS)<br>• `C1, C2, C3, C4, C5, C6, C7, C8, C9, C13, C14, C15, C16.(-)`<br>• `R2.2` (Chân âm trở đo áp 3S)<br>• `R8.2, R10.2, R12.2` (Chân âm trở chia áp Echo)<br>• `R5.2` (Trở xả Base Q1) & `Q1.Emitter`<br>• `R21.2, R22.2` (Trở kéo xuống ENA, ENB)<br>• `U1.Pin19` (GND STM32)<br>• `J1.2, J2.2` (GND Encoders)<br>• `J7.4` (GND CP2102)<br>• `J9.4, J10.4, J11.4` (GND Siêu âm)<br>• `J17.6` (GND Raspberry Pi 4)<br>• `J18.4` (GND Màn hình OLED)<br>• `NetTie_2.1` (Chân 1 cầu chập mass Net-Tie) | **ĐỔ ĐỒNG TOÀN BỘ MẶT ĐÁY PCB (GND POLYGON PLANE)** |
| **`PGND`** | $0\text{V}$ (Power) | • `J15.2` (Cực âm Pin 2S)<br>• `J12.2` (Cực âm nguồn Driver L298N)<br>• `C11, C12.(-)` (Tụ lọc nguồn động cơ)<br>• `D4.Cathode` (LED báo nguồn Motor)<br>• `R14.2` (Chân âm trở đo áp pin 2S/Motor)<br>• `NetTie_2.2` (Chân 2 cầu chập mass Net-Tie) | **ĐỔ ĐỒNG VÙNG CÔNG SUẤT RIÊNG (PGND COPPER POUR)** |

---

## 6. SƠ ĐỒ NGUYÊN LÝ KHỐI TOÀN HỆ THỐNG DẠNG NỐI DÂY (ASCII WIRING DIAGRAM)

```
======================================================================================================================================================
                                                SƠ ĐỒ ĐI DÂY & PHỐI GHÉP TÍN HIỆU TOÀN DIỆN AGV
======================================================================================================================================================

 [ KHAY PIN 3S (11.1V - 12.6V) ]
   Dây Đỏ (+) ──► J14.1 ──► [ J3: Switch A ] ──┬──► Cụm tụ [ C13: 100uF // C14: 100nF ] ──► IN+ [ U3: MODULE XL4015 ] OUT+ (5.15V DC)
                                               │                                                 |             |
                                               └──► [ R1: 10k ] ──┬──► STM32 PA6 (ADC 3S)        IN-           OUT-
                                                                  │                               │             │
                                                    [ C9: 100nF // R2: 3.3k ]                     │             │
                                                                  │                               │             │
   Dây Đen (-) ──► J14.2 (GND) ───────────────────────────────────┴───────────────────────────────┴─────────────┼───┐
                                                                                                                │   │
   ┌─────────────────────────────────────────────────── ĐƯỜNG NGUỒN SẠCH +5V ───────────────────────────────────┘   │
   │                                                                                                                │
   ├──► [ F1: Polyfuse 3A ] ──► Node +5V/PI ──┬──► [ J17.2: Type-C Pi 4 ]                                           │
   │                                          ├──► [ D1: TVS SMAJ6.0A hoặc 1.5KE6.8A ] ──► GND (Crowbar OVP)        │
   │                                          └──► [ C1: 2200uF Low ESR // C2: 100nF ] ──► GND (Chống sụt áp Pi 4)  │
   │                                                                                                                │
   ├──► STM32 Blue Pill (Pin 18: 5V)                                                                                │
   ├──► 2x Encoder LM393 (J1.1, J2.1: VCC 5V)                                                                       │
   ├──► 3x Cảm biến HC-SR04 (J9.1, J10.1, J11.1: VCC 5V) ── Kèm tụ riêng [ 100uF // 100nF ] từng con                │
   ├──► Mạch nạp CP2102 (J7.1: 5V)                                                                                  │
   └──► Còi chip BZ1 (5V) & LED D3 ──► Transistor Q1 (2N2222) ◄── [ R6: 1k ] ◄── STM32 PB5                          │
                                                                                                                    │
 [ KHAY PIN 2S (7.4V - 8.4V) ]                                                                                      │
   Dây Đỏ (+) ──► J15.1 ──► [ J4: Switch B ] ──► [ J5: Nút E-Stop ] ──┬──► Cụm tụ [ C11: 470uF // C12: 100nF ] ──► J12.1 (VM L298N)
                                                                      ├──► [ R4: 1k ] ──► [ D4: LED Đỏ ] ──► PGND   │
                                                                      └──► [ R13: 10k ] ──┬──► STM32 PA7 (ADC Motor)│
                                                                                          │                         │
                                                                           [ C10: 100nF // R14: 3.3k ]              │
                                                                                          │                         │
   Dây Đen (-) ──► J15.2 (PGND) ──────────────────────────────────────────────────────────┴─────────────────────────┤
                                                                                                                    │
                                                      [ ĐIỂM CHẬP MASS DUY NHẤT: NetTie_2 ]                         │
                                                        GND (Logic)  ═════════  PGND (Motor) ◄──────────────────────┘

------------------------------------------------------------------------------------------------------------------------------------------------------
 [ ĐƯỜNG TRUYỀN TÍN HIỆU ĐIỀU KHIỂN & CẢM BIẾN ]
   • Bánh Trái:  STM32 PA0 (TIM2_CH1) ──► [ R20: 330Ω ] ──► J13.1 (ENA) ── [ R21: 10k pull-down ]
                 STM32 PA1 (GPIO Out)  ──► [ R19: 330Ω ] ──► J13.2 (IN1)
                 STM32 PA2 (GPIO Out)  ──► [ R18: 330Ω ] ──► J13.3 (IN2) ──► Cầu H L298N (J16.1,2: OUT1,2) ──► Động cơ TT Trái (M1, M3)
   • Bánh Phải:  STM32 PA3 (TIM2_CH4) ──► [ R17: 330Ω ] ──► J13.6 (ENB) ── [ R22: 10k pull-down ]
                 STM32 PA4 (GPIO Out)  ──► [ R16: 330Ω ] ──► J13.4 (IN3)
                 STM32 PA5 (GPIO Out)  ──► [ R15: 330Ω ] ──► J13.5 (IN4) ──► Cầu H L298N (J16.3,4: OUT3,4) ──► Động cơ TT Phải (M2, M4)

   • Đĩa quang:  Encoder Trái D0 ──► [ R23: 330Ω ] ──┬──► STM32 PB6 (EXTI6 ngắt ngoài đếm xung bánh Trái)
                                                     └──► [ C15: 1nF ] ──► GND (Lọc tia lửa chổi than)
                 Encoder Phải D0 ──► [ R24: 330Ω ] ──┬──► STM32 PB7 (EXTI7 ngắt ngoài đếm xung bánh Phải)
                                                     └──► [ C16: 1nF ] ──► GND (Lọc tia lửa chổi than)

   • Siêu âm:    STM32 PB12 (Trig Trái)  ──► [ R25: 330Ω ] ──► J9.2 (US1 Trig)
                 US1 Echo (5V)           ──► [ R7: 1k ] ──┬──► STM32 PB13 (3.33V an toàn)
                                                          └──► [ R8: 2k ] ──► GND
                 STM32 PB14 (Trig Giữa)  ──► [ R26: 330Ω ] ──► J10.2 (US2 Trig)
                 US2 Echo (5V)           ──► [ R9: 1k ] ──┬──► STM32 PB15 (3.33V an toàn)
                                                          └──► [ R10: 2k ] ──► GND
                 STM32 PB8  (Trig Phải)  ──► [ R27: 330Ω ] ──► J11.2 (US3 Trig)
                 US3 Echo (5V)           ──► [ R11: 1k ] ─┬──► STM32 PB9 (3.33V an toàn)
                                                          └──► [ R12: 2k ] ──► GND

   • Giao tiếp:  STM32 PA9  (USART1_TX)  ──► [ R28: 330Ω ] ──► J7.2 (TXD CP2102) ──► Cáp USB ──► Cổng USB Pi 4 (/dev/ttyUSB0)
                 STM32 PA10 (USART1_RX)  ◄── [ R29: 330Ω ] ◄── J7.3 (RXD CP2102) ◄── Cáp USB ◄── Cổng USB Pi 4
   • Màn hình:   Raspberry Pi 4 Header (Pin 1: 3.3V, Pin 3: SDA, Pin 5: SCL, Pin 9: GND) ────────► J18 (Màn hình OLED 0.91" I2C)
======================================================================================================================================================
```
