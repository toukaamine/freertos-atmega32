/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <joerg@FreeBSD.ORG> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.        Joerg Wunsch
 * ----------------------------------------------------------------------------
 *
 * HD44780 LCD display driver
 *
 * The LCD controller is used in 4-bit mode with a full bi-directional
 * interface (i.e. R/~W is connected) so the busy flag can be read.
 *
 * $Id: hd44780.c,v 1.3 2006/10/08 21:47:36 joerg_wunsch Exp $
 */

#include "lcddefines.h"
#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include "hd44780.h"

#define GLUE(a, b)     a##b
#define PORT(x)        GLUE(PORT, x)
#define DDR(x)         GLUE(DDR, x)

#define HD44780_PORTOUT    PORT(HD44780_PORT)
#define HD44780_DDR        DDR(HD44780_PORT)

#define HD44780_DATABITS \
(_BV(HD44780_D4)|_BV(HD44780_D5)|_BV(HD44780_D6)|_BV(HD44780_D7))

/*
 * Send one pulse to the E signal (enable).  Mind the timing
 * constraints.  If readback is set to true, read the HD44780 data
 * pins right before the falling edge of E, and return that value.
 */
static inline void
hd44780_pulse_e(void) __attribute__((always_inline));

static inline void hd44780_pulse_e(void)
{
  HD44780_PORTOUT |= _BV(HD44780_E);
  /*
   * Guarantee at least 500 ns of pulse width.  For high CPU
   * frequencies, a delay loop is used.  For lower frequencies, NOPs
   * are used, and at or below 1 MHz, the native pulse width will
   * already be 1 us or more so no additional delays are needed.
   */
#if F_CPU > 4000000UL
  _delay_us(0.5);
#endif
  HD44780_PORTOUT &= ~_BV(HD44780_E);
}

/*
 * Send one nibble out to the LCD controller.
 */
static void hd44780_outnibble(uint8_t n, uint8_t rs)
{
  volatile uint8_t x;

  if (rs)
    HD44780_PORTOUT |= _BV(HD44780_RS);
  else
    HD44780_PORTOUT &= ~_BV(HD44780_RS);

  x = (HD44780_PORTOUT & ~HD44780_DATABITS) | ((n << HD44780_D4) & HD44780_DATABITS);
  HD44780_PORTOUT = x;
  hd44780_pulse_e();
}

/*
 * Send one byte to the LCD controller.  As we are in 4-bit mode, we
 * have to send two nibbles.
 */
void hd44780_outbyte(uint8_t b, uint8_t rs)
{
  hd44780_outnibble(b >> 4, rs);
  hd44780_outnibble(b & 0xf, rs);
}

/*
 * Initialize the LCD controller.
 *
 * The initialization sequence has a mandatory timing so the
 * controller can safely recognize the type of interface desired.
 * This is the only area where timed waits are really needed as
 * the busy flag cannot be probed initially.
 */
void hd44780_init(void)
{
  HD44780_DDR = _BV(HD44780_RS) | _BV(HD44780_E) | HD44780_DATABITS | _BV(HD44780_BL);

  HD44780_PORTOUT |= _BV(HD44780_BL);

  _delay_ms(40); /* 40 ms needed for Vcc = 2.7 V */
  hd44780_outnibble(HD44780_FNSET(1, 0, 0) >> 4, 0);
  _delay_ms(4.1);
  hd44780_outnibble(HD44780_FNSET(1, 0, 0) >> 4, 0);
  _delay_ms(0.1);
  hd44780_outnibble(HD44780_FNSET(1, 0, 0) >> 4, 0);
  _delay_ms(0.1);

  hd44780_outnibble(HD44780_FNSET(0, 0, 0) >> 4, 0);
  _delay_us(40);
  hd44780_outcmd(HD44780_DISPCTL(0, 0, 0));
  _delay_us(40);

}
