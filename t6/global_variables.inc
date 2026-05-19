    OUTPUT_FIELD_SIZE equ 20
		_output_field db '0', 260 dup(0), 0
		_output_field_i dw 0

		orig_mode db 00h

		_blink_index dw -1
		_blink_orig_color db 0
		_blink_counter db 0
		_need_refresh db 1

		handler_ss dw 0
		handler_sp dw 0 

		color        dw 1