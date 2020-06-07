#include <stdio.h> 

#include "contiki.h"
#include "net/ip/uip.h"
#include "net/ipv6/uip-ds6.h"
#include "net/ip/uip-udp-packet.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include "dev/radio-sensor.h"
#include "dev/sensor-common.h"
#include "st-lib.h"
#ifdef X_NUCLEO_IKS01A1
#include "dev/temperature-sensor.h"
#include "dev/humidity-sensor.h"
#include "dev/pressure-sensor.h"
#include "dev/magneto-sensor.h"
#include "dev/acceleration-sensor.h"
#include "dev/gyroscope-sensor.h"
#endif 

#define DEBUG DEBUG_PRINT
#include "net/ip/uip-debug.h"

#define PRINT_INTERVAL 10 * CLOCK_SECOND

PROCESS(sensor_demo_process, "Sensor demo process");
AUTOSTART_PROCESSES(&sensor_demo_process);

PROCESS_THREAD(sensor_demo_process, ev, data)
{
  static struct etimer etimer;
  static unsigned long _button_pressed;
  static int temperatura = 0;
  static int umidita = 0;
  static int pressione = 0;
  static int accelerazione_x = 0;
  static int accelerazione_y = 0;
  static int accelerazione_z = 0;
  static int giroscopio_x = 0;
  static int giroscopio_y = 0;
  static int giroscopio_z = 0;


  PROCESS_BEGIN();
  PROCESS_PAUSE();

  SENSORS_ACTIVATE(button_sensor);

  SENSORS_ACTIVATE(radio_sensor);

#ifdef X_NUCLEO_IKS01A1
  SENSORS_ACTIVATE(temperature_sensor);
  SENSORS_ACTIVATE(humidity_sensor);
  SENSORS_ACTIVATE(pressure_sensor);
  SENSORS_ACTIVATE(magneto_sensor);
  SENSORS_ACTIVATE(acceleration_sensor);
  SENSORS_ACTIVATE(gyroscope_sensor);
#endif 

  while(1) {
    etimer_set(&etimer, PRINT_INTERVAL);
    PROCESS_WAIT_EVENT();

    if(ev == sensors_event && data == &button_sensor) {
      _button_pressed++;
      leds_toggle(LEDS_ALL);
    }

#ifdef X_NUCLEO_IKS01A1

    temperatura = temperature_sensor.value(0);
    umidita = humidity_sensor.value(0);
    accelerazione_x = acceleration_sensor.value(X_AXIS);
    accelerazione_y = acceleration_sensor.value(Y_AXIS);
    accelerazione_z = acceleration_sensor.value(Z_AXIS);
    giroscopio_x = gyroscope_sensor.value(X_AXIS);
    giroscopio_y = gyroscope_sensor.value(Y_AXIS);
    giroscopio_z = gyroscope_sensor.value(Z_AXIS);
    pressione = pressure_sensor.value(0);

    printf("%d.%d %d.%d %d %d %d %d %d %d %d.%d\n",
		temperatura/10, ABS_VALUE(temperatura) % 10,
		umidita/10, ABS_VALUE(umidita) % 10,
		accelerazione_x,
		accelerazione_y,
		accelerazione_z,
		giroscopio_x,
		giroscopio_y,
		giroscopio_z,
		pressione,ABS_VALUE(pressione) % 10);
#endif 
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
