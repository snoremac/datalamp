
#include <stdio.h>
#include <math.h>
#include <util/atomic.h>

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/buffer/buffer.h"
#include "avrlaunch/buffer/buffer_int.h"
#include "avrlaunch/rgb_led.h"
#include "avrlaunch/scheduler.h"
#include "avrlaunch/shell.h"
#include "avrlaunch/hal/hal_uart.h"

#include "datalamp/neopixel.h"

#define STREAM_MARKER 255
#define PIXEL_COUNT 12
#define PIXEL_BYTES_COUNT 36
#define PIXEL_RESTORE_BUFFER_COUNT PIXEL_COUNT

struct pixel {
	uint8_t address;
	struct rgb colour;
};

static struct buffer _pixel_restore_buffer;
static struct pixel _pixel_restore_data[PIXEL_RESTORE_BUFFER_COUNT];

static struct buffer display_buffer;
static uint8_t display_data[PIXEL_BYTES_COUNT];

static gpio led_gpio;
static gpio display_gpio;
static uint8_t display_task_id;

static void display_task(struct task* task);
static void set_pixel(uint8_t address, uint8_t percentage);
static void set_display_data(uint8_t base_address, struct rgb* colour_data);
static void display_write(uint8_t* data);
static struct rgb percentage_to_rgb(uint8_t percentage);
static void restore_pixel_task(struct task* task);

static shell_result main_shell_handler(shell_command* command);

void setup_task(struct task* task) {
	display_gpio = GPIO(PORTD, PIN1);
	gpio_set_output(&display_gpio);
	
	shell_register_handler("emoji", main_shell_handler);
	_pixel_restore_buffer = buffer_init(_pixel_restore_data, PIXEL_RESTORE_BUFFER_COUNT, sizeof(struct pixel));
	
	display_buffer = buffer_init(display_data, PIXEL_BYTES_COUNT, sizeof(uint8_t));
	for (int i = 0; i < PIXEL_BYTES_COUNT; i++) { uint8_t zero = 0; buffer_push_no_overflow(&display_buffer, &zero); }
		
	display_task_id = scheduler_add_task(&(struct task_config){"dis", TASK_FOREVER, TASK_ASAP}, display_task, NULL);
}

static void display_task(struct task* task) {
	struct buffer* uart_buffer = uart_get_buffer();		
	while (uart_buffer->element_count > 0 && buffer_uint8_at(uart_buffer, 0) != STREAM_MARKER) {
		buffer_shift_uint8(uart_buffer);
	}
	if (uart_buffer->element_count < 4 || buffer_uint8_at(uart_buffer, 0) != STREAM_MARKER) {
		return;
	}
	//buffer_dump_uint8(uart_buffer, shell_get_stream());

	buffer_shift_uint8(uart_buffer);
	
	uint8_t pixel_address = buffer_shift_uint8(uart_buffer);
	pixel_address |= buffer_shift_uint8(uart_buffer) << 7;
	uint8_t percentage = buffer_shift_uint8(uart_buffer);
		
	set_pixel(pixel_address, percentage);
}

static void set_pixel(uint8_t address, uint8_t percentage) {
	struct rgb colour = percentage_to_rgb(percentage);
	struct pixel pixel = (struct pixel) {
		.address = address,
		.colour = colour
	};
	//shell_printf("Set: %u, %u, %u, %u\r\n", pixel.address, pixel.colour.red, pixel.colour.green, pixel.colour.blue);

	struct pixel* buffered_pixel = (struct pixel*) buffer_push_overflow(&_pixel_restore_buffer, &pixel);
	scheduler_add_task(&(struct task_config){"pix", TASK_ONCE, 5}, restore_pixel_task, buffered_pixel);	

	set_display_data(pixel.address, &RGB_WHITE);
}

static struct rgb percentage_to_rgb(uint8_t percentage) {
	return (struct rgb) { (uint8_t) ((percentage / 100.0) * 255) , 0, 0 };
}

static void restore_pixel_task(struct task* task) {
	struct pixel* pixel = (struct pixel*) task->data;
	set_display_data(pixel->address, &pixel->colour);
}

static void set_display_data(uint8_t base_address, struct rgb* colour_data) {
	//shell_printf("Res: %u, %u, %u, %u\r\n", base_address, colour_data->red, colour_data->green, colour_data->blue);
	display_data[(base_address * 3)] = colour_data->green;
	display_data[(base_address * 3) + 1] = colour_data->red;
	display_data[(base_address * 3) + 2] = colour_data->blue;
	display_write(display_data);
}

static void display_write(uint8_t* data) {
	//buffer_dump_uint8(&display_buffer, shell_get_stream());
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		neopixel_write(display_gpio.data, _BV(display_gpio.pin), data, PIXEL_BYTES_COUNT);		
	}
}

static shell_result main_shell_handler(shell_command* command) {
	if (command->args_count == 0) return SHELL_RESULT_FAIL;
	
	if (string_eq(command->command, "emoji")) {
		if (string_eq(command->args[0], "dump")) {
		} else if (string_eq(command->args[0], "")) {
		} else {
			return SHELL_RESULT_FAIL;
		}
	}
	return SHELL_RESULT_SUCCESS;
}
