; Функции для ввода-вывода строк/символов (используется соглашение cdecl)

; short strlen(const char *str)
; находит длинну строки (до завершающего нуля), адрес которой является аргументом
_strlen proc near
    push bp
    mov bp, sp
    pushregs cx, di
    
    mov di, word ptr [bp + 4] 
    xor ax, ax ; счётчик (ax)

lencyc:    
    mov cl, byte ptr [di]
    test cl, cl
    jz lenret
    inc ax
    inc di
    jmp lencyc
    
lenret:    
    popregs di, cx
    mov sp, bp
    pop bp
    ret
_strlen endp

; short strtos (const char* string, char** end_ptr, short base)
_strtos proc near
    push bp
    mov  bp, sp
    sub  sp, 10          ; локальные переменные: [bp-2] знак, [bp-4] начало цифр,
                         ; [bp-6] текущее значение, [bp-8] флаг переполнения, [bp-10] основание
    pushregs di, si, cx, bx, dx

    mov word ptr [bp-2], 1     ; sign = 1 (положительное)
    mov word ptr [bp-6], 0     ; value = 0
    mov byte ptr [bp-8], 0     ; overflow_flag = 0

    mov di, word ptr [bp+arg1] ; string
    mov si, word ptr [bp+arg2] ; end_ptr
    mov cx, word ptr [bp+arg3] ; base
    mov [bp-10], cx            ; сохраняем base

strtos_white_space_skip_loop:
    mov bl, byte ptr [di]
    xor bh, bh
    
    test bl, bl
    jz strtos_empty_string

    ; inline is_white_space check
    cmp  bl, ' '
    je   strtos_white_space_detected
    cmp  bl, 09h   ; tab
    je   strtos_white_space_detected
    cmp  bl, 0Ah   ; line feed
    je   strtos_white_space_detected
    cmp  bl, 0Bh   ; vertical tab
    je   strtos_white_space_detected
    cmp  bl, 0Ch   ; form feed
    je   strtos_white_space_detected
    cmp  bl, 0Dh   ; carriage return
    je   strtos_white_space_detected
    jmp  strtos_sign

strtos_white_space_detected:
    inc di
    jmp strtos_white_space_skip_loop

strtos_sign:
    mov bl, byte ptr [di]

    cmp bl, '-'
    je strtos_negative
    cmp bl, '+'
    je strtos_positive

strtos_sign_end:
    mov cx, [bp-10]            ; восстанавливаем base
    test cx, cx
    jz strtos_base_zero

    cmp cx, 16
    jz strtos_base_16

strtos_base_check_cnd_end:
    cmp cx, 2
    jb strtos_invalid_base

    ; digit_start
    mov word ptr [bp-4], di    ; сохраняем начало цифр

strtos_convert:
    mov bl, byte ptr [di]
    test bl, bl
    jz strtos_not_digit

    ; проверка '0'..'9'
    cmp bl, '0'
    jb  check_uppercase
    cmp bl, '9'
    ja  check_uppercase
    sub bl, '0'
    jmp digit_ok

check_uppercase:
    cmp bl, 'A'
    jb  check_lowercase
    cmp bl, 'Z'
    ja  check_lowercase
    sub bl, 'A'
    add bl, 10
    jmp digit_ok

check_lowercase:
    cmp bl, 'a'
    jb  not_digit
    cmp bl, 'z'
    ja  not_digit
    sub bl, 'a'
    add bl, 10

digit_ok:
    ; bl = цифра (0..15)
    cmp bl, cl                 ; сравнить с основанием
    jae strtos_invalid_digit_for_base

    ; проверяем флаг переполнения
    cmp byte ptr [bp-8], 0
    jne skip_accumulate

    ; аккумуляция: value = value * base + digit
    mov ax, [bp-6]             ; текущее значение
    mul cx                     ; dx:ax = ax * base
    test dx, dx
    jnz overflow_set
    add ax, bx                 ; ax = ax + digit
    jc overflow_set

    ; проверка предела в зависимости от знака
    cmp word ptr [bp-2], -1    ; отрицательное?
    je neg_limit_check
    cmp ax, 32767
    ja overflow_set
    jmp store_cur

neg_limit_check:
    cmp ax, 32768
    ja overflow_set

store_cur:
    mov [bp-6], ax
    jmp next_digit

overflow_set:
    mov byte ptr [bp-8], 1     ; устанавливаем флаг переполнения
    ; значение не обновляем, но продолжаем пропускать цифры

skip_accumulate:
next_digit:
    inc di
    jmp strtos_convert

not_digit:
    ; проверяем, была ли хотя бы одна цифра
    mov ax, [bp-4]
    cmp di, ax
    je strtos_no_digit

    ; применяем знак и переполнение
    cmp byte ptr [bp-8], 0
    jne overflow_result

    mov ax, [bp-6]
    cmp word ptr [bp-2], -1
    jne positive_result
    neg ax
positive_result:
    ; установка end_ptr
    mov bx, [bp+arg2]
    mov [bx], di
    jmp strtos_done

overflow_result:
    cmp word ptr [bp-2], -1
    je negative_overflow
    mov ax, 32767
    jmp set_endptr
negative_overflow:
    mov ax, -32768
set_endptr:
    mov bx, [bp+arg2]
    mov [bx], di
    jmp strtos_done

strtos_no_digit:
    xor ax, ax
    mov bx, [bp+arg2]
    mov cx, [bp+arg1]
    mov [bx], cx
    jmp strtos_done

strtos_invalid_digit_for_base:
    ; недопустимая цифра для данного основания – останавливаемся
    mov ax, word ptr [bp-6]
    mov word ptr [si], di
    jmp strtos_done

strtos_invalid_base:
strtos_empty_string:
    xor ax, ax
    mov di, word ptr [bp+arg1]
    mov word ptr [si], di
    jmp strtos_done

strtos_negative:
    inc di
    mov word ptr [bp-2], -1 
    jmp strtos_sign_end

strtos_positive:
    inc di
    jmp strtos_sign_end

strtos_base_zero:
    mov bl, byte ptr [di]
    test bl, bl
    jz strtos_empty_string
    cmp bl, '0'
    jne strtos_base_zero_skip
    inc di 

    mov bl, byte ptr [di]
    test bl, bl
    jz strtos_base_zero_ret
    cmp bl, 'x'
    je strtos_base_zero_16
    
    mov cx, 8
    mov [bp-10], cx
    jmp strtos_base_check_cnd_end

strtos_base_zero_skip:
    mov cx, 10
    mov [bp-10], cx
    jmp strtos_base_check_cnd_end

strtos_base_zero_16:
    inc di 
    mov cx, 16
    mov [bp-10], cx
    jmp strtos_base_check_cnd_end

strtos_base_zero_ret:
    dec di
    mov cx, 10
    mov [bp-10], cx
    jmp strtos_base_check_cnd_end

strtos_base_16:
    cmp byte ptr [di], '0'
    jne strtos_base_16_skip
    cmp byte ptr [di+1], 'x'
    je strtos_base_16_skip_prefix

strtos_base_16_skip:
    jmp strtos_base_check_cnd_end

strtos_base_16_skip_prefix:
    add di, 2
    jmp strtos_base_check_cnd_end

strtos_not_digit:
    jmp not_digit

strtos_done:
    popregs dx, bx, cx, si, di
    mov  sp, bp
    pop  bp
    ret

_strtos endp

_stoa	proc	near
	push	bp
	mov	bp,sp
	sub	sp,14
	push	si
	push	di
   ;	
   ;	    int num = value;          /* promote to int to safely handle -32768 */
   ;	
	mov	di,word ptr [bp+4]
   ;	
   ;	    int negative = 0;
   ;	
	mov	word ptr [bp-2],0
   ;	
   ;	    char temp[12];          
   ;	    int idx = 0;
   ;	
	xor	si,si
   ;	
   ;	    int i;
   ;	
   ;	    if (num < 0) {
   ;	
	or	di,di
	jge	short _stoa_convert_loop_start
   ;	
   ;	        negative = 1;
   ;	
	mov	word ptr [bp-2],1
   ;	
   ;	        num = -num;           /* now num is positive (32768 fits in int) */
   ;	
	mov	ax,di
	neg	ax
	mov	di,ax
_stoa_convert_loop_start:
   ;	
   ;	    }
   ;	
   ;	    do {
   ;	        temp[idx++] = (char)('0' + (num % 10));
   ;	
	mov	ax,di
	mov	bx,10
	cwd	
	idiv	bx
	add	dl,48
	mov	byte ptr [bp+si-14],dl
	inc	si
   ;	
   ;	        num /= 10;
   ;	
	mov	bx,10
	mov	ax,di
	cwd	
	idiv	bx
	mov	di,ax
   ;	
   ;	    } while (num > 0);
   ;	
	or	di,di
	jg	short _stoa_convert_loop_start
   ;	
   ;	
   ;	    if (negative) {
   ;	
	cmp	word ptr [bp-2],0
	je	short _stoa_after_negative
   ;	
   ;	        temp[idx++] = '-';
   ;	
	mov	byte ptr [bp+si-14],45
	inc	si
_stoa_after_negative:
   ;	
   ;	    }
   ;	
   ;	    i = 0;
   ;	
	xor	cx,cx
	jmp	short _stoa_reverse_test
_stoa_reverse_body:
   ;	
   ;	    while (idx > 0) {
   ;	        buffer[i++] = temp[--idx];
   ;	
	dec	si
	mov	al,byte ptr [bp+si-14]
	mov	bx,word ptr [bp+6]
	add	bx,cx
	mov	byte ptr [bx],al
	inc	cx
_stoa_reverse_test:
	or	si,si
	jg	short _stoa_reverse_body
   ;	
   ;	    }
   ;	    buffer[i] = '\0';
   ;	
	mov	bx,word ptr [bp+6]
	add	bx,cx
	mov	byte ptr [bx],0
   ;	
   ;	
   ;	    return buffer;
   ;	
	mov	ax,word ptr [bp+6]
	jmp	short _stoa_return
_stoa_return:
   ;	
   ;	}
   ;	
	pop	di
	pop	si
	mov	sp,bp
	pop	bp
	ret	
_stoa	endp