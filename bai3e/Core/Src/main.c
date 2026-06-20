#include "main.h"

TIM_HandleTypeDef htim3;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM3_Init(void);

void Motor_Left(void);
void Motor_Right(void);
void Motor_Stop(void);

void LEDs_Off(void);
void LEDs_Right(void);
void LEDs_Left(void);

uint8_t right_pos = 0;
uint8_t left_pos = 0;

int main(void)
{
    HAL_Init();

    SystemClock_Config();

    MX_GPIO_Init();
    MX_TIM3_Init();

    HAL_TIM_Base_Start(&htim3);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);

    while (1)
    {
        uint8_t left_btn =
            (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3) == GPIO_PIN_RESET);

        uint8_t right_btn =
            (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == GPIO_PIN_RESET);

        if(left_btn && right_btn)
        {
            Motor_Stop();
            LEDs_Off();
        }
        else if(left_btn)
        {
            Motor_Left();
            LEDs_Left();
        }
        else if(right_btn)
        {
            Motor_Right();
            LEDs_Right();
        }
        else
        {
            Motor_Stop();
            LEDs_Off();
        }

        HAL_Delay(100);
    }
}

void Motor_Left(void)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);

    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 600);
}

void Motor_Right(void)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);

    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 600);
}

void Motor_Stop(void)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);

    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 0);
}

void LEDs_Off(void)
{
    HAL_GPIO_WritePin(GPIOB,
                      GPIO_PIN_14 |
                      GPIO_PIN_1  |
                      GPIO_PIN_10 |
                      GPIO_PIN_11 |
                      GPIO_PIN_12 |
                      GPIO_PIN_13,
                      GPIO_PIN_RESET);
}

void LEDs_Right(void)
{
    uint16_t led[6] =
    {
        GPIO_PIN_14,
        GPIO_PIN_1,
        GPIO_PIN_10,
        GPIO_PIN_11,
        GPIO_PIN_12,
        GPIO_PIN_13
    };

    LEDs_Off();

    HAL_GPIO_WritePin(GPIOB,
                      led[right_pos],
                      GPIO_PIN_SET);

    right_pos++;

    if(right_pos >= 6)
    {
        right_pos = 0;
    }
}

void LEDs_Left(void)
{
    uint16_t led[6] =
    {
        GPIO_PIN_13,
        GPIO_PIN_12,
        GPIO_PIN_11,
        GPIO_PIN_10,
        GPIO_PIN_1,
        GPIO_PIN_14
    };

    LEDs_Off();

    HAL_GPIO_WritePin(GPIOB,
                      led[left_pos],
                      GPIO_PIN_SET);

    left_pos++;

    if(left_pos >= 6)
    {
        left_pos = 0;
    }
}

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_NONE;

    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType      =
        RCC_CLOCKTYPE_HCLK  |
        RCC_CLOCKTYPE_SYSCLK|
        RCC_CLOCKTYPE_PCLK1 |
        RCC_CLOCKTYPE_PCLK2;

    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    HAL_RCC_ClockConfig(&RCC_ClkInitStruct,
                        FLASH_LATENCY_0);
}

static void MX_TIM3_Init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};

    htim3.Instance = TIM3;
    htim3.Init.Prescaler = 7;
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = 999;
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

    if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
    {
        Error_Handler();
    }

    sClockSourceConfig.ClockSource =
        TIM_CLOCKSOURCE_INTERNAL;

    if (HAL_TIM_ConfigClockSource(&htim3,
        &sClockSourceConfig) != HAL_OK)
    {
        Error_Handler();
    }

    if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
    {
        Error_Handler();
    }

    sMasterConfig.MasterOutputTrigger =
        TIM_TRGO_RESET;

    sMasterConfig.MasterSlaveMode =
        TIM_MASTERSLAVEMODE_DISABLE;

    if (HAL_TIMEx_MasterConfigSynchronization(
        &htim3,
        &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 500;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

    if (HAL_TIM_PWM_ConfigChannel(
        &htim3,
        &sConfigOC,
        TIM_CHANNEL_3) != HAL_OK)
    {
        Error_Handler();
    }
}

static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    HAL_GPIO_WritePin(GPIOA,
                      GPIO_PIN_1 |
                      GPIO_PIN_2,
                      GPIO_PIN_RESET);

    GPIO_InitStruct.Pin =
        GPIO_PIN_1 |
        GPIO_PIN_2;

    GPIO_InitStruct.Mode =
        GPIO_MODE_OUTPUT_PP;

    GPIO_InitStruct.Pull =
        GPIO_NOPULL;

    GPIO_InitStruct.Speed =
        GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(GPIOA,
                  &GPIO_InitStruct);

    GPIO_InitStruct.Pin =
        GPIO_PIN_3 |
        GPIO_PIN_4;

    GPIO_InitStruct.Mode =
        GPIO_MODE_INPUT;

    GPIO_InitStruct.Pull =
        GPIO_PULLUP;

    HAL_GPIO_Init(GPIOA,
                  &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIOB,
                      GPIO_PIN_14 |
                      GPIO_PIN_1  |
                      GPIO_PIN_10 |
                      GPIO_PIN_11 |
                      GPIO_PIN_12 |
                      GPIO_PIN_13,
                      GPIO_PIN_RESET);

    GPIO_InitStruct.Pin =
        GPIO_PIN_14 |
        GPIO_PIN_1  |
        GPIO_PIN_10 |
        GPIO_PIN_11 |
        GPIO_PIN_12 |
        GPIO_PIN_13;

    GPIO_InitStruct.Mode =
        GPIO_MODE_OUTPUT_PP;

    GPIO_InitStruct.Pull =
        GPIO_NOPULL;

    GPIO_InitStruct.Speed =
        GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(GPIOB,
                  &GPIO_InitStruct);
}

void Error_Handler(void)
{
    __disable_irq();

    while (1)
    {
    }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif
