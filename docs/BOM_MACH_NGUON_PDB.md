# DANH MỤC LINH KIỆN (BOM) MẠCH NGUỒN PDB ĐỒNG BỘ PIN 4S (REV 3.0)
## TÀI LIỆU KỸ THUẬT PHỤC VỤ DỰ TOÁN, THU MUA VÀ THI CÔNG HỆ THỐNG AGV
**Chuẩn hóa 100% theo kiến trúc nguồn Pin 4S ($14.8\text{V} - 16.8\text{V}$) & Chốt nguồn P-MOSFET**  
**Đồ án 2: Robot Vận Chuyển Vật Tư Tự Hành - HCM-UTE**

---

## 1. TỔNG QUAN NÂNG CẤP KIẾN TRÚC BOM 4S (REV 3.0)

So với kiến trúc cũ 2 khay pin rời rạc (3S Logic + 2S Motor gồm 5 cell pin), kiến trúc **Rev 3.0 gom toàn bộ hệ thống về 1 khối Pin 4S 18650 duy nhất ($14.8\text{V} - 16.8\text{V}$)**:
1. **Giảm số cell pin:** Giảm từ 5 cell xuống **4 cell 18650**, chỉ dùng 1 khay pin 4 cell duy nhất, giảm tải trọng và đơn giản hóa việc sạc pin.
2. **Khối chốt nguồn thông minh (Smart Power Latch):** Bổ sung P-MOSFET công suất cao **IRF9540** ($Q_2$), Transistor kích chốt **2N2222** ($Q_3$) và **Diode Zener 12V** ($DZ_1$) kẹp bảo vệ Gate an toàn.
3. **Cơ chế One-Button Power:** 1 nút nhấn mềm nhỏ (`SW1` / `J19`) bật/tắt toàn bộ cả xe (cả Raspberry Pi, STM32 và Động cơ).
4. **Tối ưu hóa mạch và loại bỏ linh kiện thừa:** Bỏ khay pin 2S, bỏ công tắc cơ B, bỏ cọc nguồn `J15`, bỏ cầu trở đo áp 2S (`R13, R14, C10` $\to$ giải phóng chân PA7 STM32). **GIỮ NGUYÊN `NetTie_2` (`NT1`) và net `PGND`** để duy trì kiến trúc chia 2 miền mass độc lập (Mass Logic `GND` và Mass Động lực `PGND`), triệt tiêu 100% xung nhiễu chổi than và hiện tượng sụt áp ground bounce khi động cơ đảo chiều.
5. **Điều tốc động cơ bằng Software-PWM:** Cấp thẳng nguồn 4S vào Driver L298N, khống chế Duty Cycle $\le 55\%$ trên STM32 (điện áp hiệu dụng $\approx 8.1\text{V}$), không cần thêm module Buck hạ áp phần cứng cho động cơ.
6. **Bảo vệ ngõ vào ADC STM32:** Đổi trở phân áp $R_1$ từ $10\text{k}\Omega$ lên **$18\text{k}\Omega \pm 1\%$** (giữ $R_2 = 3.3\text{k}\Omega$) để kẹp áp ngõ vào ADC PA6 tối đa $2.60\text{V} < 3.3\text{V}$ khi pin 4S đầy $16.8\text{V}$.

---

## 2. BẢNG DANH MỤC LINH KIỆN (BOM) CHI TIẾT 100% SCHEMATIC PIN 4S

| STT | Kí hiệu (Designator) | Tên linh kiện / Module | Thông số kỹ thuật chi tiết | Kiểu chân (Footprint) | SL | Vai trò trong hệ thống | Đơn giá | Thành tiền | Nơi mua gợi ý |
|:---:|:---:|---|---|:---:|:---:|---|:---:|:---:|---|
| **I** | | **KHỐI PIN & ĐẦU CẮM NGUỒN VÀO (4S POWER INPUT)** | | | | | | | |
| 1 | **BT1 ÷ BT4** | Cell pin Li-ion 18650 xả cao | Dung lượng 2500mAh, dòng xả $\ge 10\text{A}$ (Sony VTC / Lishen) | Trụ $\Phi 18 \times 65\text{mm}$ | 4 | 4 cell mắc nối tiếp tạo khối nguồn $14.8\text{V}-16.8\text{V}$ | 32.000đ | 128.000đ | Cửa hàng Pin |
| 2 | **BAT_4S** | Khay pin 4 cell 18650 nối tiếp | Khay nhựa có lò xo nối tiếp sẵn, dây ra Đỏ/Đen | Dạng khay 4 slot | 1 | Gá lắp cố định 4 cell 18650 trên sàn xe | 12.000đ | 12.000đ | Hshop / Shopee |
| 3 | **J14** | Domino 2P cấp nguồn 4S | KF301-2P bước 5.08mm, 10A/300V | DIP bước 5.08mm | 1 | Cọc nhận dây Đỏ (+16.8V) và Đen (GND) từ khay pin 4S | 2.000đ | 2.000đ | Cửa hàng điện tử |
| 4 | **CHG_4S** | Đốc sạc 4 khe 18650 rời | Cắm điện 220V, tự ngắt độc lập từng khe khi đầy 4.2V | Thiết bị ngoài xe | 1 | Sạc rời an toàn phòng chống cháy nổ | 45.000đ | 45.000đ | Hshop / Shopee |
| **II**| | **KHỐI CHỐT NGUỒN TỰ ĐỘNG & BẢO VỆ (SMART POWER LATCH)** | | | | | | | |
| 5 | **Q2** | P-MOSFET công tắc nguồn chính | **IRF9540** ($V_{DS}=-100\text{V}, I_D=-19\text{A}, R_{DS(on)}=0.117\Omega$) | TO-220 | 1 | Công tắc bán dẫn ngắt sạch dòng toàn xe ($0\,\mu\text{A}$) | 8.000đ | 8.000đ | Cửa hàng điện tử |
| 6 | **Q3** | Transistor kích chốt giữ nguồn | **S8050** (hoặc 2N2222 NPN, $I_C=800\text{mA}$, 1=E, 2=B, 3=C) | TO-92 | 1 | Ghim cực Gate Q2 qua R13 xuống mức dẫn khi STM32 PB0 lên HIGH | 1.000đ | 1.000đ | Cửa hàng điện tử |
| 7 | **DZ1** | Diode Zener bảo vệ Gate | **1N4742A** (hoặc BZX55C12, $12\text{V} / 1.0\text{W}$) | DO-41 / DO-35 | 1 | Kẹp điện áp $V_{GS}$ của Q2 ở mức an toàn tối đa 12V kẹp giữa Source và Gate | 500đ | 500đ | Cửa hàng điện tử |
| 8 | **SW1** | Nút nhấn mềm bật/tắt onboard | Tact switch 4 chân $6 \times 6 \times 5\text{mm}$ | DIP 6x6mm | 1 | Bấm bật nguồn xe hoặc nhấn giữ để tắt an toàn | 500đ | 500đ | Cửa hàng điện tử |
| 9 | **J19** | Cọc nối nút bấm ngoài vỏ xe | Header 1x2P bước 2.54mm (hoặc KF301-2P) | DIP 2.54mm | 1 | Đấu nối song song SW1 ra nút bấm gắn trên vỏ robot | 500đ | 500đ | Cửa hàng điện tử |
| 10 | **D5** | Diode nhận diện nút bấm tắt | **1N4148** (hoặc 1N4007, Anot vào PB1, Catot vào SW1) | DO-35 / DO-41 | 1 | Cách ly cực Gate Q2 đưa tín hiệu nút bấm về chân STM32 PB1 an toàn | 300đ | 300đ | Cửa hàng điện tử |
| 11 | **R30** | Trở kéo lên Gate P-MOSFET | $10\text{k}\Omega \pm 5\%$ (0.25W) | Axial bước 7.62mm | 1 | Kéo Gate Q2 lên nguồn khóa MOSFET khi xe ở trạng thái tắt | 200đ | 200đ | Cửa hàng điện tử |
| 12 | **R35** | Trở phân áp định thiên Gate | $10\text{k}\Omega \pm 5\%$ (0.25W) | Axial bước 7.62mm | 1 | Nối tiếp Collector Q3 tạo cầu chia áp Gate $V_{GS} = -8.4\text{V}$ | 200đ | 200đ | Cửa hàng điện tử |
| 13 | **R31** | Trở định thiên Base Q3 | $1\text{k}\Omega \pm 5\%$ (0.25W) | Axial bước 7.62mm | 1 | Hạn dòng từ chân vi điều khiển PB0 vào cực Base Q3 | 200đ | 200đ | Cửa hàng điện tử |
| 14 | **R32** | Trở xả mass Base Q3 | $10\text{k}\Omega \pm 5\%$ (0.25W) | Axial bước 7.62mm | 1 | Khóa Base Q3 xuống GND tránh tự kích khi nạp code STM32 | 200đ | 200đ | Cửa hàng điện tử |
| 15 | **R33** | Trở đệm nhận diện Pi Halt | $1\text{k}\Omega \pm 5\%$ (0.25W) | Axial bước 7.62mm | 1 | Đệm bảo vệ chân STM32 PB10 nhận xung từ Pi 4 GPIO 26 | 200đ | 200đ | Cửa hàng điện tử |
| 16 | **R34** | Trở xả mass nhận diện Pi Halt | $10\text{k}\Omega \pm 5\%$ (0.25W) | Axial bước 7.62mm | 1 | Ghìm PB10 xuống 0V chống nhận nhầm tín hiệu khi Pi đang boot | 200đ | 200đ | Cửa hàng điện tử |
| **III**| | **KHỐI HẠ ÁP BUCK & BẢO VỆ CHỐNG BROWNOUT PI 4** | | | | | | | |
| 17 | **U3** (J6, J8) | Module Buck DC-DC XL4015 | Input 8-36V, Output chỉnh 5.15V, dòng đỉnh 5A | Module cắm 4 chân | 1 | Hạ áp từ $14.8\text{V}$ xuống $5.15\text{V}$ sạch nuôi toàn hệ thống | 28.000đ | 28.000đ | Hshop / Shopee |
| 18 | **F1** | Cầu chì tự phục hồi (Polyfuse) | $I_H = 3.0\text{A}, I_{trip} = 5.0\text{A}, V_{max} \ge 16\text{V}$ (PPTC) | Radial bước 5.08mm | 1 | Bảo vệ quá dòng / ngắn mạch ngõ vào Type-C Pi 4 | 2.500đ | 2.500đ | Cửa hàng linh kiện |
| 19 | **D1** | Diode TVS triệt quá áp Pi 4 | **SMAJ6.0A** (SMD) hoặc **1.5KE6.8A** (DIP), $V_{RWM}=6.0V$ | DO-214AC / DO-201 | 1 | Triệt xung transient, kẹp bảo vệ Crowbar OVP cho Pi 4 | 2.000đ | 2.000đ | Cửa hàng linh kiện |
| 20 | **C1** | Tụ hóa bù sụt áp Pi 4 | $2200\mu\text{F} / 16\text{V}$ Low ESR (Chịu nhiệt $105^\circ\text{C}$) | Radial $\Phi 10\text{mm}$, bước 5mm | 1 | Bù áp tức thời khi Pi boost CPU, dập sóng ripple | 3.000đ | 3.000đ | Cửa hàng điện tử |
| 21 | **C2** | Tụ gốm lọc cao tần ngõ ra Pi | $100\text{nF} (104) / 50\text{V}$ | Gốm tròn bước 2.54mm | 1 | Mắc song song C1 triệt nhiễu cao tần ngõ ra 5.15V | 300đ | 300đ | Cửa hàng điện tử |
| 22 | **C13** | Tụ hóa lọc ngõ vào Buck 4S | $100\mu\text{F} / 25\text{V}$ (hoặc 35V) | Radial $\Phi 5\text{mm}$, bước 2.54mm | 1 | Ổn định ngõ vào `4S_V` trước khi vào module XL4015 | 500đ | 500đ | Cửa hàng điện tử |
| 23 | **C14** | Tụ gốm lọc cao tần vào Buck 4S | $100\text{nF} (104) / 50\text{V}$ | Gốm tròn bước 2.54mm | 1 | Triệt xung nhọn điện áp đầu vào Buck XL4015 | 300đ | 300đ | Cửa hàng điện tử |
| **IV** | | **KHỐI ĐỘNG LỰC MOTOR L298N & BẢO VỆ** | | | | | | | |
| 24 | **U2** | Module Driver Động cơ L298N | Module cầu H L298N có tản nhiệt nhôm, chịu áp tới 35V | Cụm module cắm ngoài | 1 | Điều khiển đảo chiều và tốc độ 4 động cơ TT | 32.000đ | 32.000đ | Hshop / Shopee |
| 25 | **J15** (E.STOP) | Cọc Nút dừng khẩn cấp | Cọc cắm KF301-2P hoặc Header 1x2P | DIP bước 5.08mm | 1 | Đấu nối tiếp nút dừng khẩn cấp cơ khí E-Stop ngắt nguồn motor | 2.000đ | 2.000đ | Cửa hàng điện tử |
| 26 | **C11** | Tụ hóa bù sụt áp nguồn Motor | $470\mu\text{F} / 25\text{V}$ (hoặc $35\text{V}$) Low ESR | Radial $\Phi 8\text{mm}$, bước 5mm | 1 | Gánh dòng khởi động motor, dập sụt áp trên đường motor | 1.500đ | 1.500đ | Cửa hàng điện tử |
| 27 | **C12** | Tụ gốm lọc cao tần chổi than | $100\text{nF} (104) / 50\text{V}$ | Gốm tròn bước 2.54mm | 1 | Dập tia lửa điện cao tần từ chổi than motor | 300đ | 300đ | Cửa hàng điện tử |
| 28 | **D4** | LED báo nguồn động lực Motor | LED tròn 3mm hoặc 5mm màu Đỏ | Radial 3mm/5mm | 1 | Sáng báo nguồn động cơ đã sẵn sàng sau nút E-Stop | 500đ | 500đ | Cửa hàng điện tử |
| 29 | **R4** | Trở hạn dòng LED báo Motor | **$3.3\text{k}\Omega \pm 5\%$** (0.25W) | Axial bước 7.62mm | 1 | Hạn dòng LED D4 ở mức an toàn $\approx 4.5\text{mA}$ ở áp 16.8V | 200đ | 200đ | Cửa hàng điện tử |
| 30 | **J12** | Domino 2P cấp nguồn L298N | KF301-2P bước 5.08mm | DIP bước 5.08mm | 1 | Xuất nguồn động lực (`16V/E-STOP`, `PGND`) sang cọc L298N | 2.000đ | 2.000đ | Hshop / Shopee |
| 31 | **J13** | Header 1x6P điều khiển L298N | Hàng rào đực/cái 1x6P bước 2.54mm | Header 1x6P thẳng | 1 | Kết nối 6 tín hiệu: ENA, IN1, IN2, IN3, IN4, ENB | 1.000đ | 1.000đ | Cửa hàng điện tử |
| 32 | **R15 ÷ R20** | Cụm trở đệm GPIO Motor | $330\Omega \pm 5\%$ (0.25W) (6 con) | Axial bước 7.62mm | 6 | Đệm chống sốc ngược từ L298N về các chân STM32 | 200đ | 1.200đ | Cửa hàng điện tử |
| 33 | **R21, R22** | Trở kéo xuống (Pull-down) Enable | $10\text{k}\Omega \pm 5\%$ (0.25W) (2 con) | Axial bước 7.62mm | 2 | Khóa an toàn ENA và ENB mức LOW khi MCU boot/reset | 200đ | 400đ | Cửa hàng điện tử |
| 33b | **NT1** | Cầu chập mass NetTie 2P | **NetTie_2** (Cầu đồng hình sao nối mass) | NetTie-2 / Jumper 0Ω | 1 | Điểm chập Star Ground duy nhất nối giữa GND Logic và PGND Motor tại cọc J14.2 | 0đ | 0đ | Thiết kế PCB |
| 34 | **J16** | Domino 4P ngõ ra Động cơ | KF301-4P (hoặc 2x KF301-2P) | DIP bước 5.08mm | 1 | Nối dây ra 4 động cơ TT: OUT1, OUT2, OUT3, OUT4 | 4.000đ | 4.000đ | Hshop / Shopee |
| 35 | **M1 ÷ M4** | Động cơ giảm tốc DC TT (Vàng) | Điện áp định mức 3-9V, tỷ số truyền 1:48, trục đôi | Dạng động cơ TT kèm đĩa | 4 | Truyền động vi sai 4 bánh xe (FL, FR, BL, BR) | Có sẵn | 0đ | Nhóm nghiên cứu |
| **V**  | | **KHỐI GIÁM SÁT ĐIỆN ÁP PIN 4S (ADC PA6)** | | | | | | | |
| 36 | **R1** | Trở phân áp trên nhánh 4S | **$18\text{k}\Omega \pm 1\%$** (Điện trở chính xác 0.25W) | Axial bước 7.62mm | 1 | Nối từ `4S_V` xuống chân STM32 PA6 | 200đ | 200đ | Cửa hàng điện tử |
| 37 | **R2** | Trở phân áp dưới nhánh 4S | **$3.3\text{k}\Omega \pm 1\%$** (Điện trở chính xác 0.25W) | Axial bước 7.62mm | 1 | Nối từ chân STM32 PA6 xuống mass GND | 200đ | 200đ | Cửa hàng điện tử |
| 38 | **C9** | Tụ lọc nhiễu đọc ADC Pin 4S | $100\text{nF} (104) / 50\text{V}$ | Gốm tròn bước 2.54mm | 1 | Lọc nhiễu cao tần song song R2, ổn định đọc ADC PA6 | 300đ | 300đ | Cửa hàng điện tử |
| **VI** | | **KHỐI BÁO ĐỘNG KÉP ÂM - QUANG (BUZZER & LED)** | | | | | | | |
| 39 | **BZ1** | Còi báo chủ động (Active Buzzer) | Điện áp 5V DC, tần số phát $\approx 2.3\text{kHz}$ ($\Phi 12\text{mm}$) | DIP bước 7.62mm | 1 | Phát tiếng bíp cảnh báo pin yếu, kẹt vật cản | 3.000đ | 3.000đ | Hshop / Shopee |
| 40 | **D2** | Diode dập xung ngược cuộn còi | 1N4148 | DO-35 (Axial DIP) | 1 | Dập xung điện cảm bảo vệ transistor kích còi | 300đ | 300đ | Cửa hàng điện tử |
| 41 | **D3** | LED báo động quang đồng bộ còi | LED tròn 3mm màu Vàng hoặc Đỏ | Radial 3mm | 1 | Chớp sáng đồng bộ khi còi kêu cảnh báo trực quan | 500đ | 500đ | Cửa hàng điện tử |
| 42 | **R3** | Trở hạn dòng LED báo động D3 | $1\text{k}\Omega \pm 5\%$ (0.25W) | Axial bước 7.62mm | 1 | Hạn dòng LED D3 ở mức $\approx 3.3\text{mA}$ | 200đ | 200đ | Cửa hàng điện tử |
| 43 | **Q1** | Transistor kích còi báo | **S8050** (hoặc 2N2222 NPN) | TO-92 | 1 | Khóa đóng mở cấp dòng nuôi còi BZ1 và LED D3 từ chân STM32 PB5 | 1.000đ | 1.000đ | Cửa hàng điện tử |
| 44 | **R6** | Trở định thiên Base Q1 | $1\text{k}\Omega \pm 5\%$ (0.25W) | Axial bước 7.62mm | 1 | Hạn dòng từ chân STM32 PB5 vào Base Q1 | 200đ | 200đ | Cửa hàng điện tử |
| 45 | **R5** | Trở xả mass Base Q1 | $10\text{k}\Omega \pm 5\%$ (0.25W) | Axial bước 7.62mm | 1 | Khóa Base Q1 xuống GND chống còi kêu bíp khi MCU boot | 200đ | 200đ | Cửa hàng điện tử |
| **VII**| | **KHỐI GIAO TIẾP 3 CẢM BIẾN SIÊU ÂM (HC-SR04)** | | | | | | | |
| 46 | **J9 ÷ J11** | Header cắm 3 Cảm biến Siêu âm | Hàng rào đực 1x4P bước 2.54mm (VCC, Trig, Echo, GND) | Header 1x4P | 3 | Cắm cụm cảm biến siêu âm Trái (US1), Giữa (US2), Phải (US3)| 500đ | 1.500đ | Cửa hàng điện tử |
| 47 | **C3, C5, C7** | Cụm tụ hóa lọc nguồn 3 siêu âm | $100\mu\text{F} / 16\text{V}$ (3 con) | Radial $\Phi 5\text{mm}$, bước 2.54mm | 3 | Lọc bù sụt áp cục bộ khi phát xung sóng siêu âm 40kHz | 500đ | 1.500đ | Cửa hàng điện tử |
| 48 | **C4, C6, C8** | Cụm tụ gốm lọc cao tần 3 siêu âm| $100\text{nF} (104) / 50\text{V}$ (3 con) | Gốm tròn bước 2.54mm | 3 | Lọc nhiễu cao tần mắc song song chân VCC cảm biến | 300đ | 900đ | Cửa hàng điện tử |
| 49 | **R25 ÷ R27** | Cụm trở đệm ngõ phát xung Trig | $330\Omega \pm 5\%$ (3 con) | Axial bước 7.62mm | 3 | Đệm bảo vệ chân PB12, PB14, PB8 khi phát xung Trig | 200đ | 600đ | Cửa hàng điện tử |
| 50 | **R7, R9, R11** | Cụm trở phân áp trên ngõ Echo | $1\text{k}\Omega \pm 1\%$ (3 con) | Axial bước 7.62mm | 3 | Cầu chia áp trên giảm xung Echo từ 5V xuống 3.33V | 200đ | 600đ | Cửa hàng điện tử |
| 51 | **R8, R10, R12**| Cụm trở phân áp dưới ngõ Echo | $2\text{k}\Omega \pm 1\%$ (3 con) | Axial bước 7.62mm | 3 | Cầu chia áp dưới nối GND ($5\text{V} \times \frac{2}{3} = 3.33\text{V}$) về STM32 | 200đ | 600đ | Cửa hàng điện tử |
| **VIII**| | **KHỐI ĐỌC XUNG QUANG ENCODER (LM393 x 2)** | | | | | | | |
| 52 | **J1, J2** | Header cắm 2 Encoder LM393 | Hàng rào đực 1x3P bước 2.54mm (VCC, GND, D0) | Header 1x3P | 2 | Cắm module đĩa quang LM393 đo tốc độ 2 bánh Trái/Phải | 500đ | 1.000đ | Cửa hàng điện tử |
| 53 | **R23, R24** | Trở lọc thông thấp RC Encoder | $330\Omega \pm 5\%$ (2 con) | Axial bước 7.62mm | 2 | Kết hợp tụ C15, C16 lọc nhiễu chổi than vào PB6, PB7 | 200đ | 400đ | Cửa hàng điện tử |
| 54 | **C15, C16** | Tụ gốm lọc thông thấp RC Encoder | $1\text{nF} (102) / 50\text{V}$ (2 con) | Gốm tròn bước 2.54mm | 2 | Cắt tần cao $f_c \approx 482\text{kHz}$ dập xung tia lửa chổi than | 300đ | 600đ | Cửa hàng điện tử |
| **IX** | | **KHỐI VI ĐIỀU KHIỂN STM32 & GIAO TIẾP MÁY TÍNH** | | | | | | | |
| 55 | **U1** | Bo vi điều khiển STM32F103C8T6 | Blue Pill ARM Cortex-M3 72MHz, 64KB Flash | Bo mạch cắm Socket 2x20P | 1 | Điều khiển thời gian thực motor, đọc encoder, siêu âm | Có sẵn | 0đ | Nhóm nghiên cứu |
| 56 | **J7** | Header cắm Module CP2102 | Hàng rào đực 1x4P bước 2.54mm (5V, TX, RX, GND) | Header 1x4P | 1 | Kết nối nạp code và giao tiếp Serial UART với Pi 4 | 500đ | 500đ | Cửa hàng điện tử |
| 57 | **R28, R29** | Trở đệm đường truyền UART | $330\Omega \pm 5\%$ (2 con) | Axial bước 7.62mm | 2 | Đệm nối tiếp trên PA9 (TX1) và PA10 (RX1) dập phản xạ | 200đ | 400đ | Cửa hàng điện tử |
| **X**  | | **KHỐI KẾT NỐI RASPBERRY PI 4 & MÀN HÌNH OLED** | | | | | | | |
| 58 | **J17** | Cổng cấp nguồn & Bắt tay Pi 4 | Header 1x7P hoặc Cáp Type-C kèm cọc GPIO26 | Header 1x7P / Cáp Type-C | 1 | Cấp dòng 5.15V sạch và nhận tín hiệu Pi Halt về PB10 | 5.000đ | 5.000đ | Hshop / Shopee |
| 59 | **J18** | Header cắm Màn hình OLED I2C | Hàng rào đực 1x4P (3.3V/PI, SDA, SCL, GND) | Header 1x4P | 1 | Kết nối màn hình OLED 0.91" I2C hiển thị IP và trạng thái | 500đ | 500đ | Cửa hàng điện tử |
| **XI** | | **PHÔI BO MẠCH IN & PHỤ KIỆN GÁ LẮP** | | | | | | | |
| 60 | **PCB** | Phôi mạch in PCB 2 lớp gia công | FR4 dày 1.6mm, kích thước gọn $95\text{mm} \times 75\text{mm}$, phủ xanh | 2-Layer FR4 PCB | 1 | Bo mạch in tích hợp toàn diện hệ thống AGV 4S | 45.000đ | 45.000đ | FastPCB / JLCPCB |
| 61 | **SC1 ÷ SC4** | Bộ trụ đồng đỡ bo và ốc tán | Trụ đồng lục giác đực-cái M3x10mm kèm ốc vặn | M3 | 4 bộ | Bắt cách điện và cố định chắc chắn bo mạch lên thân xe | 2.000đ | 8.000đ | Cửa hàng linh kiện |
| **TỔNG**| | **TỔNG CHI PHÍ VẬT TƯ MẠCH NGUỒN PDB 4S HOÀN THIỆN:** | | | | | | **315.800đ** | *(Rẻ hơn mạch cũ 18.000đ)*|

*(Ghi chú: Raspberry Pi 4 Model B, Camera Logitech C270, Bo STM32 Blue Pill, 4 Động cơ TT và Màn hình OLED là trang thiết bị có sẵn của nhóm nghiên cứu, không tính vào chi phí mua mới).*
