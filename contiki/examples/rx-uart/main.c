#include "contiki.h"
#include <stdio.h>
#include "dev/uart1.h"

/*----------------------------------------------------------------------------*/
  int
  uart_rx_callback(unsigned char c)
  {
    putchar('-');
    putchar(c);
    return 0;
  }
/*----------------------------------------------------------------------------*/

PROCESS(hello_world_process, "Hello world process"); 
AUTOSTART_PROCESSES(&hello_world_process);
 

/* Implementation of the first process */
PROCESS_THREAD(hello_world_process, ev, data)
{
     // variables are declared static to ensure their values are kept
     // between kernel calls.
    static struct etimer timer;
    static int count = 0;

     // any process mustt start with this.
    PROCESS_BEGIN();
    uart1_init(BAUD2UBR(115200));       /* set the baud rate as necessary */
    uart1_set_input(uart_rx_callback);  /* set the callback function */

     // set the etimer module to generate an event in one second.
     etimer_set(&timer, CLOCK_CONF_SECOND);
     while (1)
     {
         // wait here for an event to happen
         PROCESS_WAIT_EVENT();

         // if the event is the timer event as expected...
         if(ev == PROCESS_EVENT_TIMER)
         {
             // do the process work
             printf("Hello, world #%i\n", count);
             count ++;
             
             // reset the timer so it will generate an other event
             // the exact same time after it expired (periodicity guaranteed)
             etimer_reset(&timer);
         }
     }
     // any process must end with this, even if it is never reached.
     PROCESS_END();
 }

	

