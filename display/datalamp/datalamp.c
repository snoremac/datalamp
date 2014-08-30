 
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
#include "datalamp/emoji_avg.h"

static uint8_t main_task_id;
static struct program_event current_program_event;

static void start(void);
static void stop(void);

static void main_task(struct task* task);
static shell_result main_shell_handler(shell_command* command);

static bool seek_event_marker(struct buffer* input_buffer);
static bool load_event(struct buffer* input_buffer, struct program_event* event);

static program_init program_init_callback;
static program_on_event program_on_event_callback;

void setup_task(struct task* task) {
	shell_register_handler("lamp", main_shell_handler);		
  
  program_init_callback = emoji_avg_program_init;
  program_on_event_callback = emoji_avg_on_event;
  
  struct program_config config = {};
  program_init_callback(config);
  
  start();
}

static void start() {
  main_task_id = scheduler_add_task(&(struct task_config){"main", TASK_FOREVER, TASK_ASAP}, main_task, NULL);
}

static void stop() {
  scheduler_remove_task(main_task_id);
}

static void main_task(struct task* task) { 
	struct buffer* uart_buffer = uart_get_buffer();
  if (!seek_event_marker(uart_buffer)) {
    return;
  }
  
	if (load_event(uart_buffer, &current_program_event)) {
	   program_on_event_callback(&current_program_event);    
  }
}

static bool seek_event_marker(struct buffer* input_buffer) {
	while (input_buffer->element_count > 0 && buffer_uint8_at(input_buffer, 0) != PROGRAM_EVENT_END_MARKER) {
		buffer_shift_uint8(input_buffer);
	}
	return buffer_uint8_at(input_buffer, 0) == PROGRAM_EVENT_END_MARKER
      && input_buffer->element_count >= PROGRAM_EVENT_SIZE;
}

static bool load_event(struct buffer* input_buffer, struct program_event* event) {
	buffer_shift_uint8(input_buffer);
	
  uint16_t address_low = (uint16_t) buffer_shift_uint8(input_buffer);
  uint16_t address_high = (uint16_t) buffer_shift_uint8(input_buffer);
  if (address_low == PROGRAM_EVENT_END_MARKER || address_high == PROGRAM_EVENT_END_MARKER) {
    return false;
  }
  event->address = address_low | (address_high << 7);
  //LOG_DEBUG("Address l/h/a: %u, %u, %u\r\n", address_low, address_high, address_low | (address_high << 7));
  
  uint8_t index = 0;
  while (buffer_uint8_at(input_buffer, 0) != PROGRAM_EVENT_END_MARKER && index < PROGRAM_EVENT_SIZE) {
    event->data[index++] = buffer_shift_uint8(input_buffer);    
  }
  
  return index > 0;
}

static shell_result main_shell_handler(shell_command* command) {
	if (command->args_count == 0) return SHELL_RESULT_FAIL;
	
	if (string_eq(command->command, "lamp")) {
		if (string_eq(command->args[0], "start")) {
      start();
		} else if (string_eq(command->args[0], "stop")) {
      stop();
		} else if (string_eq(command->args[0], "clear")) {
      matrix_clear();
		} else {
			return SHELL_RESULT_FAIL;
		}
	}
	return SHELL_RESULT_SUCCESS;
}

