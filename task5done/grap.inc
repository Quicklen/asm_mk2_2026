_get_mode proc near
	push bp
	mov bp, sp

	mov ah, 0Fh
	int 10h

	movzx ax, al

	mov sp, bp
	pop bp
	ret
_get_mode endp

_set_mode proc near
	push bp
	mov bp, sp

	mov ax, word ptr [bp + arg1]
	mov ah, 00h
	int 10h

	mov sp, bp
	pop bp
	ret
_set_mode endp

_setpixel proc near
	push bp
	mov bp, sp
	pusha

	mov cx, word ptr [bp + arg1]
	test cl, 1
	jz draw_pixel_raw_even_row
	mov bx, 2000h
	jmp short draw_pixel_raw_continue

draw_pixel_raw_even_row:
	xor bx, bx

draw_pixel_raw_continue:
	shr cx, 1
	mov al, 80
	mul cl

	mov dx, word ptr [bp + arg2]
	mov cx, dx
	not cl
	and cl, 00000011b
	shl cl, 1

	shr dx, 1
	shr dx, 1

	add ax, dx
	add bx, ax

	mov ah, es:[bx]
	ror ah, cl
	and ah, 11111100b
	mov al, byte ptr [bp + arg3]
	or ah, al
	rol ah, cl
	mov es:[bx], ah

	popa
	mov sp, bp
	pop bp
	ret
_setpixel endp

_clear_screen	proc	near
	push	bp
	mov	bp,sp
	push	si
	push	di
   ;	
   ;	    short x, y;
   ;	    for (y = 0; y < SCREEN_H; ++y) {
   ;	
	xor	di,di
	jmp	short y_loop_check
y_loop_body:
   ;	
   ;	        for (x = 0; x < SCREEN_W; ++x) {
   ;	
	xor	si,si
	jmp	short x_loop_check
x_loop_body:
   ;	
   ;	            setpixel(x, y, color);
   ;	
	push	word ptr [bp+4]
	push	di
	push	si
	call	near ptr _setpixel
	add	sp,6
	inc	si
x_loop_check:
	cmp	si,320
	jl	short x_loop_body
	inc	di
y_loop_check:
	cmp	di,240
	jl	short y_loop_body
   ;	
   ;	        }
   ;	    }
   ;	}
   ;	
	pop	di
	pop	si
	pop	bp
	ret	
_clear_screen	endp

_load_font	proc	near
	push	bp
	mov	bp,sp
	sub	sp,3046
	push	si
   ;	
   ;	{
   ;	    /* fread() always appends a null byte after the read data.
   ;	       To avoid overwriting memory, use a temporary buffer one byte larger. */
   ;	    char buffer[3041];          /* 3040 data bytes + 1 safety byte */
   ;	    short bytes_read;
   ;	    int i;
   ;	    char *dest;
   ;	
   ;	    bytes_read = fread(d, buffer, 3040);   /* 95 * 32 = 3040 */
   ;	
	push ds
	mov ax, ss
	mov ds, ax 

	mov	ax,3040
	push	ax
	lea	ax,word ptr [bp-3046]
	push	ax
	push	word ptr [bp+4]
	call	near ptr _fread
	add	sp,6
	mov	word ptr [bp-2],ax
	pop ds
   ;	
   ;	
   ;	    if (bytes_read == 3040) {
   ;	
	cmp	word ptr [bp-2],3040
	jne	short read_failed
   ;	
   ;	        dest = (char *)font16x16_basic;
   ;	
	mov	word ptr [bp-4],offset font16x16_basic
   ;	
   ;	        for (i = 0; i < 3040; i++)
   ;	
	xor	si,si
	jmp	short loop_checkk
loop_start:
   ;	
   ;	            dest[i] = buffer[i];
   ;	
	mov	al,byte ptr [bp+si-3046]
	mov	bx,word ptr [bp-4]
	mov	byte ptr [bx+si],al
	inc	si
loop_checkk:
	cmp	si,3040
	jl	short loop_start
read_failed:
   ;	
   ;	    }
   ;	    /* If read fails or the file is too short, the existing font remains unchanged. */
   ;	}
   ;	
	pop	si
	mov	sp,bp
	pop	bp
	ret	
_load_font	endp

_draw_char16	proc	near
	push	bp
	mov	bp,sp
	sub	sp,8
	push	si
	push	di
   ;	
   ;	    short row, col;
   ;	    const unsigned char* glyph;
   ;	    if (ch < 32 || ch > 126) return;               // only printable ASCII
   ;	
	cmp	byte ptr [bp+8],32
	jb	exit_func
	cmp	byte ptr [bp+8],126
	jbe	valid_char
exit_func:
	jmp	gun_done
valid_char:
   ;	
   ;	    glyph = font16x16_basic[ch - 32];              // 32 bytes per character
   ;	
	mov	al,byte ptr [bp+8]
	mov	ah,0
	mov	cl,5
	shl	ax,cl
	add   ax, offset font16x16_basic+64512
	mov	word ptr [bp-2],ax
   ;	
   ;	
   ;	    for (row = 0; row < 16; row++) {
   ;	
	xor	si,si
	jmp	short row_loop_check
row_loop_start:
   ;	
   ;	        for (col = 0; col < 16; col++) {
   ;	
	xor	di,di
	jmp	short col_loop_check
col_loop_start:
   ;	
   ;	            // 270° clockwise rotation: source row = dest col, source col = 15 - dest row
   ;	            short src_row = col;
   ;	
	mov	word ptr [bp-4],di
   ;	
   ;	            short src_col = 15 - row;
   ;	
	mov	ax,15
	sub	ax,si
	mov	word ptr [bp-6],ax
   ;	
   ;	            unsigned short rowbits = (glyph[src_row * 2] << 8) | glyph[src_row * 2 + 1];
   ;	
	mov	ax,word ptr [bp-4]
	shl	ax,1
	mov	bx,word ptr [bp-2]
	add	bx,ax
	mov	al,byte ptr [bx]
	mov	ah,0
	mov	cl,8
	shl	ax,cl
	mov	bx,word ptr [bp-4]
	shl	bx,1
	add	bx,word ptr [bp-2]
	mov	dl,byte ptr [bx+1]
	mov	dh,0
	or	ax,dx
	mov	word ptr [bp-8],ax
   ;	
   ;	            if (rowbits & (1 << src_col)) {
   ;	
	mov	ax,1
	mov	cl,byte ptr [bp-6]
	shl	ax,cl
	test	ax,word ptr [bp-8]
	je	short skip_pixel
   ;	
   ;	                setpixel(x + col, y + row, color);
   ;	
	push	word ptr [bp+10]
	mov	ax,word ptr [bp+6]
	add	ax,si
	push	ax
	mov	ax,word ptr [bp+4]
	add	ax,di
	push	ax
	call	near ptr _setpixel
	add	sp,6
skip_pixel:
	inc	di
col_loop_check:
	cmp	di,16
	jl	short col_loop_start
	inc	si
row_loop_check:
	cmp	si,16
	jl	short row_loop_start
gun_done:
   ;	
   ;	            }
   ;	        }
   ;	    }
   ;	}
   ;	
	pop	di
	pop	si
	mov	sp,bp
	pop	bp
	ret	
_draw_char16	endp

_draw_text	proc	near
	push	bp
	mov	bp,sp
	push	si
	push	di
	mov	di,word ptr [bp+6]     ; di = y
	mov	si,word ptr [bp+8]     ; si = str
	jmp	short check_char        ; jump to loop condition
draw_char_loop:
   ;	
   ;	    while (*str) 
   ;	        draw_char(x, y, (unsigned char)*str, color);
   ;	
	push	word ptr [bp+10]       ; color
	mov	al,byte ptr [si]       ; *str
	push	ax
	push	di                   ; y
	push	word ptr [bp+4]       ; x
	call	near ptr _draw_char16
	add	sp,8
   ;	
   ;	        y += 8;
   ;	
	add	di,16
   ;	
   ;	        str++;
   ;	
	inc	si
check_char:
	cmp	byte ptr [si],0        ; *str != 0 ?
	jne	short draw_char_loop    ; yes, continue
   ;	
   ;	    }
   ;	}
   ;	
	pop	di
	pop	si
	pop	bp
	ret	
_draw_text	endp

_fill_rect	proc	near
	push	bp
	mov	bp, sp
	push	si
	push	di

	; short x, y;
	; for (y = y1; y <= y2; y++)
	mov	di, word ptr [bp+6]      ; y = y1
	jmp	short outer_loop_check

outer_loop_body:
	; for (x = x1; x <= x2; x++)
	mov	si, word ptr [bp+4]      ; x = x1
	jmp	short inner_loop_check

inner_loop_body:
	; setpixel(x, y, color);
	mov	al, byte ptr [bp+12]
	mov	ah, 0
	push	ax
	push	di
	push	si
	call	near ptr _setpixel
	add	sp, 6
	inc	si                      ; x++

inner_loop_check:
	cmp	si, word ptr [bp+8]      ; x <= x2 ?
	jle	short inner_loop_body

	inc	di                      ; y++

outer_loop_check:
	cmp	di, word ptr [bp+10]     ; y <= y2 ?
	jle	short outer_loop_body

	pop	di
	pop	si
	pop	bp
	ret
_fill_rect	endp


_draw_ui	proc	near
	push	bp
	mov	bp,sp
	sub	sp,12
	push	si
	push	di
   ;	
   ;	{
   ;	    short rows = (BUTTONS_COUNT + max_buttons_per_line - 1) / max_buttons_per_line;
   ;	
	mov	ax,word ptr [bp+4]
	add	ax,15
	cwd	
	idiv	word ptr [bp+4]
	mov	word ptr [bp-2],ax
   ;	
   ;	    Button* btn;
   ;	    short col, row, i = 0;
   ;	
	mov	word ptr [bp-6],0
   ;	
   ;	    short x, y;
   ;	    const short offset = 80;
   ;	
	mov	word ptr [bp-12],60
   ;	
   ;	    draw_text(20, 20, "Calculator", 1);
   ;	
	mov	ax,1
	push	ax
	mov	ax,offset calc_str
	push	ax
	mov	ax,20
	push	ax
	mov	ax,20
	push	ax
	call	near ptr _draw_text
	add	sp,8
   ;	
   ;	    draw_text(40, 20, "-----------", 1);
   ;	
	mov	ax,1
	push	ax
	mov	ax,offset sep_line
	push	ax
	mov	ax,20
	push	ax
	mov	ax,40
	push	ax
	call	near ptr _draw_text
	add	sp,8
   ;	
   ;	    draw_text(80, 20, "     0     ", 1);
   ;	
	mov	ax,1
	push	ax
	mov	ax,offset _output_field
	push	ax
	mov	ax,20
	push	ax
	mov	ax,60
	push	ax
	call	near ptr _draw_text
	add	sp,8
   ;	
   ;	    
   ;	    for(row = 0; row < rows; row++)
   ;	
	xor	di,di
	jmp	short u_row_loop_check
u_row_loop_start:
   ;	
   ;	    {
   ;	        for(col = 0; col < max_buttons_per_line; col++)
   ;	
	xor	si,si
	jmp	short u_col_loop_check
u_col_loop_start:
   ;	
   ;	        {
   ;	            btn = &buttons[i];
   ;	
	mov	ax,word ptr [bp-6]
	shl	ax,1
	shl	ax,1
	shl	ax,1
	add	ax,offset _buttons
	mov	word ptr [bp-4],ax
   ;	
   ;	            x = offset + (row + 1) * 30;   // unchanged: horizontal step 30
   ;	
	mov	ax,di
	inc	ax
	mov	dx,30
	imul	dx
	mov	dx,word ptr [bp-12]
	add	dx,ax
	mov	word ptr [bp-8],dx
   ;	
   ;	            y = 20 + col * 42;             // changed: start at 20, step 42
   ;	
	mov	ax,si
	mov	dx,42
	imul	dx
	add	ax,20
	mov	word ptr [bp-10],ax
   ;	
   ;	            draw_text(x, y, btn->label, btn->color);
   ;	
	mov	bx,word ptr [bp-4]
	push	word ptr [bx+5]
	push	word ptr [bp-4]
	push	word ptr [bp-10]
	push	word ptr [bp-8]
	call	near ptr _draw_text
	add	sp,8
   ;	
   ;	            i++;
   ;	
	inc	word ptr [bp-6]
   ;	
   ;	            if(i >= BUTTONS_COUNT) break;
   ;	
	cmp	word ptr [bp-6],BUTTONS_COUNT
	jl	short continue_col
	jmp	short exit_col_loop
continue_col:
	inc	si
u_col_loop_check:
	cmp	si,word ptr [bp+4]
	jl	short u_col_loop_start
exit_col_loop:
	inc	di
u_row_loop_check:
	cmp	di,word ptr [bp-2]
	jl	short u_row_loop_start
   ;	
   ;	        }
   ;	    }
   ;	}
   ;	
	pop	di
	pop	si
	mov	sp,bp
	pop	bp
	ret	
_draw_ui	endp

_getKey2	proc	near
	push	bp
	mov	bp,sp
	sub	sp,2
   ;	
   ;	{
   ;	    unsigned short result;
   ;	    asm {
   ;	        mov ah, 1
   ;	
	mov	 ah, 1
   ;	
   ;	        int 16h
   ;	
	int	 16h
   ;	
   ;	        jz no_key
   ;	
	je	short no_key
   ;	
   ;	        mov result, ax
   ;	
	mov	 [bp-2], ax
   ;	
   ;	        jmp done
   ;	
	jmp	short a_gun_done
no_key:
   ;	
   ;	    }
   ;	
   ;	no_key:
   ;	    return 0;
   ;	
	xor	ax,ax
	jmp	short return_result
a_gun_done:
   ;	
   ;	
   ;	done:
   ;	    return result;
   ;	
	mov	ax,word ptr [bp-2]
	jmp	short return_result
return_result:
   ;	
   ;	}
   ;	
	mov	sp,bp
	pop	bp
	ret	
_getKey2	endp

_getKey	proc	near
	push	bp
	mov	bp,sp
	sub	sp,2
   ;	
   ;	{
   ;	    unsigned short result;
   ;	    asm {
   ;	        mov ah, 0
   ;	
	mov	 ah, 0
   ;	
   ;	        int 16h
   ;	
	int	 16h
   ;	
   ;	        mov result, ax
   ;	
	mov	 [bp-2], ax
   ;	
   ;	    }
   ;	    return result;
   ;	
	mov	ax,word ptr [bp-2]
	jmp	short return_result_key
return_result_key:
   ;	
   ;	}
   ;	
	mov	sp,bp
	pop	bp
	ret	
_getKey	endp

_memset	proc	near
	push	bp
	mov	bp,sp
	push	si
   ;	
   ;	{
   ;	    short i = 0;
   ;	
	xor	si,si
   ;	
   ;	    for(i = 0; i < size; i += 1)
   ;	
	xor	si,si
	jmp	short loop_check
zaloop_start:
   ;	
   ;	    {
   ;	        addr[i] = b;
   ;	
	mov	bx,word ptr [bp+4]
	mov	al,byte ptr [bp+8]
	mov	byte ptr [bx+si],al
	inc	si
loop_check:
	cmp	si,word ptr [bp+6]
	jl	short zaloop_start
   ;	
   ;	    }
   ;	}
   ;	
	pop	si
	pop	bp
	ret	
_memset	endp


_eval_expression proc near
    push bp
    mov  bp, sp
    sub  sp, 10
    pushregs si di bx cx dx

    ; локальные переменные:
    ; [bp-2]  – левый операнд  (word)
    ; [bp-4]  – правый операнд (word)
    ; [bp-6]  – операция (байт)
    ; [bp-8]  – временная переменная
    ; [bp-10] – индекс в строке

    ; --- 1. Пропустить ведущие пробелы ---
    xor  si, si                   ; si = индекс в _output_field
    jmp  @skip_spaces
@space_loop:
    inc  si
@skip_spaces:
    mov  al, byte ptr _output_field[si]
    cmp  al, ' '
    je   @space_loop

    ; --- 2. Проверить, не пустая ли строка ---
    cmp  al, 0
    je   @error_empty

    ; --- 3. Считать левый операнд (десятичный) ---
    xor  ax, ax
    mov  word ptr [bp-2], 0       ; левый = 0
    mov  di, 0                    ; di = 1 если был минус
    cmp  al, '-'
    jne  @not_neg_left
    mov  di, 1
    inc  si
    jmp  @left_digit
@not_neg_left:
    cmp  al, '+'
    jne  @left_digit
    inc  si
@left_digit:
    mov  al, byte ptr _output_field[si]
    cmp  al, '0'
    jb   @left_done
    cmp  al, '9'
    ja   @left_done
    sub  al, '0'
    mov  ah, 0
    push ax
    mov  ax, word ptr [bp-2]
    mov  cx, 10
    mul  cx                      ; ax = левый * 10
    jc   @overflow               ; переполнение при умножении
    pop  cx                      ; cx = цифра
    add  ax, cx
    jc   @overflow
    cmp  ax, 32767               ; проверка макс. положительного
    ja   @overflow
    mov  word ptr [bp-2], ax
    inc  si
    jmp  @left_digit
@left_done:
    cmp  di, 1
    jne  @no_neg_left
    neg  word ptr [bp-2]         ; применяем минус
@no_neg_left:

    ; --- 4. Оператор (один символ) ---
    mov  al, byte ptr _output_field[si]
    cmp  al, 0
    je   @error_empty
    mov  byte ptr [bp-6], al
    inc  si

    ; --- 5. Правый операнд ---
    xor  ax, ax
    mov  word ptr [bp-4], 0
    mov  di, 0
    mov  al, byte ptr _output_field[si]
    cmp  al, '-'
    jne  @not_neg_right
    mov  di, 1
    inc  si
    jmp  @right_digit
@not_neg_right:
    cmp  al, '+'
    jne  @right_digit
    inc  si
@right_digit:
    mov  al, byte ptr _output_field[si]
    cmp  al, '0'
    jb   @right_done
    cmp  al, '9'
    ja   @right_done
    sub  al, '0'
    mov  ah, 0
    push ax
    mov  ax, word ptr [bp-4]
    mov  cx, 10
    mul  cx
    jc   @overflow
    pop  cx
    add  ax, cx
    jc   @overflow
    cmp  ax, 32767
    ja   @overflow
    mov  word ptr [bp-4], ax
    inc  si
    jmp  @right_digit
@right_done:
    cmp  di, 1
    jne  @no_neg_right2
    neg  word ptr [bp-4]
@no_neg_right2:

    ; --- 6. Выполнить операцию ---
    mov  al, byte ptr [bp-6]
    cmp  al, '+'
    je   @do_add
    cmp  al, '-'
    je   @do_sub
    cmp  al, '*'
    je   @do_mul
    cmp  al, '/'
    je   @do_div
    cmp  al, '%'
    je   @do_mod
    jmp  @error_unknown_op

@do_add:
    mov  ax, word ptr [bp-2]
    add  ax, word ptr [bp-4]
    jo   @overflow
    jmp  @write_result

@do_sub:
    mov  ax, word ptr [bp-2]
    sub  ax, word ptr [bp-4]
    jo   @overflow
    jmp  @write_result

@do_mul:
    mov  ax, word ptr [bp-2]
    imul word ptr [bp-4]
    jo   @overflow                ; переполнение при IMUL (если OF=1)
    jmp  @write_result

@do_div:
    cmp  word ptr [bp-4], 0
    je   @error_div_zero
    mov  ax, word ptr [bp-2]
    cwd
    idiv word ptr [bp-4]
    jmp  @write_result

@do_mod:
    cmp  word ptr [bp-4], 0
    je   @error_div_zero
    mov  ax, word ptr [bp-2]
    cwd
    idiv word ptr [bp-4]
    mov  ax, dx                  ; остаток
    jmp  @write_result

@write_result:
    ; Преобразовать AX в строку и записать в _output_field
    push ax
    push offset _output_field
    call _itoa                   ; напишем эту функцию ниже
    add  sp, 4
    mov  word ptr _output_field_i, cx  ; длина строки
    jmp  @exit_ok

@overflow:
    mov  ax, offset err_overflow
    push ax
    push offset _output_field
    call strcpy                  ; скопировать строку ошибки
    add  sp, 4
    mov  word ptr _output_field_i, 0
    jmp  @exit_ok

@error_empty:
@error_div_zero:
    mov  ax, offset err_div_zero
    push ax
    push offset _output_field
    call strcpy
    add  sp, 4
    mov  word ptr _output_field_i, 0
    jmp  @exit_ok

@error_unknown_op:
    mov  ax, offset err_unknown_op
    push ax
    push offset _output_field
    call strcpy
    add  sp, 4
    mov  word ptr _output_field_i, 0

@exit_ok:
	mov  byte ptr _need_refresh, 1
    popregs  dx cx bx di si
    mov  sp, bp
    pop  bp
    ret
_eval_expression endp

_maing	proc	near
	push	bp
	mov	bp,sp
	sub	sp,4
	push	si
   ;	
   ;	{
   ;	    short i;
   ;	    unsigned short key;
   ;	    unsigned char current_char = ' '; 
   ;	
	mov	byte ptr [bp-3],32
@7@58:
   ;	
   ;	
   ;	    while(1)
   ;	    {
   ;	
   ;	        if(need_refresh)
   ;	
	cmp	byte ptr _need_refresh,0
	je	short @7@114
   ;	
   ;	        {
   ;	            draw_ui(4);
   ;	
	mov	ax,4
	push	ax
	call	near ptr _draw_ui
	pop	cx
   ;	
   ;	            need_refresh = 0; 
   ;	
	mov	byte ptr _need_refresh,0
@7@114:
   ;	
   ;	        }
   ;	
   ;	       if(blink_counter > 0)
   ;	
	cmp	byte ptr _blink_counter,0
	jbe	short @7@226
   ;	
   ;	        {
   ;	            blink_counter--;
   ;	
	dec	byte ptr _blink_counter
   ;	
   ;	            if(blink_counter == 0 && blink_index != -1)
   ;	
	cmp	byte ptr _blink_counter,0
	jne	short @7@226
	cmp	word ptr _blink_index,65535
	je	short @7@226
   ;	
   ;	            {
   ;	                /* restore original color */
   ;	                buttons[blink_index].color = blink_orig_color;
   ;	
	mov	al,byte ptr _blink_orig_color
	mov	ah,0
	mov	bx,word ptr _blink_index
	shl	bx,1
	shl	bx,1
	shl	bx,1
	mov	word ptr _buttons[bx+5],ax
   ;	
   ;	                need_refresh = 1;
   ;	
	mov	byte ptr _need_refresh,1
   ;	
   ;	                blink_index = -1;
   ;	
	mov	word ptr _blink_index,65535
@7@226:
   ;	
   ;	            }
   ;	        }
   ;	
   ;	        key = getKey2();
   ;	
	call	near ptr _getKey2
	mov	word ptr [bp-2],ax
   ;	
   ;	
   ;	        if (key != 0) {                 
   ;	
	cmp	word ptr [bp-2],0
	jne	@@5
	jmp	@7@646
@@5:
   ;	
   ;	            getKey();
   ;	
	call	near ptr _getKey
   ;	
   ;	
   ;	            if ((key >> 8) == 1)
   ;	
	mov	ax,word ptr [bp-2]
	mov	cl,8
	shr	ax,cl
	cmp	ax,1
	jne	short @7@310
   ;	
   ;	                break;
   ;	
	jmp	@7@674
@7@310:
            ; если нажата Escape (scan code 1) - выход
            mov  ax, word ptr [bp-2]
            mov  cl, 8
            shr  ax, cl
            cmp  ax, 1
            jne  @check_char
            jmp  @7@674            ; выход из программы

@check_char:
            ; если нет ASCII-кода - игнорируем
            test word ptr [bp-2], 255
            jz   @7@646

            ; текущий символ
            mov  al, byte ptr [bp-2]
            and  al, 255
            mov  byte ptr [bp-3], al

            ; --- обработка клавиши 'c' (очистка) ---
            cmp  byte ptr [bp-3], 99   ; 'c'
            jne  @not_clear_key

            ; очищаем поле
            push 0
            push 200
            push offset _output_field
            call _memset
            add  sp, 6
            mov  word ptr _output_field_i, 0
            jmp  @after_key_action

@not_clear_key:
            ; --- обработка клавиши '=' (вычисление) ---
            cmp  byte ptr [bp-3], '='
            jne  @not_equal_key

            call _eval_expression
            ; обновить длину строки результата
            push offset _output_field
            call _strlen
            add  sp, 2
            mov  word ptr _output_field_i, ax
            jmp  @after_key_action

@not_equal_key:
            ; --- обычный символ ---
            mov  ax, word ptr _output_field_i
            inc  ax
            cmp  ax, 200
            jge  @after_key_action
            mov  bx, word ptr _output_field_i
            mov  al, byte ptr [bp-3]
            mov  byte ptr _output_field[bx], al
            inc  word ptr _output_field_i
            mov  bx, word ptr _output_field_i
            mov  byte ptr _output_field[bx], 0

@after_key_action:
            ; --- ищем кнопку с таким символом и подсвечиваем ---
            mov  si, 0
@find_button_loop:
            cmp  si, BUTTONS_COUNT
            jge  @no_button_found
            mov  bx, si
            shl  bx, 1
            shl  bx, 1
            shl  bx, 1
            mov  al, byte ptr _buttons[bx+7]   ; code
            cmp  al, byte ptr [bp-3]
            je   @button_found
            inc  si
            jmp  @find_button_loop

@button_found:
            ; сброс предыдущей подсветки
            cmp  word ptr _blink_index, -1
            je   @2skip_restore
            mov  al, byte ptr _blink_orig_color
            mov  ah, 0
            mov  bx, word ptr _blink_index
            shl  bx, 1
            shl  bx, 1
            shl  bx, 1
            mov  word ptr _buttons[bx+5], ax
@2skip_restore:
            ; запоминаем текущую
            mov  word ptr _blink_index, si
            mov  bx, si
            shl  bx, 1
            shl  bx, 1
            shl  bx, 1
            mov  al, byte ptr _buttons[bx+5]
            mov  byte ptr _blink_orig_color, al
            mov  word ptr _buttons[bx+5], BLINK_COLOR
            mov  byte ptr _blink_counter, BLINK_DURATION

            ; перерисовка
            push 0
            call _clear_screen
            add  sp, 2
            mov  byte ptr _need_refresh, 1

@no_button_found:
@7@646:
            jmp  @7@58             ; возврат в главный цикл
@7@674:
   ;	
   ;	                    }
   ;	                }
   ;	            }
   ;	        }
   ;	
   ;	    }
   ;	}
   ;	
	pop	si
	mov	sp,bp
	pop	bp
	ret
_maing	endp