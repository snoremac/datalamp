 
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <avr/interrupt.h>

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/buffer/buffer.h"
#include "avrlaunch/buffer/buffer_int.h"
#include "avrlaunch/log.h"
#include "avrlaunch/scheduler.h"
#include "avrlaunch/shell.h"
#include "avrlaunch/hal/hal_uart.h"

#include "datalamp/matrix.h"
#include "datalamp/program.h"
#include "datalamp/chaser_basic.h"
#include "datalamp/chaser_trail.h"
#include "datalamp/emoji_avg.h"

static uint8_t main_task_id;
static struct program_event current_program_event;

static void start(void);
static void stop(void);

static void main_task(struct task* task);
static shell_result main_shell_handler(shell_command* command);

static bool seek_next_event(struct buffer* input_buffer, uint8_t event_size);
static bool load_next_event(struct buffer* input_buffer, struct program_event* event, uint8_t event_data_size);

static program_get_descriptor program_get_descriptor_callback;
static program_init program_init_callback;
static program_on_event program_on_event_callback;

void setup_task(struct task* task) {
	shell_register_handler("lamp", main_shell_handler);		
  
  // program_get_descriptor_callback = chaser_basic_program_get_descriptor;
  // program_init_callback = chaser_basic_program_init;
  // program_on_event_callback = chaser_basic_on_event;
  // program_get_descriptor_callback = chaser_trail_program_get_descriptor;
  // program_init_callback = chaser_trail_program_init;
  // program_on_event_callback = chaser_trail_on_event;
  program_get_descriptor_callback = emoji_avg_program_get_descriptor;
  program_init_callback = emoji_avg_program_init;
  program_on_event_callback = emoji_avg_on_event;
  
  start();
}

static void start() {
  matrix_init();

  struct program_config config = {};
  program_init_callback(config);
  
  main_task_id = scheduler_add_task(&(struct task_config){"main", TASK_FOREVER, TASK_ASAP}, main_task, NULL);
}

static void stop() {
  scheduler_remove_task(main_task_id);
}

static void main_task(struct task* task) { 
	struct buffer* uart_buffer = uart_get_buffer();
  struct program_event_descriptor descriptor = program_get_descriptor_callback();
  
  if (!seek_next_event(uart_buffer, descriptor.header_size + descriptor.data_size + 1)) {
    return;
  }
  
	if (load_next_event(uart_buffer, &current_program_event, descriptor.data_size)) {
    program_on_event_callback(&current_program_event);
    matrix_write();
  }
}

static bool seek_next_event(struct buffer* input_buffer, uint8_t event_size) {
	while (input_buffer->element_count > 0 && buffer_uint8_at(input_buffer, 0) != PROGRAM_EVENT_END_MARKER) {
		buffer_shift_uint8(input_buffer);
	}
	return buffer_uint8_at(input_buffer, 0) == PROGRAM_EVENT_END_MARKER
      && input_buffer->element_count >= event_size;
}

static bool load_next_event(struct buffer* input_buffer, struct program_event* event, uint8_t event_data_size) {
	buffer_shift_uint8(input_buffer);
	
  uint16_t address_low = (uint16_t) buffer_shift_uint8(input_buffer);
  uint16_t address_high = (uint16_t) buffer_shift_uint8(input_buffer);
  if (address_low == PROGRAM_EVENT_END_MARKER || address_high == PROGRAM_EVENT_END_MARKER) {
    return false;
  }
  event->address = address_low | (address_high << 7);
  LOG_DEBUG("Address l/h/a: %u, %u, %u\r\n", address_low, address_high, address_low | (address_high << 7));
  if (event->address > 255) {
    return false;
  }
  
  uint8_t index = 0;
  while (buffer_uint8_at(input_buffer, 0) != PROGRAM_EVENT_END_MARKER && index < event_data_size) {
    event->data[index++] = buffer_shift_uint8(input_buffer);    
  }
  
  return true;
}

static shell_result main_shell_handler(shell_command* command) {
	if (command->args_count == 0) return SHELL_RESULT_FAIL;
	
	if (string_eq(command->command, "lamp")) {
		if (string_eq(command->args[0], "start")) {
      start();
		} else if (string_eq(command->args[0], "stop")) {
      stop();
		} else if (string_eq(command->args[0], "clear")) {
      matrix_clear_pixels();
      matrix_write();
		} else if (string_eq(command->args[0], "reset")) {
      stop();
      matrix_clear_pixels();
      matrix_write();
      start();
		} else {
			return SHELL_RESULT_FAIL;
		}
	}
	return SHELL_RESULT_SUCCESS;
}

