/*
 * Copyright (c) 2007, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 *         Testing the broadcast layer in Rime
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

#include "contiki.h"
#include "net/rime/rime.h"
#include <stdio.h>
#include "contiki.h"
#include "net/rime/rime.h"
#include "net/linkaddr.h"
#include "dev/watchdog.h"
#include "dev/uart1.h"
#include "dev/leds.h"
#include "lib/list.h"


#include "sdn-wise.h"
#include "flowtable.h"
#include "flowtable.c"
#include "packet-buffer.h"
#include "packet-buffer.c"
#include "packet-handler.h"
#include "packet-handler.c"
#include "packet-creator.h"
#include "packet-creator.c"
#include "neighbor-table.h"
#include "neighbor-table.c"
#include "node-conf.h"
#include "node-conf.c"
#include "address.h"
#include "address.c"
#include "sdn-wise.h"
#include "flowtable.h"
#include "packet-buffer.h"
#include "packet-handler.h"
#include "packet-creator.h"
#include "neighbor-table.h"
#include "node-conf.h"
#include "address.h"
#include "contiki.h"
#include "net/rime/rime.h"
#include "net/linkaddr.h"
#include "dev/watchdog.h"
#include "dev/uart1.h"
#include "dev/leds.h"
#include "lib/list.h"




static uint8_t uart_buffer[81];
static uint8_t uart_buffer_index = 0;
uint8_t u;
uint32_t count1 = 0;
uint32_t count2 = 0;
linkaddr_t addr;
static struct unicast_conn uc;
int count = 0;



void rf_unicast_send(packet_t *p) {
  

    printf("--- rf_unicast_send - Sveglio rf_u_send_proc\n");
    printf("--- rf_unicast_send - Ho svegliato rf_u_send_proc\n");

}
void rf_broadcast_send(packet_t *p) {

    printf("--- rf_broadcast_send - Sveglio rf_b_send_proc\n");
    printf("--- rf_broadcast_send - Ho svegliato rf_b_send_proc\n");

}

/*---------------------------------------------------------------------------*/
PROCESS(example_unicast_process, "Example unicast");
AUTOSTART_PROCESSES(&example_unicast_process);
/*---------------------------------------------------------------------------*/
static void
recv_uc(struct unicast_conn *c, const linkaddr_t *from)
{
  printf("unicast message received from %d.%d\n",
	 from->u8[0], from->u8[1]);
}
/*---------------------------------------------------------------------------*/
static void
sent_uc(struct unicast_conn *c, int status, int num_tx) {
  const linkaddr_t *dest = packetbuf_addr(PACKETBUF_ADDR_RECEIVER);
  if(linkaddr_cmp(dest, &linkaddr_null)) {
    return;
  }
  printf("unicast message sent to %d.%d: status %d num_tx %d\n",
    dest->u8[0], dest->u8[1], status, num_tx);
}
/*---------------------------------------------------------------------------*/

int uart_rx_callback(unsigned char c) {

  uart_buffer_index++;
  uart_buffer[uart_buffer_index] = c;

  if (uart_buffer_index == 80) {

    count1 ++;
    printf("HO RICEVUTO DALLA SERIALE LA FRAME: %d\n",count1);
    uart_buffer_index = 0;

    packet_t *p = create_data_MELO2(uart_buffer,80);


    printf("---------------------------------------------\n");
    //printf("[TX ]: ");
    //print_packet(p);
    //printf("---------------------------------------------\n");
    addr.u8[0] = 2;
    addr.u8[1] = 0;

    uint8_t *a = (uint8_t *)p;
    packetbuf_copyfrom(a, p->header.len);
    unicast_send(&uc, &addr);
    packet_deallocate(p);
    count2 ++;
    printf("HO INVIATO TRAMITE 802.15.4 IL PACCHETTO: %d\n",count2);
    


    // if(!linkaddr_cmp(&addr, &linkaddr_node_addr)) {
    //   packet_t *p = create_data_MELO2(uart_buffer,80);
    //   packetbuf_copyfrom(p, 90);
    //   unicast_send(&uc, &addr);
    //   printf("Paccehtto inviato: %i\n", count++);
    // }

  }
  return 0;


}
/*---------------------------------------------------------------------------*/
static const struct unicast_callbacks unicast_callbacks = {recv_uc, sent_uc};
static struct unicast_conn uc;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_unicast_process, ev, data) {
  
  PROCESS_EXITHANDLER(unicast_close(&uc);)
    
  PROCESS_BEGIN();

  uart1_set_input(uart_rx_callback);


  static int count = 0;
  char messaggio[]= "";
  char *buf;
  size_t sz;

  unicast_open(&uc, 146, &unicast_callbacks);

  printf("\nCliente Local Address(OLD): %d.%d\n",linkaddr_node_addr.u8[0] , linkaddr_node_addr.u8[1]);


  //linkaddr_node_addr.u8[0] = 2;
  //linkaddr_node_addr.u8[1] = 0;

  printf("\nCliente Local Address(NEW): %d.%d\n", linkaddr_node_addr.u8[0], linkaddr_node_addr.u8[1]);

  count = 1;

  while(1) {
    static struct etimer et;
    linkaddr_t addr;
    
    etimer_set(&et, CLOCK_SECOND*4);
    
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    // sz = snprintf(NULL, 0, "Ciao, Pacchetto Numero: 5d", tmp);
    // buf = (char *)malloc(sz + 1);
    // snprintf(buf, sz+1, "select key from answer WHERE key = %s LIMIT 5;", tmp);

    // sz = snprintf(NULL, 0, "Client manda il pacchetto numero: %i", count);
    // buf = (char *)malloc(sz + 1);
    // snprintf(buf, sz+1, "Client manda il pacchetto numero: %i", count);

    // packetbuf_copyfrom(buf, strlen(buf));
    // addr.u8[0] = 2;
    // addr.u8[1] = 0;
    // if(!linkaddr_cmp(&addr, &linkaddr_node_addr)) {
    //   unicast_send(&uc, &addr);
    // }
    //printf("SEND: --> Ciao, Pacchetto Numero: %i", count);
    count ++;
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
