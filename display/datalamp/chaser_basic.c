 
#include <inttypes.h>
#include <math.h>

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/log.h"
#include "avrlaunch/rgb_led.h"

#include "datalamp/chaser_basic.h"
#include "datalamp/matrix.h"
#include "datalamp/program.h"

uint16_t last_address;

struct program_event_descriptor chaser_basic_program_get_descriptor(void) {
  return (struct program_event_descriptor) {
    .header_size = 2,
    .data_size = 0
  };
}

void chaser_basic_program_init(struct program_config config) {
}

void chaser_basic_on_event(struct program_event* event) {
  struct pixel pixel = {
    .address = last_address,
    .colour = RGB_BLACK
	};
  matrix_set_pixel(pixel);
  
  pixel.address = event->address;
  pixel.colour = RGB_SOFT_WHITE;
  matrix_set_pixel(pixel);
  
  last_address = event->address;
}

