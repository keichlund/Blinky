// Console IO is a wrapper between the actual in and output and the console code
// In an embedded system, this might interface to a UART driver.

#include "consoleIo.h"
#include <stdio.h>
#include "stdint.h"
#include "stm32f4xx_hal.h"
#include "main.h"
#include <string.h>


eConsoleError ConsoleIoInit(void)
{
	return CONSOLE_SUCCESS;
}

eConsoleError ConsoleIoReceive(uint8_t *buffer, const uint32_t bufferLength, uint32_t *readLength)
{
	uint32_t i = 0;
	uint8_t in = 0;
  
	if(HAL_UART_Receive(GetUART1Handle(), &in, 1, 20) == HAL_OK)
  {
      buffer[i] = in;
      i++;
  }
	*readLength = i;
	return CONSOLE_SUCCESS;
}

eConsoleError ConsoleIoSendString(const char *buffer)
{
	HAL_UART_Transmit(GetUART1Handle(), (uint8_t *)buffer, strlen(buffer), 250);
	return CONSOLE_SUCCESS;
}

