 
#include <stdio.h>
#include <string.h>
#include <util/atomic.h>

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/buffer/buffer.h"
#include "avrlaunch/buffer/buffer_int.h"
#include "avrlaunch/log.h"
#include "avrlaunch/rgb_led.h"
#include "avrlaunch/scheduler.h"
#include "avrlaunch/shell.h"

#include "datalamp/matrix.h"
#include "datalamp/neopixel.h"
#include "datalamp/neopixel_adafruit.h"

static uint8_t _matrix_data[PIXEL_BYTES_COUNT];

static struct buffer _pixel_restore_buffer;
static struct pixel _pixel_restore_data[PIXEL_RESTORE_BUFFER_COUNT];

static gpio matrix_gpio;
static uint8_t pixel_restore_task_id;

static void pixel_restore_task(struct task* task);
static void update_matrix_data(uint16_t base_address, struct rgb* colour_data);
static void dump_matrix_data(uint8_t* matrix_data, uint16_t length, FILE* stream);

void matrix_init(void) {
	matrix_gpio = GPIO(PORTD, PIN1);
	gpio_set_output(&matrix_gpio);
	
  matrix_clear_pixels();
  matrix_write();
		
	_pixel_restore_buffer = buffer_init(_pixel_restore_data, PIXEL_RESTORE_BUFFER_COUNT, sizeof(struct pixel));
	pixel_restore_task_id = scheduler_add_task(&(struct task_config){"res", TASK_FOREVER, 6}, pixel_restore_task, NULL);
}

void matrix_set_pixel(struct pixel pixel) {
  if (buffer_push_no_overflow(&_pixel_restore_buffer, &pixel) != NULL) {
	   update_matrix_data(pixel.address, &RGB_BLACK);
  } else {
	   update_matrix_data(pixel.address, &pixel.colour);    
  }
}

void matrix_clear_pixels() {
  memset(_matrix_data, 0, PIXEL_BYTES_COUNT);
}

static void update_matrix_data(uint16_t base_address, struct rgb* colour_data) {
	LOG_DEBUG("Update pixel data: %u, %u, %u, %u\r\n", base_address, colour_data->red, colour_data->green, colour_data->blue);
	_matrix_data[(base_address  * 3)] = colour_data->green;
	_matrix_data[(base_address * 3) + 1] = colour_data->red;
	_matrix_data[(base_address * 3) + 2] = colour_data->blue;
}

static void pixel_restore_task(struct task* task) {
  if (_pixel_restore_buffer.element_count > 0) {
	   struct pixel* pixel = (struct pixel*) buffer_shift(&_pixel_restore_buffer);
     update_matrix_data(pixel->address, &pixel->colour);
  }
}

void matrix_write() {
#if LOG_LEVEL >= LOG_LEVEL_DEBUG
  //dump_matrix_data(_matrix_data, PIXEL_BYTES_COUNT, shell_get_stream());
#endif
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
	   //neopixel_write(PIXEL_BYTES_COUNT, _matrix_data, matrix_gpio.data, _BV(matrix_gpio.pin));
	   neopixel_adafruit_write(PIXEL_BYTES_COUNT, _matrix_data);
  }
}

static void dump_matrix_data(uint8_t* matrix_data, uint16_t length, FILE* stream) {
  for (uint16_t i = 0; i < length; i += 3) {
    fprintf(stream, "(%02x, %02x, %02x) ", matrix_data[i], matrix_data[i + 1], matrix_data[i + 2]);
    if ((i + 3) % 24 == 0) {
      fputc('\r', stream);
      fputc('\n', stream);
    }
  }
  fputc('\r', stream);
  fputc('\n', stream);
}
