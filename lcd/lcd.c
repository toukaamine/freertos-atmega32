/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <joerg@FreeBSD.ORG> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.        Joerg Wunsch
 * ----------------------------------------------------------------------------
 *
 * Stdio demo, upper layer of LCD driver.
 *
 * $Id: lcd.c,v 1.1 2005/12/28 21:38:59 joerg_wunsch Exp $
 */

#include "FreeRTOS.h"
#include "lcddefines.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>

#include "semphr.h"
#include "task.h"

#include "hd44780.h"
#include "lcd.h"

FILE lcd_str = FDEV_SETUP_STREAM(xPutCharLCD, NULL, _FDEV_SETUP_WRITE);

xSemaphoreHandle xMutex;

/*
 * Setup the LCD controller.  First, call the hardware initialization
 * function, then adjust the display attributes we want.
 */
void vInitLCD(void)
{

  xMutex = xSemaphoreCreateRecursiveMutex();

  hd44780_init();

  /*
   * Function set: 4bit interface, 2lines, 5x10 font
   */
  hd44780_outcmd(HD44780_FNSET(0, 1, 1));
  _delay_us(40);
  /*
   * Clear the display.
   */
  hd44780_outcmd(HD44780_CLR);
  _delay_ms(1.6);

  /*
   * Entry mode: auto-increment address counter, no display shift in
   * effect.
   */
  hd44780_outcmd(HD44780_ENTMODE(1, 0));
  _delay_us(40);

  /*
   * Enable display, disable cursor
   */
  hd44780_outcmd(HD44780_DISPCTL(1, 0, 0));
  _delay_us(40);

  stderr = &lcd_str;
}

/*
 * Send character c to the LCD display.  After a '\n' has been seen,
 * the next character will first clear the display.
 */
portSHORT xPutCharLCD(portCHAR c, FILE *unused)
{
  static bool nl_seen;
  static uint8_t line = 0;

  if (xTakeLCD(portMAX_DELAY) == pdFALSE)
    return -1;

  vTaskSuspendAll();
  if (nl_seen && c != '\n') {
    /*
     * First character after newline, clear display and home cursor.
     */
    if (line > 1) {
      hd44780_outcmd(HD44780_CLR);
      _delay_ms(1.6);
      hd44780_outcmd(HD44780_HOME);
      _delay_ms(1.6);
      hd44780_outcmd(HD44780_DDADDR(0));
      _delay_us(40);
      line = 0;
    } else {
      hd44780_outcmd(HD44780_DDADDR(0x40));
      _delay_us(40);
    }
    nl_seen = false;
  }

  if (c == '\n') {
    nl_seen = true;
    line++;
  } else {
    hd44780_outdata(c);
    _delay_us(40);
  }
  xTaskResumeAll();

  xGiveLCD();

  return 0;
}

portBASE_TYPE xTakeLCD( portTickType xBlockTime )
{
  return xSemaphoreTakeRecursive(xMutex, xBlockTime);
}

portBASE_TYPE xGiveLCD( void )
{
  return xSemaphoreGiveRecursive(xMutex);
}
