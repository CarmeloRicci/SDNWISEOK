/*
 * Copyright (C) 2015 SDN-WISE
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * \file
 *         SDN-WISE for Contiki.
 * \author
 *         Sebastiano Milardo <s.milardo@hotmail.it>
 */

/**
 * \addtogroup sdn-wise
 * @{
 */

#include "contiki.h"
#include "net/rime/rime.h"
#include "net/linkaddr.h"
#include "dev/watchdog.h"
#include "dev/uart1.h"
#include "dev/leds.h"
#include "lib/list.h"
#if CFS_ENABLED
#include "cfs/cfs.h"
#endif
#if ELF_ENABLED
#include "loader/elfloader.h"
#endif
#include "sdn-wise.h"
#include "flowtable.h"
#include "packet-buffer.h"
#include "packet-handler.h"
#include "packet-creator.h"
#include "neighbor-table.h"
#include "node-conf.h"
#include "address.h"

#define UART_BUFFER_SIZE MAX_PACKET_LENGTH

#define UNICAST_CONNECTION_NUMBER 29
#define BROADCAST_CONNECTION_NUMBER 30

#define TIMER_EVENT 50
#define RF_U_SEND_EVENT 51
#define RF_B_SEND_EVENT 52
#define RF_U_RECEIVE_EVENT 53
#define RF_B_RECEIVE_EVENT 54
#define UART_RECEIVE_EVENT 55
#define RF_SEND_BEACON_EVENT 56
#define RF_SEND_REPORT_EVENT 57
#define RF_SEND_DATA_EVENT 58
#define NEW_PACKET_EVENT 59
#define ACTIVATE_EVENT 60

#ifndef SDN_WISE_DEBUG
#define SDN_WISE_DEBUG 0
#endif
#if SDN_WISE_DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

// Inizio Melo

#define SERIAL_BUF_SIZE 128

// Fine Melo

/*----------------------------------------------------------------------------*/
PROCESS(main_proc, "Main Process");
PROCESS(rf_u_send_proc, "RF Unicast Send Process");
PROCESS(rf_b_send_proc, "RF Broadcast Send Process");
PROCESS(packet_handler_proc, "Packet Handler Process");
PROCESS(timer_proc, "Timer Process");
PROCESS(beacon_timer_proc, "Beacon Timer Process");
PROCESS(report_timer_proc, "Report Timer Process");
PROCESS(data_timer_proc, "Data Timer Process");
AUTOSTART_PROCESSES(
    &main_proc,
    &rf_u_send_proc,
    &rf_b_send_proc,
    &timer_proc,
    &beacon_timer_proc,
    &report_timer_proc,
    &packet_handler_proc,
    &data_timer_proc);
/*----------------------------------------------------------------------------*/
static uint8_t uart_buffer[UART_BUFFER_SIZE];
static uint8_t uart_buffer_index = 0;
static uint8_t uart_buffer_expected = 0;

uint8_t u;
  #if DEBUG_MELO
    
    int i= 0;

    int NNode2 = 0;
    int NNode3 = 0;
    int NNode4 = 0; 
    int NNode5 = 0; 
    int NNode6 = 0; 
  #endif

  #if MOBILE
    static uint8_t count = 0;
    static packet_t *p;
  #endif
/*----------------------------------------------------------------------------*/
void rf_unicast_send(packet_t *p) {
  
  #if DEBUG_MELO
      printf("--- rf_unicast_send - Sveglio rf_u_send_proc\n");
  #endif
  process_post(&rf_u_send_proc, RF_U_SEND_EVENT, (process_data_t)p);
  #if DEBUG_MELO
    printf("--- rf_unicast_send - Ho svegliato rf_u_send_proc\n");
  #endif
}
/*----------------------------------------------------------------------------*/
void rf_broadcast_send(packet_t *p) {

  #if DEBUG_MELO
    printf("--- rf_broadcast_send - Sveglio rf_b_send_proc\n");
  #endif
  process_post(&rf_b_send_proc, RF_B_SEND_EVENT, (process_data_t)p);
  #if DEBUG_MELO
    printf("--- rf_broadcast_send - Ho svegliato rf_b_send_proc\n");
  #endif
}
/*----------------------------------------------------------------------------*/
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
    #if DEBUG_MELO
      printf("--- unicast_rx_callback - Sveglio RF_U_RECEIVE_EVENT del main\n");
    #endif
  if (p != NULL) {
    p->info.rssi = get_packet_rssi(packetbuf_attr(PACKETBUF_ATTR_RSSI));
    process_post(&main_proc, RF_U_RECEIVE_EVENT, (process_data_t)p);
  }
    #if DEBUG_MELO
      printf("--- unicast_rx_callback - Ho Svegliato RF_U_RECEIVE_EVENT del main\n");
    #endif
}
/*----------------------------------------------------------------------------*/
static void broadcast_rx_callback(struct broadcast_conn *c, const linkaddr_t *from) {
  packet_t *p = get_packet_from_array((uint8_t *)packetbuf_dataptr());
    #if DEBUG_MELO
      printf("--- broadcast_rx_callback - Sveglio RF_B_RECEIVE_EVENT del main\n");
    #endif
  if (p != NULL) {
    p->info.rssi = get_packet_rssi(packetbuf_attr(PACKETBUF_ATTR_RSSI));
    process_post(&main_proc, RF_B_RECEIVE_EVENT, (process_data_t)p);
  }
    #if DEBUG_MELO
      printf("--- broadcast_rx_callback - Ho Sveglio RF_B_RECEIVE_EVENT del main\n");
    #endif
}
/*----------------------------------------------------------------------------*/
int uart_rx_callback(unsigned char c) {


    #if DEBUG_MELO
        //printf("--- uart_rx_callback - SERIALE LETTURA CI SONOOOOOOOOOOOO");
    #endif


      /*#if DEBUG_MELO

          static char rx_buf[MAX_PACKET_LENGTH];
          rx_buf[i] = c;
          
              #if DEBUG_MELO
                printf("--- uart_rx_callback - Ho ricevuto qualcosa dalla porta seriale\n");
              #endif

          printf("\nReceived1 %c\n", c);  // print single charter
          //u = (uint8_t)c;                 // convert char in decimal
          //printf("\nReceived2 %u\n", u);  // convert char in decimal

          printf ("MAX:%d --- buf[%d] = %c \n",MAX_PACKET_LENGTH, i, rx_buf [i]);
          

          if(i == 21){
            packet_t *pp = get_packet_from_array(rx_buf);
            i=0;
            print_packet_MELO(pp);
             printf("--- uart_rx_callback - Ho generato un pacchetto con quello che ho letto dalla seriale\n");
            rf_unicast_send(pp);
          }
          else {

          i++;
          }

      #endif
      */

/*
  #if DEBUG_MELO
      printf("--- uart_rx_callback - Sveglio UART_RECEIVE_EVENT del main - ricevo dati dalla porta seriale\n");
      printf("\nReceived1 %c\n", c);  // print single charter
      u = (uint8_t)c;                 // convert char in decimal
      printf("\nReceived2 %u\n", u);  // convert char in decimal
  #endif 
  u = (uint8_t)c;                   // passo direttametnte il decimale
  //printf("\nReceived2 %u\n", u);  // 

  uart_buffer[uart_buffer_index] = u;  // uart_buffer[uart_buffer_index] = c; 
  if (uart_buffer_index == LEN_INDEX) {
    uart_buffer_expected = u; // uart_buffer[uart_buffer_index] = c; 
  }
  uart_buffer_index++;
  if (uart_buffer_index == uart_buffer_expected) {
    uart_buffer_index = 0;
    uart_buffer_expected = 0;
    packet_t *p = get_packet_from_array(uart_buffer);

      #if DEBUG_MELO
        //print_packet_MELO(p);
        //rf_unicast_send(p);
      #endif


    if (p != NULL) {
      p->info.rssi = 0;
      //process_post(&main_proc, UART_RECEIVE_EVENT, (process_data_t)p);
      process_post(&packet_handler_proc, NEW_PACKET_EVENT, (process_data_t)p);

    }
    #if DEBUG_MELO
      printf("--- uart_rx_callback - Ho Sveglio UART_RECEIVE_EVENT del main\n");
    #endif
  }
  return 0;

*/

    #if DEBUG_MELO

  #if SINK
    #if DEBUG_MELO
        printf("--- uart_rx_callback - Sveglio UART_RECEIVE_EVENT del main - ricevo dati dalla porta seriale\n");
        printf("\nReceived1 %c\n", c);  // print single charter
        u = (uint8_t)c;                 // convert char in decimal
        printf("\nReceived2 %u\n", u);  // convert char in decimal
    #endif 
    u = (uint8_t)c;                   // passo direttametnte il decimale
    //printf("\nReceived2 %u\n", u);  // 

    uart_buffer[uart_buffer_index] = u;  // uart_buffer[uart_buffer_index] = c; 
    if (uart_buffer_index == LEN_INDEX) {
      uart_buffer_expected = u; // uart_buffer[uart_buffer_index] = c; 
    }
    uart_buffer_index++;
    if (uart_buffer_index == uart_buffer_expected) {
      uart_buffer_index = 0;
      uart_buffer_expected = 0;
      packet_t *p = get_packet_from_array(uart_buffer);

//printf("\n\n\n");

//print_packet_MELO(p);

//printf("\n\n\n");

        #if DEBUG_MELO
          //print_packet_MELO(p);
          //rf_unicast_send(p);
        #endif


      if (p != NULL) {
        p->info.rssi = 0;
        //process_post(&main_proc, UART_RECEIVE_EVENT, (process_data_t)p);
        process_post(&packet_handler_proc, NEW_PACKET_EVENT, (process_data_t)p);

      }
      #if DEBUG_MELO
        printf("--- uart_rx_callback - Ho Sveglio UART_RECEIVE_EVENT del main\n");
      #endif
    }
    return 0;

  #endif



  #if !SINK
    #if DEBUG_MELO
        //printf("--- uart_rx_callback - Sveglio UART_RECEIVE_EVENT del main - ricevo dati dalla porta seriale\n");
        //printf("\nReceived2 %u --- index[%d]\n", (uint8_t)c, uart_buffer_index);  // convert char in decimal
    #endif
    u = (uint8_t)c;                   // passo direttametnte il decimale
    //printf("\nReceived2 %u\n", u);  // 

    uart_buffer[uart_buffer_index] = u;  // uart_buffer[uart_buffer_index] = c; 
    uart_buffer_index++;
    if (uart_buffer_index == 80) {
      uart_buffer_index = 0;
    #if DEBUG_MELO
        //printf("\nCostruisco il pacchetto con create_data_MELO2\n");
    #endif
      packet_t *p = create_data_MELO2(uart_buffer,80);
    #if DEBUG_MELO
       //printf("\n ho costruito il pacchetto con create_data_MELO2\n");
    #endif
      if (p != NULL) {
        p->info.rssi = 0;
        //process_post(&main_proc, UART_RECEIVE_EVENT, (process_data_t)p);
        process_post(&packet_handler_proc, NEW_PACKET_EVENT, (process_data_t)p);

      }
      #if DEBUG_MELO
        printf("--- uart_rx_callback - Ho Sveglio UART_RECEIVE_EVENT del main\n");
      #endif
    }
    return 0;
  #endif
  
    #endif
}

/*----------------------------------------------------------------------------*/
static const struct unicast_callbacks unicast_callbacks = {
    unicast_rx_callback};
static struct unicast_conn uc;

static const struct broadcast_callbacks broadcast_callbacks = {
    broadcast_rx_callback};
static struct broadcast_conn bc;
/*----------------------------------------------------------------------------*/
PROCESS_THREAD(main_proc, ev, data) {
  PROCESS_BEGIN();
      #if DEBUG_MELO
        printf("--- main_proc --- \n");
      #endif

  //Uart1_Init(BAUD2UBR(115200));       /* set the baud rate as necessary */
  uart1_set_input(uart_rx_callback);    /* set the callback function */

  node_conf_init();
  flowtable_init();
  packet_buffer_init();
  neighbor_table_init();
  address_list_init();
  leds_init();

  #if SINK
    print_packet_uart(create_reg_proxy());
        #if DEBUG_MELO
          printf("--- main_proc - SONO IL SINK\n");
        #endif
  #endif

  while (1) {

    PROCESS_WAIT_EVENT();

      #if DEBUG_MELO
          printf("--- main_proc - ASPETTO UN EVENTO\n");
      #endif

    switch (ev) {
    case TIMER_EVENT:

      #if DEBUG_MELO
            printf("--- main_proc - TIMER_EVENT\n");
      #endif
      
      // test_handle_open_path();
      // test_flowtable();
      // test_neighbor_table();
      // test_packet_buffer();
      // test_address_list();
      print_flowtable();
      print_neighbor_table();
      print_node_conf();

      break;

    case UART_RECEIVE_EVENT:

      leds_toggle(LEDS_GREEN);
      #if DEBUG_MELO
            printf("--- main_proc - UART_RECEIVE_EVENT - Sveglio packet_handler_proc\n");
            //print_packet_MELO((packet_t)data);
      #endif
      process_post(&packet_handler_proc, NEW_PACKET_EVENT, (process_data_t)data);
      #if DEBUG_MELO
            printf("--- main_proc - UART_RECEIVE_EVENT - Ho svegliato packet_handler_proc - STATUS --> Ho ricevuto qualcosa dalla porta seriale\n");
      #endif

      break;

    case RF_B_RECEIVE_EVENT:


      #if DEBUG_MELO
            printf("--- main_proc - RF_B_RECEIVE_EVENT\n");
      #endif

      leds_toggle(LEDS_YELLOW);
      if (!conf.is_active)
      {
        conf.is_active = 1;
        process_post(&beacon_timer_proc, ACTIVATE_EVENT, (process_data_t)NULL);
        process_post(&report_timer_proc, ACTIVATE_EVENT, (process_data_t)NULL);
      }
      #if DEBUG_MELO
            printf("--- main_proc - RF_B_RECEIVE_EVENT   STATUS --> Ho ricevuto qualcosa in Broadcast\n");
      #endif

      //break; // MELO: l'ho agiunto io, ma non va messo!!!!

    case RF_U_RECEIVE_EVENT:

      #if DEBUG_MELO
            printf("--- main_proc - RF_U_RECEIVE_EVENT\n");
      #endif

     leds_toggle(LEDS_RED);
      process_post(&packet_handler_proc, NEW_PACKET_EVENT, (process_data_t)data);
      #if DEBUG_MELO
            printf("--- main_proc - RF_U_RECEIVE_EVENT   STATUS --> Ho ricevuto qualcosa in Unicast\n");
      #endif

      break;

    case RF_SEND_BEACON_EVENT:

      #if DEBUG_MELO
            printf("--- main_proc - RF_SEND_BEACON_EVENT - Sveglio rf_broadcast_send\n");
      #endif

      rf_broadcast_send(create_beacon());

      #if DEBUG_MELO
            printf("--- main_proc - RF_SEND_BEACON_EVENT - Ho svegliato rf_broadcast_send - STATUS --> Ho inviato un beacon \n");
      #endif

      break;

    case RF_SEND_REPORT_EVENT:

      #if DEBUG_MELO
            printf("--- main_proc - RF_SEND_REPORT_EVENT - Sveglio rf_unicast_send\n");
      #endif

      rf_unicast_send(create_report());

      #if DEBUG_MELO
            printf("--- main_proc - RF_SEND_REPORT_EVENT - Ho svegliato rf_unicast_send - STATUS --> Ho inviato un report \n");
      #endif

      #if !SINK
            if (conf.reset_period == 0)
            {
              conf.distance_from_sink = _MAX_DISTANCE;
              conf.reset_period = _RESET_PERIOD;
            }
            else
            {
              conf.reset_period--;
            }
      #endif

      break;

    case RF_SEND_DATA_EVENT:
/*
      #if DEBUG_MELO
            printf("--- main_proc - RF_SEND_DATA_EVENT\n");
      #endif

      // #if MOBILE
      //       if (conf.is_active)
      //       {
      //         p = create_data(count);
      //         PRINTF("STATUS --> 1\n");
      //         PRINTF("\n");
      //         if (p != NULL)
      //         {
      //           match_packet(p);
      //           PRINTF("STATUS --> 2\n");
      //           PRINTF("\n");
      //         }
      //         count++;
      //       }
      // #endif
      #if DEBUG_MELO

        #if !SINK

        //rf_unicast_send(create_data_MELO(1,33));

        if ((&(conf.my_address))->u8[1] == 2 )
        {
          //uint8_t payload[] = {72, 101, 108, 108, 111, 32, 87, 111, 114, 108, 100, 44, 32, 83, 111, 110, 111, 32, 105, 108, 32, 110, 111, 100, 111, 32, 50};
          //rf_unicast_send(create_data_MELO1(1,payload2,27));
          rf_unicast_send( create_data_MELO1(1,50, 48, 48, (48+NNode2)) );
          NNode2++;
        }

        if ((&(conf.my_address))->u8[1] == 3 )
        {
          //uint8_t payload[] = {72, 101, 108, 108, 111, 32, 87, 111, 114, 108, 100, 44, 32, 83, 111, 110, 111, 32, 105, 108, 32, 110, 111, 100, 111, 32, 51};
          //rf_unicast_send(create_data_MELO1(1,payload3,27));
          rf_unicast_send( create_data_MELO1(1,51, 48, 48, (48+NNode3)) );
          NNode3++;
        }

        if ((&(conf.my_address))->u8[1] == 4 )
        {
        //uint8_t payload[] = {72, 101, 108, 108, 111, 32, 87, 111, 114, 108, 100, 44, 32, 83, 111, 110, 111, 32, 105, 108, 32, 110, 111, 100, 111, 32, 52};
          //rf_unicast_send(create_data_MELO1(1,payload4,27));
          rf_unicast_send( create_data_MELO1(1,52, 48, 48, 48+NNode4) );
          NNode4++;
        }

        if ((&(conf.my_address))->u8[1] == 5 )
        {
        //uint8_t payload[] = {72, 101, 108, 108, 111, 32, 87, 111, 114, 108, 100, 44, 32, 83, 111, 110, 111, 32, 105, 108, 32, 110, 111, 100, 111, 32, 52};
          //rf_unicast_send(create_data_MELO1(1,payload4,27));
          rf_unicast_send( create_data_MELO1(1,53, 48, 48, 48+NNode5) );
          NNode5++;
        }

        if ((&(conf.my_address))->u8[1] == 6 )
        {
        //uint8_t payload[] = {72, 101, 108, 108, 111, 32, 87, 111, 114, 108, 100, 44, 32, 83, 111, 110, 111, 32, 105, 108, 32, 110, 111, 100, 111, 32, 52};
          //rf_unicast_send(create_data_MELO1(1,payload4,27));
          rf_unicast_send( create_data_MELO1(1,54, 48, 48, 48+NNode6) );
          NNode6++;
        }

        //print_address_MELO(&(conf.my_address),"TEST");
        //printf("TESTTTTT -> %d \n",(&(conf.my_address))->u8[1]);
        #endif
      #endif

      #if DEBUG_MELO
            printf("--- main_proc - RF_SEND_DATA_EVENT   STATUS --> pacchetto DATA inviato (forse)\n");
      #endif
*/
      break;

    }
  }
  PROCESS_END();
}
/*----------------------------------------------------------------------------*/
PROCESS_THREAD(rf_u_send_proc, ev, data){

  static linkaddr_t addr;
  PROCESS_EXITHANDLER(unicast_close(&uc);)
  PROCESS_BEGIN();

  unicast_open(&uc, UNICAST_CONNECTION_NUMBER, &unicast_callbacks);
  while (1)
  {
    PROCESS_WAIT_EVENT_UNTIL(ev == RF_U_SEND_EVENT);
    packet_t *p = (packet_t *)data;

    if (p != NULL) {
      p->header.ttl--;
      
      #if DEBUG_MELO
            printf("--- rf_u_send_proc - Sto mandando un pacchetto in unicast, il pacchetto: \n");
      #endif

      PRINTF("[TXU]: ");
      print_packet(p);
      PRINTF("\n");

      #if DEBUG_MELO
            //printf("--- rf_u_send_proc - Vedo se il pacchetto è destinato a me...destinatario (%c)", ((char)(p->header.dst)) );
            printf("--- rf_u_send_proc - Vedo se il pacchetto unicast e' destinato a me\n");
      #endif

      #if DEBUG_MELO
            print_address_MELO(&(p->header.dst),"destinatario");
      #endif

      if (!is_my_address(&(p->header.dst))) // if = nega(il destiantario sono io ?)
      {

      #if DEBUG_MELO
              printf("--- rf_u_send_proc - No, e' destinato a me\n");
      #endif

        int i = 0;

        int sent_size = 0;

        if (LINKADDR_SIZE < ADDRESS_LENGTH)
        {
          sent_size = LINKADDR_SIZE;
        }
        else
        {
          sent_size = ADDRESS_LENGTH;
        }

        for (i = 0; i < sent_size; ++i)
        {
          addr.u8[i] = p->header.nxh.u8[(sent_size - 1) - i];
        }

        addr.u8[0] = p->header.nxh.u8[1];
        addr.u8[1] = p->header.nxh.u8[0];
        uint8_t *a = (uint8_t *)p;
        packetbuf_copyfrom(a, p->header.len);
        unicast_send(&uc, &addr);
      }

      #if SINK
            else {
              print_packet_uart(p);
                #if DEBUG_MELO
                        printf("--- rf_u_send_proc - Sono il Sink, stampo il pacchetto nella seriale\n");
                #endif
            } 
      #endif

      packet_deallocate(p);
    }
  }
  PROCESS_END();
}
/*----------------------------------------------------------------------------*/
PROCESS_THREAD(rf_b_send_proc, ev, data) {
  PROCESS_EXITHANDLER(broadcast_close(&bc);)
  PROCESS_BEGIN();
  broadcast_open(&bc, BROADCAST_CONNECTION_NUMBER, &broadcast_callbacks);
  while (1)
  {

    PROCESS_WAIT_EVENT_UNTIL(ev == RF_B_SEND_EVENT);
    packet_t *p = (packet_t *)data;

    if (p != NULL)
    {
      p->header.ttl--;

      #if DEBUG_MELO
            printf("--- rf_b_send_proc - Sto mandando un pacchetto in broadcast, il pacchetto: \n");
      #endif

      PRINTF("[TXB]: ");
      print_packet(p);
      PRINTF("\n");

      uint8_t *a = (uint8_t *)p;
      packetbuf_copyfrom(a, p->header.len);
      broadcast_send(&bc);
      packet_deallocate(p);
    }
  }
  PROCESS_END();
}
/*----------------------------------------------------------------------------*/
PROCESS_THREAD(timer_proc, ev, data) {
  static struct etimer et;
  PROCESS_BEGIN();

  while (1)
  {
    etimer_set(&et, 3 * CLOCK_SECOND);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
      #if DEBUG_MELO
        printf("--- timer_proc - Sveglio il main per attivare TIMER_EVENT\n");
      #endif
    process_post(&main_proc, TIMER_EVENT, (process_data_t)NULL);
      #if DEBUG_MELO
         printf("--- timer_proc - Ho svegliato il main che ha attivato TIMER_EVENT\n");
      #endif
  }
  PROCESS_END();
}
/*----------------------------------------------------------------------------*/
PROCESS_THREAD(beacon_timer_proc, ev, data) {
  static struct etimer et;

  PROCESS_BEGIN();
  while (1)
  {
      #if !SINK
          if (!conf.is_active)
          {
            PROCESS_WAIT_EVENT_UNTIL(ev == ACTIVATE_EVENT);
          }
      #endif

    etimer_set(&et, conf.beacon_period * CLOCK_SECOND);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
      #if DEBUG_MELO
        printf("--- beacon_timer_proc - Sveglio il main per attivare RF_SEND_BEACON_EVENT\n");
      #endif
    process_post(&main_proc, RF_SEND_BEACON_EVENT, (process_data_t)NULL);
      #if DEBUG_MELO
         printf("--- beacon_timer_proc - Ho svegliato il main che ha attivato RF_SEND_BEACON_EVENT\n");
      #endif
  }
  PROCESS_END();
}
/*----------------------------------------------------------------------------*/
PROCESS_THREAD(report_timer_proc, ev, data) {

  static struct etimer et;

  PROCESS_BEGIN();
  while (1)
  {
    #if !SINK
        if (!conf.is_active)
        {
          PROCESS_WAIT_EVENT_UNTIL(ev == ACTIVATE_EVENT);
        }
    #endif

    etimer_set(&et, conf.report_period * CLOCK_SECOND);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
      #if DEBUG_MELO
        printf("--- report_timer_proc - Sveglio il main per attivare RF_SEND_REPORT_EVENT\n");
      #endif
    process_post(&main_proc, RF_SEND_REPORT_EVENT, (process_data_t)NULL);
      #if DEBUG_MELO
         printf("--- report_timer_proc - Ho svegliato il main che ha attivato RF_SEND_REPORT_EVENT\n");
      #endif
  }
  PROCESS_END();
}
/*----------------------------------------------------------------------------*/
PROCESS_THREAD(data_timer_proc, ev, data) {

  static struct etimer et;

  PROCESS_BEGIN();
      // #if DEBUG_MELO
      //   printf("--- data_timer_proc ---\n");
      // #endif
  while (1)
  {
    etimer_set(&et, 5 * CLOCK_SECOND);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
      #if DEBUG_MELO
         printf("--- data_timer_proc - Sveglio il main per attivare RF_SEND_DATA_EVENT\n");
      #endif

    process_post(&main_proc, RF_SEND_DATA_EVENT, (process_data_t)NULL);

      #if DEBUG_MELO
         printf("--- data_timer_proc - Ho svegliato il main che ha attivato RF_SEND_DATA_EVENT\n");
      #endif
  }
  PROCESS_END();
}
/*----------------------------------------------------------------------------*/
PROCESS_THREAD(packet_handler_proc, ev, data) {

  PROCESS_BEGIN();
  while (1)
  {
    PROCESS_WAIT_EVENT_UNTIL(ev == NEW_PACKET_EVENT);
      #if DEBUG_MELO
        printf("--- packet_handler_proc - processo un pacchetto\n");
      #endif
    packet_t *p = (packet_t *)data;
    PRINTF("[RX ]: ");
    print_packet(p);
    PRINTF("\n");
      #if DEBUG_MELO
        printf("--- packet_handler_proc - Eseguo handle_packet\n");
      #endif
    handle_packet(p);
      #if DEBUG_MELO
        printf("--- packet_handler_proc - Ho eseguito handle_packet\n");
      #endif
  }
  PROCESS_END();
}
/*----------------------------------------------------------------------------*/
