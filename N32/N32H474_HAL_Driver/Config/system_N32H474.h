/**
  ******************************************************************************
  * @file    system_N32H474.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    15-December-2023
  * @brief   CMSIS Cortex-M4 Device Peripheral Access Layer System Header File.
  *
  *          This file contains the device registers structure definitions, 
  *          bits definitions, memory mapping, etc.
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

#ifndef __SYSTEM_N32H474_H
#define __SYSTEM_N32H474_H

#ifdef __cplusplus
 extern "C" {
#endif 

/** @addtogroup N32H474_System_Includes
  * @{
  */

/**
  * @}
  */


/** @addtogroup N32H474_System_Frequency
  * @{
  */
#define HSI_VALUE    ((uint32_t)8000000U) /*!< Value of the Internal oscillator in Hz */
#define HSE_VALUE    ((uint32_t)8000000U) /*!< Value of the External oscillator in Hz */
#define LSI_VALUE    ((uint32_t)40000U)  /*!< Value of the Internal Low Speed oscillator in Hz */
#define LSE_VALUE    ((uint32_t)32768U)  /*!< Value of the External Low Speed oscillator in Hz */

#define HSI48_VALUE  ((uint32_t)48000000U) /*!< Value of the Internal High Speed oscillator for USB in Hz */

/**
  * @}
  */

/** @addtogroup N32H474_System_Exported_Variables
  * @{
  */
extern uint32_t SystemCoreClock;          /*!< System Clock Frequency (Core Clock) */
extern const uint8_t  AHBPrescTable[16];  /*!< AHB prescalers table values */
extern const uint8_t  APBPrescTable[8];   /*!< APB prescalers table values */

/**
  * @}
  */

/** @addtogroup N32H474_System_Exported_Constants
  * @{
  */

/* Uncomment the line corresponding to the desired System clock (SYSCLK)
   frequency (assuming the HSE oscillator is used as System clock source) */
#define SYSCLK_FREQ_48MHz   48000000U  /*!< SYSCLK frequency: 48MHz */

/* Uncomment the following line if you need to use data in the D2 SRAM (AHB SRAM) for code execution */
/* #define DATA_IN_D2_SRAM */

/**
  * @}
  */

/** @addtogroup N32H474_System_Exported_FunctionsPrototypes
  * @{
  */

extern void SystemInit(void);
extern void SystemCoreClockUpdate(void);
extern void SetSysClock(void);

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /*__SYSTEM_N32H474_H */

/**
  * @}
  */

/**
  * @}
  */
