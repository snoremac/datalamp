
#include <avr/io.h>

; ARGS
.equ byte_counter_low, 24
.equ byte_counter_high, 25
.equ data_low, 22
.equ data_high, 23
.equ neo_port, 20
.equ neo_pin_mask, 18

; OTHER VARIABLES
.equ neo_pin_on, 17
.equ neo_pin_off, 21
.equ current_byte, 15
.equ current_byte_pre_shift, 16
.equ bit_counter, 19

.global neopixel_write

neopixel_write:
	push r15
	push r16
	push r17

	mov ZL, neo_port
	ldi ZH, 0

	ld neo_pin_on, Z
	or neo_pin_on, neo_pin_mask
	com neo_pin_mask
	ld neo_pin_off, Z
	and neo_pin_off, neo_pin_mask

	mov XL, data_low
	mov XH, data_high

	ld current_byte, X+
	ldi bit_counter, 8

pre_write_high:
	nop
	nop

; Needs 6.4 / 12.8
; Is 6 / 12 / 13
write_high:	
	st Z, neo_pin_on	;2
	mov current_byte_pre_shift, current_byte	;1
	sbrs current_byte_pre_shift, 7	;1 no skip, 2 skip
	rjmp write_low	;2
	dec bit_counter	;1
	brne shift_byte_high_phase	;1 no branch; 2 branch
	ld current_byte, X+		;2
	ldi bit_counter, 8	;1
	sbiw byte_counter_low, 1		;2
	breq end	;1 no branch; 2 branch

; Needs 7.2 / 13.6
; Is 7 / 14 / 13
write_low:
	st Z, neo_pin_off		;2
	sbrc current_byte_pre_shift, 7	;1 no skip, 2 skip
	rjmp pre_write_high		;2
	dec bit_counter		;1
	brne shift_byte_low_phase	;1 no branch; 2 branch
	ld current_byte, X+		;2
	ldi bit_counter, 8	;1
	sbiw byte_counter_low, 1		;2
	brne write_high	;1 no branch; 2 branch
	rjmp end
	
	shift_byte_high_phase:
	lsl current_byte	;1
	nop
	nop
	rjmp write_low	;2
	
shift_byte_low_phase:
	lsl current_byte	;1
	nop
	nop
	rjmp pre_write_high	;2
	
	end:
	st Z, neo_pin_off		;2

	pop r17
	pop r16
	pop r15
	ret

