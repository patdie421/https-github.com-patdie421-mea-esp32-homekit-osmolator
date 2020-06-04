#ifndef __osmolation_h
#define __osmolation_h

#define OSMOLATION_OFF     0 
#define OSMOLATION_ON      1
#define OSMOLATION_ALARM   2
#define OSMOLATION_FORCED  4
#define OSMOLATION_WORKING 5
#define OSMOLATION_FORCE   6
#define OSMOLATION_ERR     7 

struct osmolation_s {
   int8_t tank_relay;
   int8_t reserve_relay;
   int8_t level_contact;
   int8_t reserve_low_contact;
   int8_t reserve_high_contact;
   int8_t state;
};

void osmolation_init(int8_t tank_relay, int8_t reserve_relai, int8_t level_contact, int8_t reserve_low_contact, int8_t reserve_high_contact);
void osmolation_force(int8_t onoff);

#endif
