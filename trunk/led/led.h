/*
 * led.h
 *
 *  Created on: 2010-04-10
 *      Author: Marcel
 */

#ifndef LED_H_
#define LED_H_

#include "FreeRTOS.h"

void vLEDInitialise(void);
void vSetLED(unsigned portBASE_TYPE uxLED, signed portBASE_TYPE xValue);
void vToggleLED(unsigned portBASE_TYPE uxLED);

#endif /* LED_H_ */
