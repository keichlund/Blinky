// File: hx711.h.c

#include "hx711.h"
#include "consoleIo.h"
#include "console.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_def.h"
#include "stdint.h"
#include "main.h"

/* Private Defines */
#define CALIBRATION_FACTOR    -85

/*Private Variables*/
uint8_t buf[28];
uint8_t i = 0;

/* Private function prototypes*/
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
  static uint32_t sampleTick = 0;
  static uint32_t zero = 0;
  static uint8_t avgCnt = 0;
  uint8_t data[3];
  uint8_t filler = 0x00;
  uint32_t value = 0;
  int32_t correctedValue = 0;
  
  
  if( HAL_GetTick() - sampleTick >= 5)
  {
    if(HAL_GPIO_ReadPin(hx711_Dat_GPIO_Port, hx711_Dat_Pin) == GPIO_PIN_RESET)
    {
      for( uint8_t j = 0; j < 3; j++)
      {
        for( uint8_t i = 0; i < 8; i++)
        {
          HAL_GPIO_WritePin(hx711_SCK_GPIO_Port, hx711_SCK_Pin, GPIO_PIN_SET);
          DWT_Delay_us(1);
          data[j] |= HAL_GPIO_ReadPin(hx711_Dat_GPIO_Port, hx711_Dat_Pin) << (7-i);
          HAL_GPIO_WritePin(hx711_SCK_GPIO_Port, hx711_SCK_Pin, GPIO_PIN_RESET);
          DWT_Delay_us(1);
        }
      }
      
      HAL_GPIO_WritePin(hx711_SCK_GPIO_Port, hx711_SCK_Pin, GPIO_PIN_SET);
      DWT_Delay_us(1);
      HAL_GPIO_WritePin(hx711_SCK_GPIO_Port, hx711_SCK_Pin, GPIO_PIN_RESET);
      DWT_Delay_us(1);
      
      ConsoleSendParamInt16(data[0]);
      ConsoleIoSendString(", ");
      ConsoleSendParamInt16(data[1]);
      ConsoleIoSendString(", ");
      ConsoleSendParamInt16(data[2]);
      ConsoleIoSendString(", ");
      
      value = (uint32_t)data[0] << 16 | (uint32_t)data[1] << 8| (uint32_t)data[2] << 0;
      ConsoleSendParamUInt32(value);
      ConsoleSendString(",");
      correctedValue = value - zero;
      ConsoleSendParamInt32(correctedValue);
      ConsoleSendString(",");
      ConsoleSendParamInt32(correctedValue/CALIBRATION_FACTOR);
      ConsoleIoSendString("\r\n");
      
      sampleTick = HAL_GetTick();
      
      if(avgCnt <20)
      {
        zero += value/20;
        avgCnt++;
      }
    }
    HAL_GPIO_WritePin(hx711_SCK_GPIO_Port, hx711_SCK_Pin, GPIO_PIN_RESET);
  }
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