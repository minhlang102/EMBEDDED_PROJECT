#include "soil.h"

int get_moisture_soil() {
  return map(analogRead(soil_pin), 0, 4049, 50, 100);
}