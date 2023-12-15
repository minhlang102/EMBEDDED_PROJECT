#include "light.h"

int get_light() {
  return map(analogRead(light_pin), 0, 4049, 50, 500);
}