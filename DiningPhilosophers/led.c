/*
 * led.c
 *
 *  Created on: 2010-04-10
 *      Author: Marcel
 */

#include <stdlib.h>
#include <string.h>

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
/*-----------------------------------------------------------*/
#define LED_PORT    PORTB
#define LED_DDR     DDRB
#define LED_ALL_BITS_OUTPUT     ( ( unsigned char ) 0xff )
#define LED_ALL_OUTPUTS_OFF     ( ( unsigned char ) 0xff )
#define LED_MAX_OUTPUT_LED      ( ( unsigned char ) 7 )

static volatile unsigned char ucCurrentOutputValue = LED_ALL_OUTPUTS_OFF;
/*-----------------------------------------------------------*/
void vLEDInitialise(void)
{
  ucCurrentOutputValue = LED_ALL_OUTPUTS_OFF;
  DDRB = LED_ALL_BITS_OUTPUT;
  PORTB = ucCurrentOutputValue;
}
/*-----------------------------------------------------------*/
void vSetLED(unsigned portBASE_TYPE uxLED, signed portBASE_TYPE xValue)
{
  unsigned char ucBit = (unsigned char) 1;
  if (uxLED <= LED_MAX_OUTPUT_LED) {
    ucBit <<= uxLED;
    vTaskSuspendAll();
    {
      if (xValue == pdTRUE) {
        ucBit ^= (unsigned char) 0xff;
        ucCurrentOutputValue &= ucBit;
      } else {
        ucCurrentOutputValue |= ucBit;
      }
      PORTB = ucCurrentOutputValue;
    }
    xTaskResumeAll();
  }
}
/*-----------------------------------------------------------*/
void vToggleLED(unsigned portBASE_TYPE uxLED)
{
  unsigned char ucBit;

  if (uxLED <= LED_MAX_OUTPUT_LED) {
    ucBit = ((unsigned char) 1) << uxLED;

    vTaskSuspendAll();
    {
      if (ucCurrentOutputValue & ucBit) {
        ucCurrentOutputValue &= ~ucBit;
      } else {
        ucCurrentOutputValue |= ucBit;
      }

      PORTB = ucCurrentOutputValue;
    }
    xTaskResumeAll();
  }
}
