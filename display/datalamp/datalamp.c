 
#include <inttypes.h>
#include <stdio.h>

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/buffer/buffer.h"
#include "avrlaunch/buffer/buffer_int.h"
#include "avrlaunch/scheduler.h"
#include "avrlaunch/shell.h"
#include "avrlaunch/hal/hal_uart.h"

#include "datalamp/matrix.h"
#include "datalamp/program.h"
#include "datalamp/emoji_avg.h"

//static gpio led_gpio;
static uint8_t main_task_id;

static void main_task(struct task* task);
static shell_result main_shell_handler(shell_command* command);

static program_init program_init_callback;
static program_on_event program_on_event_callback;

void setup_task(struct task* task) {
	shell_register_handler("lamp", main_shell_handler);		
	main_task_id = scheduler_add_task(&(struct task_config){"main", TASK_FOREVER, TASK_ASAP}, main_task, NULL);
  
  program_init_callback = emoji_avg_program_init;
  program_on_event_callback = emoji_avg_on_event;
  
  struct program_config config = {};
  program_init_callback(config);
}

static void main_task(struct task* task) { 
	struct buffer* uart_buffer = uart_get_buffer();		
	while (uart_buffer->element_count > 0 && buffer_uint8_at(uart_buffer, 0) != PROGRAM_EVENT_END_MARKER) {
		buffer_shift_uint8(uart_buffer);
	}
	if (uart_buffer->element_count < PROGRAM_EVENT_SIZE || buffer_uint8_at(uart_buffer, 0) != PROGRAM_EVENT_END_MARKER) {
		return;
	}
	buffer_shift_uint8(uart_buffer);
	
  struct program_event event = {
    .address = 0,
    .data = { 0 }
  };
  event.address = buffer_shift_uint8(uart_buffer);
  event.address |= buffer_shift_uint8(uart_buffer) << 7;
  
  uint8_t index = 0;
  while (buffer_uint8_at(uart_buffer, 0) != PROGRAM_EVENT_END_MARKER && index < PROGRAM_EVENT_SIZE) {
	   event.data[index++] = buffer_shift_uint8(uart_buffer);    
  }
		
	program_on_event_callback(&event);
}

static shell_result main_shell_handler(shell_command* command) {
	if (command->args_count == 0) return SHELL_RESULT_FAIL;
	
	if (string_eq(command->command, "lamp")) {
		if (string_eq(command->args[0], "clear")) {
      matrix_clear();
		} else if (string_eq(command->args[0], "")) {
		} else {
			return SHELL_RESULT_FAIL;
		}
	}
	return SHELL_RESULT_SUCCESS;
}

