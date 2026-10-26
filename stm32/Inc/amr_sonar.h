/**
 ******************************************************************************
 * @file    amr_sonar.h
 * @brief   Quet tuan tu (TDM) 3 cam bien sieu am HC-SR04, khong chan CPU.
 *            Khe 0: Trai  (Trig PB12 / Echo PB13)
 *            Khe 1: Giua  (Trig PB14 / Echo PB15)
 *            Khe 2: Phai  (Trig PB8  / Echo PB9)
 *          Moi thoi diem chi 1 cam bien phat -> khong nhieu cheo.
 *          Loc trung vi 3 mau cho moi cam bien.
 ******************************************************************************
 */
#ifndef INC_AMR_SONAR_H_
#define INC_AMR_SONAR_H_

#include "amr_config.h"

typedef enum
{
    SONAR_LEFT  = 0,
    SONAR_MID   = 1,
    SONAR_RIGHT = 2
} SonarId_t;

/** Gia tri tra ve khi khong co vat can trong tam do (> 4 m). */
#define SONAR_DIST_FAR_MM   0xFFFFU

void AMR_Sonar_Init(void);

/** Goi CANG THUONG XUYEN CANG TOT trong vong lap chinh (moi vong lap).
 *  Ham do canh Echo bang polling voi bo dem 1 us, nen vong lap chinh
 *  KHONG DUOC chua HAL_Delay() hay doan code chan > 100 us. */
void AMR_Sonar_Process(void);

/** Khoang cach da loc trung vi (mm) hoac SONAR_DIST_FAR_MM. */
uint16_t AMR_Sonar_GetDistanceMm(SonarId_t id);
/** Khoang cach do duoc o lan gan nhat (chua loc) - de chan doan. */
uint16_t AMR_Sonar_GetRawMm(SonarId_t id);
/** true neu Echo khong bao gio len (cam bien hong / long day). */
bool     AMR_Sonar_IsFaulty(SonarId_t id);
/** Tang 1 moi khi hoan tat tron 1 chu ky quet 3 cam bien (75 ms). */
uint32_t AMR_Sonar_GetCycleCount(void);

#endif /* INC_AMR_SONAR_H_ */
