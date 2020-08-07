#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <esp_log.h>

#include "osmolation.h"

#include "contacts.h"
#include "flags.h"
#include "relays.h"

static char *TAG = "osmolation";

static void osmolation_timer_callback();

struct osmolation_s osmolation;

#define MAX_ON_DELAY_S 60 
#define LOOP_DELAY_MS 2000

static int8_t is_init=0;

const esp_timer_create_args_t osmolation_timer_args = {
   .callback = &osmolation_timer_callback,
   .name = "osmo",
   .arg = (void *)0
};
esp_timer_handle_t osmolation_timer;


static void osmolation_timer_callback()
{
   int8_t state = osmolation.state;
   relays_set(osmolation.tank_relay, 0);
   if(state == OSMOLATION_FORCED || is_init==0) {
      osmolation.state = OSMOLATION_ON;
   }
   else {
      osmolation.state = OSMOLATION_ALARM;
      flags_set(0,1);
   }
}


void osmolation_force(int8_t onoff)
{
   if(onoff==0) {
      esp_timer_stop(osmolation_timer);
      osmolation.state = OSMOLATION_ON;
   }
   else {
      osmolation.state = OSMOLATION_FORCE;
   }
}


static void osmolation_task(void *pvParameters)
{
   int8_t level_value = -1;
   int8_t reserve_low_value = -1;
   int8_t reserve_high_value = -1;

   esp_timer_create(&osmolation_timer_args, &osmolation_timer);

   is_init=1;

   while(1) {

      level_value = contacts_get(osmolation.level_contact);
      reserve_low_value = contacts_get(osmolation.reserve_low_contact);
      reserve_high_value = contacts_get(osmolation.reserve_high_contact);

      if(osmolation.state == OSMOLATION_ERR) {
         esp_timer_stop(osmolation_timer);
         relays_set(osmolation.tank_relay, 0);
         relays_set(osmolation.reserve_relay, 0);
         flags_set(0,1);
         osmolation.state=OSMOLATION_ALARM;
         continue;
      }

      if(osmolation.state == OSMOLATION_FORCE) {
         esp_timer_stop(osmolation_timer);
         esp_timer_start_once(osmolation_timer, MAX_ON_DELAY_S * 1000000);
         osmolation.state = OSMOLATION_FORCED;
         flags_set(0,0);
         continue;
      }

      if(osmolation.state != OSMOLATION_ALARM && reserve_low_value == 0 && reserve_high_value == 0) {
         osmolation.state=OSMOLATION_ERR;
         continue;
      }

      if(osmolation.state == OSMOLATION_ON ||
         osmolation.state == OSMOLATION_WORKING ||
         osmolation.state == OSMOLATION_FORCED) {
         if(reserve_low_value == 0) { // plus d'eau dans la reserve
            esp_timer_stop(osmolation_timer);
            relays_set(osmolation.tank_relay, 0);
            relays_set(osmolation.reserve_relay, 1);
            osmolation.state=OSMOLATION_OFF;
         }
      }

      if(reserve_high_value == 0) {
         if(osmolation.state == OSMOLATION_OFF) {
            osmolation.state=OSMOLATION_ON;
         }
         if(relays_get(osmolation.reserve_relay)==1) {
            relays_set(osmolation.reserve_relay, 0);
         }
      }

      if(osmolation.state == OSMOLATION_OFF) {
         if(reserve_low_value == 1) {
            osmolation.state=OSMOLATION_ON;
         }
      }

      if(osmolation.state==OSMOLATION_ON) {
         if(level_value == 0) {
            relays_set(osmolation.tank_relay, 1);
            esp_timer_stop(osmolation_timer);
            esp_timer_start_once(osmolation_timer, MAX_ON_DELAY_S * 1000000);
            osmolation.state=OSMOLATION_WORKING;
         }
      }

      if(osmolation.state==OSMOLATION_WORKING) {
         if(level_value == 1) {
            esp_timer_stop(osmolation_timer);
            osmolation.state=OSMOLATION_ON;
            relays_set(osmolation.tank_relay, 0);
         }
      }
      vTaskDelay(LOOP_DELAY_MS / portTICK_PERIOD_MS);      
   }
}


void osmolation_init(int8_t tank_relay, int8_t reserve_relay, int8_t level_contact, int8_t reserve_low_contact, int8_t reserve_high_contact)
{
   osmolation.tank_relay = tank_relay;
   osmolation.reserve_relay = reserve_relay;
   osmolation.level_contact = level_contact;
   osmolation.reserve_low_contact = reserve_low_contact;
   osmolation.reserve_high_contact = reserve_high_contact;
   osmolation.state = OSMOLATION_ON;

   xTaskCreate(osmolation_task, "tcp_server", 4096, (void*)0, 1, NULL);
}

