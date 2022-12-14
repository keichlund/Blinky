// hx711.h

#ifndef HX711_H
#define HX711_H

#include <stdint.h>
/* Exported Functions */

/* Run function for the hx711 module */
void hx711_Init(void);
void hx711_Run(void);
void hx711_Tare(void);
int32_t hx711_GetWeight(void);
#endif