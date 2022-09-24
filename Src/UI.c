// File: UI.c

/* Includes ------------------------------------------------------------------*/
#include "UI.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_def.h"
#include "hardware.h"

/* Private define ------------------------------------------------------------*/

#define LED_BLINK_DELAY_MS                500u
#define DEBOUNCE_DELAY_MS                  30u
#define BUTTON_STATE_PRESS        GPIO_PIN_SET
#define BUTTON_STATE_RELEASE    GPIO_PIN_RESET

/* Private variables ---------------------------------------------------------*/

GPIO_PinState ButtonStateRaw = BUTTON_STATE_RELEASE;
GPIO_PinState ButtonStateDebounced = BUTTON_STATE_RELEASE;

/* Private function prototypes -----------------------------------------------*/

void DebounceButton(void);

/* Private functions --------- -----------------------------------------------*/

void DebounceButton(void)
{
  static uint32_t stateChangeTick = 0;
  uint32_t nowTick = HAL_GetTick();

  if( ButtonStateDebounced == ButtonStateRaw)
  {
    stateChangeTick = 0;
  }
  else
  {
    if( stateChangeTick == 0)
    {
      // Record the time of the press/release event
      stateChangeTick = nowTick;
    }
    
    if( (nowTick - stateChangeTick) >= DEBOUNCE_DELAY_MS)
    {
      ButtonStateDebounced = ButtonStateRaw;
      HAL_GPIO_TogglePin(LD6_GPIO_Port, LD6_Pin);
    }
  }
}

/* Exported Functions --------------------------------------------------------*/
void UI_Init(void)
{
  HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_RESET);
}

void UI_Run(void)
{
  static uint32_t toggleTick = 0;
  uint32_t now;
  
  DebounceButton();
  
  if( ButtonStateDebounced == BUTTON_STATE_RELEASE )
  {
    now = HAL_GetTick();
    
    if( now - toggleTick >= LED_BLINK_DELAY_MS)
    {
      HAL_GPIO_TogglePin(LD4_GPIO_Port, LD4_Pin);
      HAL_GPIO_TogglePin(LD5_GPIO_Port, LD5_Pin);
      
      toggleTick = now;
    }
  }
 else
 {
   // If we aren't blinking turn LEDs off
   HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
   HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_RESET);
 }
}

/**
  * @brief  EXTI line detection callbacks.
  * @param  GPIO_Pin Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  ButtonStateRaw = HAL_GPIO_ReadPin(GPIOA, OTG_FS_PowerSwitchOn_Pin);
}