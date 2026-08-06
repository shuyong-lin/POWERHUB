/*
    The MIT License
    Copyright (c) 2025 ElmueSoft / Nakanishi Kiyomaro / Normadotcom
    https://netcult.ch/elmue/CANable Firmware Update
*/

#include "settings.h"
#include "interrupts.h"
#include "can.h"
#include "led.h"

extern PCD_HandleTypeDef PCD_Handle;
#if (LED_WS2815_MODE == 0 || LED_WS2815_ENABLE == 0) 

#else
extern DMA_HandleTypeDef hdma_tim3_ch2;
#endif

// Non maskable interrupt.
void NMI_Handler()
{
    while (1)
    {
    }
}

void HardFault_Handler()
{
    while (1)
    {
    }
}

void MemManage_Handler()
{
    while (1)
    {
    }
}

void BusFault_Handler()
{
    while (1)
    {
    }
}

void UsageFault_Handler()
{
    while (1)
    {
    }
}

// ---------------------------------------------------------------------

// System service call via SWI instruction.
void SVC_Handler()
{
}

void DebugMon_Handler()
{
}

// Pendable request for system service.
void PendSV_Handler()
{
}

// Handle SysTick interrupt
void SysTick_Handler()
{
    HAL_IncTick();
    HAL_SYSTICK_IRQHandler();
}

// ---------------------------------------------------------------------

// Handle Low Priority USB interrupts for STM32G4xx (Control, Bulk, Interrupt, Reset, Suspend, Wakeup, SOF)
void USB_LP_IRQHandler()
{
    HAL_PCD_IRQHandler(&PCD_Handle);
}

// Handle High Priority USB interrupts for STM32G4xx (Isochronous, not used)
void USB_HP_IRQHandler()
{
    HAL_PCD_IRQHandler(&PCD_Handle);
}

// Handle all USB interrupts for STM32G0xx
void USB_UCPD1_2_IRQHandler()
{
    HAL_PCD_IRQHandler(&PCD_Handle);
}

// ---------------------------------------------------------------------

// Handle FDCAN interrupts for STM32G4xx
void FDCAN1_IT0_IRQHandler(void)
{
    // This calls HAL_FDCAN_TimestampWraparoundCallback()
    HAL_FDCAN_IRQHandler(can_get_handle(0));
}

// Handle FDCAN interrupts for STM32G0xx
void TIM16_FDCAN_IT0_IRQHandler(void)
{
    // This calls HAL_FDCAN_TimestampWraparoundCallback()
    HAL_FDCAN_IRQHandler(can_get_handle(0));
}

#if (LED_WS2815_MODE == 0 || LED_WS2815_ENABLE == 0) 
#else
//This function handles DMA1 channel1 global interrupt for STM32G4xx.
void DMA1_Channel1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel1_IRQn 0 */

  /* USER CODE END DMA1_Channel1_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_tim3_ch2);
  /* USER CODE BEGIN DMA1_Channel1_IRQn 1 */

  /* USER CODE END DMA1_Channel1_IRQn 1 */
}
#endif