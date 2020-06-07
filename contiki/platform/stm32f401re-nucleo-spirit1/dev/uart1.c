/**
 * \addtogroup stm32w-cpu
 *
 * @{
 */

/*
 * Copyright (c) 2010, STMicroelectronics.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 */

/**
* \file
*					Machine dependent STM32W UART1 code.
* \author
*					Salvatore Pitrulli
* \version
*					0.1
* \since
*					03.04.2010
*/

#include <stdio.h>
#include <stdlib.h>
#include "sys/energest.h"
#include "dev/uart1.h"
#include "dev/watchdog.h"
#include "lib/ringbuf.h"
#include "dev/leds.h"
#include "cpu/arm/stm32f401/regs.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_uart.h"

#define RXBUFFERSIZE 100
#define RXPIN GPIO_PIN_7
#define TXPIN GPIO_PIN_6
#define DATAPORT GPIOB
#define BAUDRATE 115200
#define UART_PRIORITY 6
#define UART_RX_SUBPRIORITY   0

uint8_t aRxBuffer[RXBUFFERSIZE];

UART_HandleTypeDef _huart;

static int (*uart1_input_handler) (unsigned char c);

void uart1_rx_interrupt(void);

void uart1_tx_interrupt(void);

static volatile uint8_t transmitting;

#ifdef UART1_CONF_TX_WITH_INTERRUPT
#define TX_WITH_INTERRUPT UART1_CONF_TX_WITH_INTERRUPT
#else /* UART1_CONF_TX_WITH_INTERRUPT */
#define TX_WITH_INTERRUPT     1
#endif /* UART1_CONF_TX_WITH_INTERRUPT */


#if TX_WITH_INTERRUPT
#ifdef UART1_CONF_TX_BUFSIZE
#define UART1_TX_BUFSIZE UART1_CONF_TX_BUFSIZE
#else /* UART1_CONF_TX_BUFSIZE */
#define UART1_TX_BUFSIZE 64
#endif /* UART1_CONF_TX_BUFSIZE */
static struct ringbuf txbuf;
static uint8_t txbuf_data[UART1_TX_BUFSIZE];
#endif /* TX_WITH_INTERRUPT */
/*---------------------------------------------------------------------------*/
void
uart1_writeb(unsigned char c)
{
  watchdog_periodic();
#if TX_WITH_INTERRUPT
  /*
   * Put the outgoing byte on the transmission buffer. If the buffer
   * is full, we just keep on trying to put the byte into the buffer
   * until it is possible to put it there.
   */
  while(ringbuf_put(&txbuf, c) == 0);

  /*
   * If there is no transmission going, we need to start it by putting
   * the first byte into the UART.
   */
  if(transmitting == 0) {
    transmitting = 1;
    SC1_DATA = ringbuf_get(&txbuf);
    INT_SC1FLAG = INT_SCTXFREE;
    INT_SC1CFG |= INT_SCTXFREE;
  }
#else /* TX_WITH_INTERRUPT */

  /* Loop until the transmission buffer is available. */
  while((INT_SC1FLAG & INT_SCTXFREE) == 0);

  /* Transmit the data. */
  SC1_DATA = c;

  INT_SC1FLAG = INT_SCTXFREE;
#endif /* TX_WITH_INTERRUPT */
}
/*---------------------------------------------------------------------------*/
#if ! NETSTACK_CONF_WITH_IPV4
/* If NETSTACK_CONF_WITH_IPV4 is defined, putchar() is defined by the SLIP driver */
#endif /* ! NETSTACK_CONF_WITH_IPV4 */
/*---------------------------------------------------------------------------*/
/*
 * Initalize the RS232 port.
 */
void
uart1_init(unsigned long ubr)
{
   _huart.Instance = USART2;
    _huart.Init.BaudRate = BAUDRATE;
    _huart.Init.WordLength = UART_WORDLENGTH_8B;
    _huart.Init.StopBits = UART_STOPBITS_1;
    _huart.Init.Parity = UART_PARITY_NONE;
    _huart.Init.Mode = UART_MODE_TX_RX;
    _huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    _huart.Init.OverSampling = UART_OVERSAMPLING_16;
    if(HAL_UART_Init(&_huart) != HAL_OK)
    {
      printf("Error!");
    }

    if(HAL_UART_Receive_IT(&_huart, (uint8_t *)aRxBuffer, RXBUFFERSIZE) != HAL_OK)
    {
      printf("Error!");
    }
    __HAL_UART_ENABLE_IT(&_huart, UART_IT_RXNE);
    HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);  
}
/*---------------------------------------------------------------------------*/
void
halSc1Isr(void)
{
  ENERGEST_ON(ENERGEST_TYPE_IRQ);

  if(INT_SC1FLAG & INT_SCRXVAL) {
    uart1_rx_interrupt();
    INT_SC1FLAG = INT_SCRXVAL;
  }
#if TX_WITH_INTERRUPT
  else if(INT_SC1FLAG & INT_SCTXFREE) {
    uart1_tx_interrupt();
    INT_SC1FLAG = INT_SCTXFREE;
  }
#endif /* TX_WITH_INTERRUPT */

  ENERGEST_OFF(ENERGEST_TYPE_IRQ);
}
/*--------------------------------------------------------------------------*/
void
uart1_rx_interrupt(void)
{
  uint8_t c;

  c = SC1_DATA;
  if(uart1_input_handler != NULL) {
    uart1_input_handler(c);
  }
}
/*---------------------------------------------------------------------------*/
#if TX_WITH_INTERRUPT
void
uart1_tx_interrupt(void)
{
  if(ringbuf_elements(&txbuf) == 0) {
    transmitting = 0;
    INT_SC1CFG &= ~INT_SCTXFREE;
  } else {
    SC1_DATA = ringbuf_get(&txbuf);
  }
}
#endif /* TX_WITH_INTERRUPT */
/** @} */
