 
#include <inttypes.h>
#include <math.h>

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/log.h"
#include "avrlaunch/rgb_led.h"

#include "datalamp/chaser_trail.h"
#include "datalamp/matrix.h"
#include "datalamp/program.h"

uint16_t lit_pixels;

struct program_event_descriptor chaser_trail_program_get_descriptor(void) {
  return (struct program_event_descriptor) {
    .header_size = 2,
    .data_size = 0
  };
}

void chaser_trail_program_init(struct program_config config) {
}

void chaser_trail_on_event(struct program_event* event) {
  if (lit_pixels == PIXEL_COUNT) {
    matrix_clear_pixels();
    lit_pixels = 0;
  }
  
  struct pixel pixel = {
    .address = event->address,
    .colour = RGB_SOFT_WHITE
	};
  matrix_set_pixel(pixel);
  lit_pixels++;
}

