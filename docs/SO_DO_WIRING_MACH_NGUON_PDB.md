# SƠ ĐỒ WIRING & NETLIST ĐẤU DÂY BO MẠCH NGUỒN (PDB)
## POWER DISTRIBUTION BOARD - ROBOT VẬN CHUYỂN NỘI BỘ (AGV)
### TÀI LIỆU KỸ THUẬT PHỤC VỤ VẼ SCHEMATIC VÀ LAYOUT PCB (ALTIUM / EASYEDA / KICAD)

---

## 1. BẢNG NETLIST TOÀN BỘ MẠCH (THEO KÍ HIỆU BOM)

Bảng dưới đây liệt kê tất cả các đường mạch (**Nets**) trên PCB, kết nối chính xác giữa các chân (**Pins**) của từng linh kiện theo đúng định danh trong Bảng BOM:

| Tên Net | Chức năng tín hiệu | Danh sách các chân linh kiện kết nối vào Net (Wiring Nodes) | Đặc tính đường dây (Layout Rule) |
|---|---|---|---|
| **`BAT3S_RAW`** | Cực dương thô Pin 3S ($9.0\text{V} - 12.6\text{V}$) | • `J1.1` (Cực + Pin 3S)<br>• `J3.1` (Chân vào Công tắc A)<br>• `R1.1` (Chân 1 trở phân áp 10k) | Độ rộng trace $\ge 1.5\text{mm}$, dòng $2\text{A}$ |
| **`SW_A_OUT`** | Dương nguồn 3S sau Công tắc A | • `J3.2` (Chân ra Công tắc A)<br>• `MOD1.IN+` (`J6A.1` - Chân IN+ module XL4015) | Độ rộng trace $\ge 1.5\text{mm}$, chịu dòng $2\text{A}$ |
| **`VCC_5V15`** | Ngõ ra 5.15V DC sạch từ XL4015 | • `MOD1.OUT+` (`J6B.1` - Chân OUT+ module XL4015)<br>• `C1.(+)` (Cực dương tụ hóa $2200\mu\text{F}$)<br>• `C2.1` (Tụ gốm 100nF)<br>• `F1.1` (Chân vào Cầu chì Polyfuse 3A)<br>• `J8.1` (Chân 5V cấp sang STM32)<br>• `J9.1` (Chân VCC Siêu âm Trái) + `C3.(+)` + `C4.1`<br>• `J10.1` (Chân VCC Siêu âm Giữa) + `C5.(+)` + `C6.1`<br>• `J11.1` (Chân VCC Siêu âm Phải) + `C7.(+)` + `C8.1`<br>• `BZ1.(+)` (Cực dương Còi chip 5V)<br>• `D2.K` (Cathode Diode 1N4148 dập xung còi)<br>• `R4.1` (Trở $1\text{k}\Omega$ hạn dòng LED 5V) | Phủ đồng Copper Pour hoặc trace $\ge 2.0\text{mm}$ |
| **`PI_5V`** | Nguồn 5.15V bảo vệ cấp cổng Type-C Pi 4 | • `F1.2` (Chân ra Cầu chì Polyfuse 3A)<br>• `D1.K` (Cathode Diode TVS 5V SMAJ5.0A)<br>• `J7.1` (Cực dương ngõ ra cắm Pi 4) | Độ rộng trace $\ge 2.5\text{mm}$, cạo lớp mask tráng thiếc hàn |
| **`BAT2S_RAW`** | Cực dương thô Pin 2S ($6.0\text{V} - 8.4\text{V}$) | • `J2.1` (Cực + Pin 2S)<br>• `J4.1` (Chân vào Công tắc B) | Độ rộng trace $\ge 2.5\text{mm}$, dòng đỉnh $3\text{A}$ |
| **`SW_B_OUT`** | Dương nguồn 2S sau Công tắc B | • `J4.2` (Chân ra Công tắc B)<br>• `J5.1` (Chân vào nút Dừng khẩn cấp E-Stop) | Độ rộng trace $\ge 2.5\text{mm}$ |
| **`VM_7V4`** | Nguồn động lực cấp Motor (sau E-Stop) | • `J5.2` (Chân ra nút E-Stop)<br>• `J12.1` (Cọc ra cấp chân 12V/VM Driver L298N)<br>• `R5.1` (Trở $1.5\text{k}\Omega$ hạn dòng LED 7.4V) | Độ rộng trace $\ge 3.0\text{mm}$, cạo lớp mask tráng thiếc hàn |
| **`ADC_BAT3S`** | Cầu chia áp đo dung lượng Pin 3S | • `R1.2` (Chân 2 trở 10k)<br>• `R2.1` (Chân 1 trở 3.3k)<br>• `J13.1` (Chân nối về chân PA6 ADC của STM32) | Trace tín hiệu $0.3\text{mm} - 0.4\text{mm}$, tránh xa đường motor |
| **`BUZZER_CTRL`** | Lệnh kích còi từ STM32 | • `J13.2` (Chân nối từ chân PB5 của STM32)<br>• `R3.1` (Chân vào trở $1\text{k}\Omega$) | Trace tín hiệu $0.3\text{mm}$ |
| **`BUZZER_DRIVE`**| Chân điều khiển cuộn còi | • `R3.2` (Nối vào cực Base transistor `Q1.B`)<br>• `Q1.C` (Cực Collector Q1)<br>• `BZ1.(-)` (Cực âm Còi Buzzer)<br>• `D2.A` (Anode Diode 1N4148) | Trace $0.5\text{mm}$ |
| **`LED_5V_NET`** | Đèn báo nguồn 5.15V Logic | • `R4.2` (Chân ra trở 1k)<br>• `LED1.A` (Anode LED xanh lá) | Trace $0.4\text{mm}$ |
| **`LED_VM_NET`** | Đèn báo nguồn 7.4V Motor | • `R5.2` (Chân ra trở 1.5k)<br>• `LED2.A` (Anode LED đỏ) | Trace $0.4\text{mm}$ |
| **`US1_TRIG`** | Xung kích Siêu âm Trái | • `J9.3` (Chân Trig cảm biến US1)<br>• `J13.3` (Nối về chân PB12 của STM32) | Trace tín hiệu $0.3\text{mm}$ |
| **`US1_ECHO`** | Xung phản hồi Siêu âm Trái | • `J9.4` (Chân Echo cảm biến US1)<br>• `J13.4` (Nối về chân PB13 của STM32) | Trace tín hiệu $0.3\text{mm}$ |
| **`US2_TRIG`** | Xung kích Siêu âm Giữa | • `J10.3` (Chân Trig cảm biến US2)<br>• `J13.5` (Nối về chân PB14 của STM32) | Trace tín hiệu $0.3\text{mm}$ |
| **`US2_ECHO`** | Xung phản hồi Siêu âm Giữa | • `J10.4` (Chân Echo cảm biến US2)<br>• `J13.6` (Nối về chân PB15 của STM32) | Trace tín hiệu $0.3\text{mm}$ |
| **`US3_TRIG`** | Xung kích Siêu âm Phải | • `J11.3` (Chân Trig cảm biến US3)<br>• `J13.7` (Nối về chân PB8 của STM32) | Trace tín hiệu $0.3\text{mm}$ |
| **`US3_ECHO`** | Xung phản hồi Siêu âm Phải | • `J11.4` (Chân Echo cảm biến US3)<br>• `J13.8` (Nối về chân PB9 của STM32) | Trace tín hiệu $0.3\text{mm}$ |
| **`GND`** | **Mặt phẳng Mass Chung (Star Ground)** | • `J1.2` (Cực âm Pin 3S)<br>• `J2.2` (Cực âm Pin 2S)<br>• `MOD1.IN-` (`J6A.2`) & `MOD1.OUT-` (`J6B.2`)<br>• `C1.(-)` & `C2.2`<br>• `D1.A` (Anode Diode TVS)<br>• `J7.2` (GND cấp Pi 4)<br>• `J8.2` (GND cấp STM32)<br>• `J9.2` (GND US1) + `C3.(-)` + `C4.2`<br>• `J10.2` (GND US2) + `C5.(-)` + `C6.2`<br>• `J11.2` (GND US3) + `C7.(-)` + `C8.2`<br>• `J12.2` (GND cấp Driver L298N)<br>• `R2.2` (Chân âm trở phân áp ADC)<br>• `Q1.E` (Cực Emitter Transistor Q1)<br>• `LED1.K` (Cathode LED xanh lá)<br>• `LED2.K` (Cathode LED đỏ) | **Đổ đồng toàn bộ mặt đáy (GND Plane / Polygon Pour)** |

---

## 2. BẢNG ĐẤU NỐI CHI TIẾT TỪNG CHÂN (PIN-TO-PIN CONNECTION)

### A. Nhóm Cổng Nối Đầu Vào (J1 - J5)
* **`J1` (Cọc cắm Pin 3S):**
  * `Pin 1` (Dương Pin 3S): Nối sang `J3.1` (Công tắc A) và `R1.1` (Đo áp).
  * `Pin 2` (Âm Pin 3S): Nối vào `GND` (Star Ground).
* **`J2` (Cọc cắm Pin 2S):**
  * `Pin 1` (Dương Pin 2S): Nối sang `J4.1` (Công tắc B).
  * `Pin 2` (Âm Pin 2S): Nối vào `GND` (Star Ground).
* **`J3` (Chân cắm Công tắc cơ A - Logic ON/OFF):**
  * `Pin 1`: Nhận `BAT3S_RAW` từ `J1.1`.
  * `Pin 2`: Xuất `SW_A_OUT` sang chân `IN+` của Module XL4015 (`J6A.1`).
* **`J4` (Chân cắm Công tắc cơ B - Motor ON/OFF):**
  * `Pin 1`: Nhận `BAT2S_RAW` từ `J2.1`.
  * `Pin 2`: Xuất `SW_B_OUT` sang chân vào của nút dừng khẩn cấp E-Stop (`J5.1`).
* **`J5` (Chân cắm Nút Dừng Khẩn Cấp E-Stop):**
  * `Pin 1`: Nhận `SW_B_OUT` từ `J4.2`.
  * `Pin 2`: Xuất `VM_7V4` sang cọc cấp Driver L298N (`J12.1`) và trở `R5.1` (LED đỏ).

---

### B. Khối Hạ Áp XL4015 & Lọc Nguồn, Bảo Vệ
* **`MOD1` (Socket cắm Module Buck XL4015):**
  * Chân `IN+` (`J6A.1`): Nhận điện áp từ `J3.2` (`SW_A_OUT`).
  * Chân `IN-` (`J6A.2`): Nối `GND`.
  * Chân `OUT+` (`J6B.1`): Xuất điện áp $5.15\text{V}$ (`VCC_5V15`).
  * Chân `OUT-` (`J6B.2`): Nối `GND`.
* **Cụm tụ lọc ngõ ra XL4015 (`C1` & `C2`):**
  * Tụ hóa `C1` ($2200\mu\text{F}/16\text{V}$): Cực `(+)` nối `VCC_5V15`, Cực `(-)` nối `GND`.
  * Tụ gốm `C2` ($100\text{nF}$): Chân 1 nối `VCC_5V15`, Chân 2 nối `GND` (đặt sát chân C1).
* **Bảo vệ ngõ ra Pi 4 (`F1` & `D1`):**
  * Cầu chì tự phục hồi `F1` (Polyfuse 3A): Chân 1 nối `VCC_5V15`, Chân 2 nối `PI_5V` (`J7.1`).
  * Diode TVS `D1` (SMAJ5.0A): Cathode (vạch trắng) nối `PI_5V` (`J7.1`), Anode nối `GND`.
* **`J7` (Cọc ra cấp nguồn cho Raspberry Pi 4):**
  * `Pin 1` ($5.15\text{V}$ đã bảo vệ): Nhận `PI_5V` từ sau F1.
  * `Pin 2` (GND): Nối `GND`.
  *(Đầu dây từ J7 sẽ hàn trực tiếp vào giắc đực USB Type-C cắm vào cổng nguồn của Pi 4)*.

---

### C. Khối Giám Sát Điện Áp Pin 3S & Còi Buzzer
* **Cầu chia áp đo dung lượng Pin 3S (`R1` & `R2`):**
  * `R1` ($10\text{k}\Omega$ 1%): Chân 1 nối cực dương Pin 3S (`J1.1`), Chân 2 nối node `ADC_BAT3S`.
  * `R2` ($3.3\text{k}\Omega$ 1%): Chân 1 nối node `ADC_BAT3S`, Chân 2 nối `GND`.
  * Node `ADC_BAT3S` nối sang chân `J13.1` (kéo về chân `PA6` của STM32).
* **Mạch kích còi báo động (`BZ1`, `Q1`, `R3`, `D2`):**
  * Còi chip `BZ1`: Cực `(+)` nối `VCC_5V15`, Cực `(-)` nối Collector của Transistor `Q1.C`.
  * Diode dập xung ngược `D2` (1N4148): Mắc song song ngược với còi (Cathode nối `VCC_5V15`, Anode nối `Q1.C`).
  * Transistor `Q1` (2N2222): 
    * Cực `Collector`: Nối cực `(-)` còi BZ1.
    * Cực `Base`: Nối qua trở `R3` ($1\text{k}\Omega$). Chân còn lại của R3 nối sang `J13.2` (nhận tín hiệu `PB5` từ STM32).
    * Cực `Emitter`: Nối `GND`.

---

### D. Khối Phân Phối Cảm Biến & Động Lực (J8 - J12)
* **`J8` (Header 2P cấp nguồn STM32):**
  * `Pin 1`: Nối `VCC_5V15` (cắm dây sang chân `5V` bo Blue Pill).
  * `Pin 2`: Nối `GND` (cắm dây sang chân `GND` bo Blue Pill).
* **`J9` (Header 4P cắm Cảm biến Siêu âm Trái - US1):**
  * `Pin 1` (VCC): Nối `VCC_5V15`. Mắc song song tụ hóa `C3` ($100\mu\text{F}$) và tụ gốm `C4` ($100\text{nF}$) xuống GND ngay tại chân này.
  * `Pin 2` (GND): Nối `GND`.
  * `Pin 3` (Trig): Nối sang `J13.3` (về chân `PB12` của STM32).
  * `Pin 4` (Echo): Nối sang `J13.4` (về chân `PB13` của STM32 - chân 5V-Tolerant).
* **`J10` (Header 4P cắm Cảm biến Siêu âm Giữa - US2):**
  * `Pin 1` (VCC): Nối `VCC_5V15` kèm tụ lọc `C5` ($100\mu\text{F}$) + `C6` ($100\text{nF}$).
  * `Pin 2` (GND): Nối `GND`.
  * `Pin 3` (Trig): Nối sang `J13.5` (về chân `PB14` của STM32).
  * `Pin 4` (Echo): Nối sang `J13.6` (về chân `PB15` của STM32 - chân 5V-Tolerant).
* **`J11` (Header 4P cắm Cảm biến Siêu âm Phải - US3):**
  * `Pin 1` (VCC): Nối `VCC_5V15` kèm tụ lọc `C7` ($100\mu\text{F}$) + `C8` ($100\text{nF}$).
  * `Pin 2` (GND): Nối `GND`.
  * `Pin 3` (Trig): Nối sang `J13.7` (về chân `PB8` của STM32).
  * `Pin 4` (Echo): Nối sang `J13.8` (về chân `PB9` của STM32 - chân 5V-Tolerant).
* **`J12` (Cọc Domino 2P cấp nguồn Motor L298N):**
  * `Pin 1`: Nối `VM_7V4` (nguồn 7.4V sau E-Stop, cắm sang chân 12V/VM của L298N).
  * `Pin 2`: Nối `GND` (cắm sang chân GND công suất của L298N).
* **`J13` (Header 1x8P kết nối toàn bộ tín hiệu về STM32):**
  * `Pin 1`: `ADC_BAT3S` $\to$ Cắm chân `PA6` của STM32.
  * `Pin 2`: `BUZZER_CTRL` $\to$ Cắm chân `PB5` của STM32.
  * `Pin 3`: `US1_TRIG` $\to$ Cắm chân `PB12` của STM32.
  * `Pin 4`: `US1_ECHO` $\to$ Cắm chân `PB13` của STM32.
  * `Pin 5`: `US2_TRIG` $\to$ Cắm chân `PB14` của STM32.
  * `Pin 6`: `US2_ECHO` $\to$ Cắm chân `PB15` của STM32.
  * `Pin 7`: `US3_TRIG` $\to$ Cắm chân `PB8` của STM32.
  * `Pin 8`: `US3_ECHO` $\to$ Cắm chân `PB9` của STM32.

---

### E. Khối Đèn Báo Nguồn LED (LED1, LED2)
* **LED Xanh lá (`LED1` - Báo nguồn Logic 5.15V):**
  * `R4` ($1\text{k}\Omega$): Chân 1 nối `VCC_5V15`, Chân 2 nối Anode của `LED1`.
  * `LED1`: Cathode (chân ngắn/vát mép) nối `GND`.
* **LED Đỏ (`LED2` - Báo nguồn Motor 7.4V sau E-Stop):**
  * `R5` ($1.5\text{k}\Omega$): Chân 1 nối `VM_7V4`, Chân 2 nối Anode của `LED2`.
  * `LED2`: Cathode nối `GND`.

---

## 3. SƠ ĐỒ NGUYÊN LÝ DẠNG KHỐI ASCII (SCHEMATIC OVERVIEW)

```
========================================================================================================================
                                      SƠ ĐỒ NGUYÊN LÝ CHI TIẾT BO MẠCH NGUỒN PDB
========================================================================================================================

[ NHÁNH A: LOGIC & CẢM BIẾN ]
                                                                                   +-------[ F1: Polyfuse 3A ]-------+
                                                                                   |                                 |
                                                                                   |                                 ▼
   J1 (Pin 3S)                               J6A (Socket)       J6B (Socket)       |   +--> D1 (TVS 5V) --> GND      J7 (Type-C Pi 4)
  [+] Pin 1 ------> J3 (Công tắc A) --------> IN+ [ MOD1 ] OUT+ -------------------+---|                             [+] Pin 1 (5.15V)
  [-] Pin 2 --+     (Bật/Tắt Logic)           IN- [XL4015] OUT- ---+                   +--> C1 (2200uF/16V) --> GND  [-] Pin 2 (GND)
              |                                 |            |     |                   +--> C2 (100nF) ------> GND
              |                                 +------------+     |                   +--> R4 (1k) -> LED1 -> GND
              |                                        |           |                   |
              |                                        +-----------+                   +--> J8.1  (Cấp 5V STM32)
              |                                                    |                   |
              |                                                    |                   +--> J9.1  (VCC US1)  [C3: 100uF + C4: 100nF]
              |   +--[ R1: 10k ]---+                               |                   +--> J10.1 (VCC US2)  [C5: 100uF + C6: 100nF]
              +-->|                |----> J13.1 (PA6 ADC STM32)    |                   +--> J11.1 (VCC US3)  [C7: 100uF + C8: 100nF]
                  +--[ R2: 3.3k ]--+                               |                   |
                           |                                       |                   +--> BZ1 (+) & D2 (Cathode)
                          GND                                      |                          │
                                                                   |                          ▼
                                                                   |                        BZ1 (-) & D2 (Anode)
                                                                   |                          │
                                                                   |                          ▼ Collector
                                                                   |                   STM32 PB5 ---> J13.2 ---> [ R3: 1k ] ---> Base Q1 (2N2222)
                                                                   |                                                               │ Emitter
                                                                   |                                                               ▼
                                                                   |                                                              GND
                                                                   |
[ NHÁNH B: ĐỘNG LỰC CÔNG SUẤT ]                                    |
   J2 (Pin 2S)                                                     |
  [+] Pin 1 ------> J4 (Công tắc B) ------> J5 (Nút E-Stop) -------|-----------------> J12.1 (VM 7.4V cấp Driver L298N)
                    (Bật/Tắt Motor)         (Dừng khẩn cấp)        |                   +--> R5 (1.5k) -> LED2 (Đỏ) -> GND
  [-] Pin 2 -------------------------------------------------------+
                                                                   |
                                                                   ▼
                                                  [ STAR GROUND PLANE (MẶT PHẲNG MASS CHUNG) ]
                                              (Cực âm Pin 3S = Cực âm Pin 2S = GND XL4015 = GND Tải)
========================================================================================================================
```

---

## 4. HƯỚNG DẪN BẮT ĐẦU VẼ TRÊN PHẦN MỀM (ALTIUM / EASYEDA / KICAD)

1. **Tạo Schematic:**
   * Lấy linh kiện đúng theo các footprint trong bảng BOM (ví dụ: Terminal Block KF301-2P, Header 2.54mm, Tụ hóa bước chân 5mm).
   * Đặt nhãn Net (**Net Label**) chính xác theo cột **Tên Net** ở Mục 1 để phần mềm tự động bắt dây mà không cần kéo dây chéo nhau làm rối bản vẽ.
2. **Chuyển sang PCB Layout:**
   * Xếp `J1, J2, J3, J4, J5` (các cọc đầu vào và công tắc) dọc theo **1 cạnh của bo mạch** để dễ luồn dây ra vỏ xe.
   * Xếp `J7` (ra Pi 4) và `J8, J13` (ra STM32) hướng về phía tầng đặt Pi và STM32.
   * Xếp 3 cổng `J9, J10, J11` hướng về phía đầu xe nơi lắp 3 cảm biến siêu âm.
   * Đặt `C1` ($2200\mu\text{F}$) sát chân ngõ ra của module `XL4015`.
3. **Đổ đồng (GND Pour):**
   * Sau khi đi xong các đường nguồn và tín hiệu ở lớp mặt trên (**Top Layer**), phủ đồng toàn bộ mặt dưới (**Bottom Layer**) gán net là `GND`.
