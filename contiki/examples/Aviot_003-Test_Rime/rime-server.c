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

uint32_t count = 0;


static uint8_t get_packet_rssi(uint16_t raw_value) {
  // TODO the exact rssi value depends on the radio
  // http://sourceforge.net/p/contiki/mailman/message/31805752/
  #if COOJA
    return (uint8_t)-raw_value;
  #else
    return (uint8_t)raw_value;
  #endif
}

/*----------------------------------------------------------------------------*/
static void unicast_rx_callback(struct unicast_conn *c, const linkaddr_t *from) {
  packet_t *p = get_packet_from_array((uint8_t *)packetbuf_dataptr());

  count ++;

  printf("--- Ho ricevuto da %d.%d, il pacchetto numero: %d\n",from->u8[0], from->u8[1], count);

  if (p != NULL) {
    p->info.rssi = get_packet_rssi(packetbuf_attr(PACKETBUF_ATTR_RSSI));
    
    //printf("[RX ]: ");
    //print_packet(p);
    //printf("\n");
    printf("---------------------------------------------\n");
    printf("[RX ]: ");
    print_packet(p);
    printf("---------------------------------------------\n");
    packet_deallocate(p);

  }
   


}
/*----------------------------------------------------------------------------*/

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
recv_uc(struct unicast_conn *c, const linkaddr_t *from) {
  /*
  packet_t *p = get_packet_from_array((uint8_t *)packetbuf_dataptr());

  //if (p != NULL) {
    //p->info.rssi = get_packet_rssi(packetbuf_attr(PACKETBUF_ATTR_RSSI));
  //}

  count ++;
  printf("unicast message received from %d.%d numero: %d  Frame: %d Packet: %s\n",
            from->u8[0], from->u8[1],count, (count*20), (uint8_t *) packetbuf_dataptr());

    printf("[RX]: ");
    print_packet(p);
    printf("\n");

    packet_deallocate(p);
  */

}
/*---------------------------------------------------------------------------*/
static void
sent_uc(struct unicast_conn *c, int status, int num_tx)
{
  const linkaddr_t *dest = packetbuf_addr(PACKETBUF_ADDR_RECEIVER);
  if(linkaddr_cmp(dest, &linkaddr_null)) {
    return;
  }
  printf("unicast message sent to %d.%d: status %d num_tx %d\n",
    dest->u8[0], dest->u8[1], status, num_tx);
}
/*---------------------------------------------------------------------------*/
static const struct unicast_callbacks unicast_callbacks = {unicast_rx_callback, sent_uc};
static struct unicast_conn uc;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_unicast_process, ev, data)
{
  PROCESS_EXITHANDLER(unicast_close(&uc);)
    
  PROCESS_BEGIN();

  static int count = 0;
  char messaggio[]= "";

  unicast_open(&uc, 146, &unicast_callbacks);

  printf("\nServer Local Address(OLD): %d.%d\n",linkaddr_node_addr.u8[0] , linkaddr_node_addr.u8[1]);

  //linkaddr_node_addr.u8[0] = 5;
  //linkaddr_node_addr.u8[1] = 0;

  printf("\nServer Local Address(NEW): %d.%d\n", linkaddr_node_addr.u8[0], linkaddr_node_addr.u8[1]);

  while(1) {
    static struct etimer et;
    linkaddr_t addr;
    
    etimer_set(&et, CLOCK_SECOND);
    
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    //  packetbuf_copyfrom("Hello", 5);
    //  addr.u8[0] = 2;
    //  addr.u8[1] = 0;
    //  if(!linkaddr_cmp(&addr, &linkaddr_node_addr)) {
    //    unicast_send(&uc, &addr);
    //  }

  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
