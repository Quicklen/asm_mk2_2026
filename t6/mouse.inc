
_mouse_init proc near
    push bp
    mov bp, sp

    mov ax, 0         ; инициализировать мышь
	int 33h
	
	mov ax, 1         ; показать курсор мыши
	int 33h
	
    mov ax, cs
    mov es, ax
	mov ax, 000Ch     ; установить обработчик событий мыши
	mov cx, 0002h     ; событие - нажатие левой кнопки
	mov dx, offset _mouse_handler ; ES:DX - адрес обработчика
	int 33h

    mov sp, bp
    pop bp
    ret
_mouse_init endp

_mouse_handler proc far
	pusha
	pushf
	
	; ; сохранили глобально старый стек
	mov word ptr [handler_ss], ss
	mov word ptr [handler_sp], sp

	; ; настроили новый стек
	mov ax, mstack
	mov ss, ax
	mov sp, 0FFFFh

    sub dx, 70

    push dx
    push cx
    call _button_pressed
    add sp, 4

	; вернули старый стек
	mov sp, word ptr [handler_sp]
	mov ss, word ptr [handler_ss]
	
	popf
	popa
	retf

_mouse_handler endp

_button_pressed proc near
    push bp
    mov  bp, sp
    sub  sp, 18
    push si
    push di
    mov  di, word ptr [bp+4]       ; di = x (first argument)
    ; {
    ;     const short offset = 80;
    mov  word ptr [bp-2], -30      ; (Note: constant as in original)
    ;     const short max_buttons_per_line = 4;
    mov  word ptr [bp-4], BUTTONS_PER_LINE
    ;     const short btn_width = 42;   // horizontal step
    mov  word ptr [bp-6], 80
    ;     const short btn_height = 30;  // vertical step
    mov  word ptr [bp-8], 30
    ;     short i, row, col;
    ;     short btn_x, btn_y;
    ;
    ;     for (i = 0; i < BUTTONS_COUNT; i++) {
    xor  si, si                     ; i = 0
    jmp  for_loop_condition

for_loop_body:
    ;         row = i / max_buttons_per_line;
    mov  ax, si
    cwd
    idiv word ptr [bp-4]
    mov  word ptr [bp-10], ax       ; row
    ;         col = i % max_buttons_per_line;
    mov  ax, si
    cwd
    idiv word ptr [bp-4]
    mov  word ptr [bp-12], dx       ; col
    ;         btn_x = offset + (col + 1) * btn_width;
    mov  ax, word ptr [bp-12]
    inc  ax
    imul word ptr [bp-6]
    mov  dx, word ptr [bp-2]
    add  dx, ax
    mov  word ptr [bp-14], dx       ; btn_x
    ;         btn_y = 20 + row * btn_height;
    mov  ax, word ptr [bp-10]
    imul word ptr [bp-8]
    ; add  ax, 20                   ; (commented in original)
    mov  word ptr [bp-16], ax       ; btn_y
    ;         // Check if (x,y) lies inside the button rectangle
    ;         if (x >= btn_x && x <= btn_x + btn_width &&
    ;             y >= btn_y && y <= btn_y + btn_height) {
    cmp  di, word ptr [bp-14]
    jl   button_not_found
    mov  ax, word ptr [bp-14]
    add  ax, word ptr [bp-6]
    cmp  ax, di
    jl   button_not_found
    mov  ax, word ptr [bp+6]        ; y (second argument)
    cmp  ax, word ptr [bp-16]
    jl   button_not_found
    mov  ax, word ptr [bp-16]
    add  ax, word ptr [bp-8]
    cmp  ax, word ptr [bp+6]
    jl   button_not_found

    ;             unsigned char current_char = buttons[i].code;
    mov  bx, si
    shl  bx, 1
    shl  bx, 1
    shl  bx, 1
    mov  al, byte ptr _buttons[bx+7]
    mov  byte ptr [bp-17], al

    ;             // Restore previous blinking button if any
    ;             if (blink_index != -1) {
    cmp  word ptr _blink_index, 65535
    je   no_previous_blink
    ;                 buttons[blink_index].color = blink_orig_color;
    mov  al, byte ptr _blink_orig_color
    mov  ah, 0
    mov  bx, word ptr _blink_index
    shl  bx, 1
    shl  bx, 1
    shl  bx, 1
    mov  word ptr _buttons[bx+5], ax
    ;                 blink_index = -1;
    mov  word ptr _blink_index, 65535
no_previous_blink:

    ;             // Process button action
    ;             if (current_char == 'c') {
    cmp  byte ptr [bp-17], 'c'       ; 'c'
    jne  not_clear
    ;                 // Clear output
    ;                 memset(output_field, 200, 0);
    ; mov  al, 0
    ; push ax
    ; mov  ax, 200
    ; push ax
    ; mov  ax, offset _output_field
    ; push ax
    ; call near ptr _memset
    ; add  sp, 6

	xor ax, ax
    push OUTPUT_FIELD_SIZE
	push ax
	push offset _output_field
	call _memset
	add sp, 6

    ;                 output_field_i = 0;
    mov  word ptr _output_field_i, 0
    jmp  after_append
not_clear:

	cmp byte ptr [bp-17], '='
	jne not_solve

   ;	                            solve_expression(output_field, output_field);
   ;	
    push OUTPUT_FIELD_SIZE
    mov	ax,offset _output_field
	push	ax
	mov	ax,offset _output_field
	push	ax
	; call	near ptr _solve_expression
    add sp, 6

    mov word ptr [_output_field_i], ax

	jmp after_append

not_solve:

    ;                 // Append character to output field
    ;                 if (output_field_i + 1 < 200) {
    mov  ax, word ptr _output_field_i
    inc  ax
    cmp  ax, OUTPUT_FIELD_SIZE
    jge  after_append
    ;                     output_field[output_field_i] = current_char;
    mov  bx, word ptr _output_field_i
    mov  al, byte ptr [bp-17]
    mov  byte ptr _output_field[bx], al
    ;                     output_field_i++;
    inc  word ptr _output_field_i
    ;                     output_field[output_field_i] = '\0';
    mov  bx, word ptr _output_field_i
    mov  byte ptr _output_field[bx], 0
after_append:

    ;             // Start blinking effect on the pressed button
    ;             blink_index = i;
    mov  word ptr _blink_index, si
    ;             blink_orig_color = buttons[i].color;
    mov  bx, si
    shl  bx, 1
    shl  bx, 1
    shl  bx, 1
    mov  al, byte ptr _buttons[bx+5]
    mov  byte ptr _blink_orig_color, al
    ;             buttons[i].color = BLINK_COLOR;
    mov  bx, si
    shl  bx, 1
    shl  bx, 1
    shl  bx, 1
    mov  word ptr _buttons[bx+5], 3
    ;             blink_counter = BLINK_DURATION;
    mov  byte ptr _blink_counter, 30

    ;             // Force screen redraw
    ;             clear_screen(0);
    xor  ax, ax
    push ax
    call near ptr _clear_screen
    pop  cx
    ;             need_refresh = 1;
    mov  byte ptr _need_refresh, 1
    ;             break;  // Button found, exit loop
    jmp  for_loop_end

button_not_found:
    inc  si

for_loop_condition:
    cmp  si, BUTTONS_COUNT                     ; BUTTONS_COUNT = 16
    jl   for_loop_body

for_loop_end:
    ; }
    pop  di
    pop  si
    mov  sp, bp
    pop  bp
    ret
_button_pressed endp