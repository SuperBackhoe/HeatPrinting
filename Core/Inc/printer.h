#pragma once
#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f1xx_hal.h"

    void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
    /* Exported functions prototypes ---------------------------------------------*/
    void Error_Handler(void);

// GPIO 和 定时器通道定义
#define STB1 GPIO_PIN_14
#define STB2 GPIO_PIN_15
#define STB3 GPIO_PIN_8
#define STB4 GPIO_PIN_11
#define STB5 GPIO_PIN_12
#define STB6 GPIO_PIN_15
#define LAT GPIO_PIN_13

    // 函数声明
    void STB_GPIO_Init(void);
    void Heater_ON(void);
    void Heater_OFF(void);
    void Send_Data(void);
    void USART3_Receive_IT(void);
    void Printer(void);
    // void Send_AT_Command(UART_HandleTypeDef *huart, const char *command);
    void main_Loop(void);

#ifdef __cplusplus
}
#endif