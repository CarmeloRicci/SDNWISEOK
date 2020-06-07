#include "contiki.h"
#include "dev/battery-sensor.h"

const struct sensors_sensor battery_sensor;
/*---------------------------------------------------------------------------*/
static int
value(int type)
{
  return 255;
}
/*---------------------------------------------------------------------------*/
static int
configure(int type, int c)
{
  return 1;
}
/*---------------------------------------------------------------------------*/
static int
status(int type)
{
  return 1;
}
/*---------------------------------------------------------------------------*/
SENSORS_SENSOR(battery_sensor, BATTERY_SENSOR, value, configure, status);
