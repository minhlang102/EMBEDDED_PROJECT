#ifndef TEMP_HUMI_HEADER
#define TEMP_HUMI_HEADER

#include "DHT.h"
#include "global.h"

void begin_temp_humi();

int get_temp();

int get_humi();
#include "global.h"

#endif