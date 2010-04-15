/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <joerg@FreeBSD.ORG> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.        Joerg Wunsch
 * ----------------------------------------------------------------------------
 *
 * General stdiodemo defines
 *
 * $Id: defines.h,v 1.2 2006/10/08 21:47:36 joerg_wunsch Exp $
 */

/* HD44780 LCD port connections */
#ifndef HD44780_PORT
#define HD44780_PORT C
#endif

#ifndef HD44780_FIRST_PIN
#define HD44780_FIRST_PIN PORT1
#endif

#define HD44780_BL  HD44780_FIRST_PIN
#define HD44780_RS  (HD44780_FIRST_PIN + 1)
#define HD44780_E   (HD44780_FIRST_PIN + 2)
/* The data bits have to be in ascending order. */
#define HD44780_D4  (HD44780_FIRST_PIN + 3)
#define HD44780_D5  (HD44780_FIRST_PIN + 4)
#define HD44780_D6  (HD44780_FIRST_PIN + 5)
#define HD44780_D7  (HD44780_FIRST_PIN + 6)







