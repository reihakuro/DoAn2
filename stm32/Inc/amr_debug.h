/**
 ******************************************************************************
 * @file    amr_debug.h
 * @brief   Xuat log van ban qua USART1 KHONG CHAN CPU (ring buffer + bom
 *          tung byte khi thanh ghi TXE trong). Khong can bat ngat USART1.
 *          Chi dung o Giai doan 1 (AMR_DEBUG_UART_ENABLE = 1).
 ******************************************************************************
 */
#ifndef INC_AMR_DEBUG_H_
#define INC_AMR_DEBUG_H_

#include "amr_config.h"

void AMR_Debug_Init(void);
/** Dua chuoi vao hang doi; neu day thi bo bot phan du (khong bao gio chan). */
void AMR_Debug_Write(const char *s);
/** printf rut gon (chi so nguyen, khong float) vao hang doi. */
void AMR_Debug_Printf(const char *fmt, ...);
/** Goi moi vong lap chinh: day toi da vai byte ra USART1. */
void AMR_Debug_Pump(void);

#endif /* INC_AMR_DEBUG_H_ */
