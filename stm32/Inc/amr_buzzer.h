/**
 ******************************************************************************
 * @file    amr_buzzer.h
 * @brief   Coi Active 5V (PB5 -> 2N2222), phat mau am thanh khong chan CPU.
 ******************************************************************************
 */
#ifndef INC_AMR_BUZZER_H_
#define INC_AMR_BUZZER_H_

#include "amr_config.h"

typedef enum
{
    BUZZ_OFF = 0,
    BUZZ_SHORT,         /* 1 tieng 100 ms : nhan lenh / khoi dong              */
    BUZZ_DOUBLE,        /* 2 x 150 ms     : pin yeu (lap moi 5 s)              */
    BUZZ_LONG,          /* 1000 ms        : cap tram / bat dau tat may         */
    BUZZ_ALARM          /* Bip lien tuc   : vat can, ket banh, mat ket noi     */
} BuzzerPattern_t;

void AMR_Buzzer_Init(void);

/** Phat 1 mau am thanh 1 lan (SHORT / DOUBLE / LONG). Mau moi de len mau cu. */
void AMR_Buzzer_Play(BuzzerPattern_t pattern);

/** Bat/tat bao dong lien tuc - uu tien cao nhat, de len moi mau khac. */
void AMR_Buzzer_SetAlarm(bool on);

/** Goi dinh ky (moi 10 ms) tu vong lap chinh. */
void AMR_Buzzer_Update(void);

#endif /* INC_AMR_BUZZER_H_ */
