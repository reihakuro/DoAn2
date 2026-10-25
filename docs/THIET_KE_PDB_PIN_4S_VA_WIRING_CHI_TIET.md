# THIẾT KẾ MẠCH NGUỒN PDB ĐỒNG BỘ PIN 4S & SƠ ĐỒ ĐI DÂY CHI TIẾT
## TÀI LIỆU KỸ THUẬT NÂNG CẤP KIẾN TRÚC NGUỒN TẬP TRUNG (REV 3.0 - ĐỒ ÁN 2 HCM-UTE)
**Dự án:** Robot Vận Chuyển Vật Tư Tự Hành Trong Nhà (AGV)  
**Kiến trúc:** 1 Khay Pin 4S Duy Nhất ($14.8\text{V} - 16.8\text{V}$) + Chốt Nguồn Thông Minh P-MOSFET + Tự Ngắt An Toàn Raspberry Pi 4 & STM32 + Điều Tốc Động Cơ Bằng Phần Mềm (PWM)

---

## 1. TỔNG QUAN KIẾN TRÚC MẠCH NGUỒN PDB 4S (REV 3.0)

So với kiến trúc cũ 2 nhánh rời rạc (3S Logic + 2S Motor gồm 5 cell pin), kiến trúc **Rev 3.0 gom toàn bộ hệ thống về 1 khối Pin 4S 18650 duy nhất ($14.8\text{V} - 16.8\text{V}$)** mang lại những đột phá kỹ thuật:

```
                                SƠ ĐỒ KHỐI PHÂN PHỐI NGUỒN 4S REV 3.0
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
   • Bắt tay Pi: Pi 4 GPIO 26 (dtoverlay=gpio-poweroff) ──► Cọc J3 (PI-HALT) ──► [ R32: 1k ] ──┬──► STM32 PB10 (PI_HALT_DET: Nhận biết Pi đã tắt hẳn)
                                                                                                └──► [ R33: 10k ] ──► GND (Chống trôi chân lúc Pi boot)
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

---

## 7. CẤU HÌNH PHẦN MỀM & MÃ NGUỒN TÍCH HỢP

### 7.1. Cấu hình trên Raspberry Pi 4 (Linux Kernel Device Tree)
Mở file cấu hình boot:
```bash
sudo nano /boot/firmware/config.txt
# (hoặc /boot/config.txt trên các phiên bản Raspberry Pi OS cũ)
```
Thêm vào cuối file:
```ini
# Báo tín hiệu ra chân GPIO 26 khi Raspberry Pi 4 đã tắt hoàn toàn (Halt)
dtoverlay=gpio-poweroff,gpiopin=26,active_low=0
```

---

### 7.2. Cấu hình Tính toán ADC Pin 4S trên STM32 (Chân PA6)
Cầu phân áp trên bản vẽ KiCad: $R_1 = 43\text{k}\Omega$ (hoặc $18\text{k}\Omega$), $R_2 = 10\text{k}\Omega$ (hoặc $3.3\text{k}\Omega$).

* **Trường hợp dùng $R_1 = 43\text{k}\Omega, R_2 = 10\text{k}\Omega$ (chuẩn bản vẽ KiCad hiện tại):**
  $$V_{ADC} = V_{\text{Pin\_4S}} \times \frac{R_2}{R_1 + R_2} = V_{\text{Pin\_4S}} \times \frac{10}{43 + 10} = V_{\text{Pin\_4S}} \times \frac{10}{53} \approx V_{\text{Pin\_4S}} \times 0.18868$$
  $$\implies V_{\text{Pin\_4S}} = V_{ADC} \times \frac{53}{10} = V_{ADC} \times 5.30$$
  - Pin đầy ($16.8\text{V}$): $V_{ADC} = 16.8 \times \frac{10}{53} \approx 3.170\text{V} < 3.3\text{V}$ (ADC Raw $\approx 3935 / 4095$).
  - Ngưỡng cảnh báo Cấp 1 ($13.6\text{V}$): $V_{ADC} \approx 2.566\text{V}$ (ADC Raw $\approx 3185$).
  - Ngưỡng ngắt tắt máy Cấp 2 ($12.0\text{V}$): $V_{ADC} \approx 2.264\text{V}$ (ADC Raw $\approx 2810$).

* **Trường hợp dùng $R_1 = 18\text{k}\Omega, R_2 = 3.3\text{k}\Omega$ (chuẩn dự phòng):**
  $$V_{ADC} = V_{\text{Pin\_4S}} \times \frac{3.3}{18 + 3.3} \approx V_{\text{Pin\_4S}} \times 0.15493 \implies V_{\text{Pin\_4S}} = V_{ADC} \times 6.4545$$

* Điện áp đầy ($16.8\text{V}$): $V_{ADC} \approx 2.603\text{V} \implies \text{ADC Raw} \approx 3230$ ($12\text{-bit}$).
* Điện áp danh định ($14.8\text{V}$): $V_{ADC} \approx 2.293\text{V} \implies \text{ADC Raw} \approx 2845$.
* **Cảnh báo sớm Cấp 1 ($13.6\text{V}$, tương đương $3.4\text{V}/\text{cell}$):** $V_{ADC} \approx 2.107\text{V} \implies \text{ADC Raw} \approx 2615$. Xe hoàn tất nốt nhiệm vụ hiện tại, còi bíp cảnh báo, không nhận đơn hàng mới.
* **Cưỡng bức Safe-Shutdown Cấp 2 ($12.0\text{V}$, tương đương $3.0\text{V}/\text{cell}$):** $V_{ADC} \approx 1.859\text{V} \implies \text{ADC Raw} \approx 2307$. Lập tức dừng bánh xe, ra lệnh Pi 4 shutdown, chờ Pi tắt rồi hạ `PB0` ngắt sạch điện toàn xe.

---

### 7.3. Thuật toán Điều Khiển & Safe Shutdown trên STM32

```c
#include "main.h"

// Giới hạn xung PWM cho động cơ chạy nguồn 4S (Duty cycle tối đa 55%)
#define MAX_MOTOR_PWM_DUTY   550   // Trên thang Timer Period 1000

// Hàm khởi tạo chốt nguồn trong main():
void System_Power_Init(void) {
    // PB0 cấu hình Output Push-Pull
    // KÍCH HOẠT CHỐT NGUỒN NGAY TRONG DÒNG ĐẦU TIÊN CỦA MAIN()
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
}

// Hàm điều khiển bánh xe an toàn với nguồn 4S:
void Set_Motor_Speed(int16_t left_speed, int16_t right_speed) {
    // Khống chế không bao giờ vượt quá 55% xung PWM
    if (left_speed > MAX_MOTOR_PWM_DUTY) left_speed = MAX_MOTOR_PWM_DUTY;
    if (left_speed < -MAX_MOTOR_PWM_DUTY) left_speed = -MAX_MOTOR_PWM_DUTY;
    if (right_speed > MAX_MOTOR_PWM_DUTY) right_speed = MAX_MOTOR_PWM_DUTY;
    if (right_speed < -MAX_MOTOR_PWM_DUTY) right_speed = -MAX_MOTOR_PWM_DUTY;
    
    // Ghi giá trị vào thanh ghi Timer PWM (TIM2 CH1 và CH4)...
}

// Quy trình Safe-Shutdown toàn xe:
void Execute_Safe_Shutdown(void) {
    // 1. Phanh dừng toàn bộ động cơ ngay lập tức
    Set_Motor_Speed(0, 0);
    
    // 2. Phát còi cảnh báo tắt máy
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
    HAL_Delay(500);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
    
    // 3. Gửi lệnh tắt máy khẩn cấp sang Raspberry Pi 4 qua UART
    HAL_UART_Transmit(&huart1, (uint8_t*)"CMD:SHUTDOWN_NOW\r\n", 18, 100);
    
    // 4. Chờ Raspberry Pi 4 hoàn tất Shutdown (PB10 lên mức 1 do dtoverlay=gpio-poweroff kích)
    uint32_t start_time = HAL_GetTick();
    while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == GPIO_PIN_RESET) {
        // Cơ chế Watchdog Timeout dự phòng: Quá 25s nếu Pi bị treo kernel thì vẫn cưỡng bức ngắt
        if (HAL_GetTick() - start_time > 25000) {
            break;
        }
        HAL_Delay(50);
    }
    
    // 5. Đệm thêm 1.5 giây để tụ xả sạch và chip Pi rơi vào trạng thái tĩnh hoàn toàn
    HAL_Delay(1500);
    
    // 6. HẠ CHÂN PB0 VỀ 0 -> P-MOSFET ĐÓNG -> NGẮT 100% ĐIỆN TOÀN BỘ XE (0µA)!
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    
    while(1); // Hệ thống mất điện tại đây
}
```
