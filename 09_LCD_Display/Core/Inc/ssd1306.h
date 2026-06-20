/*
 * ssd1306.h
 *
 * Created on: Jun 4, 2026
 * Author: dangt
 */

#ifndef INC_SSD1306_H_
#define INC_SSD1306_H_

#include <stddef.h>
#include <stdint.h>

// Ép buộc include file HAL của STM32F1
#include "stm32f1xx_hal.h"

#include "ssd1306_conf.h"
#include "ssd1306_fonts.h"

/* --- Kiểu dữ liệu màu sắc --- */
typedef enum {
    Black = 0x00,
    White = 0x01
} SSD1306_COLOR;

/* --- Cấu trúc quản lý trạng thái OLED --- */
typedef struct {
    uint16_t CurrentX;
    uint16_t CurrentY;
    uint8_t Inverted;
    uint8_t Initialized;
} SSD1306_t;

/* --- Các hàm điều khiển OLED --- */
uint8_t ssd1306_Init(void);
void ssd1306_Fill(SSD1306_COLOR color);
void ssd1306_UpdateScreen(void);
void ssd1306_SetCursor(uint8_t x, uint8_t y);
char ssd1306_WriteChar(char ch, FontDef Font, SSD1306_COLOR color);
char ssd1306_WriteString(char* str, FontDef Font, SSD1306_COLOR color);

#endif /* INC_SSD1306_H_ */
