# BẢNG DANH MỤC LINH KIỆN MẠCH NGUỒN & HỆ THỐNG (BILL OF MATERIALS - BOM)
## DỰ ÁN: XE TỰ HÀNH VẬN CHUYỂN LINH KIỆN NỘI BỘ (AGV - ĐỒ ÁN 2)
### BO MẠCH PHÂN PHỐI NGUỒN & GIAO TIẾP NGOẠI VI (POWER DISTRIBUTION & INTERFACE BOARD)
**Chuẩn hóa 100% theo Schematic KiCad: `power-source-board.kicad_sch` (Rev 2.0 - Đồ án 2 HCM-UTE)**

---

## 1. TỔNG QUAN KIẾN TRÚC MẠCH VÀ CÁC ĐIỂM CẢI TIẾN TRÊN SCHEMATIC

Bo mạch phân phối nguồn và giao tiếp ngoại vi (PDB & Interface Board) được thiết kế tích hợp toàn bộ các khối chức năng của robot lên một bo mạch in PCB 2 lớp duy nhất:
* **Kiến trúc nguồn kép độc lập:**
  * **Nhánh A (Logic):** Khay 3S (`BT1, BT2, BT3`) chứa 3 cell Li-ion 18650 nối tiếp ($11.1\text{V} - 12.6\text{V}$) cấp qua cọc `J14`, công tắc cơ `J3` (Switch A), lọc nguồn bằng cụm `C13 (100uF) + C14 (100nF)`, qua Module Buck XL4015 (`U3`) hạ áp xuống đường nguồn sạch `+5V`.
  * **Nhánh B (Động lực Motor):** Khay 2S (`BT4, BT5`) chứa 2 cell Li-ion 18650 nối tiếp ($7.4\text{V} - 8.4\text{V}$) cấp qua cọc `J15`, công tắc cơ `J4` (Switch B), nối tiếp nút dừng khẩn cấp E-Stop `J5` (Latched Button), lọc dòng xả động cơ qua `C11 (470uF Low-ESR) + C12 (100nF)`, có LED báo nguồn `D4 (Red)` qua trở hạn dòng `R4 (1k)`. Cấp trực tiếp vào cọc nguồn Driver L298N `J12`.
* **Cơ chế tách Mass (Star Ground & NetTie):**
  * Mass mạch Logic (`GND`) và Mass công suất động cơ (`PGND`) được đi riêng biệt hoàn toàn trên PCB và chỉ chập chung tại **1 điểm duy nhất thông qua linh kiện Net-Tie (`NetTie_2`)**, triệt tiêu 100% hiện tượng xung nhiễu chổi than và hiện tượng sụt áp mass (ground bounce) làm treo Raspberry Pi 4 hoặc sai lệch ADC STM32.
* **Các cải tiến kỹ thuật phần cứng đã cập nhật trên Schematic:**
  1. **Đo áp kép 2 nhánh pin (PA6 & PA7):** Bổ sung thêm cầu phân áp `R13 (10k) + R14 (3.3k) + C10 (100nF)` về chân **PA7** để giám sát điện áp nhánh động cơ `12V/8V4`, song song với cầu phân áp `R1 (10k) + R2 (3.3k) + C9 (100nF)` trên nhánh 3S về **PA6**.
  2. **Mạch đệm trở và phân áp cảm biến siêu âm:**
     * Đường phát xung `Trig`: Thêm trở đệm nối tiếp `R25 (330Ω), R26 (330Ω), R27 (330Ω)` trên PB12, PB14, PB8 nhằm triệt dao động ký sinh và bảo vệ chân vi điều khiển.
     * Đường phản hồi `Echo`: Tích hợp cầu chia áp $5\text{V} \to 3.33\text{V}$ (`R7=1k/R8=2k`, `R9=1k/R10=2k`, `R11=1k/R12=2k`) chuyển mức an toàn tuyệt đối về PB13, PB15, PB9 thay vì nối trực tiếp 5V.
  3. **Bộ lọc thông thấp RC chống nhiễu Encoder:**
     * Bổ sung `R23 (330Ω) + C15 (1nF)` cho kênh Left (PB6) và `R24 (330Ω) + C16 (1nF)` cho kênh Right (PB7), cắt tần số cao $f_c \approx 482\text{kHz}$ dập triệt để gai xung tia lửa chổi than động cơ nhiễm vào xung đếm.
  4. **Điện trở đệm & Pull-down ngõ điều khiển Motor L298N:**
     * Bổ sung 6 trở đệm nối tiếp `R15, R16, R17, R18, R19, R20 (330Ω)` trên các đường PWM/GPIO từ STM32 sang L298N.
     * Bổ sung trở kéo xuống `R21 (10k), R22 (10k)` trên 2 đường ENA, ENB đảm bảo motor ở trạng thái ngắt an toàn (Disable) khi vi điều khiển đang boot/reset hoặc nạp code.
  5. **Mạch báo động kép Âm - Quang (Buzzer & LED):**
     * Nhánh còi `BZ1` (kèm diode dập xung ngược `D2 - 1N4148`) được đấu song song thêm đèn LED `D3` (qua trở hạn dòng `R3 - 1k`), điều khiển bởi transistor `Q1 (2N2222)`, có trở xả `R5 (10k)` chống float kích còi bậy khi khởi động.
  6. **Đệm nối tiếp UART:** Bổ sung `R28 (330Ω), R29 (330Ω)` trên đường TX1 (PA9) và RX1 (PA10) kết nối mạch nạp USB-TTL CP2102 `J7`.

---

## 2. CHUYÊN ĐỀ PHÂN TÍCH & CÂN NHẮC LỰA CHỌN DIODE TVS KHI ÁP BUCK ĐẠT 5.1V - 5.2V

### 2.1. Đặt vấn đề kỹ thuật
Để tránh hiện tượng cảnh báo sụt áp (*"Low voltage warning / Under-voltage detected"*) và hiện tượng nghẽn xung nhịp CPU (CPU throttling) trên **Raspberry Pi 4 Model B** khi xử lý tác vụ nặng (nhận dạng Camera, chạy mạng nơ-ron, truyền stream mạng), module Buck XL4015 được tinh chỉnh ngõ ra ở mức **$5.15\text{V}$** (dải thực tế $5.10\text{V} - 5.20\text{V}$). 
Mức điện áp này là bắt buộc nhằm bù đắp:
* Nội trở sụt áp của cầu chì tự phục hồi Polyfuse $F1$ ($R_{poly} \approx 0.03\Omega - 0.08\Omega$).
* Trở kháng tiếp xúc của cọc domino, cáp dây dẫn Type-C ($R_{cable} \approx 0.1\Omega$).
* Khi dòng tải vọt từ $0.6\text{A}$ lên $2.5\text{A} - 3.0\text{A}$, sụt áp $\Delta V = I \cdot R_{total} \approx 0.35\text{V}$. Nếu chỉnh $5.00\text{V}$, áp vào chip PMIC của Pi 4 sẽ sụt xuống còn $4.65\text{V} < 4.75\text{V}$ (gây treo máy). Do đó, áp đặt $5.15\text{V}$ sẽ duy trì điện áp tại bo mạch Pi 4 luôn nằm lý tưởng trong dải $4.85\text{V} - 5.05\text{V}$.

Tuy nhiên, việc đặt áp danh định ở **$5.10\text{V} - 5.20\text{V}$** đặt ra bài toán nghiêm ngặt về việc lựa chọn thông số cho **Diode TVS bảo vệ ngõ ra (`D1`)**.

---

### 2.2. Phân tích các thông số then chốt của Diode TVS
Một diode triệt xung điện áp quá độ (Transient Voltage Suppressor - TVS) có 3 tham số sống còn:
1. **$V_{RWM}$ (Reverse Standoff Voltage / Working Peak Reverse Voltage):** Điện áp làm việc cực đại cho phép liên tục. Ở điện áp này, TVS hoàn toàn ở trạng thái **NGẮT (OFF)**, dòng rò tĩnh $I_R$ cực nhỏ (thường $< 5\mu\text{A}$).
2. **$V_{BR}$ (Breakdown Voltage):** Điện áp đánh thủng (đo tại dòng thử $I_T = 1\text{mA}$ hoặc $10\text{mA}$). Đây là ngưỡng TVS bắt đầu chuyển mạnh sang trạng thái dẫn điện để kẹp áp.
3. **$V_C$ (Clamping Voltage):** Điện áp kẹp lớn nhất trên 2 đầu TVS khi có dòng xung đỉnh $I_{PP}$ chạy qua.
4. **$P_{PPM}$ (Peak Pulse Power Dissipation):** Khả năng tiêu tán công suất xung (ví dụ xung chuẩn $10/1000\mu\text{s}$).

---

### 2.3. Rủi ro thực tế khi sử dụng TVS 5.0V (SMAJ5.0A) với áp Buck 5.1V - 5.2V
Hiện tại, trên bản vẽ Schematic ban đầu đang ghi tên linh kiện là **`SMAJ5.0A`**. Khi vận hành ở dải áp $5.10\text{V} - 5.20\text{V}$, linh kiện này bộc lộ các nhược điểm kỹ thuật nghiêm trọng:
* **Vi phạm nguyên tắc Standoff Voltage:** $V_{RWM}$ của SMAJ5.0A là **$5.0\text{V}$**. Điện áp làm việc thực tế $V_{bus} = 5.15\text{V} - 5.20\text{V} > V_{RWM}$.
* **Hiện tượng rò dòng ở "vùng đầu gối" (Knee Conduction):** Mặc dù ngưỡng đánh thủng danh định của SMAJ5.0A là $V_{BR\_min} = 6.40\text{V}$, nhưng trong khoảng từ $5.0\text{V}$ đến $6.4\text{V}$, tiếp giáp p-n bắt đầu rỉ dòng qua. Theo datasheet của Littelfuse/MCC, tại $5.0\text{V}$ dòng rò tối đa đã lên tới $800\mu\text{A}$. Khi áp bus vọt lên $5.20\text{V}$ và môi trường trong thân xe robot nóng lên ($40^\circ\text{C} - 50^\circ\text{C}$ do nhiệt tỏa ra từ L298N và XL4015), dòng rò qua SMAJ5.0A tăng theo hàm mũ:
  $$I_R(T) \approx I_R(25^\circ\text{C}) \cdot 2^{\frac{T - 25}{10}}$$
  Dòng rò có thể lên tới **$2\text{mA} - 10\text{mA}$**, gây tự phát nhiệt trên diode TVS ($P = V \times I_R$), làm sụt giảm tuổi thọ linh kiện và gây lãng phí pin.
* **Dung sai linh kiện thị trường trôi nổi:** Các loại diode TVS phổ thông trên thị trường (Shopee/chợ Nhật Tảo) thường có dung sai $\pm 10\%$. Nếu gặp linh kiện có $V_{BR}$ lệch về cận dưới ($5.8\text{V} - 6.0\text{V}$), việc áp Buck có gợn sóng dao động (ripple $50-100\text{mV}_{p-p}$) hoặc xung hồi tiếp từ chổi than có thể chạm vào ngưỡng dẫn, khiến TVS bị nóng rực hoặc kích hoạt sai làm sụt nguồn của Pi 4.

---

### 2.4. Bảng so sánh toàn diện các phương án Diode TVS

| Tiêu chí kỹ thuật | **SMAJ5.0A** (Hiện tại trên Schem) | **SMAJ6.0A** (Khuyến nghị chuẩn SMD) | **SMBJ6.0A** (Khuyến nghị chịu tải cao) | **1.5KE6.8A / P6KE6.8A** (Chuẩn chân cắm DIP) |
|---|:---:|:---:|:---:|:---:|
| **Kiểu đóng gói (Footprint)** | SMD DO-214AC (SMA) | SMD DO-214AC (SMA) | SMD DO-214AA (SMB) | DO-201 / DO-15 (Axial DIP) |
| **Điện áp Standoff ($V_{RWM}$)** | **$5.0\text{V}$** *(Vi phạm: $5.0 < 5.2\text{V}$)* | **$6.0\text{V}$** *(Lý tưởng: $6.0 > 5.2\text{V}$)* | **$6.0\text{V}$** *(Lý tưởng: $6.0 > 5.2\text{V}$)* | **$5.8\text{V}$** *(Lý tưởng: $5.8 > 5.2\text{V}$)* |
| **Điện áp Breakdown ($V_{BR\_min}$)** | $6.40\text{V}$ | **$6.67\text{V}$** | **$6.67\text{V}$** | **$6.45\text{V}$** |
| **Điện áp Breakdown ($V_{BR\_max}$)** | $7.00\text{V}$ | **$7.37\text{V}$** | **$7.37\text{V}$** | **$7.14\text{V}$** |
| **Điện áp kẹp xung ($V_C$ tại $I_{PP}$)** | $9.2\text{V}$ | $10.3\text{V}$ | $10.3\text{V}$ | $10.5\text{V}$ |
| **Dòng rò tại $5.2\text{V}$ ($I_R$)** | Cao ($500\mu\text{A} \div vài\text{ mA}$) | **Cực thấp ($< 5\mu\text{A}$)** | **Cực thấp ($< 5\mu\text{A}$)** | **Cực thấp ($< 5\mu\text{A}$)** |
| **Công suất xung đỉnh ($P_{PPM}$)** | $400\text{W}$ ($10/1000\mu\text{s}$) | $400\text{W}$ | $600\text{W}$ | $1500\text{W}$ / $600\text{W}$ |
| **Khả năng phối hợp ngắt F1 (Polyfuse)**| Rất tốt (dẫn kéo sụt áp) | Rất tốt (dẫn dập Crowbar) | Rất tốt (chịu quá tải cao) | Cực tốt (chịu quá dòng xung lớn) |
| **Mức độ sẵn hàng & Giá** | Rất phổ biến (~1.500đ) | Rất phổ biến (~2.000đ) | Rất phổ biến (~2.500đ) | Rất phổ biến (~2.000đ) |

---

### 2.5. Cơ chế phối hợp bảo vệ Crowbar (Polyfuse F1 + TVS D1 + Tụ C1)
Trong sơ đồ mạch, TVS $D1$ không làm việc đơn độc mà phối hợp nhịp nhàng theo chuỗi bảo vệ 3 lớp:
1. **Lớp 1 - Làm mịn và ghìm tốc độ quá áp ($C1 - 2200\mu\text{F}$):** Khi module XL4015 bị đoản mạch MOSFET nội (chập ngõ vào 12.6V ra ngõ ra), dung lượng cực lớn $2200\mu\text{F}$ sẽ làm chậm tốc độ tăng áp $\frac{dV}{dt}$ ($i = C \frac{dV}{dt}$).
2. **Lớp 2 - Kẹp áp đánh thủng (TVS $D1$):**
   * Nếu chọn **`SMAJ6.0A`** (hoặc `1.5KE6.8A`), ngay khi điện áp vọt qua ngưỡng $6.67\text{V}$ (ngưỡng PMIC Raspberry Pi 4 vẫn chịu đựng được trong thời gian ngắn vài chục micro-giây), TVS chuyển ngay sang trạng thái dẫn bão hòa cực mạnh (Avalanche breakdown) về mass.
   * Dòng điện chạy qua TVS tăng vọt lên đến hàng chục ampe, giữ chặt điện áp ngõ vào Pi 4 không thể vượt lên mức 12V gây nổ chip.
3. **Lớp 3 - Ngắt dòng bảo vệ vĩnh viễn (Polyfuse $F1$):** Dòng quá tải khổng lồ chạy qua cầu chì tự phục hồi $F1$ ($I_{hold} = 3.0\text{A}, I_{trip} \approx 5.0\text{A}$) khiến nhiệt lượng Joule ($I^2 R$) làm điện trở polymer của F1 tăng vọt lên hàng Mega-ohm chỉ trong vòng vài mili-giây, ngắt hoàn toàn nguồn điện cấp vào nhánh Pi 4.

---

### 2.6. Khuyến nghị triển khai chính thức cho Đề tài
1. **Ưu tiên số 1 (Dạng dán SMD):** Cập nhật BOM và chỉ định linh kiện **`SMAJ6.0A`** (hoặc `SMBJ6.0A` nếu muốn footprint lớn tản nhiệt tốt hơn). Đây là lựa chọn hoàn hảo nhất về mặt kỹ thuật cho áp Buck $5.10\text{V} - 5.20\text{V}$.
2. **Ưu tiên số 2 (Dạng chân cắm DIP):** Nếu sinh viên gia công mạch in tại xưởng thực hành bằng phương pháp khoan lỗ hàn thủ công, sử dụng linh kiện **`1.5KE6.8A`** hoặc **`P6KE6.8A`** ($V_{RWM} = 5.8\text{V}, V_{BR} = 6.45\text{V} - 7.14\text{V}$). Điện áp Standoff 5.8V tạo biên an toàn hoàn hảo $11\%$ so với mức 5.2V của Buck.
3. **Lưu ý cân chỉnh Buck XL4015 trên bàn test:**
   * Dùng đồng hồ VOM kiểm tra không tải, vặn biến trở đa vòng trên XL4015 để điện áp tại ngõ ra đạt đúng **$5.15\text{V} \pm 0.05\text{V}$**.
   * Sau khi chỉnh chuẩn, nhỏ 1 giọt **keo silicon / sơn móng tay** vào ốc tinh chỉnh của biến trở để khóa cứng giá trị, tránh rung lắc cơ học khi robot chạy làm sai lệch điện áp.

---

## 3. BẢNG DANH MỤC LINH KIỆN (BOM) CHI TIẾT ĐỒNG BỘ 100% SCHEMATIC

| STT | Kí hiệu (Designator) | Tên linh kiện / Module | Thông số kỹ thuật chi tiết | Kiểu đóng gói (Footprint) | SL | Vai trò / Khối chức năng trong hệ thống | Đơn giá | Thành tiền | Nơi mua gợi ý |
|:---:|:---:|---|---|:---:|:---:|---|:---:|:---:|---|
| **I** | | **KHỐI PIN & ĐẦU CẮM NGUỒN VÀO (POWER INPUTS)** | | | | | | | |
| 1 | **BT1, BT2, BT3** | Cell pin Li-ion 18650 Nhánh A | Dung lượng 2500mAh, dòng xả $\ge 10\text{A}$ (Sony/Lishen) | Trụ tròn $\Phi 18 \times 65\text{mm}$ | 3 | Mắc nối tiếp khay 3S ($11.1\text{V}-12.6\text{V}$) nuôi Pi & Logic | 32.000đ | 96.000đ | Shopee Pin Chuyên Nghiệp |
| 2 | **BT4, BT5** | Cell pin Li-ion 18650 Nhánh B | Dung lượng 2500mAh, dòng xả $\ge 10\text{A}$ (Sony/Lishen) | Trụ tròn $\Phi 18 \times 65\text{mm}$ | 2 | Mắc nối tiếp khay 2S ($7.4\text{V}-8.4\text{V}$) nuôi Động cơ Motor | 32.000đ | 64.000đ | Shopee Pin Chuyên Nghiệp |
| 3 | **J14** | Domino 2P cắm Khay Pin 3S | KF301-2P bước 5.08mm, 10A/300V | DIP bước 5.08mm | 1 | Đấu nối dây dương (+12.6V) và âm (GND) khay pin 3S | 2.000đ | 2.000đ | Hshop / Shopee |
| 4 | **J15** | Domino 2P cắm Khay Pin 2S | KF301-2P bước 5.08mm, 10A/300V | DIP bước 5.08mm | 1 | Đấu nối dây dương (+8.4V) và âm (PGND) khay pin 2S | 2.000đ | 2.000đ | Hshop / Shopee |
| 5 | **J3** | Cọc nối Công tắc A (Switch A) | Header 1x2P hoặc KF301-2P bước 5.08mm | DIP 2.54 / 5.08mm | 1 | Nối công tắc gạt bật/tắt nguồn 3S nhánh Logic (`3S_V`) | 2.000đ | 2.000đ | Cửa hàng điện tử |
| 6 | **J4** | Cọc nối Công tắc B (Switch B) | Header 1x2P hoặc KF301-2P bước 5.08mm | DIP 2.54 / 5.08mm | 1 | Nối công tắc gạt bật/tắt nguồn 2S nhánh Motor | 2.000đ | 2.000đ | Cửa hàng điện tử |
| 7 | **J5** | Cọc nối Nút E-Stop (Latched Button)| Header 1x2P hoặc KF301-2P bước 5.08mm | DIP 2.54 / 5.08mm | 1 | Nối nút nhấn dừng khẩn cấp cơ khí khóa nguồn Motor | 2.000đ | 2.000đ | Cửa hàng điện tử |
| **II** | | **KHỐI HẠ ÁP BUCK & BẢO VỆ CHỐNG BROWNOUT PI 4** | | | | | | | |
| 8 | **U3** (J6, J8) | Module Buck DC-DC XL4015 | Input 8-36V, Output chỉnh 5.15V, dòng đỉnh 5A | Module cắm 4 chân (hoặc hàn) | 1 | Hạ áp từ $11.1\text{V}$ xuống $5.15\text{V}$ sạch nuôi toàn hệ thống | 28.000đ | 28.000đ | Hshop / Shopee |
| 9 | **F1** | Cầu chì tự phục hồi (Polyfuse) | $I_H = 3.0\text{A}, I_{trip} = 5.0\text{A}, V_{max} \ge 16\text{V}$ (PPTC) | Radial DIP bước 5.08mm | 1 | Bảo vệ quá dòng / ngắn mạch ngõ vào cổng Type-C Pi 4 | 2.500đ | 2.500đ | Shopee / Cửa hàng |
| 10 | **D1** | **Diode TVS triệt quá áp ngõ ra Pi** | **SMAJ6.0A** (SMD) hoặc **1.5KE6.8A** (DIP), $V_{RWM}=6.0V$ | DO-214AC (SMA) / DO-201 | 1 | Triệt xung transient, dập bảo vệ OVP Crowbar cho Pi 4 | 2.000đ | 2.000đ | Cửa hàng linh kiện |
| 11 | **C1** | Tụ hóa bù sụt áp Pi 4 | $2200\mu\text{F} / 16\text{V}$ Low ESR (Chịu nhiệt $105^\circ\text{C}$) | Radial $\Phi 10\text{mm}$, bước 5mm | 1 | Bù áp tức thời khi Pi boost xung CPU, dập sóng ripple | 3.000đ | 3.000đ | Cửa hàng điện tử |
| 12 | **C2** | Tụ gốm dập xung cao tần Pi 4 | $100\text{nF} (0.1\mu\text{F} - 104) / 50\text{V}$ | Gốm tròn bước 2.54mm | 1 | Mắc song song C1 dập xung cao tần ngõ ra Pi 4 | 300đ | 300đ | Cửa hàng điện tử |
| 13 | **C13** | Tụ hóa lọc ngõ vào Buck 3S | $100\mu\text{F} / 25\text{V}$ | Radial $\Phi 5\text{mm}$, bước 2.54mm | 1 | Ổn định đường nguồn `3S_V` trước khi vào module XL4015 | 500đ | 500đ | Cửa hàng điện tử |
| 14 | **C14** | Tụ gốm lọc ngõ vào Buck 3S | $100\text{nF} (104) / 50\text{V}$ | Gốm tròn bước 2.54mm | 1 | Lọc nhiễu cao tần trên đường nguồn `3S_V` | 300đ | 300đ | Cửa hàng điện tử |
| **III** | | **KHỐI ĐỘNG LỰC MOTOR L298N & BẢO VỆ** | | | | | | | |
| 15 | **U2** | Module Driver Động cơ Cầu H L298N| Module L298N có tản nhiệt nhôm, 2A/kênh | Cụm module cắm ngoài | 1 | Mạch cầu H điều khiển đảo chiều và PWM 4 động cơ TT | 32.000đ | 32.000đ | Hshop / Shopee |
| 16 | **C11** | Tụ hóa xả tức thời nguồn Motor | $470\mu\text{F} / 25\text{V}$ Low ESR | Radial $\Phi 8\text{mm}$, bước 3.5/5mm | 1 | Gánh xung dòng khởi động động cơ, bù sụt áp `12V/8V4` | 1.500đ | 1.500đ | Cửa hàng điện tử |
| 17 | **C12** | Tụ gốm lọc cao tần nguồn Motor | $100\text{nF} (104) / 50\text{V}$ | Gốm tròn bước 2.54mm | 1 | Dập xung cao tần từ chổi than trên đường `12V/8V4` | 300đ | 300đ | Cửa hàng điện tử |
| 18 | **D4** | LED báo nguồn Motor (12V/8V4) | LED tròn 3mm hoặc 5mm màu Đỏ (Red) | Radial 3mm/5mm | 1 | Sáng báo đường nguồn động lực đã bật và thông qua E-Stop | 500đ | 500đ | Cửa hàng điện tử |
| 19 | **R4** | Trở hạn dòng LED báo nguồn Motor | $1\text{k}\Omega \pm 5\%$ (0.25W kim loại) | Axial bước 7.62mm | 1 | Hạn dòng LED D4 ở mức an toàn $\approx 6\text{mA}$ | 200đ | 200đ | Cửa hàng điện tử |
| 20 | **J12** | Domino 2P cấp nguồn L298N | KF301-2P bước 5.08mm | DIP bước 5.08mm | 1 | Xuất nguồn động lực (`12V/8V4`, `PGND`) sang cọc L298N | 2.000đ | 2.000đ | Hshop / Shopee |
| 21 | **J13** | Header 1x6P điều khiển L298N | Hàng rào đực/cái 1x6 bước 2.54mm | Header 1x6P thẳng | 1 | Kết nối 6 tín hiệu: ENA, IN1, IN2, IN3, IN4, ENB | 1.000đ | 1.000đ | Cửa hàng điện tử |
| 22 | **R15, R16, R17, R18, R19, R20** | Cụm trở đệm bảo vệ GPIO Motor | $330\Omega \pm 5\%$ (0.25W) (6 con) | Axial bước 7.62mm | 6 | Đệm chống sốc ngược từ L298N về PA5, PA4, PA3, PA2, PA1, PA0 | 200đ | 1.200đ | Cửa hàng điện tử |
| 23 | **R21, R22** | Trở kéo xuống (Pull-down) Enable | $10\text{k}\Omega \pm 5\%$ (0.25W) (2 con) | Axial bước 7.62mm | 2 | Khóa an toàn ENA (PA0) & ENB (PA3) mức LOW khi MCU reset | 200đ | 400đ | Cửa hàng điện tử |
| 24 | **J16** | Domino 4P ngõ ra Động cơ | KF301-4P (hoặc 2x KF301-2P) 5.08mm | DIP bước 5.08mm | 1 | Nối dây ra 4 động cơ: OUT1, OUT2, OUT3, OUT4 | 4.000đ | 4.000đ | Hshop / Shopee |
| 25 | **M1, M2, M3, M4** | Động cơ giảm tốc DC TT (Vàng) | Điện áp 3-9V, tỷ số 1:48, trục đôi | Dạng động cơ TT kèm đĩa | 4 | Truyền động 4 bánh xe robot (FL, FR, BL, BR) | Có sẵn | 0đ | Thiết bị có sẵn |
| **IV** | | **KHỐI GIÁM SÁT ĐIỆN ÁP 2 NHÁNH PIN (ADC PA6 & PA7)** | | | | | | | |
| 26 | **R1** | Trở phân áp trên nhánh 3S | $10\text{k}\Omega \pm 1\%$ (Điện trở chính xác 0.25W) | Axial bước 7.62mm | 1 | Nối từ `3S_V` xuống chân PA6 ADC STM32 | 200đ | 200đ | Cửa hàng điện tử |
| 27 | **R2** | Trở phân áp dưới nhánh 3S | $3.3\text{k}\Omega \pm 1\%$ (Điện trở chính xác 0.25W) | Axial bước 7.62mm | 1 | Nối từ chân PA6 ADC STM32 xuống mass GND | 200đ | 200đ | Cửa hàng điện tử |
| 28 | **C9** | Tụ gốm lọc nhiễu ADC Pin 3S | $100\text{nF} (104) / 50\text{V}$ | Gốm tròn bước 2.54mm | 1 | Lọc nhiễu cao tần song song R2, chống jitter đọc ADC | 300đ | 300đ | Cửa hàng điện tử |
| 29 | **R13** | Trở phân áp trên nhánh Motor (2S) | $10\text{k}\Omega \pm 1\%$ (Điện trở chính xác 0.25W) | Axial bước 7.62mm | 1 | Nối từ `12V/8V4` xuống chân PA7 ADC STM32 | 200đ | 200đ | Cửa hàng điện tử |
| 30 | **R14** | Trở phân áp dưới nhánh Motor (2S)| $3.3\text{k}\Omega \pm 1\%$ (Điện trở chính xác 0.25W) | Axial bước 7.62mm | 1 | Nối từ chân PA7 ADC STM32 xuống mass PGND | 200đ | 200đ | Cửa hàng điện tử |
| 31 | **C10** | Tụ gốm lọc nhiễu ADC Motor | $100\text{nF} (104) / 50\text{V}$ | Gốm tròn bước 2.54mm | 1 | Lọc nhiễu cao tần song song R14, ổn định đọc ADC PA7 | 300đ | 300đ | Cửa hàng điện tử |
| **V** | | **KHỐI BÁO ĐỘNG KÉP ÂM - QUANG (BUZZER & LED)** | | | | | | | |
| 32 | **BZ1** | Còi báo chủ động (Active Buzzer) | Điện áp 5V DC, tần số phát $\approx 2.3\text{kHz}$ ($\Phi 12\text{mm}$) | DIP bước chân 7.62mm | 1 | Còi phát tiếng bíp báo pin yếu, sự cố kẹt đường | 3.000đ | 3.000đ | Hshop / Shopee |
| 33 | **D2** | Diode dập xung ngược cuộn còi | 1N4148 (High-speed switching diode) | DO-35 (Axial DIP) | 1 | Dập xung điện cảm bảo vệ transistor kích còi | 300đ | 300đ | Cửa hàng điện tử |
| 34 | **D3** | LED báo động quang (kết hợp còi) | LED tròn 3mm màu Vàng hoặc Đỏ | Radial 3mm | 1 | Nhấp nháy đồng bộ khi còi kêu cảnh báo trực quan | 500đ | 500đ | Cửa hàng điện tử |
| 35 | **R3** | Trở hạn dòng LED báo động D3 | $1\text{k}\Omega \pm 5\%$ (0.25W) | Axial bước 7.62mm | 1 | Giới hạn dòng LED D3 ở mức $\approx 3.3\text{mA}$ | 200đ | 200đ | Cửa hàng điện tử |
| 36 | **Q1** | Transistor đệm kích còi | 2N2222 (hoặc SS8050 / C1815 NPN) | TO-92 | 1 | Khóa đóng mở cấp dòng cho còi BZ1 và LED D3 | 1.000đ | 1.000đ | Cửa hàng điện tử |
| 37 | **R6** | Trở định thiên cực Base Q1 | $1\text{k}\Omega \pm 5\%$ (0.25W) | Axial bước 7.62mm | 1 | Hạn dòng từ chân vi điều khiển PB5 vào Base Q1 | 200đ | 200đ | Cửa hàng điện tử |
| 38 | **R5** | Trở xả mass chống trôi (Pull-down)| $10\text{k}\Omega \pm 5\%$ (0.25W) | Axial bước 7.62mm | 1 | Khóa Base Q1 xuống GND tránh còi kêu bíp khi boot STM32| 200đ | 200đ | Cửa hàng điện tử |
| **VI** | | **KHỐI GIAO TIẾP 3 CẢM BIẾN SIÊU ÂM (HC-SR04)** | | | | | | | |
| 39 | **J9** | Header cắm Siêu âm Trái (US1) | Hàng rào đực 1x4P bước 2.54mm (VCC, Trig, Echo, GND) | Header 1x4P | 1 | Cắm dây nối cụm cảm biến HC-SR04 Trái | 500đ | 500đ | Cửa hàng điện tử |
| 40 | **J10** | Header cắm Siêu âm Giữa (US2) | Hàng rào đực 1x4P bước 2.54mm (VCC, Trig, Echo, GND) | Header 1x4P | 1 | Cắm dây nối cụm cảm biến HC-SR04 Giữa | 500đ | 500đ | Cửa hàng điện tử |
| 41 | **J11** | Header cắm Siêu âm Phải (US3) | Hàng rào đực 1x4P bước 2.54mm (VCC, Trig, Echo, GND) | Header 1x4P | 1 | Cắm dây nối cụm cảm biến HC-SR04 Phải | 500đ | 500đ | Cửa hàng điện tử |
| 42 | **C3, C5, C7** | Cụm tụ hóa lọc nguồn 3 siêu âm | $100\mu\text{F} / 16\text{V}$ (3 con) | Radial $\Phi 5\text{mm}$, bước 2.54mm | 3 | Lọc gợn sụt áp cục bộ khi phát xung sóng siêu âm 40kHz | 500đ | 1.500đ | Cửa hàng điện tử |
| 43 | **C4, C6, C8** | Cụm tụ gốm lọc cao tần 3 siêu âm| $100\text{nF} (104) / 50\text{V}$ (3 con) | Gốm tròn bước 2.54mm | 3 | Lọc nhiễu cao tần mắc song song tại chân VCC siêu âm | 300đ | 900đ | Cửa hàng điện tử |
| 44 | **R25, R26, R27**| Cụm trở đệm ngõ phát xung Trig | $330\Omega \pm 5\%$ (0.25W) (3 con) | Axial bước 7.62mm | 3 | Đệm bảo vệ chân PB12, PB14, PB8 khi phát xung Trig | 200đ | 600đ | Cửa hàng điện tử |
| 45 | **R7, R9, R11** | Cụm trở phân áp trên ngõ Echo | $1\text{k}\Omega \pm 1\%$ (3 con) | Axial bước 7.62mm | 3 | Cầu chia áp trên giảm xung Echo từ 5V xuống 3.33V | 200đ | 600đ | Cửa hàng điện tử |
| 46 | **R8, R10, R12**| Cụm trở phân áp dưới ngõ Echo | $2\text{k}\Omega \pm 1\%$ (3 con) | Axial bước 7.62mm | 3 | Cầu chia áp dưới nối GND ($5\text{V} \times \frac{2}{3} = 3.33\text{V}$) về PB13, PB15, PB9 | 200đ | 600đ | Cửa hàng điện tử |
| **VII**| | **KHỐI ĐỌC XUNG QUANG ENCODER (LM393 x 2)** | | | | | | | |
| 47 | **J1** | Header cắm Encoder Trái | Hàng rào đực 1x3P bước 2.54mm (VCC, GND, D0) | Header 1x3P | 1 | Cắm module đĩa quang LM393 đo tốc độ bánh Trái | 500đ | 500đ | Cửa hàng điện tử |
| 48 | **J2** | Header cắm Encoder Phải | Hàng rào đực 1x3P bước 2.54mm (VCC, GND, D0) | Header 1x3P | 1 | Cắm module đĩa quang LM393 đo tốc độ bánh Phải | 500đ | 500đ | Cửa hàng điện tử |
| 49 | **R23, R24** | Trở lọc thông thấp RC Encoder | $330\Omega \pm 5\%$ (2 con) | Axial bước 7.62mm | 2 | Kết hợp tụ C15, C16 lọc nhiễu chổi than vào PB6, PB7 | 200đ | 400đ | Cửa hàng điện tử |
| 50 | **C15, C16** | Tụ gốm lọc thông thấp RC Encoder | $1\text{nF} (102) / 50\text{V}$ (2 con) | Gốm tròn bước 2.54mm | 2 | Cắt tần cao $f_c \approx 482\text{kHz}$ dập tia lửa điện chổi than | 300đ | 600đ | Cửa hàng điện tử |
| **VIII**| | **KHỐI VI ĐIỀU KHIỂN STM32 & GIAO TIẾP MÁY TÍNH** | | | | | | | |
| 51 | **U1** | Bo mạch Vi điều khiển STM32 | STM32F103C8T6 (Blue Pill ARM Cortex-M3 72MHz) | Bo mạch cắm Socket 2x20P | 1 | Điều khiển thời gian thực motor, đọc encoder, siêu âm | Có sẵn | 0đ | Thiết bị có sẵn |
| 52 | **J7** | Header cắm Mạch nạp/UART CP2102| Hàng rào đực 1x4P bước 2.54mm (5V, TX, RX, GND) | Header 1x4P | 1 | Kết nối cổng nạp code và giao tiếp Serial với Pi 4 | 500đ | 500đ | Cửa hàng điện tử |
| 53 | **R28, R29** | Trở đệm đường truyền UART | $330\Omega \pm 5\%$ (2 con) | Axial bước 7.62mm | 2 | Đệm nối tiếp trên PA9 (TX1) và PA10 (RX1) dập phản xạ | 200đ | 400đ | Cửa hàng điện tử |
| **IX** | | **KHỐI KẾT NỐI RASPBERRY PI 4 & MÀN HÌNH OLED** | | | | | | | |
| 54 | **J17** | Cổng kết nối cấp nguồn Raspberry Pi 4 | Header 1x6P hoặc Cọc cắm dây Type-C đực (+5V/PI, GND) | Header 1x6P / Cáp Type-C | 1 | Cấp dòng $5.15\text{V}$ sạch đã bảo vệ sang bo Pi 4 | 5.000đ | 5.000đ | Hshop / Shopee |
| 55 | **J18** | Header cắm Màn hình OLED I2C | Hàng rào đực 1x4P (3.3V/PI, SDA, SCL, GND) | Header 1x4P | 1 | Kết nối màn hình OLED 0.91" I2C hiển thị IP và trạng thái | 500đ | 500đ | Cửa hàng điện tử |
| **X**  | | **LIÊN KẾT MASS CHUNG & PHÔI BO MẠCH IN** | | | | | | | |
| 56 | **NetTie_2** | Điểm chập Mass Star Ground | Đường pad đồng nối tắt trên PCB 2 lớp | Net-Tie Footprint | 1 | Chập duy nhất 1 điểm giữa GND Logic và PGND Motor | 0đ | 0đ | Layout trên PCB |
| 57 | **PCB** | Phôi mạch in PCB 2 lớp gia công | FR4 dày 1.6mm, kích thước $100\text{mm} \times 80\text{mm}$, phủ xanh | 2-Layer FR4 PCB | 1 | Bo mạch in tích hợp toàn diện hệ thống AGV | 45.000đ | 45.000đ | FastPCB / JLCPCB |
| 58 | **SC1-SC4** | Bộ trụ đồng đỡ bo và ốc tán | Trụ đồng lục giác đực-cái M3x10mm kèm ốc vặn | M3 | 4 bộ | Bắt cách điện và cố định chắc chắn bo mạch lên thân xe | 2.000đ | 8.000đ | Cửa hàng linh kiện |
| 59 | **CHG1** | Đốc sạc đôi pin 18650 rời | Cắm điện 220V, tự ngắt khi đầy 4.2V/cell | Thiết bị sạc ngoài xe | 1 | Sạc rời độc lập đảm bảo tuyệt đối an toàn cháy nổ | 28.000đ | 28.000đ | Shopee / Hshop |
| **TỔNG**| | **TỔNG CHI PHÍ VẬT TƯ MẠCH NGUỒN & GIAO TIẾP HOÀN THIỆN:** | | | | | | **333.600đ** | *(Dưới 350.000đ)* |

*(Ghi chú: Toàn bộ các thiết bị giá trị cao như Raspberry Pi 4, STM32 Blue Pill, Camera Logitech C270, 4 Động cơ TT và Màn hình OLED là trang thiết bị có sẵn của nhóm nghiên cứu, không tính vào chi phí mua mới).*
