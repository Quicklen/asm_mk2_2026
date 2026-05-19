
mouse_handler proc far
	pusha
	pushf
	
	; ; сохранили глобально старый стек
	mov word ptr [handler_ss], ss
	mov word ptr [handler_sp], sp

	; ; настроили новый стек
	mov ax, mstack
	mov ss, ax
	mov sp, 0FFFFh

    ; nop
	; int 3

    ; push cx
    ; push dx

    ; push 2
    ; push cx
    ; push dx
    ; call _setpixel
    ; add sp, 6

    ; pop dx
    ; pop cx


    sub dx, 70
    ; sub cx, 20

    push dx
    push cx
    call _ButtonPressed
    add sp, 4

    ; cx, dx - x y

	; ; mov ax, 2
	; ; int 33h		; спрятать курсор
	
	; ; mov ax, 0B800h
    ; ; mov es, ax
	

	; ; mov ax, 1   ; показать курсор мыши
	; ; int 33h
	
	; вернули старый стек

	; ; вернули старый стек
	mov sp, word ptr [handler_sp]
	mov ss, word ptr [handler_ss]
	

	popf
	popa
	retf

mouse_handler endp


_ButtonPressed  proc near
    push  bp
    mov   bp, sp
    sub   sp, 18
    push  si
    push  di

    mov   di, word ptr [bp+4]      ; x
    mov   word ptr [bp-2], 60      ; offset
    mov   word ptr [bp-4], 4       ; max per line
    mov   word ptr [bp-6], 30      ; width
    mov   word ptr [bp-8], 42      ; height

    xor   si, si                   ; i = 0
@loop:
    cmp   si, BUTTONS_COUNT
    jge   @done

    ; row = i / 4, col = i % 4
    mov   ax, si
    cwd
    idiv  word ptr [bp-4]
    mov   word ptr [bp-10], ax     ; row
    mov   word ptr [bp-12], dx     ; col

    ; btn_x = offset + (row+1)*width
    mov   ax, word ptr [bp-10]
    inc   ax
    imul  word ptr [bp-6]
    add   ax, word ptr [bp-2]
    mov   word ptr [bp-14], ax

    ; btn_y = 20 + col*height
    mov   ax, word ptr [bp-12]
    imul  word ptr [bp-8]
    add   ax, 20
    mov   word ptr [bp-16], ax

    ; проверка прямоугольника
    cmp   di, word ptr [bp-14]
    jl    @next
    mov   ax, word ptr [bp-14]
    add   ax, word ptr [bp-6]
    cmp   di, ax
    jg    @next

    mov   ax, word ptr [bp+6]      ; y
    cmp   ax, word ptr [bp-16]
    jl    @next
    mov   bx, word ptr [bp-16]
    add   bx, word ptr [bp-8]
    cmp   ax, bx
    jg    @next

    ; ---------- попадание ----------
    mov   bx, si
    shl   bx, 1
    shl   bx, 1
    shl   bx, 1                   ; bx = смещение кнопки
    mov   al, byte ptr _buttons[bx+7]  ; code
    mov   byte ptr [bp-17], al

    ; если была подсветка – сбросить
    cmp   word ptr _blink_index, -1
    je    @skip_restore
    mov   al, byte ptr _blink_orig_color
    mov   ah, 0
    mov   bx, word ptr _blink_index
    shl   bx, 1
    shl   bx, 1
    shl   bx, 1
    mov   word ptr _buttons[bx+5], ax
    mov   word ptr _blink_index, -1
@skip_restore:

    ; обработка символа
    cmp   byte ptr [bp-17], 'c'
    jne   @not_clear
    push  0
    push  200
    push  offset _output_field
    call  _memset
    add   sp, 6
    mov   word ptr _output_field_i, 0
    jmp   @after_char
@not_clear:
    mov   ax, word ptr _output_field_i
    inc   ax
    cmp   ax, 200
    jge   @after_char
    mov   bx, word ptr _output_field_i
    mov   al, byte ptr [bp-17]
    mov   byte ptr _output_field[bx], al
    inc   word ptr _output_field_i
    mov   bx, word ptr _output_field_i
    mov   byte ptr _output_field[bx], 0
@after_char:

    ; запуск мигания
    mov   word ptr _blink_index, si
    mov   bx, si
    shl   bx, 1
    shl   bx, 1
    shl   bx, 1
    mov   al, byte ptr _buttons[bx+5]
    mov   byte ptr _blink_orig_color, al
    mov   word ptr _buttons[bx+5], BLINK_COLOR  ; <-- использовать константу
    mov   byte ptr _blink_counter, BLINK_DURATION
    push  0
    call  _clear_screen
    pop   cx
    mov   byte ptr _need_refresh, 1
    jmp   @done

@next:
    inc   si
    jmp   @loop

@done:
    pop   di
    pop   si
    mov   sp, bp
    pop   bp
    ret
_ButtonPressed  endp
