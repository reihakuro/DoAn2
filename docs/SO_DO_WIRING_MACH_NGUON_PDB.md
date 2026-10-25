# SƠ ĐỒ WIRING, NETLIST & BẢNG PHÂN BỔ CHÂN (PINOUT) HỆ THỐNG AGV PIN 4S (REV 3.0)
## TÀI LIỆU KỸ THUẬT PHỤC VỤ THI CÔNG, KIỂM TRA ĐI DÂY & PHÂN TÍCH SCHEMATIC
**Chuẩn hóa 100% theo bản vẽ KiCad Pin 4S ($14.8\text{V} - 16.8\text{V}$) - Đồ án 2 HCM-UTE**

---

## 1. TỔNG QUAN KIẾN TRÚC PHẦN CỨNG 4S & CẢI TIẾN TRÊN SCHEMATIC

Bản vẽ nguyên lý chuẩn hóa `power-source-board.kicad_sch` (Rev 3.0) tích hợp toàn bộ các khối chức năng từ phân phối nguồn tập trung 4S, chống brownout, mạch điều khiển vi sai động cơ, đọc cảm biến và bảo vệ quá áp:
1. **Kiến trúc nguồn 1 khối Pin 4S đồng bộ ($14.8\text{V} - 16.8\text{V}$):**
   * Khay 4S (`BT1 ÷ BT4` nối tiếp) $\to$ Cọc `J14` $\to$ Khối chốt nguồn P-MOSFET IRF9540 `Q2` $\to$ Đường nguồn chính `4S_V`.
   * **Nhánh Logic:** `4S_V` $\to$ Lọc nguồn `C13 (100uF) + C14 (100nF)` $\to$ Module Buck XL4015 `U3` hạ áp ra `+5V` sạch.
   * **Nhánh Động lực:** `4S_V` $\to$ Nút dừng khẩn cấp E-Stop `J5` $\to$ Đường nguồn `16V/E-STOP` $\to$ Cọc `J12` cấp thẳng Driver L298N.
2. **Khối Chốt Nguồn Thông Minh (Smart Power Latch) & Tự Ngắt Tuyệt Đối ($0\,\mu\text{A}$):**
   * **Khởi động:** Nhấn nút nhấn mềm `SW1` (hoặc cọc `J19` ngoài vỏ xe) $\to$ Gate kéo xuống GND $\to$ P-MOSFET mở cấp nguồn 4S vào Buck XL4015. STM32 boot và kích chân **PB0 (`POWER_HOLD`)** lên mức HIGH để tự khóa giữ nguồn.
   * **Bảo vệ Gate an toàn:** Cầu phân áp Gate $R_{30} = 10\text{k}\Omega, R_{35} = 10\text{k}\Omega$ kết hợp **Diode Zener 12V `DZ1`** kẹp điện áp $V_{GS}$ luôn ở mức $-8.4\text{V}$ an toàn, chống sốc nổ Gate.
   * **Tự ngắt an toàn:** Nhận biết Raspberry Pi 4 đã hoàn tất shutdown (Halt) qua chân **PB10 (`PI_HALT_DET`)** kết nối với tín hiệu Kernel `dtoverlay=gpio-poweroff` (GPIO 26 của Pi 4). Khi Pi 4 đã Halt (hoặc pin 4S cạn $<12.0\text{V}$), STM32 hạ chân **PB0** về LOW $\to$ P-MOSFET đóng lại, **cắt 100% điện áp toàn xe (dòng tiêu thụ về $0\mu\text{A}$)**.
3. **Mạch bảo vệ ngõ ra Raspberry Pi 4 (Crowbar OVP):**
   * Đường nguồn từ `+5V` đi qua cầu chì tự phục hồi `F1 (Polyfuse 3A)` để tạo thành đường `+5V/PI`.
   * Tại node `+5V/PI` mắc song song:
     * **Diode TVS `D1`:** SMAJ6.0A (SMD) hoặc 1.5KE6.8A (DIP) triệt xung quá áp.
     * **Cụm tụ bù sụt áp:** Tụ hóa Low-ESR cực lớn `C1 (2200uF/16V)` và tụ gốm `C2 (100nF)` triệt tiêu hoàn toàn cảnh báo sụt áp khi Pi 4 boost CPU.
4. **Giám sát điện áp Pin 4S (ADC PA6):**
   * Cầu phân áp đo dung lượng pin 4S: $R_1 (18\text{k}\Omega) + R_2 (3.3\text{k}\Omega) + C_9 (100\text{nF})$ đưa về chân **PA6**. Điện áp tối đa $2.60\text{V} < 3.3\text{V}$ bảo vệ an toàn chân ADC.
   * Giải phóng chân **PA7** (không còn dùng cho pin 2S) làm chân chức năng mở rộng dự phòng.
5. **Điều tốc động cơ bằng Software-PWM:**
   * Cấp trực tiếp $14.8\text{V} - 16.8\text{V}$ vào Driver L298N. STM32 khống chế xung PWM tối đa $\le 55\%$ (điện áp hiệu dụng $\approx 8.1\text{V}$). Không cần thêm mạch hạ áp phần cứng.
6. **Kiến trúc Chia 2 Miền Mass Tách Biệt (Split Ground với Star-Point NetTie_2):**
   * **TUYỆT ĐỐI GIỮ NGUYÊN `NetTie_2` (`NT1`)**: Hệ thống phân tách thành 2 miền mass độc lập:
     - **Miền Mass Logic (`GND`)**: Bo STM32 Blue Pill, Raspberry Pi 4, Module Buck XL4015, các cảm biến siêu âm, encoder và cầu phân áp ADC PA6.
     - **Miền Mass Động Lực (`PGND`)**: Cọc cấp nguồn Driver L298N (`J12.2`), tụ bù sụt áp $C_{11} (470\mu\text{F})$, tụ gốm $C_{12} (100\text{nF})$, LED báo nguồn động cơ $D_4$.
     - Hai miền mass này chỉ nối với nhau tại **DUY NHẤT 1 ĐIỂM** qua linh kiện `NT1` (`NetTie_2`) đặt sát cọc âm pin 4S `J14.2`. Thiết kế này ngăn chặn 100% dòng xung cảm ứng $\frac{di}{dt}$ từ động cơ chạy qua bo mạch điều khiển gây sụt áp hoặc treo chip.

---

## 2. BẢNG PHÂN BỔ CHÂN VI ĐIỀU KHIỂN STM32F103C8T6 (BLUE PILL - U1)

| Tên chân STM32 | Số chân vật lý | Tên Net trên Schematic | Chế độ vi điều khiển | Chức năng kỹ thuật chi tiết & Mạch đệm phối hợp |
|:---:|:---:|---|---|---|
| **5V** | Pin 18 | `+5V` | Power In | Nhận nguồn $5.15\text{V}$ sạch từ ngõ ra module Buck XL4015 `U3` nuôi LDO 3.3V onboard |
| **GND** | Pin 19 | `GND` | Power Mass | Nối vào mặt phẳng mass chung Star Ground |
| **3V3** | Pin 20 | NC (Không nối) | Power Out | Để ngỏ (không cấp ngược vào hệ thống để tránh xung đột) |
| **PA0** | Pin 10 | `ENA/PA0` | TIM2_CH1 (PWM) | Điều tốc PWM bánh Trái qua trở đệm $R_{20}=330\Omega$, có trở kéo xuống $R_{21}=10\text{k}\Omega$ |
| **PA1** | Pin 11 | `IN1/PA1` | GPIO Output Push-Pull | Đảo chiều quay 1 cụm bánh Trái qua trở đệm $R_{19}=330\Omega$ |
| **PA2** | Pin 12 | `IN2/PA2` | GPIO Output Push-Pull | Đảo chiều quay 2 cụm bánh Trái qua trở đệm $R_{18}=330\Omega$ |
| **PA3** | Pin 13 | `ENB/PA3` | TIM2_CH4 (PWM) | Điều tốc PWM bánh Phải qua trở đệm $R_{17}=330\Omega$, có trở kéo xuống $R_{22}=10\text{k}\Omega$ |
| **PA4** | Pin 14 | `IN3/PA4` | GPIO Output Push-Pull | Đảo chiều quay 1 cụm bánh Phải qua trở đệm $R_{16}=330\Omega$ |
| **PA5** | Pin 15 | `IN4/PA5` | GPIO Output Push-Pull | Đảo chiều quay 2 cụm bánh Phải qua trở đệm $R_{15}=330\Omega$ |
| **PA6** | Pin 16 | `PA6` | ADC1_IN6 (Analog) | **Đo điện áp khối Pin 4S** qua cầu chia áp $R_1=18\text{k}\Omega / R_2=3.3\text{k}\Omega$ và tụ lọc $C_9=100\text{nF}$ |
| **PA7** | Pin 17 | NC (Dự phòng) | Floating | **Giải phóng tự do** (trước đây đo pin 2S, nay để ngỏ làm chân dự phòng mở rộng) |
| **PB0** | Pin 33 | `POWER_HOLD` | GPIO Output Push-Pull | **Chốt giữ nguồn xe:** Xuất mức 1 kích Base Transistor Q3 (S8050/2N2222) qua trở đệm $R_{30}=1\text{k}\Omega$, có trở xả mass $R_{31}=10\text{k}\Omega$ |
| **PA8** | Pin 29 | NC (Dự phòng) | Floating | **Giải phóng tự do** (trước đây làm POWER_HOLD, nay chuyển sang PB0) |
| **PA9** | Pin 30 | `TXD/TX` | USART1_TX (AF-PP) | Truyền dữ liệu Serial tốc độ 115200 baud sang CP2102 qua trở đệm $R_{28}=330\Omega$ |
| **PA10**| Pin 31 | `RXD/RX` | USART1_RX (Input Pull-up)| Nhận lệnh vận tốc từ Pi 4 qua CP2102 qua trở đệm $R_{29}=330\Omega$ |
| **PB1** | Pin 34 | `PWR_BTN_DET` | GPIO Input Pull-up | **Nhận diện nút bấm tắt:** Nối vào Anot Diode D5 (Catot nối nút mềm SW1), nhấn giữ $>1.5\text{s}$ để shutdown |
| **PB5** | Pin 13 | `PB5` | GPIO Output | Lệnh kích còi báo động qua trở $R_6=1\text{k}\Omega$ vào Base $Q_1 (S8050/2N2222)$, có trở xả $R_5=10\text{k}\Omega$ |
| **PB6** | Pin 42 | `D0/PB6` | EXTI Line 6 (Ngắt ngoài)| Đếm xung đĩa quang bánh Trái (nhận từ LM393 qua mạch lọc RC $R_{23}=330\Omega + C_{15}=1\text{nF}$) |
| **PB7** | Pin 43 | `D0/PB7` | EXTI Line 7 (Ngắt ngoài)| Đếm xung đĩa quang bánh Phải (nhận từ LM393 qua mạch lọc RC $R_{24}=330\Omega + C_{16}=1\text{nF}$) |
| **PB8** | Pin 45 | `Trig/PB8` | GPIO Output Push-Pull | Phát xung siêu âm góc Phải (US3) qua trở đệm $R_{27}=330\Omega$ |
| **PB9** | Pin 46 | `Echo/PB9` | GPIO Input Floating | Nhận xung phản hồi siêu âm góc Phải từ cầu phân áp $R_{11}=1\text{k}\Omega / R_{12}=2\text{k}\Omega$ |
| **PB10**| Pin 35 | `PI_HALT_DET` | GPIO Input Floating/Pull-down | **Nhận tín hiệu Pi 4 đã Halt:** Đọc từ Pi 4 GPIO 26 (cọc J3) qua trở đệm $R_{32}=1\text{k}\Omega$ và trở xả $R_{33}=10\text{k}\Omega$ |
| **PB12**| Pin 25 | `Trig/PB12` | GPIO Output Push-Pull | Phát xung siêu âm góc Trái (US1) qua trở đệm $R_{25}=330\Omega$ |
| **PB13**| Pin 26 | `Echo/PB13` | GPIO Input Floating | Nhận xung phản hồi siêu âm góc Trái từ cầu phân áp $R_7=1\text{k}\Omega / R_8=2\text{k}\Omega$ |
| **PB14**| Pin 27 | `Trig/PB14` | GPIO Output Push-Pull | Phát xung siêu âm Trước Giữa (US2) qua trở đệm $R_{26}=330\Omega$ |
| **PB15**| Pin 28 | `Echo/PB15` | GPIO Input Floating | Nhận xung phản hồi siêu âm Trước Giữa từ cầu phân áp $R_9=1\text{k}\Omega / R_{10}=2\text{k}\Omega$ |

---

## 3. BẢNG CHI TIẾT 15 ĐẦU NỐI VÀ CONNECTOR (J1 ĐẾN J19)

| Kí hiệu | Tên cổng / Khối chức năng | Kiểu chân (Footprint) | Số chân | Chi tiết định tuyến từng chân (Pin-to-Pin Assignment) |
|:---:|---|:---:|:---:|---|
| **J1** | Encoder LM393 Trái | Header 1x3P bước 2.54mm | 3P | • **Pin 1:** `+5V`<br>• **Pin 2:** `GND`<br>• **Pin 3:** `D0` (qua trở $R_{23}=330\Omega$, tụ $C_{15}=1\text{nF}$ vào STM32 **PB6**) |
| **J2** | Encoder LM393 Phải | Header 1x3P bước 2.54mm | 3P | • **Pin 1:** `+5V`<br>• **Pin 2:** `GND`<br>• **Pin 3:** `D0` (qua trở $R_{24}=330\Omega$, tụ $C_{16}=1\text{nF}$ vào STM32 **PB7**) |
| **J5** | Cọc Nút Dừng E-Stop | KF301-2P hoặc Header 1x2P | 2P | • **Pin 1:** Nhận điện áp `4S_V` sau P-MOSFET Q2<br>• **Pin 2:** Xuất điện áp động lực `16V/E-STOP` sang cọc cấp nguồn L298N `J12.1` |
| **J6, J8**| Socket Module Buck XL4015 | Header cái 1x2P bước 2.54mm | 4P | • **J6/J8 In Pin 1:** Nhận `4S_V` từ Drain P-MOSFET Q2<br>• **J6/J8 In Pin 2:** Nối `GND`<br>• **J6/J8 Out Pin 1:** Xuất nguồn hạ áp sạch `+5V` (chỉnh 5.15V)<br>• **J6/J8 Out Pin 2:** Nối `GND` |
| **J7** | Header cắm Module CP2102 | Header 1x4P bước 2.54mm | 4P | • **Pin 1:** `+5V`<br>• **Pin 2:** TXD (qua trở đệm $R_{28}=330\Omega$ vào STM32 **PA9 - TX1**)<br>• **Pin 3:** RXD (qua trở đệm $R_{29}=330\Omega$ vào STM32 **PA10 - RX1**)<br>• **Pin 4:** `GND` |
| **J9** | Siêu âm Trái (US1) | Header 1x4P bước 2.54mm | 4P | • **Pin 1:** `+5V` (Lọc $C_3+C_4$)<br>• **Pin 2:** `Trig` (từ STM32 **PB12** qua trở $R_{25}=330\Omega$)<br>• **Pin 3:** `Echo` (qua cầu trở $R_7=1\text{k}\Omega / R_8=2\text{k}\Omega$ về STM32 **PB13**)<br>• **Pin 4:** `GND` |
| **J10** | Siêu âm Giữa (US2) | Header 1x4P bước 2.54mm | 4P | • **Pin 1:** `+5V` (Lọc $C_5+C_6$)<br>• **Pin 2:** `Trig` (từ STM32 **PB14** qua trở $R_{26}=330\Omega$)<br>• **Pin 3:** `Echo` (qua cầu trở $R_9=1\text{k}\Omega / R_{10}=2\text{k}\Omega$ về STM32 **PB15**)<br>• **Pin 4:** `GND` |
| **J11** | Siêu âm Phải (US3) | Header 1x4P bước 2.54mm | 4P | • **Pin 1:** `+5V` (Lọc $C_7+C_8$)<br>• **Pin 2:** `Trig` (từ STM32 **PB8** qua trở $R_{27}=330\Omega$)<br>• **Pin 3:** `Echo` (qua cầu trở $R_{11}=1\text{k}\Omega / R_{12}=2\text{k}\Omega$ về STM32 **PB9**)<br>• **Pin 4:** `GND` |
| **J12** | Cấp nguồn Driver L298N | Domino 2P bước 5.08mm | 2P | • **Pin 1:** `16V/E-STOP` (Cực dương động lực sau nút E-Stop, lọc $C_{11}+C_{12}$)<br>• **Pin 2:** `PGND` (Mass động lực Driver L298N, nối tụ C11, C12 và qua NT1 về cọc pin J14.2) |
| **J13** | Điều khiển Driver L298N | Header 1x6P bước 2.54mm | 6P | • **Pin 1:** `ENA/PA0` (PWM Bánh Trái - STM32 **PA0**)<br>• **Pin 2:** `IN1/PA1` (Chiều 1 Trái - STM32 **PA1**)<br>• **Pin 3:** `IN2/PA2` (Chiều 2 Trái - STM32 **PA2**)<br>• **Pin 4:** `IN3/PA4` (Chiều 1 Phải - STM32 **PA4**)<br>• **Pin 5:** `IN4/PA5` (Chiều 2 Phải - STM32 **PA5**)<br>• **Pin 6:** `ENB/PA3` (PWM Bánh Phải - STM32 **PA3**) |
| **J14** | **Cọc nối Khay Pin 4S** | Domino 2P bước 5.08mm | 2P | • **Pin 1:** `+16.8V` (Dây Đỏ cực dương khay pin 4S nối vào Source Q2)<br>• **Pin 2:** `GND` (Dây Đen cực âm khay pin 4S nối về mặt phẳng mass chính) |
| **J16** | Cọc ngõ ra 4 Động cơ TT | Domino 4P bước 5.08mm | 4P | • **Pin 1:** `OUT1` $\to$ Cực dương Motor Trái Trước (M1) và Trái Sau (M3)<br>• **Pin 2:** `OUT2` $\to$ Cực âm Motor Trái Trước (M1) và Trái Sau (M3)<br>• **Pin 3:** `OUT3` $\to$ Cực dương Motor Phải Trước (M2) và Phải Sau (M4)<br>• **Pin 4:** `OUT4` $\to$ Cực âm Motor Phải Trước (M2) và Phải Sau (M4) |
| **J17** | Cổng cấp nguồn & Giao tiếp Pi 4| Header 1x7P bước 2.54mm (kèm dây Type-C) | 7P | • **Pin 2:** `5V` (Nối vào đường `+5V/PI` sau Polyfuse F1 và Diode TVS D1)<br>• **Pin 6:** `GND` (Mass hệ thống)<br>• **Pin 1:** `3V3` (Xuất nguồn `+3.3V/PI` từ Pi sang nuôi OLED J18)<br>• **Pin 3:** `SDA` (Tuyến dữ liệu I2C từ Pi sang OLED J18)<br>• **Pin 5:** `SCL` (Tuyến xung nhịp I2C từ Pi sang OLED J18)<br>• **Pin 7:** `GPIO 26` (Tín hiệu Pi 4 báo Halt qua `dtoverlay=gpio-poweroff` về STM32 **PB10**) |
| **J18** | Màn hình OLED 0.91" I2C | Header 1x4P bước 2.54mm | 4P | • **Pin 1:** `+3.3V/PI` (Lấy từ Pin 1 của Pi 4 qua `J17.1`)<br>• **Pin 2:** `SDA` (Tuyến dữ liệu I2C từ `J17.3`)<br>• **Pin 3:** `SCL` (Tuyến xung nhịp I2C từ `J17.5`)<br>• **Pin 4:** `GND` (Mass hệ thống) |
| **J19** | Cọc nối nút nguồn vỏ xe | Header 1x2P bước 2.54mm | 2P | • **Pin 1:** Nối song song nút mềm SW1 (`GATE_CTRL`) để gắn nút nhấn ngoài vỏ xe<br>• **Pin 2:** Nối `GND` |

---

## 4. BẢNG NETLIST TOÀN BỘ MẠCH IN PCB 4S

| Tên Net trên Mạch | Điện áp danh định | Danh sách các chân linh kiện kết nối vào Net | Đặc tính layout mạch in (PCB Design Rules) |
|---|:---:|---|---|
| **`+16.8V`** | $12.0\text{V} - 16.8\text{V}$ | • `J14.1` (Cực dương khay pin 4S)<br>• `Q2.Source` (Chân Source P-MOSFET IRF9540)<br>• `R14.1` (Chân 1 trở kéo lên Gate $10\text{k}\Omega$)<br>• `DZ1.Cathode` (Đầu vạch đen Diode Zener 12V) | Đường công suất nguồn chính $\ge 2.5\text{mm}$, chịu dòng đỉnh 5A |
| **`GATE_4S`** | $0\text{V} - 16.8\text{V}$ | • `Q2.Gate` (Chân Gate P-MOSFET IRF9540)<br>• `R14.2` (Chân 2 trở kéo lên Gate $10\text{k}\Omega$)<br>• `DZ1.Anode` (Đầu Anot Diode Zener 12V 1N4742A)<br>• `R34.1` (Trở nhánh nút nhấn $10\text{k}\Omega$)<br>• `R13.1` (Trở nhánh chốt $Q_3$ $10\text{k}\Omega$) | Đường mạch tín hiệu điều khiển Gate $0.4\text{mm}$, bố trí sát chân MOSFET |
| **`GATE_CTRL`** | $0\text{V} / 8.4\text{V}$ | • `R34.2` (Chân 2 trở nhánh nút nhấn $10\text{k}\Omega$)<br>• `SW1.1`, `J19.1` (Chân 1 nút nhấn mềm bật/tắt)<br>• `D5.Cathode` (Đầu Catot vạch đen Diode 1N4148/1N4007)<br>*(Nhánh chốt nguồn Q3 đi riêng qua R13.2 vào Collector Q3)* | Tuyến tín hiệu đóng mở nguồn $0.4\text{mm}$ |
| **`4S_V`** | $12.0\text{V} - 16.8\text{V}$ | • `Q2.Drain` (Chân Drain P-MOSFET IRF9540 - sau chốt nguồn)<br>• `U3.IN+` (Chân vào Buck XL4015)<br>• `C13.(+)` (Tụ hóa ngõ vào Buck $100\mu\text{F}$)<br>• `C14.1` (Tụ gốm $100\text{nF}$)<br>• `R1.1` (Chân 1 trở phân áp $18\text{k}\Omega$ đo áp pin 4S)<br>• `J15.1` (Chân vào cọc nút dừng khẩn cấp E.STOP) | Phủ đồng Copper Pour riêng tuyến công suất 4S, độ rộng $\ge 2.5\text{mm}$ |
| **`16V/E-STOP`** | $12.0\text{V} - 16.8\text{V}$ | • `J15.2` (Chân ra cọc nút dừng khẩn cấp E.STOP)<br>• `J12.1` (Chân dương cấp Driver L298N-POWER)<br>• `C11.(+)` (Tụ hóa lớn $470\mu\text{F}$)<br>• `C12.1` (Tụ gốm $100\text{nF}$)<br>• `R4.1` (Trở hạn dòng $3.3\text{k}\Omega$ LED báo nguồn Motor) | Tuyến công suất động cơ $\ge 2.5\text{mm}$, cạo lớp phủ xanh quét thiếc tăng cường |
| **`+5V`** | $5.15\text{V}$ | • `U3.OUT+` (Chân ra Buck XL4015)<br>• `F1.1` (Chân vào Cầu chì Polyfuse 3A)<br>• `U1.Pin18` (Chân 5V bo STM32 Blue Pill)<br>• `J1.1`, `J2.1` (VCC 2 Encoder LM393)<br>• `J9.1`, `J10.1`, `J11.1` (VCC 3 Siêu âm) + $C_3, C_5, C_7.(+)$ + $C_4, C_6, C_8.1$<br>• `BZ1.(+)` (Cực dương còi báo)<br>• `D2.Cathode` (Diode dập xung còi)<br>• `R3.1` (Trở LED báo động)<br>• `J7.1` (VCC cổng nạp/UART CP2102) | Phủ đồng toàn bộ lớp mặt nguồn Logic, độ rộng tối thiểu $\ge 2.0\text{mm}$ |
| **`+5V/PI`** | $5.15\text{V}$ (bảo vệ) | • `F1.2` (Chân ra Cầu chì Polyfuse 3A)<br>• `D1.Cathode` (Diode TVS SMAJ6.0A hoặc 1.5KE6.8A)<br>• `C1.(+)` (Cực dương tụ hóa lớn $2200\mu\text{F}$)<br>• `C2.1` (Tụ gốm $100\text{nF}$)<br>• `J17.2` (Cọc 5V cấp cáp USB Type-C sang Pi 4) | Đường đồng $\ge 2.5\text{mm}$, đặt C1, C2 và TVS D1 sát cạnh cọc cắm Type-C J17 |
| **`+3.3V/PI`** | $3.3\text{V}$ | • `J17.1` (Chân nhận nguồn 3.3V sạch từ Pi 4)<br>• `J18.1` (Chân VCC cấp màn hình OLED 0.91") | Đường mạch tín hiệu $0.4\text{mm}$ |
| **`POWER_HOLD`**| $0 / 3.3\text{V}$ | • `U1.PB0` (STM32 Output giữ chốt nguồn Pin 33)<br>• `R30.1` (Trở đệm Base $1\text{k}\Omega$)<br>• `R31.1` (Trở xả mass $10\text{k}\Omega$) | Đường mạch logic $0.3\text{mm}$ |
| **`PI_HALT`** | $0 / 3.3\text{V}$ | • `J3.1` (Cọc PI-HALT nhận từ GPIO 26 Raspberry Pi 4)<br>• `R32.1` (Trở đệm $1\text{k}\Omega$)<br>• `R33.1` (Trở xả mass $10\text{k}\Omega$)<br>• `U1.PB10` (Chân Input nhận tín hiệu Halt của STM32 Pin 35) | Đường mạch logic bắt tay tắt máy $0.3\text{mm}$ |
| **`PWR_BTN`** | $0 / 3.3\text{V}$ | • `D5.Anode` (Đầu Anot Diode 1N4148/1N4007)<br>• `U1.PB1` (Chân Input nhận diện nhấn nút tắt của STM32 Pin 34) | Tuyến đọc nút bấm $0.3\text{mm}$ |
| **`PA6`** | $0 - 2.60\text{V}$ | • `R1.2` (Chân 2 trở phân áp $18\text{k}\Omega$)<br>• `R2.1` (Chân 1 trở phân áp $3.3\text{k}\Omega$)<br>• `C9.1` (Tụ gốm $100\text{nF}$)<br>• `U1.PA6` (Chân ADC1_IN6 STM32) | Tuyến analog $0.3\text{mm}$, bọc mass bảo vệ tín hiệu |
| **`GND`** | $0\text{V}$ (Mass Logic)| • `J14.2` (Cực âm khay pin 4S)<br>• `NT1.1` (Chân 1 cầu chập mass NetTie_2)<br>• `SW1.2`, `J19.2` (Chân âm nút nhấn mềm)<br>• `Q3.Emitter`, `R32.2`, `R34.2`<br>• `U3.IN-` & `U3.OUT-` (GND XL4015)<br>• `D1.Anode` (TVS SMAJ6.0A)<br>• `C1, C2, C3, C4, C5, C6, C7, C8, C9, C13, C14, C15, C16.(-)`<br>• `R2.2` (Chân âm trở đo áp pin 4S)<br>• `R8.2, R10.2, R12.2` (Chân âm cầu trở Echo)<br>• `R5.2` (Trở xả Base Q1) & `Q1.Emitter`<br>• `R21.2, R22.2` (Trở kéo xuống ENA, ENB)<br>• `U1.Pin19` (GND STM32 Blue Pill)<br>• `J1.2, J2.2` (GND Encoders)<br>• `J7.4` (GND CP2102)<br>• `J9.4, J10.4, J11.4` (GND Siêu âm)<br>• `J17.6` (GND Raspberry Pi 4)<br>• `J18.4` (GND Màn hình OLED) | **ĐỔ ĐỒNG MẶT PHẲNG MASS LOGIC (GND PLANE)** |
| **`PGND`** | $0\text{V}$ (Mass Motor)| • `NT1.2` (Chân 2 cầu chập mass NetTie_2 tại J14.2)<br>• `J12.2` (Cọc âm cấp nguồn Driver L298N)<br>• `C11.(-)` (Tụ hóa bù sụt áp motor $470\mu\text{F}$)<br>• `C12.2` (Tụ gốm $100\text{nF}$ lọc cao tần motor)<br>• `D4.Cathode` (Cực âm LED báo nguồn Motor) | **ĐỔ ĐỒNG VÙNG ĐỘNG LỰC MOTOR (PGND PLANE)**, chỉ nối với GND qua `NT1` tại J14.2 |

---

## 5. SƠ ĐỒ ĐI DÂY TOÀN DIỆN MẠCH NGUỒN 4S (ASCII WIRING DIAGRAM)

```
======================================================================================================================================================
                                         SƠ ĐỒ ĐI DÂY & NGUYÊN LÝ KHỐI TOÀN DIỆN AGV PIN 4S (REV 3.0)
======================================================================================================================================================

 [ KHAY PIN 4S (14.8V - 16.8V) ]
   Dây Đỏ (+) ──► J14.1 ──┬──► Source [ Q2: P-MOSFET IRF9540N ] Drain ──┬──► Cụm tụ [ C13: 100uF // C14: 100nF ] ──► IN+ [ U3: MODULE XL4015 ] OUT+ (5.15V)
                          │         ▲ (Chân 3)           (Chân 2)        │                                                 |             |
                          │   Gate  │ (Chân 1)                           ├──► [ R1: 43k/18k ] ──┬──► STM32 PA6 (ADC 4S)    IN-           OUT-
                          ├──[R14]──┤                                    │                      │                          │             │
                          │  (10k)  │                                    │    [ C9: 100nF // R2: 10k/3.3k ]                │             │
                          │         │                                    │                      │                          │             │
                          └──[DZ1]──┘ (Zener 12V kẹp áp Gate)            │                      │                          │             │
                                    │                                    │                      │                          │             │
                 ┌──────────────────┴──────────────────┐                 │                      │                          │             │
                 │ Nhánh 1: Nút nhấn                   │ Nhánh 2:        │                      │                          │             │
                 ▼                                     ▼ Tự chốt nguồn   │                      │                          │             │
            [ R34: 10k ]                         [ R13: 10k ]            │                      │                          │             │
                 │                                     │                 │                      │                          │             │
     ┌───────────┴───────────┐                         │                 │                      │                          │             │
     │                       │                         │                 │                      │                          │             │
[ Nút bấm SW1 ]        [ Diode D5 ]                    ▼                 │                      │                          │             │
 (hoặc J19)             (1N4148)           Collector [ Q3: S8050/2N2222 ]│                      │                          │             │
     │                 Catot (vạch)                    ▲ (Chân 3)        │                      │                          │             │
     │                       │                  Base ──┴── [ R30: 1k ]   │                      │                          │             │
    GND                      │                         │         ▲       │                      │                          │             │
                           Anot                   [ R31: 10k ]   │       │                      │                          │             │
                             │                         │     STM32 PB0   │                      │                          │             │
                             ▼                        GND   (POWER_HOLD) │                      │                          │             │
                      Chân STM32 PB1                                     │                      │                          │             │
                   (Nhận diện bấm tắt)                                   ▼                      │                          │             │
                                                          [ J5: Nút Dừng E-Stop ]               │                          │             │
                                                                         │                      │                          │             │
                                                                         ▼                      │                          │             │
                                                ┌── Cụm tụ [ C11: 470uF // C12: 100nF ] ──► PGND│                          │             │
                                                ├── [ R4: 3.3k ] ──► [ D4: LED Đỏ ] ──► PGND    │                          │             │
                                                │                                               │                          │             │
                                                ├──► J12.1 (Chân cấp dương 12V Driver L298N)    │                          │             │
                                                └──► J12.2 (Chân âm mass PGND Driver L298N) ──► PGND                    │             │
                                                                                                  │                        │             │
                                                                                          [ NT1: NetTie_2 ]                │             │
                                                                                                  │                        │             │
   Dây Đen (-) ──► J14.2 (GND) ───────────────────────────────────────────────────────────────────┴────────────────────────┴─────────────┼───┐
                                                                                                                                         │   │
   ┌─────────────────────────────────────────────────── ĐƯỜNG NGUỒN SẠCH +5V ───────────────────────────────────────────────────────────┘   │
   │                                                                                                                                        │
   │                                                                                                                                        │
   ├──► [ F1: Polyfuse 3A ] ──► Node +5V/PI ──┬──► [ J17.2: Cáp Type-C Pi 4 ]                                                                │
   │                                          ├──► [ D1: TVS SMAJ6.0A hoặc 1.5KE6.8A ] ──► GND (Crowbar OVP)                                │
   │                                          └──► [ C1: 2200uF Low ESR // C2: 100nF ] ──► GND (Chống sụt áp Pi 4)                          │
   │                                                                                                                                        │
   ├──► STM32 Blue Pill (Pin 18: 5V)                                                                                                        │
   ├──► 2x Encoder LM393 (J1.1, J2.1: VCC 5V)                                                                                               │
   ├──► 3x Cảm biến HC-SR04 (J9.1, J10.1, J11.1: VCC 5V) ── Kèm cụm tụ riêng [ 100uF // 100nF ] từng con                                   │
   ├──► Mạch nạp CP2102 (J7.1: 5V)                                                                                                          │
   └──► Còi chip BZ1 (5V) & LED D3 ──► Transistor Q1 (2N2222) ◄── [ R6: 1k ] ◄── STM32 PB5                                                  │

------------------------------------------------------------------------------------------------------------------------------------------------------
 [ ĐƯỜNG TRUYỀN TÍN HIỆU ĐIỀU KHIỂN, BẢO VỆ & CẢM BIẾN ]
   • Bắt tay Pi: Pi 4 GPIO 26 (dtoverlay=gpio-poweroff) ──► J17.7 ──► [ R33: 1k ] ──┬──► STM32 PB10 (PI_HALT_DET: Nhận biết Pi đã tắt hẳn)
                                                                                    └──► [ R34: 10k ] ──► GND (Chống trôi chân lúc Pi boot)
   • Bánh Trái:  STM32 PA0 (TIM2_CH1 PWM ≤ 55%) ──► [ R20: 330Ω ] ──► J13.1 (ENA) ── [ R21: 10k pull-down ]
                 STM32 PA1 (GPIO Out)          ──► [ R19: 330Ω ] ──► J13.2 (IN1)
                 STM32 PA2 (GPIO Out)          ──► [ R18: 330Ω ] ──► J13.3 (IN2) ──► Cầu H L298N (J16.1,2: OUT1,2) ──► Motor Trái (M1, M3)
   • Bánh Phải:  STM32 PA3 (TIM2_CH4 PWM ≤ 55%) ──► [ R17: 330Ω ] ──► J13.6 (ENB) ── [ R22: 10k pull-down ]
                 STM32 PA4 (GPIO Out)          ──► [ R16: 330Ω ] ──► J13.4 (IN3)
                 STM32 PA5 (GPIO Out)          ──► [ R15: 330Ω ] ──► J13.5 (IN4) ──► Cầu H L298N (J16.3,4: OUT3,4) ──► Motor Phải (M2, M4)

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
