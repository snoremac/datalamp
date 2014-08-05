
#include <avr/io.h>

; ARGS
.equ neo_port, 24
.equ neo_pin_mask, 22
.equ data_low, 20
.equ data_high, 21
.equ byte_counter, 18

; OTHER VARIABLES
.equ zero, 0
.equ tmp, 23
.equ neo_pin_on, 17
.equ neo_pin_off, 19
.equ current_byte, 15
.equ current_byte_pre_shift, 16
.equ bit_counter, 25

.global neopixel_write

neopixel_write:
	in tmp, _SFR_IO_ADDR(SREG)
	cli
	push r15
	push r16
	push r17

	mov ZL, neo_port
	ldi ZH, zero

	ld neo_pin_on, Z
	or neo_pin_on, neo_pin_mask
	com neo_pin_mask
	ld neo_pin_off, Z
	and neo_pin_off, neo_pin_mask

	mov XL, data_low
	mov XH, data_high

	ld current_byte, X+
	ldi bit_counter, 8

write_high:
	st Z, neo_pin_on
	mov current_byte_pre_shift, current_byte
	sbrs current_byte_pre_shift, 7
	rjmp write_low
	dec bit_counter
	brne shift_byte_high_phase
	ld current_byte, X+
	ldi bit_counter, 8
	dec byte_counter
	rjmp write_low

shift_byte_high_phase:
	lsl current_byte
	nop
	nop
	nop

write_low:
	st Z, neo_pin_off
	sbrc current_byte_pre_shift, 7
	rjmp next_bit
	dec bit_counter
	brne shift_byte_low_phase
	ld current_byte, X+
	ldi bit_counter, 8
	dec byte_counter
	rjmp next_bit

shift_byte_low_phase:
	lsl current_byte
	nop
	nop

next_bit:
	cpse byte_counter, zero
	rjmp write_high

end:
	pop r17
	pop r16
	pop r15
	out _SFR_IO_ADDR(SREG), tmp
	ret
