 
#include <inttypes.h>
#include <math.h>

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/log.h"
#include "avrlaunch/rgb_led.h"

#include "datalamp/emoji_avg.h"
#include "datalamp/matrix.h"
#include "datalamp/program.h"

struct program_event_descriptor emoji_avg_program_get_descriptor(void) {
  return (struct program_event_descriptor) {
    .header_size = 2,
    .data_size = 1
  };
}

void emoji_avg_program_init(struct program_config config) {
}

void emoji_avg_on_event(struct program_event* event) {
  uint8_t avg = event->data[0];
	double sensitivity = 20;

	uint16_t adjusted_avg = (uint16_t) round(avg * sensitivity);
  if (adjusted_avg > 255) {
    adjusted_avg = 255;
  }
  //LOG_DEBUG("Avg: %u, %u, %u\r\n", avg, (uint8_t) sensitivity, adjusted_avg);

	uint8_t red = 0;
	uint8_t green = 0;
	
	if (adjusted_avg < 127) {
		green = 255;
		red = 255 - ((128 - adjusted_avg) * 2)		;
	} else {
		green = 255 - ((adjusted_avg - 128) * 2);
		red = 255;
	}

	green = (uint8_t) round((adjusted_avg / 255.0) * green);
	red = (uint8_t) round((adjusted_avg / 255.0) * red);
  
  struct pixel pixel = {
    .address = event->address,
    .colour = {
		  .red = red,
		  .green = green,
		  .blue = 0
    }
	};

  matrix_set_pixel(pixel);
}

