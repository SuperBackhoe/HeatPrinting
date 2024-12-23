#pragma once
#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f1xx_hal.h"

    void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

    /* Exported functions prototypes ---------------------------------------------*/
    void Error_Handler(void);

// GPIO 和 定时器通道定义
#define GPIO_AP GPIO_PIN_0
#define GPIO_AM GPIO_PIN_1
#define GPIO_BP GPIO_PIN_2
#define GPIO_BM GPIO_PIN_3
#define GPIO_EN GPIO_PIN_4
#define GPIO_NF GPIO_PIN_5

    // 函数声明
    void Motor_Run(uint8_t steps);
    void Motor_Stop(void);
    void Motor_Enable(void);
    void Motor_GPIO_Init(void);

#ifdef __cplusplus
}
#endif