#include "global.h"

int temp_value = 0;
int humi_value = 0;
int distance_value = 0;
int soil_value = 0;
int light_value = 0;

SENSOR_TYPE sensor_type;

bool check_condition(int start, int end, int value) {
  return (value < end) && (value > start);
}