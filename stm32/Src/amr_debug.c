/**
 ******************************************************************************
 * @file    amr_debug.c
 * @brief   Hiện thực cơ chế truyền nhật ký gỡ lỗi qua hàng đệm vòng tròn và ngoại vi USART1.
 * @details
 * Cung cấp hàng đợi Ring Buffer không chặn, định dạng chuỗi số nguyên và bơm từng byte
 * ra phần cứng thông qua cờ trạng thái TXE mà không cần phụ thuộc vào ngắt truyền.
 *
 * @author  Nhóm Đề án 2 - Robot vận chuyển nội bộ
 * @version 2.1
 * @date    2026
 ******************************************************************************
 */
#include "amr_debug.h"
#include <stdarg.h>
#include <stdio.h>

#define DBG_BUF_SIZE    512U            /* Luy thua cua 2 */
#define DBG_BUF_MASK    (DBG_BUF_SIZE - 1U)

static char     s_buf[DBG_BUF_SIZE];
static uint16_t s_head;                 /* Vi tri ghi */
static uint16_t s_tail;                 /* Vi tri doc */

void AMR_Debug_Init(void)
{
    s_head = s_tail = 0U;
}

void AMR_Debug_Write(const char *s)
{
#if AMR_DEBUG_UART_ENABLE
    while (*s != '\0')
    {
        uint16_t next = (uint16_t)((s_head + 1U) & DBG_BUF_MASK);
        if (next == s_tail) { break; }  /* Day -> bo phan con lai */
        s_buf[s_head] = *s++;
        s_head = next;
    }
#else
    (void)s;
#endif
}

void AMR_Debug_Printf(const char *fmt, ...)
{
#if AMR_DEBUG_UART_ENABLE
    char line[160];
    va_list ap;
    va_start(ap, fmt);
    (void)vsnprintf(line, sizeof(line), fmt, ap);
    va_end(ap);
    AMR_Debug_Write(line);
#else
    (void)fmt;
#endif
}

void AMR_Debug_Pump(void)
{
#if AMR_DEBUG_UART_ENABLE
    /* Moi lan goi chi day toi da 1 byte khi TXE = 1 -> khong bao gio cho. */
    if ((s_tail != s_head) && ((USART1->SR & USART_SR_TXE) != 0U))
    {
        USART1->DR = (uint8_t)s_buf[s_tail];
        s_tail = (uint16_t)((s_tail + 1U) & DBG_BUF_MASK);
    }
#endif
}
