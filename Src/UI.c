// File: UI.c

#include "UI.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_def.h"
#include "hardware.h"

/* Private Defines */

#define LED_BLINK_DELAY_MS      500

/* Exported Functions */

void UI_Run(void)
{
  static uint32_t toggleTick = 0;
  uint32_t now;
  
 if(HAL_GPIO_ReadPin(GPIOA, OTG_FS_PowerSwitchOn_Pin) == GPIO_PIN_RESET)
 {
   now = HAL_GetTick();
   
   if( now - toggleTick >= LED_BLINK_DELAY_MS)
   {
     HAL_GPIO_TogglePin(GPIOD, LD4_Pin);
     toggleTick = now;
   }
 }
 else
 {
   HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
 }
}