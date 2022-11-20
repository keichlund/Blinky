// File: hx711.h.c

#include "hx711.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_def.h"
#include "stdint.h"
#include "main.h"

/* Private Defines */

uint8_t buf[28];
uint8_t i = 0;

__STATIC_INLINE void DWT_Delay_us(volatile uint32_t microseconds);
uint32_t DWT_Delay_Init(void);


/* Exported Functions */
void hx711_Init(void)
{
  HAL_GPIO_WritePin(hx711_SCK_GPIO_Port, hx711_SCK_Pin, GPIO_PIN_RESET);
  DWT_Delay_Init();
}

void hx711_Run(void)
{
  if(HAL_GPIO_ReadPin(hx711_Dat_GPIO_Port, hx711_Dat_Pin) == GPIO_PIN_RESET)
  {
    for( uint8_t i = 0; i < 26; i++)
    {
      HAL_GPIO_WritePin(hx711_SCK_GPIO_Port, hx711_SCK_Pin, GPIO_PIN_SET);
      DWT_Delay_us(1);
      buf[i] = HAL_GPIO_ReadPin(hx711_Dat_GPIO_Port, hx711_Dat_Pin);
      HAL_GPIO_WritePin(hx711_SCK_GPIO_Port, hx711_SCK_Pin, GPIO_PIN_RESET);
      DWT_Delay_us(1);
    }
    i = 0;
  }
  HAL_GPIO_WritePin(hx711_SCK_GPIO_Port, hx711_SCK_Pin, GPIO_PIN_RESET);
}


uint32_t DWT_Delay_Init(void);
 
 
 
/**
 * @brief  This function provides a delay (in microseconds)
 * @param  microseconds: delay in microseconds
 */
__STATIC_INLINE void DWT_Delay_us(volatile uint32_t microseconds)
{
  uint32_t clk_cycle_start = DWT->CYCCNT;
 
  /* Go to number of cycles for system */
  microseconds *= (HAL_RCC_GetHCLKFreq() / 1000000);
 
  /* Delay till end */
  while ((DWT->CYCCNT - clk_cycle_start) < microseconds);
}

uint32_t DWT_Delay_Init(void)
{
    /* Disable TRC */
    CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk; // ~0x01000000;
    /* Enable TRC */
    CoreDebug->DEMCR |=  CoreDebug_DEMCR_TRCENA_Msk; // 0x01000000;
 
    /* Disable clock cycle counter */
    DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk; //~0x00000001;
    /* Enable  clock cycle counter */
    DWT->CTRL |=  DWT_CTRL_CYCCNTENA_Msk; //0x00000001;
 
    /* Reset the clock cycle counter value */
    DWT->CYCCNT = 0;
 
    /* 3 NO OPERATION instructions */
    __ASM volatile ("NOP");
    __ASM volatile ("NOP");
    __ASM volatile ("NOP");
 
    /* Check if clock cycle counter has started */
    if(DWT->CYCCNT)
    {
       return 0; /*clock cycle counter started*/
    }
    else
    {
      return 1; /*clock cycle counter not started*/
    }
}