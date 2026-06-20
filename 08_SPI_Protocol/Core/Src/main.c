#include "main.h"
#include "ssd1306.h"
#include <stdlib.h>

I2C_HandleTypeDef hi2c1;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);

/* ---- Font 5x7 chỉ cần H,E,L,O ---- */
static const uint8_t FONT[][5] = {
    {0x3E,0x51,0x49,0x45,0x3E}, // 0
    {0x00,0x42,0x7F,0x40,0x00}, // 1
    {0x42,0x61,0x51,0x49,0x46}, // 2
    {0x21,0x41,0x45,0x4B,0x31}, // 3
    {0x18,0x14,0x12,0x7F,0x10}, // 4
    {0x27,0x45,0x45,0x45,0x39}, // 5
    {0x3C,0x4A,0x49,0x49,0x30}, // 6
    {0x01,0x71,0x09,0x05,0x03}, // 7
    {0x36,0x49,0x49,0x49,0x36}, // 8
    {0x06,0x49,0x49,0x29,0x1E}, // 9
    {0x7E,0x11,0x11,0x11,0x7E}, // A=10
    {0x7F,0x49,0x49,0x49,0x36}, // B=11
    {0x3E,0x41,0x41,0x41,0x22}, // C=12
    {0x7F,0x41,0x41,0x22,0x1C}, // D=13
    {0x7F,0x49,0x49,0x49,0x41}, // E=14
    {0x7F,0x09,0x09,0x09,0x01}, // F=15
    {0x3E,0x41,0x49,0x49,0x3A}, // G=16
    {0x7F,0x08,0x08,0x08,0x7F}, // H=17
    {0x41,0x7F,0x41,0x00,0x00}, // I=18
    {0x20,0x40,0x41,0x3F,0x01}, // J=19
    {0x7F,0x08,0x14,0x22,0x41}, // K=20
    {0x7F,0x40,0x40,0x40,0x40}, // L=21
    {0x7F,0x02,0x0C,0x02,0x7F}, // M=22
    {0x7F,0x04,0x08,0x10,0x7F}, // N=23
    {0x3E,0x41,0x41,0x41,0x3E}, // O=24
    {0x7F,0x09,0x09,0x09,0x06}, // P=25
    {0x3E,0x41,0x51,0x21,0x5E}, // Q=26
    {0x7F,0x09,0x19,0x29,0x46}, // R=27
    {0x46,0x49,0x49,0x49,0x31}, // S=28
    {0x01,0x01,0x7F,0x01,0x01}, // T=29
    {0x3F,0x40,0x40,0x40,0x3F}, // U=30
    {0x1F,0x20,0x40,0x20,0x1F}, // V=31
    {0x3F,0x40,0x38,0x40,0x3F}, // W=32
    {0x63,0x14,0x08,0x14,0x63}, // X=33
    {0x07,0x08,0x70,0x08,0x07}, // Y=34
    {0x61,0x51,0x49,0x45,0x43}, // Z=35
    {0x00,0x00,0x00,0x00,0x00}, // space=36
};

void DrawChar(int x, int y, char c, int scale) {
    int idx = -1;
    if(c >= 'A' && c <= 'Z') idx = c - 'A' + 10;
    else if(c >= 'a' && c <= 'z') idx = c - 'a' + 10;
    else if(c >= '0' && c <= '9') idx = c - '0';
    else if(c == ' ') idx = 36;
    if(idx < 0) return;

    for(int col = 0; col < 5; col++) {
        uint8_t bits = FONT[idx][col];
        for(int row = 0; row < 7; row++) {
            if(bits & (1 << row)) {
                for(int sy = 0; sy < scale; sy++)
                    for(int sx = 0; sx < scale; sx++)
                        ssd1306_DrawPixel(
                            x + col*scale + sx,
                            y + row*scale + sy,
                            White);
            }
        }
    }
}

void DrawStr(int x, int y, const char* s, int scale) {
    while(*s) {
        DrawChar(x, y, *s++, scale);
        x += 6 * scale;
    }
}

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_I2C1_Init();

    ssd1306_Init();
    ssd1306_Fill(Black);

    /* Hiện chữ HELLO to ở giữa màn hình */
    /* scale=2: mỗi chữ rộng 12px, HELLO = 5 chữ x 12 = 60px */
    /* x = (128-60)/2 = 34, y = (64-14)/2 = 25 */
    DrawStr(19, 28, "27:DANG TRONG PHUC", 1);

    ssd1306_UpdateScreen();

    while(1) {}
}

/* ================================================================
 * HARDWARE INIT
 * ================================================================ */
static void MX_I2C1_Init(void) {
    __HAL_RCC_I2C1_CLK_ENABLE();
    __HAL_RCC_I2C1_FORCE_RESET();
    __HAL_RCC_I2C1_RELEASE_RESET();

    hi2c1.Instance             = I2C1;
    hi2c1.Init.ClockSpeed      = 100000;
    hi2c1.Init.DutyCycle       = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1     = 0;
    hi2c1.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2     = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;
    HAL_I2C_Init(&hi2c1);
}

static void MX_GPIO_Init(void) {
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitTypeDef g = {0};
    g.Pin   = GPIO_PIN_6 | GPIO_PIN_7;
    g.Mode  = GPIO_MODE_AF_OD;
    g.Pull  = GPIO_NOPULL;
    g.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &g);
}

void SystemClock_Config(void) {
    RCC_OscInitTypeDef osc = {0};
    RCC_ClkInitTypeDef clk = {0};
    osc.OscillatorType      = RCC_OSCILLATORTYPE_HSI;
    osc.HSIState            = RCC_HSI_ON;
    osc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    osc.PLL.PLLState        = RCC_PLL_ON;
    osc.PLL.PLLSource       = RCC_PLLSOURCE_HSI_DIV2;
    osc.PLL.PLLMUL          = RCC_PLL_MUL16;
    if(HAL_RCC_OscConfig(&osc) != HAL_OK) Error_Handler();

    clk.ClockType      = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                        |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    clk.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    clk.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    clk.APB1CLKDivider = RCC_HCLK_DIV2;
    clk.APB2CLKDivider = RCC_HCLK_DIV1;
    if(HAL_RCC_ClockConfig(&clk, FLASH_LATENCY_2) != HAL_OK) Error_Handler();
}

void Error_Handler(void) {
    __disable_irq();
    while(1) {}
}
