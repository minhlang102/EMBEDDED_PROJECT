#include "global.h"

volatile int temp_value = 0;
volatile int distance_value = 0;
volatile int humi_value = 0;
volatile int light_value = 0;

SENSOR_TYPE sensor_type;

long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}