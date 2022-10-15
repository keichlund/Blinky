// UI.h

#ifndef UI_h
#define UI_h

#include <stdint.h>
/* Exported Functions */

/* Run function for the user interface module */
void UI_Run(void);
void UI_SetBagFullVolume(uint32_t newVolume);
uint32_t UI_GetBagFullVolume(void);
#endif