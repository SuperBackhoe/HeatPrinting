#include "printer.h"
#include "motor.h"

SPI_HandleTypeDef hspi1;
UART_HandleTypeDef huart3;

uint8_t outputData;
uint8_t rx_buffer;    // 存储接收到的数据
uint8_t rx_index = 0; // 缓冲区索引

static void MX_SPI1_Init(void);
static void MX_USART3_UART_Init(void);
void STB_GPIO_Init(void);
void Printer(void);
void HeaterON(uint8_t STB_num);

static void MX_SPI1_Init(void)
{

    /* USER CODE BEGIN SPI1_Init 0 */

    /* USER CODE END SPI1_Init 0 */

    /* USER CODE BEGIN SPI1_Init 1 */

    /* USER CODE END SPI1_Init 1 */
    /* SPI1 parameter configuration*/
    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi1.Init.NSS = SPI_NSS_SOFT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi1.Init.CRCPolynomial = 10;
    if (HAL_SPI_Init(&hspi1) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN SPI1_Init 2 */

    /* USER CODE END SPI1_Init 2 */
}

static void MX_USART3_UART_Init(void)
{

    /* USER CODE BEGIN USART3_Init 0 */

    /* USER CODE END USART3_Init 0 */

    /* USER CODE BEGIN USART3_Init 1 */

    /* USER CODE END USART3_Init 1 */
    huart3.Instance = USART3;
    huart3.Init.BaudRate = 115200;
    huart3.Init.WordLength = UART_WORDLENGTH_8B;
    huart3.Init.StopBits = UART_STOPBITS_1;
    huart3.Init.Parity = UART_PARITY_NONE;
    huart3.Init.Mode = UART_MODE_TX_RX;
    huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart3.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart3) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN USART3_Init 2 */
    HAL_NVIC_SetPriority(USART3_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART3_IRQn);
    /* USER CODE END USART3_Init 2 */
}

void Send_Data(void)
{
    STB_GPIO_Init();
    MX_SPI1_Init();
    // 传入数据
    static const uint8_t Halo[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xf0, 0x00, 0x00, 0x00,
        0xfd, 0xf8, 0x00, 0x00, 0xf0, 0x00, 0x00, 0x00,
        0x78, 0xf0, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00,
        0x78, 0xf0, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00,
        0x78, 0xf0, 0x02, 0x00, 0x70, 0x00, 0x10, 0x00,
        0x78, 0xf0, 0x7e, 0x00, 0x70, 0x00, 0x7e, 0x00,
        0x7f, 0xf0, 0xfe, 0x00, 0x70, 0x00, 0xff, 0x00,
        0x7f, 0xf0, 0xee, 0x00, 0x70, 0x00, 0xef, 0x00,
        0x78, 0xf0, 0xee, 0x00, 0x70, 0x00, 0xe7, 0x00,
        0x78, 0xf0, 0xee, 0x00, 0x70, 0x00, 0xef, 0x00,
        0x78, 0xf0, 0xff, 0x00, 0x70, 0x00, 0xff, 0x00,
        0xfd, 0xf8, 0xff, 0x00, 0xf8, 0x00, 0x7e, 0x00};

    for (int i = 0; i < 16; i++)
    {
        // 预热打印头

        // 发送数据
        HAL_SPI_Transmit(&hspi1, &Halo[i * 8], 8, 1000);
        // 拉低LAT，延时1us
        HAL_GPIO_WritePin(GPIOB, LAT, GPIO_PIN_RESET);
        HAL_Delay(1);
        // 拉高LAT锁存数据
        HAL_GPIO_WritePin(GPIOB, LAT, GPIO_PIN_SET);
        Heater_ON(); // 打开脉冲选通
        // HAL_Delay(50);
        Motor_Run(4); // 步进一点行
        Motor_Stop(); // 停止步进
    }
}

void STB_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    // 配置选冲脉冲STB1～6
    //  启用 GPIO 时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStruct.Pin = STB3 | STB4 | STB5 | STB6;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;     // 推挽输出
    GPIO_InitStruct.Pull = GPIO_NOPULL;             // 无上下拉
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM; // 中速
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOA, STB3 | STB4 | STB5 | STB6, GPIO_PIN_RESET);

    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitStruct.Pin = STB1 | STB2 | LAT;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;     // 推挽输出
    GPIO_InitStruct.Pull = GPIO_NOPULL;             // 无上下拉
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM; // 中速
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOB, STB1 | STB2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, LAT, GPIO_PIN_SET);
}

void Heater_ON(void)
{
    HAL_GPIO_WritePin(GPIOA, STB4, GPIO_PIN_SET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(GPIOA, STB4, GPIO_PIN_RESET);
    // HAL_GPIO_WritePin(GPIOA, STB_num, GPIO_PIN_SET); // 打开STB1～6选冲脉冲
}

void HeaterON(uint8_t STB_num)
{
    switch (STB_num)
    {
    case 1:
        HAL_GPIO_WritePin(GPIOB, STB1, GPIO_PIN_SET);
        HAL_Delay(50);
        HAL_GPIO_WritePin(GPIOB, STB1, GPIO_PIN_RESET);
        break;
    case 2:
        HAL_GPIO_WritePin(GPIOB, STB2, GPIO_PIN_SET);
        HAL_Delay(50);
        HAL_GPIO_WritePin(GPIOB, STB2, GPIO_PIN_RESET);
        break;
    case 3:
        HAL_GPIO_WritePin(GPIOA, STB3, GPIO_PIN_SET);
        HAL_Delay(50);
        HAL_GPIO_WritePin(GPIOA, STB3, GPIO_PIN_RESET);
        break;
    case 4:
        HAL_GPIO_WritePin(GPIOA, STB4, GPIO_PIN_SET);
        HAL_Delay(50);
        HAL_GPIO_WritePin(GPIOA, STB4, GPIO_PIN_RESET);
        break;
    case 5:
        HAL_GPIO_WritePin(GPIOA, STB5, GPIO_PIN_SET);
        HAL_Delay(50);
        HAL_GPIO_WritePin(GPIOA, STB5, GPIO_PIN_RESET);
        break;
    case 6:
        HAL_GPIO_WritePin(GPIOA, STB6, GPIO_PIN_SET);
        HAL_Delay(50);
        HAL_GPIO_WritePin(GPIOA, STB6, GPIO_PIN_RESET);
        break;
    case 7:
        HAL_GPIO_WritePin(GPIOB, STB1 | STB2, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA, STB3 | STB4 | STB5 | STB6, GPIO_PIN_SET);
        HAL_Delay(50);
        HAL_GPIO_WritePin(GPIOB, STB1 | STB2, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOA, STB3 | STB4 | STB5 | STB6, GPIO_PIN_RESET);
        break;
    default:
        Heater_OFF();
        break;
    }
}

void Heater_OFF(void)
{
    HAL_GPIO_WritePin(GPIOA, STB3 | STB4 | STB5 | STB6, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, STB1 | STB2, GPIO_PIN_RESET); // 关闭STB1～6选冲脉冲
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART3)
    {
        outputData = rx_buffer; // 读取接收到的数据
        rx_index = 1;
        HAL_UART_Receive_IT(&huart3, &rx_buffer, 1); // 重新启动接收中断
    }
}

void Printer(void)
{
    STB_GPIO_Init();
    MX_SPI1_Init();
    uint8_t STB_num = 1;
    for (int i = 0; i < 36; i++)
    {
        if (STB_num > 6)
        {
            STB_num = 1;
            Motor_Run(4); // 步进一点行
            Motor_Stop(); // 停止步进
        }
        static const uint8_t testImage[] = {0x00, 0x01, 0xcf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        // 发送数据
        HAL_SPI_Transmit(&hspi1, &testImage[i * 8], 8, 1000);
        // 拉低LAT，延时1us
        HAL_GPIO_WritePin(GPIOB, LAT, GPIO_PIN_RESET);
        HAL_Delay(1);
        // 拉高LAT锁存数据
        HAL_GPIO_WritePin(GPIOB, LAT, GPIO_PIN_SET);
        // Heater_ON(); // 打开脉冲选通
        HeaterON(STB_num);
        STB_num++;
    }
}

void USART3_Receive_IT(void)
{
    MX_USART3_UART_Init();
    HAL_UART_Receive_IT(&huart3, &rx_buffer, 1); // 启动接收中断
}

void main_Loop(void)
{
    if (rx_index)
    {
        // outputData = GetReceivedData();
        if (outputData == 1)
        {
            HAL_UART_Transmit(&huart3, &outputData, sizeof(outputData), HAL_MAX_DELAY);
            Send_Data();
            Motor_Run(64);
            Motor_Stop();
            outputData = 0;
        }
        else if (outputData != 0)
        {
            HAL_UART_Transmit(&huart3, &outputData, sizeof(outputData), HAL_MAX_DELAY);
            Motor_Run(32);
            Motor_Stop();
            outputData = 0;
        }
    }
}

/*void Send_AT_Command(UART_HandleTypeDef *huart, const char *command)
{
    // 构造 AT 指令（通常以换行符结束）
    char at_command[100];
    // snprintf(at_command, sizeof(at_command), "%s\r\n", command);

    // 发送 AT 指令
    if (HAL_UART_Transmit(huart, (uint8_t *)at_command, strlen(at_command), HAL_MAX_DELAY) != HAL_OK)
    {
        Error_Handler(); // 如果发送失败，进行错误处理
    }
    HAL_Delay(3000);
    HAL_UART_Receive(huart, rx_buffer, sizeof(rx_buffer), HAL_MAX_DELAY);
}

 uint8_t STB_num = 0;
    if (STB_num = 1)
    {
        HAL_GPIO_WritePin(GPIOB, STB1, GPIO_PIN_SET);
        HAL_Delay(10);
        HAL_GPIO_WritePin(GPIOB, STB1, GPIO_PIN_RESET);
    }
    if (STB_num = 2)
    {
        HAL_GPIO_WritePin(GPIOB, STB2, GPIO_PIN_SET);
        HAL_Delay(10);
        HAL_GPIO_WritePin(GPIOB, STB2, GPIO_PIN_RESET);
    }
    if (STB_num = 3)
    {
        HAL_GPIO_WritePin(GPIOA, STB3, GPIO_PIN_SET);
        HAL_Delay(10);
        HAL_GPIO_WritePin(GPIOA, STB3, GPIO_PIN_RESET);
    }
    if (STB_num = 4)
    {
        HAL_GPIO_WritePin(GPIOA, STB4, GPIO_PIN_SET);
        HAL_Delay(10);
        HAL_GPIO_WritePin(GPIOA, STB4, GPIO_PIN_RESET);
    }
    if (STB_num = 5)
    {
        HAL_GPIO_WritePin(GPIOA, STB5, GPIO_PIN_SET);
        HAL_Delay(10);
        HAL_GPIO_WritePin(GPIOA, STB5, GPIO_PIN_RESET);
    }
    if (STB_num = 6)
    {
        HAL_GPIO_WritePin(GPIOA, STB6, GPIO_PIN_SET);
        HAL_Delay(10);
        HAL_GPIO_WritePin(GPIOA, STB6, GPIO_PIN_RESET);
    }

*/