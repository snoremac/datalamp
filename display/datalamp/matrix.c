 
#include <stdio.h>
#include <util/atomic.h>

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/buffer/buffer.h"
#include "avrlaunch/buffer/buffer_int.h"
#include "avrlaunch/log.h"
#include "avrlaunch/rgb_led.h"
#include "avrlaunch/scheduler.h"

#include "datalamp/matrix.h"
#include "datalamp/neopixel.h"

static struct buffer matrix_buffer;
static uint8_t matrix_data[PIXEL_BYTES_COUNT];

static struct buffer _pixel_restore_buffer;
static struct pixel _pixel_restore_data[PIXEL_RESTORE_BUFFER_COUNT];

static gpio matrix_gpio;
static uint8_t pixel_restore_task_id;

static void pixel_restore_task(struct task* task);
static void update_matrix_data(uint8_t base_address, struct rgb* colour_data);
static void write_matrix_data(uint8_t* data);

void matrix_init(void) {
	matrix_gpio = GPIO(PORTD, PIN1);
	gpio_set_output(&matrix_gpio);
	
	matrix_buffer = buffer_init(matrix_data, PIXEL_BYTES_COUNT, sizeof(uint8_t));
  matrix_clear();
		
	//_pixel_restore_buffer = buffer_init(_pixel_restore_data, PIXEL_RESTORE_BUFFER_COUNT, sizeof(struct pixel));
	//pixel_restore_task_id = scheduler_add_task(&(struct task_config){"res", TASK_FOREVER, TASK_ASAP}, pixel_restore_task, NULL);
}

void matrix_set_pixel(struct pixel pixel) {
	//LOG_WARN("Set pixel: %u, %u, %u, %u\r\n", pixel.address, pixel.colour.red, pixel.colour.green, pixel.colour.blue);
  //buffer_push_overflow(&_pixel_restore_buffer, &pixel);
	//update_matrix_data(pixel.address, &RGB_BLACK);
	update_matrix_data(pixel.address, &pixel.colour);
}

void matrix_clear() {
	for (int i = 0; i < PIXEL_BYTES_COUNT; i++) { uint8_t zero = 0; buffer_push_no_overflow(&matrix_buffer, &zero); }  
	write_matrix_data(matrix_data);
}

static void pixel_restore_task(struct task* task) {
  if (_pixel_restore_buffer.element_count > 0) {
	   struct pixel* pixel = (struct pixel*) buffer_shift(&_pixel_restore_buffer);
     update_matrix_data(pixel->address, &pixel->colour);
  }
}

static void update_matrix_data(uint8_t base_address, struct rgb* colour_data) {
	//LOG_DEBUG("Update pixel data: %u, %u, %u, %u\r\n", base_address, colour_data->red, colour_data->green, colour_data->blue);
	matrix_data[(base_address * 3)] = colour_data->green;
	matrix_data[(base_address * 3) + 1] = colour_data->red;
	matrix_data[(base_address * 3) + 2] = colour_data->blue;
	write_matrix_data(matrix_data);
}

static void write_matrix_data(uint8_t* data) {
  //buffer_dump_uint8(&matrix_buffer, shell_get_stream());
	// ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
  cli();
		neopixel_write(matrix_gpio.data, _BV(matrix_gpio.pin), data, PIXEL_BYTES_COUNT);
  sei();
  delay(0.05);
	
}
