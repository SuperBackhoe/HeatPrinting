#include "main.h"
#include "motor.h"

TIM_HandleTypeDef htim2;

static void MX_TIM2_Init(void);

const uint8_t motorTable[][4] =
    {
        {1, 0, 0, 1},
        {0, 0, 0, 1},
        {0, 0, 1, 1},
        {0, 0, 1, 0},
        {0, 1, 1, 0},
        {0, 1, 0, 0},
        {1, 1, 0, 0},
        {1, 0, 0, 0}};

void Motor_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 启用 GPIO 时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // 配置 GPIO_AP | GPIO_AM | GPIO_BP | GPIO_BM 为复用推挽输出模式
    GPIO_InitStruct.Pin = GPIO_AP | GPIO_AM | GPIO_BP | GPIO_BM;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;       // 复用推挽输出
    GPIO_InitStruct.Pull = GPIO_NOPULL;           // 无上下拉
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; // 低速
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIOA, GPIO_AP | GPIO_AM | GPIO_BP | GPIO_BM, GPIO_PIN_RESET);
}

void Motor_Enable(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_EN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  // 复用推挽输出
    GPIO_InitStruct.Pull = GPIO_NOPULL;          // 无上下拉
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; // 低速
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOA, GPIO_EN, GPIO_PIN_SET);
}

static void MX_TIM2_Init(void)
{

    /* USER CODE BEGIN TIM2_Init 0 */

    /* USER CODE END TIM2_Init 0 */

    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};

    /* USER CODE BEGIN TIM2_Init 1 */

    /* USER CODE END TIM2_Init 1 */
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 7200 - 1;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 9; // Period决定PPS(pulse per second) PPS = 1 / Period
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
    {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 51117;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN TIM2_Init 2 */
    // 启用更新中断
    __HAL_TIM_ENABLE_IT(&htim2, TIM_IT_UPDATE);
    // 配置定时器中断
    HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
    /* USER CODE END TIM2_Init 2 */
    HAL_TIM_MspPostInit(&htim2);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
}

void Motor_Stop(void)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_AP | GPIO_AM | GPIO_BP | GPIO_BM | GPIO_EN, GPIO_PIN_RESET);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 0);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, 0);
}

void Motor_Run(uint8_t steps)
{
    Motor_GPIO_Init();
    MX_TIM2_Init();
    Motor_Enable();
    uint32_t pulse = (htim2.Init.Period * 0.78); // pulse和PPS参考数据手册
    uint8_t Step_Line = steps / 4;               // 步进4步为一点行，确定步进行数
    static uint8_t current_step = 0;
    for (int i = 0; i < steps; i++)
    {
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, pulse * motorTable[current_step][0]);
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, pulse * motorTable[current_step][1]);
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, pulse * motorTable[current_step][2]);
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, pulse * motorTable[current_step][3]);
        HAL_Delay(10); // 控制电机转速，越小越快，不宜过小，建议大于5，容易丢步
        current_step++;
        if (current_step == 8)
        {
            Step_Line = Step_Line + 2;
            current_step = 0;
        }
    }
}