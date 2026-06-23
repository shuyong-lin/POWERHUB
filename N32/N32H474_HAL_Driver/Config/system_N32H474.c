
/**
  ******************************************************************************
  * @file    system_N32H474.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    15-December-2023
  * @brief   CMSIS Cortex-M4 Device Peripheral Access Layer System Source File.
  *
  *          This file provides two functions and one global variable to be called from 
  *          user application:
  *            1. SystemInit(): This function is called at startup just after reset and 
  *               before branch to main program. This call is made inside the "startup_N32H474.s"
  *               file.
  *            2. SystemCoreClock variable: Contains the core clock (HCLK), it can be used
  *               by the user application to setup the SysTick timer or configure other
  *               parameters.
  *            3. SystemCoreClockUpdate(): Updates the variable SystemCoreClock and must
  *               be called whenever the core clock is changed during program execution.
  *
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

/** @addtogroup CMSIS
  * @{
  */

/** @addtogroup N32H474_System
  * @{
  */ 

/** @addtogroup N32H474_System_Private_Includes
  * @{
  */

#include "N32H474xx.h"

/**
  * @}
  */

/** @addtogroup N32H474_System_Private_TypesDefinitions
  * @{
  */

/**
  * @}
  */

/** @addtogroup N32H474_System_Private_Defines
  * @{
  */

/*!< Uncomment the line corresponding to the desired System clock (SYSCLK)
   frequency (assuming the HSE oscillator is used as System clock source)
   :  
  */
#if defined (N32H474xx)
  #define __HSE_VALUE    ((uint32_t)8000000U)    /*!< Value of the External oscillator in Hz */
  #define __HSI_VALUE    ((uint32_t)8000000U)    /*!< Value of the Internal oscillator in Hz */
  #define __LSI_VALUE    ((uint32_t)40000U)     /*!< Value of the Internal Low Speed oscillator in Hz */
  #define __LSE_VALUE    ((uint32_t)32768U)     /*!< Value of the External Low Speed oscillator in Hz */
#endif /* N32H474xx */

/*!< Uncomment the following line if you need to use external SRAM  */
/* #define DATA_IN_ExtSRAM */

/*!< Uncomment the following line if you need to relocate your vector Table in
     Internal SRAM. */
/* #define VECT_TAB_SRAM */
#define VECT_TAB_OFFSET  0x00000000U /*!< Vector Table base offset field. 
                                   This value must be a multiple of 0x200. */

/**
  * @}
  */

/** @addtogroup N32H474_System_Private_Macros
  * @{
  */

/**
  * @}
  */

/** @addtogroup N32H474_System_Private_Variables
  * @{
  */

/*******************************************************************************
*  Clock Definitions
*******************************************************************************/
#if defined (N32H474xx)
uint32_t SystemCoreClock = 8000000U;        /*!< System Clock Frequency (Core Clock) */
#endif /* N32H474xx */

const uint8_t  AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};
const uint8_t  APBPrescTable[8] =  {0, 0, 0, 0, 1, 2, 3, 4};

/**
  * @}
  */

/** @addtogroup N32H474_System_Private_FunctionPrototypes
  * @{
  */

static void SetSysClk(void);
static void SetSysClockToHSE(void);
static void SetSysClockToPLL(void);
static void SystemClock_Config(void);

/**
  * @}
  */

/** @addtogroup N32H474_System_Private_Functions
  * @{
  */

/**
  * @brief  Setup the microcontroller system.
  *         Initialize the FPU setting, Vector Table location and 
  *         the PLL configuration is reset.
  * @param  None
  * @retval None
  */

void SystemInit (void)
{
  /* FPU settings ------------------------------------------------------------*/
  #if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
    SCB->CPACR |= ((uint32_t)0x3 << 10*2);  /* set CP10 Full Access */
    SCB->CPACR |= ((uint32_t)0x3 << 11*2);  /* set CP11 Full Access */
  #endif

  /* Reset the RCC clock configuration to the default reset state ------------*/
  /* Set HSION bit */
  RCC->CR |= (uint32_t)0x00000001;

  /* Reset SW[1:0], HPRE[3:0], PPRE[2:0], ADCPRE[1:0], MCO[2:0] bits */
  RCC->CFGR &= (uint32_t)0xF8FFB7FU;

  /* Reset HSEON, CSSON and PLLON bits */
  RCC->CR &= (uint32_t)0xFEF6FFFF;

  /* Reset PLLCFGR register */
  RCC->PLLCFGR = 0x24003010;

  /* Reset HSEBYP bit */
  RCC->CR &= (uint32_t)0xFFFBFFFF;

  /* Disable all interrupts */
  RCC->CIR = 0x00000000;

#ifdef DATA_IN_ExtSRAM
  /* Configure the Vector Table location add offset address ------------------*/
#ifdef VECT_TAB_SRAM
  SCB->VTOR = SRAM_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal SRAM */
#else
  SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal FLASH */
#endif  
#endif

  /* Configure the System clock source, PLL Multiplier and Divider factors, 
     AHB/APBx prescalers and Flash settings ----------------------------------*/
  SetSysClk();

  /* Configure the Vector Table location add offset address ------------------*/
#ifdef VECT_TAB_SRAM
  SCB->VTOR = SRAM_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal SRAM */
#else
  SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal FLASH */
#endif  
}

/**
  * @brief  Update SystemCoreClock variable according to Clock Register Values.
  *         The SystemCoreClock variable contains the core clock (HCLK), it can
  *         be used by the user application to setup the SysTick timer or configure
  *         other parameters.
  *           
  * @note   Each time the core clock (HCLK) changes, this function must be called
  *         to update SystemCoreClock variable value. Otherwise, any configuration
  *         based on this variable will be incorrect.
  *           
  * @note   - The system frequency computed by this function is not the real 
  *           frequency in the chip. It is calculated based on the predefined 
  *           constant and the selected clock source:
  *             
  *           - If SYSCLK source is HSI, SystemCoreClock will be equal to HSI_VALUE (*8000000Hz*).
  *             
  *           - If SYSCLK source is HSE, SystemCoreClock will be equal to HSE_VALUE 
  *             (*8000000Hz*), multiplied by the PLL factor.
  *             
  *           - If SYSCLK source is PLL, SystemCoreClock will be equal to 
  *             HSE_VALUE (*8000000Hz*), multiplied by the PLL factor.
  *             
  *         - The result of this function could be not correct when using fractional
  *           value for HSE crystal.
  *           
  *         - When the HSI is used as PLL clock source, the real HSI frequency
  *           may not be exactly 8MHz, so the result might be different.
  *           
  * @param  None
  * @retval None
  */
void SystemCoreClockUpdate (void)
{
  uint32_t tmp = 0, pllmull = 0, pllsource = 0, presc = 0;

  /* Get SYSCLK source -------------------------------------------------------*/
  tmp = RCC->CFGR & RCC_CFGR_SWS;

  switch (tmp)
  {
    case 0x00:  /* HSI used as system clock */
      SystemCoreClock = HSI_VALUE;
      break;
    case 0x04:  /* HSE used as system clock */
      SystemCoreClock = HSE_VALUE;
      break;
    case 0x08:  /* PLL used as system clock */

      /* Get PLL clock source and multiplication factor ----------------------*/
      pllmull = RCC->PLLCFGR & RCC_PLLCFGR_PLLMUL;
      pllsource = RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC;

      pllmull = (pllmull >> 18) + 2;

      if (pllsource == 0x00)
      {
        /* HSI oscillator clock divided by 2 selected as PLL clock entry */
        SystemCoreClock = (HSI_VALUE >> 1) * pllmull;
      }
      else
      {
        /* HSE oscillator clock selected as PLL clock entry */
        SystemCoreClock = (HSE_VALUE) * pllmull;
      }
      break;
    default:
      SystemCoreClock = HSI_VALUE;
      break;
  }

  /* Compute HCLK frequency --------------------------------------------------*/
  /* Get HCLK prescaler */
  tmp = RCC->CFGR & RCC_CFGR_HPRE;
  tmp = tmp >> 4;
  presc = AHBPrescTable[tmp];
  /* HCLK frequency */
  SystemCoreClock >>= presc;
}

/**
  * @brief  Configures the System clock source, PLL Multiplier and Divider factors,
  *         AHB/APBx prescalers and Flash settings
  * @param  None
  * @retval None
  */
static void SetSysClk(void)
{
  __IO uint32_t StartUpCounter = 0, HSEStatus = 0;

  /* SYSCLK, HCLK, PCLK1 and PCLK2 configuration ---------------------------*/    
  /* Enable HSE */
  RCC->CR |= ((uint32_t)RCC_CR_HSEON);

  /* Wait till HSE is ready and if Time out is reached exit */
  do
  {
    HSEStatus = RCC->CR & RCC_CR_HSERDY;
    StartUpCounter++;
  } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

  if ((RCC->CR & RCC_CR_HSERDY) != RESET)
  {
    HSEStatus = (uint32_t)0x01;
  }
  else
  {
    HSEStatus = (uint32_t)0x00;
  }  

  if (HSEStatus == (uint32_t)0x01)
  {
    /* Enable Flash prefetch */
    FLASH->ACR |= FLASH_ACR_PRFTEN;

    /* Enable Prefetch Buffer */
    FLASH->ACR |= FLASH_ACR_LATENCY_4;

    /* HCLK = SYSCLK / 1*/
    RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;

    /* PCLK1 = HCLK / 4*/
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV4;

    /* PCLK2 = HCLK / 2*/
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV2;

    /* Configure PLLs ------------------------------------------------------*/
    /* PLL configuration: PLLCLK = HSE * 9 = 72 MHz */
    RCC->PLLCFGR = (uint32_t)(RCC_PLLCFGR_PLLSRC_HSE | RCC_PLLCFGR_PLLMUL9);

    /* Enable PLL */
    RCC->CR |= RCC_CR_PLLON;

    /* Wait till PLL is ready */
    while((RCC->CR & RCC_CR_PLLRDY) == 0)
    {
    }

    /* Select PLL as system clock source */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;    

    /* Wait till PLL is used as system clock source */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08)
    {
    }
  }
  else
  { /* If HSE fails to start-up, the application will have wrong clock 
         configuration. User can add here some code to deal with this error */
  }
}

/**
  * @brief  Configures the System clock source, PLL Multiplier and Divider factors,
  *         AHB/APBx prescalers and Flash settings
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
    */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
