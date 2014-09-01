#ifndef DATALAMP_MATRIX_H
#define DATALAMP_MATRIX_H

#include <inttypes.h>

#include "avrlaunch/rgb_led.h"

#define PIXEL_COUNT 256
#define PIXEL_BYTES_COUNT 768
#define PIXEL_RESTORE_BUFFER_COUNT 32

struct pixel {
	uint16_t address;
	struct rgb colour;
};

void matrix_init(void);

void matrix_set_pixel(struct pixel pixel);

void matrix_clear_pixels(void);

void matrix_write(void);

#endif