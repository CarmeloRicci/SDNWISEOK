/*
#include "contiki.h"
#include "dev/serial-line.h"
#include <stdio.h>

#include "contiki.h"
#include "net/rime/rime.h"
#include "net/linkaddr.h"
#include "dev/watchdog.h"
#include "dev/uart1.h"
#include "dev/leds.h"
#include "lib/list.h"

#include "sdn-wise.h"
#include "sdn-wise.c"
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


static uint8_t uart_buffer[80];
static uint8_t uart_buffer_index = 0;
uint8_t u;
linkaddr_t addr;
static struct unicast_conn uc;
int count = 0;

static struct unicast_conn uc;

int uart_rx_callback(unsigned char c){
#if DEBUG_MELO
    uart_buffer_index++;
    uart_buffer[uart_buffer_index] = c;

  if (uart_buffer_index == 80) {

    printf("HO RICEVUTO UNA FRAME\n");
    uart_buffer_index = 0;


    if(!linkaddr_cmp(&addr, &linkaddr_node_addr)) {
      packet_t *p = create_data_MELO2(uart_buffer,80);
      packetbuf_copyfrom(p, 90);
      unicast_send(&uc, &addr);
      printf("Paccehtto inviato: %i\n", count++);
    }

  }
  return 0;
    #endif
}

PROCESS(my_first_app_process, "My_First_App");
AUTOSTART_PROCESSES(&my_first_app_process);

static void
recv_uc(struct unicast_conn *c, const linkaddr_t *from){
  printf("unicast message received from %d.%d\n",
	 from->u8[0], from->u8[1]);
}

static void
sent_uc(struct unicast_conn *c, int status, int num_tx){
  const linkaddr_t *dest = packetbuf_addr(PACKETBUF_ADDR_RECEIVER);
  if(linkaddr_cmp(dest, &linkaddr_null)) {
    return;
  }
  //printf("unicast message sent to %d.%d: status %d num_tx %d\n",
  //  dest->u8[0], dest->u8[1], status, num_tx);
}

static const struct unicast_callbacks unicast_callbacks = {recv_uc, sent_uc};


PROCESS_THREAD(my_first_app_process, ev, data){
  PROCESS_BEGIN();
  
  unicast_open(&uc, 146, &unicast_callbacks);
    linkaddr_node_addr.u8[0] = 0;
    linkaddr_node_addr.u8[1] = 2;
    addr.u8[0] = 0;
    addr.u8[1] = 1;
  uart1_set_input(uart_rx_callback);

  for (;;)
  {
    PROCESS_YIELD();
    printf("sonone main");
  }
  PROCESS_END();
}


*/
/*

#include "contiki.h"
#include "net/rime/rime.h"
#include "net/linkaddr.h"
#include "dev/watchdog.h"
#include "dev/uart1.h"
#include "dev/leds.h"
#include "lib/list.h"

#include "contiki.h"
#include "net/rime/rime.h"

//~ #include "dev/button-sensor.h"

#include "dev/leds.h"

#include <stdio.h>

//---------------------------------------------------------------------------//
PROCESS(example_unicast_process, "Example unicast");

AUTOSTART_PROCESSES(&example_unicast_process);
//---------------------------------------------------------------------------//
static void recv_uc(struct unicast_conn *c, const rimeaddr_t *from) {
	printf("unicast message received from %d.%d: %s", from->u8[0], from->u8[1],
			(char*) packetbuf_dataptr());
	printf("\n");
}
static const struct unicast_callbacks unicast_callbacks = { recv_uc };
static struct unicast_conn uc;
//---------------------------------------------------------------------------//
PROCESS_THREAD(example_unicast_process, ev, data) {
	PROCESS_EXITHANDLER(unicast_close(&uc);)

	PROCESS_BEGIN();

		unicast_open(&uc, 128, &unicast_callbacks);

		printf("rimeaddr_node_addr = [%u, %u]\n", rimeaddr_node_addr.u8[0], rimeaddr_node_addr.u8[1]);

		static int i = 0;
		static struct etimer et;

		etimer_set(&et, 1*CLOCK_SECOND);

		while(1) {
			rimeaddr_t addr;

			PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

			char msg[64]; int len;
			len = sprintf(msg, "hello world #%u", i) + 1 ;
			i++;

			packetbuf_copyfrom(msg, len);
			addr.u8[0] = 182;
			addr.u8[1] = 206;
			if (rimeaddr_node_addr.u8[0] == 107 && rimeaddr_node_addr.u8[1] == 179) {
				unicast_send(&uc, &addr);
				printf("unicast message sent [%i bytes]\n", len);
			}

			etimer_reset(&et);

		}

		PROCESS_END();
	}
//---------------------------------------------------------------------------//
*/



// ************************************* ECHO DELLA SERIALE *************************************

/*
#include "contiki.h"
#include <stdio.h>
#include "dev/serial-line.h"
#include "dev/uart1.h"
#include "dev/leds.h"
#include "lib/list.h"
#include "net/rime/rime.h"
#include "net/linkaddr.h"
#include "dev/watchdog.h"
#include "contiki.h"
#include "net/rime/rime.h"
#include "net/linkaddr.h"  
#include "dev/watchdog.h"
#include "dev/uart1.h"
#include "dev/leds.h"  
#include "lib/list.h"
#include "sdn-wise.h"
#include "flowtable.h"
#include "packet-buffer.h"
#include "packet-handler.h"
#include "packet-creator.h"
#include "neighbor-table.h"
#include "node-conf.h"


int serial_input_byte_melo(unsigned char c) {
    printf("got input byte: %d ('%c')\n", c, c);
    return 0;
}

PROCESS(main_process, "main process");
AUTOSTART_PROCESSES(&main_process);

PROCESS_THREAD(main_process, ev, data) {
    PROCESS_BEGIN();
    
    //uart1_Init(BAUD2UBR(115200));
    //uart1_Init(BAUD2UBR(115200));
    uart1_set_input(serial_input_byte_melo);

    // for(;;) {
    //     PROCESS_WAIT_EVENT();
    //     if (ev == serial_line_event_message && data != NULL) {
    //        printf("got input string: '%s'\n", (const char *) data);
    //     }
    // }
    PROCESS_END();
}


/*
#include "contiki.h"

#include <stdio.h>

PROCESS(hello_world_process, "Hello world process");
AUTOSTART_PROCESSES(&hello_world_process);

PROCESS_THREAD(hello_world_process, ev, data)
{
  PROCESS_BEGIN();

  printf("Hello, world\n");
  
  PROCESS_END();
}

*/


// ************************************* ECHO DELLA SERIALE  + BUFFER *************************************

#include "contiki.h"
#include <stdio.h>
#include "dev/serial-line.h"
#include "dev/uart1.h"
#include "dev/leds.h"
#include "lib/list.h"
#include "net/rime/rime.h"
#include "net/linkaddr.h"
#include "dev/watchdog.h"
#include "contiki.h"
#include "net/rime/rime.h"
#include "net/linkaddr.h"  
#include "dev/watchdog.h"
#include "dev/uart1.h"
#include "dev/leds.h"  
#include "lib/list.h"
#include "sdn-wise.h"
#include "flowtable.h"
#include "packet-buffer.h"
#include "packet-handler.h"
#include "packet-creator.h"
#include "neighbor-table.h"
#include "node-conf.h"


static uint8_t uart_buffer[85];
static uint32_t uart_buffer_index = 0;
static uint32_t count = 0;
static uint32_t count1 = 0;

int serial_input_byte_melo(unsigned char c) {

    uart_buffer[uart_buffer_index] = c;
    uart_buffer_index++;
    count1++;
    //printf("Count1: %d\n",count1);
  if (uart_buffer_index == 80) {
    count++;
    printf("FRAME N: %d\n",count);
    //for (int jj = 0; jj < 80; jj++){
    //  printf("%c ", uart_buffer[jj] );
    //}
    //printf("\n");
    uart_buffer_index = 0;
    count1=0;
  }
  
    return 0;
}

PROCESS(main_process, "main process");
AUTOSTART_PROCESSES(&main_process);

PROCESS_THREAD(main_process, ev, data) {
    PROCESS_BEGIN();
    
    //uart1_Init(BAUD2UBR(115200));
    //uart1_Init(BAUD2UBR(115200));
    uart1_set_input(serial_input_byte_melo);
    printf("Iniziamo\n");

    PROCESS_END();
}
