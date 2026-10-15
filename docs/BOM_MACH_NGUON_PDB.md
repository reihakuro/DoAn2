# BẢNG DANH MỤC LINH KIỆN MẠCH NGUỒN (BILL OF MATERIALS - BOM)
## BO MẠCH PHÂN PHỐI NGUỒN & BẢO VỆ CHỐNG BROWNOUT (POWER DISTRIBUTION BOARD - PDB)
### DỰ ÁN: XE TỰ HÀNH VẬN CHUYỂN LINH KIỆN NỘI BỘ (AGV - ĐỒ ÁN 2)

---

## 1. TỔNG QUAN KIẾN TRÚC MẠCH NGUỒN TÍCH HỢP (PDB)

Bo mạch phân phối nguồn (**Power Distribution Board - PDB**) tích hợp toàn bộ cơ cấu nguồn 2 nhánh cách ly vào **1 mạch in PCB duy nhất**, giải quyết triệt để vấn đề dây nối lộn xộn, sụt áp tiếp xúc và xung nhiễu chổi than từ động cơ.

### Các khối chức năng tích hợp trên PCB:
1. **Khối Đầu Vào Nguồn (Dual Power Input):** Cọc đấu nối Pin 3S ($11.1\text{V}$) và Pin 2S ($7.4\text{V}$), chân nối Công tắc A, Công tắc B và nút Dừng khẩn cấp (E-Stop).
2. **Khối Ổn Áp Sạch 5.15V (Logic Buck Regulator):** Socket cắm module XL4015 (5A) kèm tụ hóa dập xung transient $2200\mu\text{F}$ và tụ gốm $100\text{nF}$.
3. **Khối Bảo Vệ Đa Lớp:** Cầu chì tự phục hồi Polyfuse 3A chống ngắn mạch + Diode TVS 5V chống quá áp ngõ vào Raspberry Pi 4.
4. **Khối Đo Điện Áp Pin (Battery Voltage Sensing):** Cầu phân áp $10\text{k}\Omega / 3.3\text{k}\Omega$ sai số 1% đưa về ADC STM32.
5. **Khối Phân Phối Nguồn Tải Sạch (Clean Distribution):**
   * Cổng ra $5.15\text{V}$ cấp Type-C Pi 4.
   * Header cấp $5.15\text{V}$ nuôi STM32 Blue Pill.
   * 3 cụm header cắm 3 Cảm biến Siêu âm HC-SR04 (tích hợp sẵn tụ lọc $100\mu\text{F} + 100\text{nF}$ cho từng con).
6. **Khối Còi Báo Động (Active Buzzer 5V):** Còi chip 5V + mạch đệm transistor NPN 2N2222 + diode dập xung ngược.
7. **Khối Ngõ Ra Động Lực (Motor Power Output):** Cọc cắm ra $7.4\text{V}$ (sau E-Stop) cấp thẳng chân 12V/VM của Driver L298N / TB6612FNG.
8. **Khối Đèn Báo Trạng Thái (LED Indicators):** 1 LED xanh lá (Logic 5V OK) và 1 LED đỏ (Motor 7.4V OK).

---

## 2. BẢNG BOM CHI TIẾT KÈM KÍ HIỆU LINH KIỆN (DESIGNATOR)

| STT | Kí hiệu (Designator) | Tên linh kiện / Module | Thông số kỹ thuật chi tiết | Kiểu đóng gói (Footprint) | SL | Vai trò / Khối chức năng | Đơn giá | Thành tiền | Nơi mua gợi ý |
|:---:|:---:|---|---|:---:|:---:|---|:---:|:---:|---|
| **I** | | **KHỐI ĐẦU VÀO & CÔNG TẮC** | | | | | | | |
| 1 | **J1** | **Domino 2P (Pin 3S Input)** | KF301-2P (5.08mm), chịu dòng 10A/300V | DIP / Hole 5.08mm | 1 | Cọc cắm khay pin 3S (11.1V danh định) | 2.000đ | 2.000đ | Shopee / Hshop |
| 2 | **J2** | **Domino 2P (Pin 2S Input)** | KF301-2P (5.08mm), chịu dòng 10A/300V | DIP / Hole 5.08mm | 1 | Cọc cắm khay pin 2S (7.4V danh định) | 2.000đ | 2.000đ | Shopee / Hshop |
| 3 | **J3** | **Domino 2P (Công tắc A)** | KF301-2P (hoặc Header XH2.54-2P) | DIP / Hole 5.08mm | 1 | Chân cắm dây ra công tắc cơ bật/tắt Logic | 2.000đ | 2.000đ | Shopee / Hshop |
| 4 | **J4** | **Domino 2P (Công tắc B)** | KF301-2P (hoặc Header XH2.54-2P) | DIP / Hole 5.08mm | 1 | Chân cắm dây ra công tắc cơ bật/tắt Motor | 2.000đ | 2.000đ | Shopee / Hshop |
| 5 | **J5** | **Domino 2P (Nút E-Stop)** | KF301-2P bước 5.08mm | DIP / Hole 5.08mm | 1 | Chân cắm dây ra nút dừng khẩn cấp E-Stop | 2.000đ | 2.000đ | Shopee / Hshop |
| **II** | | **KHỐI HẠ ÁP & BẢO VỆ CHỐNG BROWNOUT** | | | | | | | |
| 6 | **U1 / MOD1**| **Module Buck DC-DC XL4015** | Input 8-36V, Output chỉnh 5.15V, 5A | Socket cắm 4 chân (IN+, IN-, OUT+, OUT-) | 1 | Hạ áp từ 11.1V xuống 5.15V nuôi Pi 4 & MCU | 28.000đ | 28.000đ | Shopee / Hshop |
| 7 | **J6A, J6B** | **Rào chân cái (Socket XL4015)**| Hàng rào cái 1x2 chân bước 2.54mm (hoặc 1x4) | SIP-2 / SIP-4 | 2 | Chân đế socket cắm module XL4015 tháo rời | 1.000đ | 2.000đ | Cửa hàng điện tử |
| 8 | **F1** | **Cầu chì tự phục hồi (Polyfuse)**| $I_H = 3.0\text{A}, V_{max} \ge 16\text{V}, I_{trip} \approx 5.0\text{A}$ | Radial DIP bước 5.08mm | 1 | Chống quá dòng / ngắn mạch vào Type-C Pi 4 | 2.500đ | 2.500đ | Shopee / Nshop |
| 9 | **D1** | **Diode TVS triệt quá áp 5V** | SMAJ5.0A (SMD) hoặc 1.5KE6.8A (DIP) | SMA hoặc DO-201 | 1 | Dập xung nhọn quá áp bảo vệ Pi 4 | 2.000đ | 2.000đ | Shopee / Cửa hàng |
| **III**| | **KHỐI TỤ LỌC NHIỄU & DẬP TRANSIENT** | | | | | | | |
| 10 | **C1** | **Tụ hóa lớn dập sụt áp Pi 4** | $2200\mu\text{F} / 16\text{V}$ (Low ESR) | Radial $\Phi 10\text{mm}$, bước 5mm | 1 | Bù dòng sụt áp tức thời khi Pi boost CPU | 3.000đ | 3.000đ | Cửa hàng điện tử |
| 11 | **C2** | **Tụ gốm dập xung cao tần Pi 4** | $100\text{nF} (0.1\mu\text{F} - 104) / 50\text{V}$ | Gốm tròn bước 2.54mm | 1 | Lọc nhiễu cao tần ngõ ra 5.15V song song C1 | 300đ | 300đ | Cửa hàng điện tử |
| 12 | **C3, C5, C7**| **Tụ hóa lọc 3 Cảm biến Siêu âm**| $100\mu\text{F} / 16\text{V}$ (3 con) | Radial $\Phi 5\text{mm}$, bước 2.54mm | 3 | Lọc gợn nguồn VCC cho 3 HC-SR04 | 500đ | 1.500đ | Cửa hàng điện tử |
| 13 | **C4, C6, C8**| **Tụ gốm lọc 3 Cảm biến Siêu âm**| $100\text{nF} (104) / 50\text{V}$ (3 con) | Gốm tròn bước 2.54mm | 3 | Lọc nhiễu cao tần cho 3 HC-SR04 | 300đ | 900đ | Cửa hàng điện tử |
| **IV** | | **KHỐI GIÁM SÁT PIN & CẢNH BÁO ÂM THANH** | | | | | | | |
| 14 | **R1** | **Điện trở phân áp nhánh trên** | $10\text{k}\Omega \pm 1\%$ (0.25W kim loại) | Axial bước 7.62mm | 1 | Kéo từ Pin 3S(+) xuống node ADC_BAT | 200đ | 200đ | Cửa hàng điện tử |
| 15 | **R2** | **Điện trở phân áp nhánh dưới** | $3.3\text{k}\Omega \pm 1\%$ (0.25W kim loại) | Axial bước 7.62mm | 1 | Kéo từ node ADC_BAT xuống GND | 200đ | 200đ | Cửa hàng điện tử |
| 16 | **BZ1**| **Còi chip báo động (Active Buzzer)**| Còi chíp chủ động 5V DC ($\Phi 12\text{mm}$) | DIP chân 7.62mm | 1 | Phát âm thanh cảnh báo pin yếu & dừng khẩn | 3.000đ | 3.000đ | Shopee / Hshop |
| 17 | **Q1** | **Transistor NPN kích còi** | 2N2222 (hoặc SS8050 / C1815) | TO-92 | 1 | Mạch đệm khóa mở còi từ chân STM32 PB5 | 1.000đ | 1.000đ | Cửa hàng điện tử |
| 18 | **R3** | **Điện trở định thiên cực Base**| $1\text{k}\Omega \pm 5\%$ (0.25W) | Axial bước 7.62mm | 1 | Hạn dòng chân vi điều khiển PB5 vào Base Q1 | 200đ | 200đ | Cửa hàng điện tử |
| 19 | **D2** | **Diode dập xung ngược cuộn còi**| 1N4148 (hoặc 1N4007) | DO-35 (DIP) | 1 | Bảo vệ Transistor Q1 chống áp phản kháng | 300đ | 300đ | Cửa hàng điện tử |
| **V**  | | **KHỐI PHÂN PHỐI NGUỒN NGÕ RA (OUTPUT CONNECTORS)**| | | | | | | |
| 20 | **J7** | **Domino 2P (Nguồn Pi 4 Out)** | KF301-2P (5.08mm) | DIP bước 5.08mm | 1 | Cọc ra 5.15V hàn cáp ngắn sang Type-C Pi 4 | 2.000đ | 2.000đ | Shopee / Hshop |
| 21 | **J8** | **Header 2P (Nguồn STM32 Out)** | Hàng rào đực bước 2.54mm (5.15V, GND) | Header 1x2 | 1 | Cắm dây bus Dupont sang chân 5V STM32 | 500đ | 500đ | Cửa hàng điện tử |
| 22 | **J9** | **Header 4P (Siêu âm Trái - US1)**| Hàng rào đực bước 2.54mm (VCC, GND, Trig, Echo) | Header 1x4 | 1 | Cắm cáp nối HC-SR04 góc Trái | 500đ | 500đ | Cửa hàng điện tử |
| 23 | **J10**| **Header 4P (Siêu âm Giữa - US2)**| Hàng rào đực bước 2.54mm (VCC, GND, Trig, Echo) | Header 1x4 | 1 | Cắm cáp nối HC-SR04 Giữa | 500đ | 500đ | Cửa hàng điện tử |
| 24 | **J11**| **Header 4P (Siêu âm Phải - US3)**| Hàng rào đực bước 2.54mm (VCC, GND, Trig, Echo) | Header 1x4 | 1 | Cắm cáp nối HC-SR04 góc Phải | 500đ | 500đ | Cửa hàng điện tử |
| 25 | **J12**| **Domino 2P (Nguồn Motor VM Out)**| KF301-2P (5.08mm) | DIP bước 5.08mm | 1 | Cọc ra 7.4V (sau E-Stop) cấp chân 12V/VM L298N | 2.000đ | 2.000đ | Shopee / Hshop |
| 26 | **J13**| **Header Tín hiệu kết nối STM32**| Hàng rào đực 1x8 chân bước 2.54mm | Header 1x8 | 1 | Gom các chân ADC, Buzzer, Trig/Echo về STM32 | 1.000đ | 1.000đ | Cửa hàng điện tử |
| **VI** | | **KHỐI ĐÈN BÁO TRẠNG THÁI (LED INDICATORS)** | | | | | | | |
| 27 | **LED1**| **LED báo nguồn Logic 5V** | LED tròn 3mm màu Xanh lá (Green) | Radial 3mm | 1 | Sáng báo nguồn 5.15V nhánh A hoạt động tốt | 500đ | 500đ | Cửa hàng điện tử |
| 28 | **R4** | **Trở hạn dòng LED 5V** | $1\text{k}\Omega \pm 5\%$ (0.25W) | Axial bước 7.62mm | 1 | Giới hạn dòng LED1 ở mức $\approx 3.3\text{mA}$ | 200đ | 200đ | Cửa hàng điện tử |
| 29 | **LED2**| **LED báo nguồn Motor 7.4V** | LED tròn 3mm màu Đỏ (Red) | Radial 3mm | 1 | Sáng báo nguồn 7.4V nhánh B (sau E-Stop) | 500đ | 500đ | Cửa hàng điện tử |
| 30 | **R5** | **Trở hạn dòng LED 7.4V** | $1.5\text{k}\Omega \pm 5\%$ (0.25W) | Axial bước 7.62mm | 1 | Giới hạn dòng LED2 ở mức $\approx 3.6\text{mA}$ | 200đ | 200đ | Cửa hàng điện tử |
| **VII**| | **PHÔI MẠCH IN PCB & GIA CÔNG CỐ ĐỊNH** | | | | | | | |
| 31 | **PCB** | **Phôi mạch in PCB 2 lớp** | Kích thước: $80\text{mm} \times 60\text{mm}$, FR4 dày $1.6\text{mm}$ | 2-Layer FR4 | 1 | Bo mạch in gia công CNC / FastPCB / JLCPCB | 35.000đ | 35.000đ | FastPCB / JLCPCB / Tự làm |
| 32 | **SC1-4**| **Bộ ốc vít & Trụ đồng đỡ bo**| Trụ đồng lục giác M3x10mm kèm ốc tán | M3x10mm | 4 | Bắt cố định mạch PDB lên sàn mica robot | 2.000đ | 8.000đ | Shopee / Hshop |

---

## 3. TỔNG KẾT CHI PHÍ MẠCH NGUỒN PDB

* **Tổng số loại linh kiện:** 32 mục (đã gán định danh từ `J1` đến `J13`, `R1`-`R5`, `C1`-`C8`, `D1`-`D2`, `Q1`, `F1`, `BZ1`, `LED1`-`LED2`).
* **Tổng chi phí linh kiện trên bo:** $\approx 64.000\text{đ}$
* **Chi phí gia công phôi mạch PCB + Trụ đồng M3:** $\approx 43.000\text{đ}$
* **TỔNG KINH PHÍ HOÀN THIỆN BO NGUỒN (Dự toán thực tế):** **$\approx 107.000\text{đ}$**
