/**
  ******************************************************************************
  * @file      startup_N32H474.s
  * @author    MCD Application Team
  * @version   V1.0.0
  * @date      15-December-2023
  * @brief     STM32H474xx devices vector table for GCC based toolchains. 
  *            This module performs:
  *                - Set the initial SP
  *                - Set the initial PC == Reset_Handler,
  *                - Set the vector table entries with the exceptions ISR address
  *                - Branch to main in the C library (which eventually calls main()).
  *            After Reset the Cortex-M processor is in Thread mode,
  *            priority is Privileged, and the Stack is set to Main.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2023国民技术有限公司</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of Nuvoton Technology nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

  .syntax unified
  .cpu cortex-m4
  .fpu softvfp
  .thumb

.global  g_pfnVectors
.global  Default_Handler

/* start address for the initialization values of the .data section */
.word  _sidata
/* start address for the .data section */
.word  _sdata
/* end address for the .data section */
.word  _edata
/* start address for the .bss section */
.word  _sbss
/* end address for the .bss section */
.word  _ebss

    .section  .text.Reset_Handler
  .weak  Reset_Handler
  .type  Reset_Handler, %function
Reset_Handler:  
  ldr   sp, =_estack      /* set stack pointer */

/* Copy the data segment initializers from flash to SRAM */
  movs  r1, #0
  b  LoopCopyDataInit

CopyDataInit:
  ldr  r3, =_sidata
  ldr  r3, [r3, r1]
  str  r3, [r0, r1]
  adds  r1, r1, #4

LoopCopyDataInit:
  ldr  r0, =_sdata
  ldr  r3, =_edata
  adds  r2, r0, r1
  cmp  r2, r3
  bcc  CopyDataInit

/* Zero fill the bss segment. */
  ldr  r2, =_sbss
  ldr  r4, =_ebss
  movs  r3, #0
  b  LoopFillZerobss

FillZerobss:
  str  r3, [r2]
  adds  r2, r2, #4

LoopFillZerobss:
  cmp  r2, r4
  bcc  FillZerobss

/* Call the clock system initialization function.*/
  bl  SystemInit
/* Call the application's entry point.*/
  bl  main
  bx  lr
  .size  Reset_Handler, .-Reset_Handler

/**
 * @brief  This is the code that gets called when an exception occurs.
 * @param  None
 * @retval : None
 */
    .section  .text.Default_Handler,"ax",%progbits
Default_Handler:
Infinite_Loop:
  b  Infinite_Loop
  .size  Default_Handler, .-Default_Handler

/******************************************************************************
*
* The minimal vector table for a Cortex-M4.  Note that the proper constructs
* must be placed on this to ensure that it aligns properly to a 256 byte
* boundary.
*
******************************************************************************/
    .section  .isr_vector,"a",%progbits
  .type  g_pfnVectors, %object
  .size  g_pfnVectors, .-g_pfnVectors

g_pfnVectors:
  .word  _estack
  .word  Reset_Handler
  .word  NMI_Handler
  .word  HardFault_Handler
  .word  MemManage_Handler
  .word  BusFault_Handler
  .word  UsageFault_Handler
  .word  0
  .word  0
  .word  0
  .word  0
  .word  SVC_Handler
  .word  DebugMon_Handler
  .word  0
  .word  PendSV_Handler
  .word  SysTick_Handler

  /* External Interrupts */
  .word  WWDG_IRQHandler                   /* Window WatchDog              */
  .word  PVD_AVD_IRQHandler                 /* PVD/AVD through EXTI Line    */
  .word  RTC_TAMP_IRQHandler                /* RTC through the EXTI line    */
  .word  FLASH_IRQHandler                   /* FLASH                        */
  .word  RCC_CRS_IRQHandler                 /* RCC & CRS                    */
  .word  EXTI0_IRQHandler                   /* EXTI Line0                   */
  .word  EXTI1_IRQHandler                   /* EXTI Line1                   */
  .word  EXTI2_IRQHandler                   /* EXTI Line2                   */
  .word  EXTI3_IRQHandler                   /* EXTI Line3                   */
  .word  EXTI4_IRQHandler                   /* EXTI Line4                   */
  .word  DMA1_Channel1_IRQHandler           /* DMA1 Channel 1               */
  .word  DMA1_Channel2_IRQHandler           /* DMA1 Channel 2               */
  .word  DMA1_Channel3_IRQHandler           /* DMA1 Channel 3               */
  .word  DMA1_Channel4_IRQHandler           /* DMA1 Channel 4               */
  .word  DMA1_Channel5_IRQHandler           /* DMA1 Channel 5               */
  .word  DMA1_Channel6_IRQHandler           /* DMA1 Channel 6               */
  .word  DMA1_Channel7_IRQHandler           /* DMA1 Channel 7               */
  .word  ADC1_IRQHandler                    /* ADC1                         */
  .word  DAC1_IRQHandler                    /* DAC1 & DAC2                  */
  .word  CORDIC_IRQHandler                  /* CORDIC                       */
  .word  FMAC_IRQHandler                    /* FMAC                         */
  .word  EXTI9_5_IRQHandler                 /* External Line[9:5]s          */
  .word  TIM1_BRK_TIM15_IRQHandler          /* TIM1 Break and TIM15         */
  .word  TIM1_UP_TIM16_IRQHandler           /* TIM1 Update and TIM16       */
  .word  TIM1_TRG_COM_TIM17_IRQHandler      /* TIM1 Trigger and Commutation and TIM17 */
  .word  TIM1_CC_IRQHandler                 /* TIM1 Capture Compare         */
  .word  TIM2_IRQHandler                    /* TIM2                         */
  .word  TIM3_IRQHandler                    /* TIM3                         */
  .word  TIM4_IRQHandler                    /* TIM4                         */
  .word  I2C1_EV_IRQHandler                  /* I2C1 Event                   */
  .word  I2C1_ER_IRQHandler                  /* I2C1 Error                   */
  .word  I2C2_EV_IRQHandler                  /* I2C2 Event                   */
  .word  I2C2_ER_IRQHandler                  /* I2C2 Error                   */
  .word  SPI1_IRQHandler                    /* SPI1                         */
  .word  SPI2_IRQHandler                    /* SPI2                         */
  .word  USART1_IRQHandler                  /* USART1                       */
  .word  USART2_IRQHandler                  /* USART2                       */
  .word  USART3_IRQHandler                  /* USART3                       */
  .word  EXTI15_10_IRQHandler               /* External Line[15:10]s        */
  .word  RTC_Alarm_IRQHandler               /* RTC Alarms (A & B) through EXTI Line */
  .word  DFSDM1_FLT0_IRQHandler             /* DFSDM1 Filter0              */
  .word  DFSDM1_FLT1_IRQHandler             /* DFSDM1 Filter1              */
  .word  DFSDM1_FLT2_IRQHandler             /* DFSDM1 Filter2              */
  .word  DFSDM1_FLT3_IRQHandler             /* DFSDM1 Filter3              */
  .word  TIM5_IRQHandler                    /* TIM5                         */
  .word  SPI3_IRQHandler                    /* SPI3                         */
  .word  DMA1_Channel8_IRQHandler           /* DMA1 Channel8               */
  .word  DMA2_Channel1_IRQHandler           /* DMA2 Channel1               */
  .word  DMA2_Channel2_IRQHandler           /* DMA2 Channel2               */
  .word  DMA2_Channel3_IRQHandler           /* DMA2 Channel3               */
  .word  DMA2_Channel4_IRQHandler           /* DMA2 Channel4               */
  .word  DMA2_Channel5_IRQHandler           /* DMA2 Channel5               */
  .word  DFSDM2_FLT0_IRQHandler             /* DFSDM2 Filter0              */
  .word  DFSDM2_FLT1_IRQHandler             /* DFSDM2 Filter1              */
  .word  I2C3_EV_IRQHandler                  /* I2C3 Event                   */
  .word  I2C3_ER_IRQHandler                  /* I2C3 Error                   */
  .word  SAI1_IRQHandler                    /* SAI1                         */
  .word  SAI2_IRQHandler                    /* SAI2                         */
  .word  QUADSPI_IRQHandler                /* QUADSPI                     */
  .word  CEC_IRQHandler                     /* HDMI_CEC                     */
  .word  UART4_IRQHandler                  /* UART4                        */
  .word  UART5_IRQHandler                  /* UART5                        */
  .word  TIM6_DAC_IRQHandler                /* TIM6 and DAC1&2 underrun errors */
  .word  TIM7_IRQHandler                    /* TIM7                         */
  .word  DMA2_Channel6_IRQHandler           /* DMA2 Channel6               */
  .word  DMA2_Channel7_IRQHandler           /* DMA2 Channel7               */
  .word  LPTIM1_IRQHandler                  /* LPTIM1                       */
  .word  LPTIM2_IRQHandler                  /* LPTIM2                       */
  .word  LPTIM3_IRQHandler                  /* LPTIM3                       */
  .word  LPTIM4_IRQHandler                  /* LPTIM4                       */
  .word  LPTIM5_IRQHandler                  /* LPTIM5                       */
  .word  LPUART1_IRQHandler                 /* LPUART1                      */
  .word  WWDG_RST_IRQHandler                /* Window Watchdog reset interrupt     */
  .word  CRS_IRQHandler                     /* Clock Recovery Global Interrupt      */
  .word  PWR_SOTF_BLEACT_RFPHASE_IRQHandler  /* PWR wake-up CPU  interrupts         */
  .word  IPCC_C1_RX_IRQHandler              /* IPCC CPU1 RX occupied interrupt     */
  .word  IPCC_C1_TX_IRQHandler              /* IPCC CPU1 TX free interrupt         */
  .word  HSEM_IRQHandler                    /* HSEM0 Interrupt                  */
  .word  I2C4_EV_IRQHandler                 /* I2C4 Event                       */
  .word  I2C4_ER_IRQHandler                 /* I2C4 Error                       */
  .word  UCPD1_IRQHandler                   /* UCPD1                           */
  .word  ICACHE_IRQHandler                  /* Instruction Cache error interrupt     */
  .word  SECURE_WATCHDOG_IRQHandler         /* Secure Watchdog Interrupt            */
  .word  DMA2_Channel8_IRQHandler           /* DMA2 Channel8                   */
  .word  DMA2_Channel9_IRQHandler           /* DMA2 Channel9                   */
  .word  DMA2_Channel10_IRQHandler          /* DMA2 Channel10                  */
  .word  DMA2_Channel11_IRQHandler          /* DMA2 Channel11                  */
  .word  DMA2_Channel12_IRQHandler          /* DMA2 Channel12                  */
  .word  DMA2_Channel13_IRQHandler          /* DMA2 Channel13                  */
  .word  DMA2_Channel14_IRQHandler          /* DMA2 Channel14                  */
  .word  DMA2_Channel15_IRQHandler          /* DMA2 Channel15                  */
  .word  DMA2S_IRQHandler                   /* DMA2D                          */
  .word  DCMI_PSSI_IRQHandler              /* DCMI/PSSI                      */
  .word  LTDC_IRQHandler                    /* LTDC                           */
  .word  LTDC_ER_IRQHandler                 /* LTDC error                     */
  .word  DSI_IRQHandler                     /* DSI                            */
  .word  MDMA_IRQHandler                    /* MDMA                           */
  .word  SDMMC1_IRQHandler                  /* SDMMC1                         */
  .word  SDMMC2_IRQHandler                  /* SDMMC2                         */
  .word  MDIOS_IRQHandler                   /* MDIOS                         */
  .word  MDMA_ERROR_IRQHandler              /* MDMA error                     */
  .word  JPEG_IRQHandler                    /* JPEG                           */
  .word  SYNFAXI_IRQHandler                 /* SYNFAXI                        */
  .word  DTS_IRQHandler                     /* Digital Temperature Sensor interrupt */
  .word  GPIOJ_IRQHandler                   /* GPIOJ                          */
  .word  GPIOI_IRQHandler                   /* GPIOI                          */
  .word  GPIOH_IRQHandler                   /* GPIOH                          */
  .word  GPIOG_IRQHandler                   /* GPIOG                          */
  .word  GPIOF_IRQHandler                   /* GPIOF                          */
  .word  GPIOE_IRQHandler                   /* GPIOE                          */
  .word  GPIOD_IRQHandler                   /* GPIOD                          */
  .word  GPIOC_IRQHandler                   /* GPIOC                          */
  .word  GPIOB_IRQHandler                   /* GPIOB                          */
  .word  GPIOA_IRQHandler                   /* GPIOA                          */
  .word  AES1_IRQHandler                    /* AES1                           */
  .word  RNG_IRQHandler                     /* RNG                            */
  .word  PKA_IRQHandler                     /* PKA                            */
  .word  DMA2D_IRQHandler                   /* DMA2D                          */
  .word  SAI3_IRQHandler                    /* SAI3                           */
  .word  SAI4_IRQHandler                    /* SAI4                           */
  .word  DTS_CAL_IRQHandler                 /* Digital Temperature Sensor calibration interrupt */
  .word  DMA1_Channel9_IRQHandler          /* DMA1 Channel9                  */
  .word  DMA1_Channel10_IRQHandler          /* DMA1 Channel10                 */
  .word  DMA1_Channel11_IRQHandler          /* DMA1 Channel11                 */
  .word  DMA1_Channel12_IRQHandler          /* DMA1 Channel12                 */
  .word  DMA1_Channel13_IRQHandler          /* DMA1 Channel13                 */
  .word  DMA1_Channel14_IRQHandler          /* DMA1 Channel14                 */
  .word  DMA1_Channel15_IRQHandler          /* DMA1 Channel15                 */
  .word  DMA1_Channel16_IRQHandler          /* DMA1 Channel16                 */
  .word  DMA1_Channel17_IRQHandler          /* DMA1 Channel17                 */
  .word  DMA1_Channel18_IRQHandler          /* DMA1 Channel18                 */
  .word  DMA1_Channel19_IRQHandler          /* DMA1 Channel19                 */
  .word  DMA1_Channel20_IRQHandler          /* DMA1 Channel20                 */
  .word  DMA1_Channel21_IRQHandler          /* DMA1 Channel21                 */
  .word  DMA1_Channel22_IRQHandler          /* DMA1 Channel22                 */
  .word  DMA1_Channel23_IRQHandler          /* DMA1 Channel23                 */
  .word  DMA1_Channel24_IRQHandler          /* DMA1 Channel24                 */
  .word  DMA1_Channel25_IRQHandler          /* DMA1 Channel25                 */
  .word  DMA1_Channel26_IRQHandler          /* DMA1 Channel26                 */
  .word  DMA1_Channel27_IRQHandler          /* DMA1 Channel27                 */
  .word  DMA1_Channel28_IRQHandler          /* DMA1 Channel28                 */
  .word  DMA1_Channel29_IRQHandler          /* DMA1 Channel29                 */
  .word  DMA1_Channel30_IRQHandler          /* DMA1 Channel30                 */
  .word  DMA1_Channel31_IRQHandler          /* DMA1 Channel31                 */
  .word  DMAMUX_IRQHandler                  /* DMAMUX                         */

/*******************************************************************************
*
* Provide weak aliases for each Exception handler to the Default_Handler.
* As they are weak aliases, any function with the same name will override
* this definition.
*
*******************************************************************************/

  .weak      NMI_Handler
  .thumb_set NMI_Handler,Default_Handler

  .weak      HardFault_Handler
  .thumb_set HardFault_Handler,Default_Handler

  .weak      MemManage_Handler
  .thumb_set MemManage_Handler,Default_Handler

  .weak      BusFault_Handler
  .thumb_set BusFault_Handler,Default_Handler

  .weak      UsageFault_Handler
  .thumb_set UsageFault_Handler,Default_Handler

  .weak      SVC_Handler
  .thumb_set SVC_Handler,Default_Handler

  .weak      DebugMon_Handler
  .thumb_set DebugMon_Handler,Default_Handler

  .weak      PendSV_Handler
  .thumb_set PendSV_Handler,Default_Handler

  .weak      SysTick_Handler
  .thumb_set SysTick_Handler,Default_Handler

  .weak      WWDG_IRQHandler
  .thumb_set WWDG_IRQHandler,Default_Handler

  .weak      PVD_AVD_IRQHandler
  .thumb_set PVD_AVD_IRQHandler,Default_Handler

  .weak      RTC_TAMP_IRQHandler
  .thumb_set RTC_TAMP_IRQHandler,Default_Handler

  .weak      FLASH_IRQHandler
  .thumb_set FLASH_IRQHandler,Default_Handler

  .weak      RCC_CRS_IRQHandler
  .thumb_set RCC_CRS_IRQHandler,Default_Handler

  .weak      EXTI0_IRQHandler
  .thumb_set EXTI0_IRQHandler,Default_Handler

  .weak      EXTI1_IRQHandler
  .thumb_set EXTI1_IRQHandler,Default_Handler

  .weak      EXTI2_IRQHandler
  .thumb_set EXTI2_IRQHandler,Default_Handler

  .weak      EXTI3_IRQHandler
  .thumb_set EXTI3_IRQHandler,Default_Handler

  .weak      EXTI4_IRQHandler
  .thumb_set EXTI4_IRQHandler,Default_Handler

  .weak      DMA1_Channel1_IRQHandler
  .thumb_set DMA1_Channel1_IRQHandler,Default_Handler

  .weak      DMA1_Channel2_IRQHandler
  .thumb_set DMA1_Channel2_IRQHandler,Default_Handler

  .weak      DMA1_Channel3_IRQHandler
  .thumb_set DMA1_Channel3_IRQHandler,Default_Handler

  .weak      DMA1_Channel4_IRQHandler
  .thumb_set DMA1_Channel4_IRQHandler,Default_Handler

  .weak      DMA1_Channel5_IRQHandler
  .thumb_set DMA1_Channel5_IRQHandler,Default_Handler

  .weak      DMA1_Channel6_IRQHandler
  .thumb_set DMA1_Channel6_IRQHandler,Default_Handler

  .weak      DMA1_Channel7_IRQHandler
  .thumb_set DMA1_Channel7_IRQHandler,Default_Handler

  .weak      ADC1_IRQHandler
  .thumb_set ADC1_IRQHandler,Default_Handler

  .weak      DAC1_IRQHandler
  .thumb_set DAC1_IRQHandler,Default_Handler

  .weak      CORDIC_IRQHandler
  .thumb_set CORDIC_IRQHandler,Default_Handler

  .weak      FMAC_IRQHandler
  .thumb_set FMAC_IRQHandler,Default_Handler

  .weak      EXTI9_5_IRQHandler
  .thumb_set EXTI9_5_IRQHandler,Default_Handler

  .weak      TIM1_BRK_TIM15_IRQHandler
  .thumb_set TIM1_BRK_TIM15_IRQHandler,Default_Handler

  .weak      TIM1_UP_TIM16_IRQHandler
  .thumb_set TIM1_UP_TIM16_IRQHandler,Default_Handler

  .weak      TIM1_TRG_COM_TIM17_IRQHandler
  .thumb_set TIM1_TRG_COM_TIM17_IRQHandler,Default_Handler

  .weak      TIM1_CC_IRQHandler
  .thumb_set TIM1_CC_IRQHandler,Default_Handler

  .weak      TIM2_IRQHandler
  .thumb_set TIM2_IRQHandler,Default_Handler

  .weak      TIM3_IRQHandler
  .thumb_set TIM3_IRQHandler,Default_Handler

  .weak      TIM4_IRQHandler
  .thumb_set TIM4_IRQHandler,Default_Handler

  .weak      I2C1_EV_IRQHandler
  .thumb_set I2C1_EV_IRQHandler,Default_Handler

  .weak      I2C1_ER_IRQHandler
  .thumb_set I2C1_ER_IRQHandler,Default_Handler

  .weak      I2C2_EV_IRQHandler
  .thumb_set I2C2_EV_IRQHandler,Default_Handler

  .weak      I2C2_ER_IRQHandler
  .thumb_set I2C2_ER_IRQHandler,Default_Handler

  .weak      SPI1_IRQHandler
  .thumb_set SPI1_IRQHandler,Default_Handler

  .weak      SPI2_IRQHandler
  .thumb_set SPI2_IRQHandler,Default_Handler

  .weak      USART1_IRQHandler
  .thumb_set USART1_IRQHandler,Default_Handler

  .weak      USART2_IRQHandler
  .thumb_set USART2_IRQHandler,Default_Handler

  .weak      USART3_IRQHandler
  .thumb_set USART3_IRQHandler,Default_Handler

  .weak      EXTI15_10_IRQHandler
  .thumb_set EXTI15_10_IRQHandler,Default_Handler

  .weak      RTC_Alarm_IRQHandler
  .thumb_set RTC_Alarm_IRQHandler,Default_Handler

  .weak      DFSDM1_FLT0_IRQHandler
  .thumb_set DFSDM1_FLT0_IRQHandler,Default_Handler

  .weak      DFSDM1_FLT1_IRQHandler
  .thumb_set DFSDM1_FLT1_IRQHandler,Default_Handler

  .weak      DFSDM1_FLT2_IRQHandler
  .thumb_set DFSDM1_FLT2_IRQHandler,Default_Handler

  .weak      DFSDM1_FLT3_IRQHandler
  .thumb_set DFSDM1_FLT3_IRQHandler,Default_Handler

  .weak      TIM5_IRQHandler
  .thumb_set TIM5_IRQHandler,Default_Handler

  .weak      SPI3_IRQHandler
  .thumb_set SPI3_IRQHandler,Default_Handler

  .weak      DMA1_Channel8_IRQHandler
  .thumb_set DMA1_Channel8_IRQHandler,Default_Handler

  .weak      DMA2_Channel1_IRQHandler
  .thumb_set DMA2_Channel1_IRQHandler,Default_Handler

  .weak      DMA2_Channel2_IRQHandler
  .thumb_set DMA2_Channel2_IRQHandler,Default_Handler

  .weak      DMA2_Channel3_IRQHandler
  .thumb_set DMA2_Channel3_IRQHandler,Default_Handler

  .weak      DMA2_Channel4_IRQHandler
  .thumb_set DMA2_Channel4_IRQHandler,Default_Handler

  .weak      DMA2_Channel5_IRQHandler
  .thumb_set DMA2_Channel5_IRQHandler,Default_Handler

  .weak      DFSDM2_FLT0_IRQHandler
  .thumb_set DFSDM2_FLT0_IRQHandler,Default_Handler

  .weak      DFSDM2_FLT1_IRQHandler
  .thumb_set DFSDM2_FLT1_IRQHandler,Default_Handler

  .weak      I2C3_EV_IRQHandler
  .thumb_set I2C3_EV_IRQHandler,Default_Handler

  .weak      I2C3_ER_IRQHandler
  .thumb_set I2C3_ER_IRQHandler,Default_Handler

  .weak      SAI1_IRQHandler
  .thumb_set SAI1_IRQHandler,Default_Handler

  .weak      SAI2_IRQHandler
  .thumb_set SAI2_IRQHandler,Default_Handler

  .weak      QUADSPI_IRQHandler
  .thumb_set QUADSPI_IRQHandler,Default_Handler

  .weak      CEC_IRQHandler
  .thumb_set CEC_IRQHandler,Default_Handler

  .weak      UART4_IRQHandler
  .thumb_set UART4_IRQHandler,Default_Handler

  .weak      UART5_IRQHandler
  .thumb_set UART5_IRQHandler,Default_Handler

  .weak      TIM6_DAC_IRQHandler
  .thumb_set TIM6_DAC_IRQHandler,Default_Handler

  .weak      TIM7_IRQHandler
  .thumb_set TIM7_IRQHandler,Default_Handler

  .weak      DMA2_Channel6_IRQHandler
  .thumb_set DMA2_Channel6_IRQHandler,Default_Handler

  .weak      DMA2_Channel7_IRQHandler
  .thumb_set DMA2_Channel7_IRQHandler,Default_Handler

  .weak      LPTIM1_IRQHandler
  .thumb_set LPTIM1_IRQHandler,Default_Handler

  .weak      LPTIM2_IRQHandler
  .thumb_set LPTIM2_IRQHandler,Default_Handler

  .weak      LPTIM3_IRQHandler
  .thumb_set LPTIM3_IRQHandler,Default_Handler

  .weak      LPTIM4_IRQHandler
  .thumb_set LPTIM4_IRQHandler,Default_Handler

  .weak      LPTIM5_IRQHandler
  .thumb_set LPTIM5_IRQHandler,Default_Handler

  .weak      LPUART1_IRQHandler
  .thumb_set LPUART1_IRQHandler,Default_Handler

  .weak      WWDG_RST_IRQHandler
  .thumb_set WWDG_RST_IRQHandler,Default_Handler

  .weak      CRS_IRQHandler
  .thumb_set CRS_IRQHandler,Default_Handler

  .weak      PWR_SOTF_BLEACT_RFPHASE_IRQHandler
  .thumb_set PWR_SOTF_BLEACT_RFPHASE_IRQHandler,Default_Handler

  .weak      IPCC_C1_RX_IRQHandler
  .thumb_set IPCC_C1_RX_IRQHandler,Default_Handler

  .weak      IPCC_C1_TX_IRQHandler
  .thumb_set IPCC_C1_TX_IRQHandler,Default_Handler

  .weak      HSEM_IRQHandler
  .thumb_set HSEM_IRQHandler,Default_Handler

  .weak      I2C4_EV_IRQHandler
  .thumb_set I2C4_EV_IRQHandler,Default_Handler

  .weak      I2C4_ER_IRQHandler
  .thumb_set I2C4_ER_IRQHandler,Default_Handler

  .weak      UCPD1_IRQHandler
  .thumb_set UCPD1_IRQHandler,Default_Handler

  .weak      ICACHE_IRQHandler
  .thumb_set ICACHE_IRQHandler,Default_Handler

  .weak      SECURE_WATCHDOG_IRQHandler
  .thumb_set SECURE_WATCHDOG_IRQHandler,Default_Handler

  .weak      DMA2_Channel8_IRQHandler
  .thumb_set DMA2_Channel8_IRQHandler,Default_Handler

  .weak      DMA2_Channel9_IRQHandler
  .thumb_set DMA2_Channel9_IRQHandler,Default_Handler

  .weak      DMA2_Channel10_IRQHandler
  .thumb_set DMA2_Channel10_IRQHandler,Default_Handler

  .weak      DMA2_Channel11_IRQHandler
  .thumb_set DMA2_Channel11_IRQHandler,Default_Handler

  .weak      DMA2_Channel12_IRQHandler
  .thumb_set DMA2_Channel12_IRQHandler,Default_Handler

  .weak      DMA2_Channel13_IRQHandler
  .thumb_set DMA2_Channel13_IRQHandler,Default_Handler

  .weak      DMA2_Channel14_IRQHandler
  .thumb_set DMA2_Channel14_IRQHandler,Default_Handler

  .weak      DMA2_Channel15_IRQHandler
  .thumb_set DMA2_Channel15_IRQHandler,Default_Handler

  .weak      DMA2S_IRQHandler
  .thumb_set DMA2S_IRQHandler,Default_Handler

  .weak      DCMI_PSSI_IRQHandler
  .thumb_set DCMI_PSSI_IRQHandler,Default_Handler

  .weak      LTDC_IRQHandler
  .thumb_set LTDC_IRQHandler,Default_Handler

  .weak      LTDC_ER_IRQHandler
  .thumb_set LTDC_ER_IRQHandler,Default_Handler

  .weak      DSI_IRQHandler
  .thumb_set DSI_IRQHandler,Default_Handler

  .weak      MDMA_IRQHandler
  .thumb_set MDMA_IRQHandler,Default_Handler

  .weak      SDMMC1_IRQHandler
  .thumb_set SDMMC1_IRQHandler,Default_Handler

  .weak      SDMMC2_IRQHandler
  .thumb_set SDMMC2_IRQHandler,Default_Handler

  .weak      MDIOS_IRQHandler
  .thumb_set MDIOS_IRQHandler,Default_Handler

  .weak      MDMA_ERROR_IRQHandler
  .thumb_set MDMA_ERROR_IRQHandler,Default_Handler

  .weak      JPEG_IRQHandler
  .thumb_set JPEG_IRQHandler,Default_Handler

  .weak      SYNFAXI_IRQHandler
  .thumb_set SYNFAXI_IRQHandler,Default_Handler

  .weak      DTS_IRQHandler
  .thumb_set DTS_IRQHandler,Default_Handler

  .weak      GPIOJ_IRQHandler
  .thumb_set GPIOJ_IRQHandler,Default_Handler

  .weak      GPIOI_IRQHandler
  .thumb_set GPIOI_IRQHandler,Default_Handler

  .weak      GPIOH_IRQHandler
  .thumb_set GPIOH_IRQHandler,Default_Handler

  .weak      GPIOG_IRQHandler
  .thumb_set GPIOG_IRQHandler,Default_Handler

  .weak      GPIOF_IRQHandler
  .thumb_set GPIOF_IRQHandler,Default_Handler

  .weak      GPIOE_IRQHandler
  .thumb_set GPIOE_IRQHandler,Default_Handler

  .weak      GPIOD_IRQHandler
  .thumb_set GPIOD_IRQHandler,Default_Handler

  .weak      GPIOC_IRQHandler
  .thumb_set GPIOC_IRQHandler,Default_Handler

  .weak      GPIOB_IRQHandler
  .thumb_set GPIOB_IRQHandler,Default_Handler

  .weak      GPIOA_IRQHandler
  .thumb_set GPIOA_IRQHandler,Default_Handler

  .weak      AES1_IRQHandler
  .thumb_set AES1_IRQHandler,Default_Handler

  .weak      RNG_IRQHandler
  .thumb_set RNG_IRQHandler,Default_Handler

  .weak      PKA_IRQHandler
  .thumb_set PKA_IRQHandler,Default_Handler

  .weak      DMA2D_IRQHandler
  .thumb_set DMA2D_IRQHandler,Default_Handler

  .weak      SAI3_IRQHandler
  .thumb_set SAI3_IRQHandler,Default_Handler

  .weak      SAI4_IRQHandler
  .thumb_set SAI4_IRQHandler,Default_Handler

  .weak      DTS_CAL_IRQHandler
  .thumb_set DTS_CAL_IRQHandler,Default_Handler

  .weak      DMA1_Channel9_IRQHandler
  .thumb_set DMA1_Channel9_IRQHandler,Default_Handler

  .weak      DMA1_Channel10_IRQHandler
  .thumb_set DMA1_Channel10_IRQHandler,Default_Handler

  .weak      DMA1_Channel11_IRQHandler
  .thumb_set DMA1_Channel11_IRQHandler,Default_Handler

  .weak      DMA1_Channel12_IRQHandler
  .thumb_set DMA1_Channel12_IRQHandler,Default_Handler

  .weak      DMA1_Channel13_IRQHandler
  .thumb_set DMA1_Channel13_IRQHandler,Default_Handler

  .weak      DMA1_Channel14_IRQHandler
  .thumb_set DMA1_Channel14_IRQHandler,Default_Handler

  .weak      DMA1_Channel15_IRQHandler
  .thumb_set DMA1_Channel15_IRQHandler,Default_Handler

  .weak      DMA1_Channel16_IRQHandler
  .thumb_set DMA1_Channel16_IRQHandler,Default_Handler

  .weak      DMA1_Channel17_IRQHandler
  .thumb_set DMA1_Channel17_IRQHandler,Default_Handler

  .weak      DMA1_Channel18_IRQHandler
  .thumb_set DMA1_Channel18_IRQHandler,Default_Handler

  .weak      DMA1_Channel19_IRQHandler
  .thumb_set DMA1_Channel19_IRQHandler,Default_Handler

  .weak      DMA1_Channel20_IRQHandler
  .thumb_set DMA1_Channel20_IRQHandler,Default_Handler

  .weak      DMA1_Channel21_IRQHandler
  .thumb_set DMA1_Channel21_IRQHandler,Default_Handler

  .weak      DMA1_Channel22_IRQHandler
  .thumb_set DMA1_Channel22_IRQHandler,Default_Handler

  .weak      DMA1_Channel23_IRQHandler
  .thumb_set DMA1_Channel23_IRQHandler,Default_Handler

  .weak      DMA1_Channel24_IRQHandler
  .thumb_set DMA1_Channel24_IRQHandler,Default_Handler

  .weak      DMA1_Channel25_IRQHandler
  .thumb_set DMA1_Channel25_IRQHandler,Default_Handler

  .weak      DMA1_Channel26_IRQHandler
  .thumb_set DMA1_Channel26_IRQHandler,Default_Handler

  .weak      DMA1_Channel27_IRQHandler
  .thumb_set DMA1_Channel27_IRQHandler,Default_Handler

  .weak      DMA1_Channel28_IRQHandler
  .thumb_set DMA1_Channel28_IRQHandler,Default_Handler

  .weak      DMA1_Channel29_IRQHandler
  .thumb_set DMA1_Channel29_IRQHandler,Default_Handler

  .weak      DMA1_Channel30_IRQHandler
  .thumb_set DMA1_Channel30_IRQHandler,Default_Handler

  .weak      DMA1_Channel31_IRQHandler
  .thumb_set DMA1_Channel31_IRQHandler,Default_Handler

  .weak      DMAMUX_IRQHandler
  .thumb_set DMAMUX_IRQHandler,Default_Handler

