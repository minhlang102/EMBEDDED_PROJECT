#ifndef GLOBAL_HEADER
#define GLOBAL_HEADER

#include "stdint.h"
#define maxCapacity 12
#define temp_humi_pin         13
#define light_pin             34
#define ultrasonic_trig_pin   19
#define ultrasonic_echo_pin   18
#define soil_pin              35
#define rgb_R_pin             27
#define rgb_G_pin             26
#define rgb_B_pin             25
#define led_pin               2
#define lcd_scl_pin           22
#define lcd_sda_pin           21
#define pump_pin              4
#define RX_pin                16
#define TX_pin                17

typedef enum {
  TEMP,
  HUMI,
  LIGHT,
  SOIL
} SENSOR_TYPE;

extern int temp_value;
extern int humi_value;
extern int distance_value;
extern int soil_value;
extern int light_value;

typedef struct sensor_data_t {
    uint8_t id;
    uint16_t value;
} sensor_data;

bool check_condition(int start, int end, int value);

#endif
