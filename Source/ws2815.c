/*
    The MIT License
    Copyright (c) 2025 ElmueSoft / Nakanishi Kiyomaro / Normadotcom
    https://netcult.ch/elmue/CANable Firmware Update
*/

#include "settings.h"
#include "led.h"
#include "ws2815.h"

#if (LED_WS2815_ENABLE > 0)

#include "stm32g4xx_hal.h"

static ws28xx_class ws2815_inst[LED_WS2815_NUMBER];
static ws28xx_class ws2815_inst_last[LED_WS2815_NUMBER];

static bool ws2815_initialized = false;
static bool ws2815_inst_changed = true;

extern led_class led_inst[CHANNEL_COUNT];

#if (LED_WS2815_MODE == 0)

#define WS2815_RESET_CYCLES   1500
#define WS2815_T1H_CYCLES     10
#define WS2815_T1L_CYCLES      3 //  定义WS2815协议中T1L（低电平时间）的时钟周期数 该值用于控制WS2815LED灯带的数据传输时序 T1L表示逻辑"1"信号中低电平部分所占的时钟周期数 总共需要3个时钟周期来完成逻辑"1"的低电平部分
#define WS2815_T0H_CYCLES      1
#define WS2815_T0L_CYCLES     12

static void ws2815_send_bit_gpio(uint8_t bit)
{
    HAL_GPIO_WritePin(LED_WS2815_PORT, LED_WS2815_PIN, GPIO_PIN_SET);
    if (bit)
    {
        for (volatile uint32_t i = 0; i < WS2815_T1H_CYCLES; i++) { __NOP(); }
        HAL_GPIO_WritePin(LED_WS2815_PORT, LED_WS2815_PIN, GPIO_PIN_RESET);
        for (volatile uint32_t i = 0; i < WS2815_T1L_CYCLES; i++) { __NOP(); }
    }
    else
    {
        for (volatile uint32_t i = 0; i < WS2815_T0H_CYCLES; i++) { __NOP(); }
        HAL_GPIO_WritePin(LED_WS2815_PORT, LED_WS2815_PIN, GPIO_PIN_RESET);
        for (volatile uint32_t i = 0; i < WS2815_T0L_CYCLES; i++) { __NOP(); }
    }
}

static void ws2815_send_byte_gpio(uint8_t value)
{
    __disable_irq();
    for (uint8_t bit = 0; bit < 8; bit++)
    {
        ws2815_send_bit_gpio((value << bit) & 0x80);
    }
    __enable_irq();
}

static void ws2815_reset_gpio(void)
{
    HAL_GPIO_WritePin(LED_WS2815_PORT, LED_WS2815_PIN, GPIO_PIN_RESET);
    for (volatile uint32_t i = 0; i < WS2815_RESET_CYCLES; i++) { __NOP(); }
}

static void ws2815_set_pixel_gpio(uint8_t channel)
{

    if(channel >= LED_WS2815_NUMBER)
        channel = LED_WS2815_NUMBER - 1;


    if (ws2815_inst_last[channel].green != ws2815_inst[channel].green ||
        ws2815_inst_last[channel].red   != ws2815_inst[channel].red   ||
        ws2815_inst_last[channel].blue  != ws2815_inst[channel].blue)
    {
        ws2815_inst_last[channel].green = ws2815_inst[channel].green;
        ws2815_inst_last[channel].red   = ws2815_inst[channel].red;
        ws2815_inst_last[channel].blue  = ws2815_inst[channel].blue;
        ws2815_inst_changed = true;
    }

}

#else

#define WS2815_TIM_PSC      0U
#define WS2815_TIM_PERIOD   199U // 1.25us / (1 / (TIM_CLK / (TIM_PSC + 1))) - 1
#define WS2815_T1_TIMCCR    136U // WS2815_TIM_PERIOD * 0.8
#define WS2815_T0_TIMCCR    40U // WS2815_TIM_PERIOD * 0.2
#define WS2815_RESET_CYCLES 80U // 100us / code cycles(1.25us) = 80 cycles

DMA_HandleTypeDef hdma_tim3_ch2;
static TIM_HandleTypeDef ws2815_tim = {0};
static uint16_t ws2815_dma_buffer[WS2815_RESET_CYCLES + 24*LED_WS2815_NUMBER] = {0};
// static uint16_t ws2815_dma_buffer2[WS2815_RESET_CYCLES + 24*LED_WS2815_NUMBER] = {0};


void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* tim_pwmHandle)
{

  if(tim_pwmHandle->Instance==TIM3)
  {
  /* USER CODE BEGIN TIM3_MspInit 0 */

  /* USER CODE END TIM3_MspInit 0 */
    /* TIM3 clock enable */
    __HAL_RCC_TIM3_CLK_ENABLE();

    /* TIM3 DMA Init */
    /* TIM3_CH2 Init */
    hdma_tim3_ch2.Instance = DMA1_Channel1;
    hdma_tim3_ch2.Init.Request = DMA_REQUEST_TIM3_CH2;
    hdma_tim3_ch2.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_tim3_ch2.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_tim3_ch2.Init.MemInc = DMA_MINC_ENABLE;
    hdma_tim3_ch2.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_tim3_ch2.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_tim3_ch2.Init.Mode = DMA_NORMAL;
    hdma_tim3_ch2.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_tim3_ch2) != HAL_OK)
    {
      __disable_irq();
        while (1)
        {
        }
    }

    __HAL_LINKDMA(tim_pwmHandle,hdma[TIM_DMA_ID_CC2],hdma_tim3_ch2);

  /* USER CODE BEGIN TIM3_MspInit 1 */

  /* USER CODE END TIM3_MspInit 1 */
  }
}

void HAL_TIM_MspPostInit(TIM_HandleTypeDef* timHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(timHandle->Instance==TIM3)
  {
  /* USER CODE BEGIN TIM3_MspPostInit 0 */

  /* USER CODE END TIM3_MspPostInit 0 */

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**TIM3 GPIO Configuration
    PA7     ------> TIM3_CH2
    */
    GPIO_InitStruct.Pin = LED_WS2815_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN TIM3_MspPostInit 1 */

  /* USER CODE END TIM3_MspPostInit 1 */
  }

}

void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef* tim_pwmHandle)
{

  if(tim_pwmHandle->Instance==TIM3)
  {
  /* USER CODE BEGIN TIM3_MspDeInit 0 */

  /* USER CODE END TIM3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM3_CLK_DISABLE();

    /* TIM3 DMA DeInit */
    HAL_DMA_DeInit(tim_pwmHandle->hdma[TIM_DMA_ID_CC2]);
  /* USER CODE BEGIN TIM3_MspDeInit 1 */

  /* USER CODE END TIM3_MspDeInit 1 */
  }
}

static void ws2815_build_dma_pattern(uint8_t value, uint16_t* buffer, uint32_t start_index)
{
    for (uint8_t bit = 0; bit < 8; bit++)
    {
        uint8_t bit_value = (value << bit) & 0x80;
        buffer[start_index + bit] = bit_value ? WS2815_T1_TIMCCR : WS2815_T0_TIMCCR;
    }
}

static void ws2815_set_pixel_timer(uint8_t channel)
{

    if(channel >= LED_WS2815_NUMBER)
        channel = LED_WS2815_NUMBER - 1;

    if (ws2815_inst_last[channel].green != ws2815_inst[channel].green ||
        ws2815_inst_last[channel].red   != ws2815_inst[channel].red   ||
        ws2815_inst_last[channel].blue  != ws2815_inst[channel].blue)
    {
        ws2815_inst_last[channel].green = ws2815_inst[channel].green;
        ws2815_inst_last[channel].red   = ws2815_inst[channel].red;
        ws2815_inst_last[channel].blue  = ws2815_inst[channel].blue;
        ws2815_build_dma_pattern(ws2815_inst[channel].green, ws2815_dma_buffer, WS2815_RESET_CYCLES + channel * 24);
        ws2815_build_dma_pattern(ws2815_inst[channel].red,   ws2815_dma_buffer, WS2815_RESET_CYCLES + channel * 24 + 8);
        ws2815_build_dma_pattern(ws2815_inst[channel].blue,  ws2815_dma_buffer, WS2815_RESET_CYCLES + channel * 24 + 16);
        ws2815_inst_changed = true;
    }

}

#endif

void ws2815_init(void)
{
    if (ws2815_initialized)
        return;

#if (LED_WS2815_MODE == 0)
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = 0;
    GPIO_InitStruct.Pin = LED_WS2815_PIN;

    HAL_GPIO_Init(LED_WS2815_PORT, &GPIO_InitStruct);
    HAL_GPIO_WritePin(LED_WS2815_PORT, LED_WS2815_PIN, GPIO_PIN_RESET);
    ws2815_reset_gpio();
#else

    __HAL_RCC_DMAMUX1_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE(); //  使能DMA1控制器的时钟

    // HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 1, 0);
    // HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

    // TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};

    ws2815_tim.Instance = LED_WS2815_TIM;
    ws2815_tim.Init.Prescaler = WS2815_TIM_PSC;
    ws2815_tim.Init.CounterMode = TIM_COUNTERMODE_UP;
    ws2815_tim.Init.Period = WS2815_TIM_PERIOD;
    ws2815_tim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    ws2815_tim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    ws2815_tim.Init.RepetitionCounter = 0;
    // if (HAL_TIM_Base_Init(&ws2815_tim) != HAL_OK)
    // {
    //     __disable_irq();
    //     while (1)
    //     {
    //     }
    // }

    // sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    // if (HAL_TIM_ConfigClockSource(&ws2815_tim, &sClockSourceConfig) != HAL_OK)
    // {
    //     __disable_irq();
    //     while (1)
    //     {
    //     }
    // }

    if(HAL_TIM_PWM_Init(&ws2815_tim) != HAL_OK)
    {
        while (1)
        __disable_irq();
        {
        }
    }
    
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if(HAL_TIMEx_MasterConfigSynchronization(&ws2815_tim, &sMasterConfig) != HAL_OK)
    {
        __disable_irq();
        while (1)
        {
        }
    }

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if(HAL_TIM_PWM_ConfigChannel(&ws2815_tim, &sConfigOC, LED_WS2815_CHANNEL) != HAL_OK)
    {
        __disable_irq();
        while (1)
        {
        }
    }

    HAL_TIM_MspPostInit(&ws2815_tim);

    for (uint32_t i = 0; i < (sizeof(ws2815_dma_buffer) / sizeof(ws2815_dma_buffer[0])); i++)
    {
        ws2815_dma_buffer[i] = 0;
    }

#endif

    ws2815_update();
    ws2815_initialized = true;
}

static void ws2815_set_pixel(uint8_t channel)
{
    if(channel >= LED_WS2815_NUMBER)
        channel = LED_WS2815_NUMBER - 1;
#if (LED_WS2815_MODE == 0)
    ws2815_set_pixel_gpio(channel);
#else
    ws2815_set_pixel_timer(channel);
#endif
}

void ws2815_set_rx(uint8_t channel, bool status)
{
    if(channel >= LED_WS2815_NUMBER)
        channel = LED_WS2815_NUMBER - 1;
    ws2815_inst[channel].blue = status ? 0x0F : 0x00;
    // ws2815_inst[channel].red = status ? 0x00 : 0x0F;
    ws2815_set_pixel(channel); //  设置对应通道的像素值
    ws2815_update();
}

void ws2815_set_tx(uint8_t channel, bool status)
{
    if(channel >= LED_WS2815_NUMBER)
        channel = LED_WS2815_NUMBER - 1;
    // ws2815_inst[channel].blue = status ? 0x00 : 0x0F;
    ws2815_inst[channel].green = status ? 0x0F : 0x00;
    ws2815_set_pixel(channel);
    ws2815_update();
    
}

void ws2815_set_pwr(uint8_t status)
{
    if(0 == status)//under voltage
    {
        ws2815_inst[0].red = 0x00;
        ws2815_inst[0].green = 0x00;
        ws2815_inst[0].blue = 0x0F;
    }
    else if(1 == status)
    {
        ws2815_inst[0].red = 0x0F;
        ws2815_inst[0].green = 0x00;
        ws2815_inst[0].blue = 0x00;
    }
    else if(2 == status)
    {
        ws2815_inst[0].red = 0x00;
        ws2815_inst[0].green = 0x0F;
        ws2815_inst[0].blue = 0x00;
    }
    ws2815_set_pixel(0);
    ws2815_update();
}

void ws2815_update(void)
{
    if (ws2815_inst_changed)
    // if (1)
    {
        ws2815_inst_changed = false;
#if (LED_WS2815_MODE == 0)

        ws2815_reset_gpio();
        for(int8_t i = 0; i < LED_WS2815_NUMBER; i++)
        {
            ws2815_send_byte_gpio(ws2815_inst[i].green);
            ws2815_send_byte_gpio(ws2815_inst[i].red);
            ws2815_send_byte_gpio(ws2815_inst[i].blue);
        }
#else
        HAL_TIM_PWM_Stop_DMA(&ws2815_tim, LED_WS2815_CHANNEL);
        HAL_TIM_PWM_Start_DMA(&ws2815_tim, LED_WS2815_CHANNEL, (uint32_t *)ws2815_dma_buffer, sizeof(ws2815_dma_buffer) / sizeof(ws2815_dma_buffer[0]));
#endif
    }
}

#endif
