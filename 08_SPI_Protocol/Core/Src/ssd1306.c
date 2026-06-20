#include "ssd1306.h"

extern I2C_HandleTypeDef hi2c1;
static uint8_t SSD1306_Buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];
static SSD1306_t SSD1306;

static void ssd1306_WriteCommand(uint8_t byte) {
    uint8_t data_to_send[2] = {0x00, byte};
    HAL_I2C_Master_Transmit(&hi2c1, SSD1306_I2C_ADDR, data_to_send, 2, 100);
}

uint8_t ssd1306_Init(void) {
    for(volatile uint32_t wait = 0; wait < 2000000; wait++) {}

    ssd1306_WriteCommand(0xAE);
    ssd1306_WriteCommand(0x20);
    ssd1306_WriteCommand(0x10);
    ssd1306_WriteCommand(0xB0);
    ssd1306_WriteCommand(0xC8);
    ssd1306_WriteCommand(0x00);
    ssd1306_WriteCommand(0x10);
    ssd1306_WriteCommand(0x40);
    ssd1306_WriteCommand(0x81);
    ssd1306_WriteCommand(0x7F);
    ssd1306_WriteCommand(0xA1);
    ssd1306_WriteCommand(0xA6);
    ssd1306_WriteCommand(0xA8);
    ssd1306_WriteCommand(SSD1306_HEIGHT - 1);
    ssd1306_WriteCommand(0xA4);
    ssd1306_WriteCommand(0xD3);
    ssd1306_WriteCommand(0x00);
    ssd1306_WriteCommand(0xD5);
    ssd1306_WriteCommand(0x80);
    ssd1306_WriteCommand(0xD9);
    ssd1306_WriteCommand(0x22);
    ssd1306_WriteCommand(0xDA);
    ssd1306_WriteCommand(0x12);
    ssd1306_WriteCommand(0xDB);
    ssd1306_WriteCommand(0x20);
    ssd1306_WriteCommand(0x8D);
    ssd1306_WriteCommand(0x14);
    ssd1306_WriteCommand(0xAF);

    ssd1306_Fill(Black);
    ssd1306_UpdateScreen();

    SSD1306.Initialized = 1;
    return 1;
}

void ssd1306_Fill(SSD1306_COLOR color) {
    for(uint16_t i = 0; i < sizeof(SSD1306_Buffer); i++) {
        SSD1306_Buffer[i] = (color == Black) ? 0x00 : 0xFF;
    }
}

void ssd1306_UpdateScreen(void) {
    uint8_t data_to_send[SSD1306_WIDTH + 1];
    data_to_send[0] = 0x40;

    for(uint8_t i = 0; i < 8; i++) {
        ssd1306_WriteCommand(0xB0 + i);
        ssd1306_WriteCommand(0x00);
        ssd1306_WriteCommand(0x10);

        for(uint8_t j = 0; j < SSD1306_WIDTH; j++) {
            data_to_send[j+1] = SSD1306_Buffer[SSD1306_WIDTH * i + j];
        }
        HAL_I2C_Master_Transmit(&hi2c1, SSD1306_I2C_ADDR, data_to_send, SSD1306_WIDTH + 1, 100);
    }
}

void ssd1306_SetCursor(uint8_t x, uint8_t y) {
    SSD1306.CurrentX = x;
    SSD1306.CurrentY = y;
}

void ssd1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color) {
    if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) return;
    if (color == White) {
        SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] |= (1 << (y % 8));
    } else {
        SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
    }
}

char ssd1306_WriteChar(char ch, FontDef Font, SSD1306_COLOR color) {
    uint32_t i, b, j;
    if (ch < 32 || ch > 126) return 0;

    for(i = 0; i < Font.FontHeight; i++) {
        b = Font.data[(ch - 32) * Font.FontHeight + i];
        for(j = 0; j < Font.FontWidth; j++) {
            if((b >> j) & 0x01) {
                ssd1306_DrawPixel(SSD1306.CurrentX + j, SSD1306.CurrentY + i, color);
            } else {
                ssd1306_DrawPixel(SSD1306.CurrentX + j, SSD1306.CurrentY + i, (SSD1306_COLOR)!color);
            }
        }
    }
    SSD1306.CurrentX += Font.FontWidth;
    return ch;
}

char ssd1306_WriteString(char* str, FontDef Font, SSD1306_COLOR color) {
    while (*str) {
        ssd1306_WriteChar(*str, Font, color);
        str++;
    }
    return *str;
}
