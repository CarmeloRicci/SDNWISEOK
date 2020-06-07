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
 *         SDN-WISE Packet Creator.
 * \author
 *         Sebastiano Milardo <s.milardo@hotmail.it>
 */

/**
 * \addtogroup sdn-wise
 * @{
 */
#include <stdio.h>
#include "packet-buffer.h"
#include "packet-creator.h"
#include "node-conf.h"
#include "address.h"
#include "neighbor-table.h"
#include "sdn-wise.h"

#if BATTERY_ENABLED
#include "dev/battery-sensor.h"
#endif

#ifdef X_NUCLEO_IKS01A1
#include "dev/temperature-sensor.h"
#include "dev/humidity-sensor.h"
#include "dev/pressure-sensor.h"
#include "dev/sensor-common.h"
#define NO_OF_SENSORS 3
#endif /*X_NUCLEO_IKS01A1*/

#ifndef SDN_WISE_DEBUG
#define SDN_WISE_DEBUG 0
#endif
#if SDN_WISE_DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif
/*----------------------------------------------------------------------------*/
packet_t* 
create_beacon(void)
{
  packet_t* p = create_packet_empty();
  if (p != NULL){
    p->header.net = conf.my_net;
    set_broadcast_address(&(p->header.dst));
    p->header.src = conf.my_address; 
    p->header.typ = BEACON;
    p->header.nxh = conf.sink_address;
  
    set_payload_at(p, BEACON_HOPS_INDEX, conf.hops_from_sink);

#if BATTERY_ENABLED           
    SENSORS_ACTIVATE(battery_sensor);
    set_payload_at(p, BEACON_BATT_INDEX, battery_sensor.value(0));
    SENSORS_DEACTIVATE(battery_sensor);
#else
    set_payload_at(p, BEACON_BATT_INDEX, 0xff);
#endif
  }
  return p;
}
/*----------------------------------------------------------------------------*/
packet_t* 
create_data(uint8_t count)
{
#ifdef X_NUCLEO_IKS01A1  
    int i = 0; 
    uint8_t sensor_values[sizeof(int)*NO_OF_SENSORS];
    int* sensor_values_ptr = &sensor_values;
    SENSORS_ACTIVATE(temperature_sensor);
    SENSORS_ACTIVATE(humidity_sensor);
    SENSORS_ACTIVATE(pressure_sensor);

    sensor_values_ptr[1] = temperature_sensor.value(0);
    sensor_values_ptr[2] = humidity_sensor.value(0);
    sensor_values_ptr[3] = pressure_sensor.value(0);

    SENSORS_DEACTIVATE(temperature_sensor);
    SENSORS_DEACTIVATE(humidity_sensor);
    SENSORS_DEACTIVATE(pressure_sensor);
#endif

  packet_t* p = NULL;
#if !SINK
  p = create_packet_empty();
    if (p != NULL){
      p->header.net = conf.my_net;
      p->header.dst = get_address_from_int(1); // Replace 5 with your dst
      p->header.src = conf.my_address;
      p->header.typ = DATA;
      p->header.nxh = conf.nxh_vs_sink;
#ifdef X_NUCLEO_IKS01A1 
      for (i = 0; i < sizeof(int)*NO_OF_SENSORS; i++){
        set_payload_at(p, i, sensor_values[i]);
      }
#else
      set_payload_at(p, 0, count);
#endif
    }
#endif
  return p;
}
/*----------------------------------------------------------------------------*/
packet_t* 
create_report(void)
{  
  packet_t* p = create_packet_empty();
  if (p != NULL){
    p->header.net = conf.my_net;
    p->header.dst = conf.sink_address;
    p->header.src = conf.my_address; 
    p->header.typ = REPORT;
    p->header.nxh = conf.nxh_vs_sink;
    
    set_payload_at(p, BEACON_HOPS_INDEX, conf.hops_from_sink);
                
#if BATTERY_ENABLED          
    SENSORS_ACTIVATE(battery_sensor);
    set_payload_at(p, BEACON_BATT_INDEX, battery_sensor.value(0));
    SENSORS_DEACTIVATE(battery_sensor);
#else
    set_payload_at(p, BEACON_BATT_INDEX, 0xff);
#endif

    fill_payload_with_neighbors(p);
  }
  return p;
}
/*----------------------------------------------------------------------------*/
packet_t* 
create_reg_proxy(void)
{  
  uint8_t payload[] = {
    48, 48, 48, 48, 48, 48, 48, 49,
     0,  1,  2,  3,  4,  5,  0,  0,
     0,  0,  0,  0,  0,  1,-64,-88,
     1, 108, 39, 6
  }; 

  packet_t* p = create_packet_payload(
    conf.my_net, 
    &conf.sink_address, 
    &conf.my_address, 
    REG_PROXY, 
    &conf.nxh_vs_sink,
    payload, 
    28);
  return p;
}
/*----------------------------------------------------------------------------*/
void
create_and_send_request(packet_t* p)
{
  PRINTF("packet.c --- create_and_send_request - Inizio\n");
  uint8_t i = 0;    
    
  if (p->header.len < MAX_PAYLOAD_LENGTH){  
    packet_t* r = create_packet_empty();
    if (r != NULL){
      PRINTF("packet.c --- create_and_send_request - Ho già il pacchetto\n");
      r->header.net = conf.my_net;
      r->header.dst = conf.sink_address;
      r->header.src = conf.my_address; 
      r->header.typ = REQUEST;
      r->header.nxh = conf.nxh_vs_sink;

      uint8_t* a = (uint8_t*)p;
      set_payload_at(r, 0, conf.requests_count);
      set_payload_at(r, 1, 0);
      set_payload_at(r, 2, 1);     
      for (i = 0; i < (p->header.len); ++i){
        set_payload_at(r, i+3, a[i]);
      }
    rf_unicast_send(r);
    PRINTF("packet.c --- create_and_send_request - Mando in unicast\n");
    conf.requests_count++;

    }
  } else {   
    PRINTF("packet.c --- create_and_send_request - Non ho già il pacchetto\n");
    packet_t* r1 = create_packet_empty();
    packet_t* r2 = create_packet_empty();
    
    if (r1 != NULL && r2 != NULL){
      r1->header.net = conf.my_net;
      r1->header.dst = conf.sink_address;
      r1->header.src = conf.my_address; 
      r1->header.typ = REQUEST;
      r1->header.nxh = conf.nxh_vs_sink;
      
      r2->header.net = conf.my_net;
      r2->header.dst = conf.sink_address;
      r2->header.src = conf.my_address; 
      r2->header.typ = REQUEST;
      r2->header.nxh = conf.nxh_vs_sink;

      set_payload_at(r1, 0, conf.requests_count);
      set_payload_at(r1, 1, 0);
      set_payload_at(r1, 2, 2);

      set_payload_at(r2, 0, conf.requests_count);
      set_payload_at(r2, 1, 1);
      set_payload_at(r2, 2, 2);     
      
      uint8_t* a = (uint8_t*)p;     
      for (i = 0; i < MAX_PAYLOAD_LENGTH; ++i){
        set_payload_at(r1, i+3, a[i]);
      }

      for (i = 0; i < (p->header.len - MAX_PAYLOAD_LENGTH); ++i){
        set_payload_at(r2, i+3, a[i + MAX_PAYLOAD_LENGTH]);
      }

      rf_unicast_send(r1);
      PRINTF("packet.c --- create_and_send_request - Mando 1/2 pacchetti in unicast\n");
      rf_unicast_send(r2);

      PRINTF("packet.c --- create_and_send_request - Mando 2/2 pacchetti in unicast\n");
      conf.requests_count++;
    } else {
      if (r1 == NULL){
        packet_deallocate(r1);
      } else {
        packet_deallocate(r2);
      }
    }
  }
  packet_deallocate(p);
  //PRINTF("packet.c --- create_and_send_request - dealloco il pacchetto\n");
}
/*----------------------------------------------------------------------------*/
packet_t* 
create_config(void)
{
  // TODO 
  return NULL;
}
/*----------------------------------------------------------------------------*/
/** @} */

  #if DEBUG_MELO
    #if !SINK
        packet_t * create_data_MELO2(uint8_t payload[], int dim) {
        printf("!!!create_data_MELO2!!!\n\n\n");
        #if !SINK
        packet_t *p = create_packet_payload(
        conf.my_net,
        &conf.sink_address,
        &conf.my_address,
        DATA,
        &conf.nxh_vs_sink,
        payload,
        dim);

        //p->header.ttl--;
        
        #if DEBUG_MELO
          print_packet_MELO(p);
        #endif
        
        return p;
        }
    #endif
#endif
    //   return p;
    //   #endif
      
    //   #if SINK
    //   packet_t *p = NULL;
    //   return p;
    //   #endif
    // }
  #endif

     #if DEBUG_MELO
    packet_t * create_data_MELO1(int addressdest, int nodo, int n1, int n2, int n3) {

      packet_t *p = NULL;

    #if !SINK
        p = create_packet_empty();
        if (p != NULL) {
          p->header.net = conf.my_net;
          p->header.dst = get_address_from_int(addressdest); // Replace 1 with your dst
          p->header.src = conf.my_address;
          p->header.typ = DATAMELO;
          p->header.nxh = conf.my_address;
          
          set_payload_at(p, 0, 83);
          set_payload_at(p, 1, 111);
          set_payload_at(p, 2, 110);
          set_payload_at(p, 3, 111);
          set_payload_at(p, 4, 32);
          set_payload_at(p, 5, 105);
          set_payload_at(p, 6, 108);
          set_payload_at(p, 7, 32);
          set_payload_at(p, 8, 110);
          set_payload_at(p, 9, 111);
          set_payload_at(p, 10, 100);
          set_payload_at(p, 11, 111);
          set_payload_at(p, 12, 32);
          set_payload_at(p, 13, nodo);
          set_payload_at(p, 14, 44);
          set_payload_at(p, 15, 32);
          set_payload_at(p, 16, 112);
          set_payload_at(p, 17, 97);
          set_payload_at(p, 18, 99);
          set_payload_at(p, 19, 99);
          set_payload_at(p, 20, 104);
          set_payload_at(p, 21, 101);
          set_payload_at(p, 22, 116);
          set_payload_at(p, 23, 116);
          set_payload_at(p, 24, 111);
          set_payload_at(p, 25, 32);
          set_payload_at(p, 26, 110);
          set_payload_at(p, 27, 32);
          set_payload_at(p, 28, n1);
          set_payload_at(p, 29, n2);
          set_payload_at(p, 30, n3);
          
        }
      #endif
      return p;
    }
   #endif




     #if DEBUG_MELO
    packet_t * create_data_MELO3(int addressdest) {

      packet_t *p = NULL;

    #if !SINK
        p = create_packet_empty();
        if (p != NULL) {
          p->header.net = conf.my_net;
          p->header.dst = get_address_from_int(addressdest); // Replace 1 with your dst
          p->header.src = conf.my_address;
          p->header.typ = DATA;
          p->header.nxh = conf.nxh_vs_sink;
          
          set_payload_at(p, 0, 39 );
          set_payload_at(p, 1, 49);
          set_payload_at(p, 2, 32);
          set_payload_at(p, 3, 50);
          set_payload_at(p, 4, 32);
          set_payload_at(p, 5, 51);
          set_payload_at(p, 6, 32);
          set_payload_at(p, 7, 52);
          set_payload_at(p, 8, 32);
          set_payload_at(p, 9, 53);
          set_payload_at(p, 10, 32);
          set_payload_at(p, 11, 54);
          set_payload_at(p, 12, 32);
          set_payload_at(p, 13, 55);
          set_payload_at(p, 14, 32);
          set_payload_at(p, 15, 56);
          set_payload_at(p, 16, 32);
          set_payload_at(p, 17, 57);
          set_payload_at(p, 18, 32);
          set_payload_at(p, 19, 49);
          set_payload_at(p, 20, 48);
          set_payload_at(p, 21, 32);
          set_payload_at(p, 22, 49);
          set_payload_at(p, 23, 49);
          set_payload_at(p, 24, 32);
          set_payload_at(p, 25, 49);
          set_payload_at(p, 26, 50);
          set_payload_at(p, 27, 32);
          set_payload_at(p, 28, 49);
          set_payload_at(p, 29, 51);
          set_payload_at(p, 30, 32);
          set_payload_at(p, 31, 49);
          set_payload_at(p, 32, 52);
          set_payload_at(p, 33, 32);
          set_payload_at(p, 34, 49);
          set_payload_at(p, 35, 53);
          set_payload_at(p, 36, 32);
          set_payload_at(p, 37, 49);
          set_payload_at(p, 38, 54);
          set_payload_at(p, 39, 32);
          set_payload_at(p, 40, 49);
          set_payload_at(p, 41, 55);
          set_payload_at(p, 42, 32);
          set_payload_at(p, 43, 49);
          set_payload_at(p, 44, 56);
          set_payload_at(p, 45, 32);
          set_payload_at(p, 46, 49);
          set_payload_at(p, 47, 57);
          set_payload_at(p, 48, 32);
          set_payload_at(p, 49, 50);
          set_payload_at(p, 50, 48);
          set_payload_at(p, 51, 32);
          set_payload_at(p, 52, 50);
          set_payload_at(p, 53, 49);
          set_payload_at(p, 54, 32);
          set_payload_at(p, 55, 50);
          set_payload_at(p, 56, 50);
          set_payload_at(p, 57, 32);
          set_payload_at(p, 58, 50);
          set_payload_at(p, 59, 51);
          set_payload_at(p, 60, 32);
          set_payload_at(p, 61, 50);
          set_payload_at(p, 62, 52);
          set_payload_at(p, 63, 32);
          set_payload_at(p, 64, 50);
          set_payload_at(p, 65, 53);
          set_payload_at(p, 66, 32);
          set_payload_at(p, 67, 50);
          set_payload_at(p, 68, 54);
          set_payload_at(p, 69, 32);
          set_payload_at(p, 70, 50);
          set_payload_at(p, 71, 55);
          set_payload_at(p, 72, 32);
          set_payload_at(p, 73, 50);
          set_payload_at(p, 74, 56);
          set_payload_at(p, 75, 32);
          set_payload_at(p, 76, 50);
          set_payload_at(p, 77, 57);
          set_payload_at(p, 78, 32);
          set_payload_at(p, 78, 50);
          
        }
      #endif
      return p;
    }
   #endif



     #if DEBUG_MELO
    packet_t * create_data_MELO4(int addressdest,int addresssorce) {

      packet_t *p = NULL;

    #if !SINK
        p = create_packet_empty();
        if (p != NULL) {
          p->header.net = conf.my_net;
          p->header.dst = get_address_from_int(addresssorce); // Replace 1 with your dst
          p->header.src = get_address_from_int(addressdest);
          p->header.typ = OPEN_PATH;
          p->header.nxh = get_address_from_int(addresssorce);
          
          p->header.ttl--;

          set_payload_at(p, 0, 0);
          set_payload_at(p, 1, 0);
          set_payload_at(p, 2, 1);
          set_payload_at(p, 3, 0);
          set_payload_at(p, 4, 2);
          
        }
      #endif
      return p;
    }
   #endif

