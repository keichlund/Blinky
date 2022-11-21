// File: hx711.h.c

#include "hx711.h"
#include "consoleIo.h"
#include "console.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_def.h"
#include "stdint.h"
#include "stdbool.h"
#include "main.h"

/* Private Defines ---------------------------------------------------------*/
#define CALIBRATION_FACTOR                                  -85
#define SAMPLE_RATE_HZ                                     100u
#define AVERAGE_PERIOD_MS                                  1000u
#define SAMPLE_PERIOD_MS                (1000u / SAMPLE_RATE_HZ)
#define SAMPLES_IN_AVERAGE  (AVERAGE_PERIOD_MS / SAMPLE_RATE_HZ)
#define HX711_GAIN_128_A                                      1   //Channel A, Gain 128
#define HX711_GAIN_32_B                                       2   //Channel B, Gain 32
#define HX711_GAIN_64_A                                       3   //Channel C, Gain 64

/* Private variables ---------------------------------------------------------*/
static uint32_t TareValue = 0u;
static uint16_t AvgSampleArray[SAMPLES_IN_AVERAGE];
static uint32_t AvgAccumulator = 0;
static uint16_t AvgIndex = 0;

/* Private function prototypes -----------------------------------------------*/
__STATIC_INLINE void DWT_Delay_us(volatile uint32_t microseconds);
uint32_t DWT_Delay_Init(void);

/* Private functions ---------------------------------------------------------*/

// This function does not check if the sensor is ready to read
uint32_t ReadRaw(uint8_t gain)
{
  uint8_t data[3];
  
  // 3 bytes of data
  for( uint8_t j = 0; j < 3; j++)
  {
    // send 8 clock pulses/reads
    for( uint8_t i = 0; i < 8; i++)
    {
      HAL_GPIO_WritePin(hx711_SCK_GPIO_Port, hx711_SCK_Pin, GPIO_PIN_SET);
      DWT_Delay_us(1);
      data[j] |= HAL_GPIO_ReadPin(hx711_Dat_GPIO_Port, hx711_Dat_Pin) << (7-i);
      HAL_GPIO_WritePin(hx711_SCK_GPIO_Port, hx711_SCK_Pin, GPIO_PIN_RESET);
      DWT_Delay_us(1);
    }
  }
  
  for(uint8_t i = 0; i < gain; i++)
  {
    //Extra clock cycles after the read sets the channel and gain for the next read
    HAL_GPIO_WritePin(hx711_SCK_GPIO_Port, hx711_SCK_Pin, GPIO_PIN_SET);
    DWT_Delay_us(1);
    HAL_GPIO_WritePin(hx711_SCK_GPIO_Port, hx711_SCK_Pin, GPIO_PIN_RESET);
    DWT_Delay_us(1);
  }
  
  return((uint32_t)data[0] << 16 | (uint32_t)data[1] << 8| (uint32_t)data[2] << 0);
}

//New data is ready when the data line is low
bool IsDataReady(void)
{
  return(HAL_GPIO_ReadPin(hx711_Dat_GPIO_Port, hx711_Dat_Pin) == GPIO_PIN_RESET);
}

/* Exported Functions --------------------------------------------------------*/
void hx711_Init(void)
{
  HAL_GPIO_WritePin(hx711_SCK_GPIO_Port, hx711_SCK_Pin, GPIO_PIN_RESET);
  DWT_Delay_Init();
}

void hx711_Run(void)
{
  static uint32_t sampleTick = 0;
  
  if( HAL_GetTick() - sampleTick >= SAMPLE_RATE_HZ)
  {      
    if(IsDataReady())
    {
      AvgAccumulator -= AvgSampleArray[AvgIndex];
      AvgSampleArray[AvgIndex] = ReadRaw(HX711_GAIN_128_A);
      AvgAccumulator += AvgSampleArray[AvgIndex];
      AvgIndex++;
      
      if(AvgIndex >= SAMPLES_IN_AVERAGE)
      {
        AvgIndex = 0;
        ConsoleSendParamInt32(hx711_GetWeight());
        ConsoleIoSendString("\r\n");  
      }
      
      sampleTick = HAL_GetTick();
    }
  }
}


//      ConsoleSendParamInt16(data[0]);
//      ConsoleIoSendString(", ");
//      ConsoleSendParamInt16(data[1]);
//      ConsoleIoSendString(", ");
//      ConsoleSendParamInt16(data[2]);
//      ConsoleIoSendString(", ");
//      
//      value = (uint32_t)data[0] << 16 | (uint32_t)data[1] << 8| (uint32_t)data[2] << 0;
//      ConsoleSendParamUInt32(value);
//      ConsoleSendString(",");
//      correctedValue = value - zero;
//      ConsoleSendParamInt32(correctedValue);
//      ConsoleSendString(",");
//      ConsoleSendParamInt32(correctedValue/CALIBRATION_FACTOR);
//      ConsoleIoSendString("\r\n");  

void hx711_Tare(void)
{
  TareValue = AvgAccumulator / SAMPLES_IN_AVERAGE;
}

int32_t hx711_GetWeight(void)
{
  int32_t val = (AvgAccumulator / SAMPLES_IN_AVERAGE);
  val = val - TareValue;
  val = (int32_t)val / CALIBRATION_FACTOR;
  return( val);
}


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
