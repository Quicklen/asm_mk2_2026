; Функции для ввода-вывода строк/символов (используется соглашение cdecl)


;char* strcat (char* dst, const char* src);
strcat proc near
    push bp
    mov  bp, sp
    push si
    push di

    mov  di, word ptr [bp+arg1]   ; di = dst (destination)
    mov  si, word ptr [bp+arg2]   ; si = src (source)
    mov  ax, di                   ; save return value = dst

    ; 1. find end of dst string
strcat_find_end:
    cmp  byte ptr es:[di], 0
    je   strcat_copy
    inc  di
    jmp  strcat_find_end

    ; 2. copy src to dst (including null terminator)
strcat_copy:
    mov  cl, byte ptr [si]        ; load byte from src (ds segment)
    mov  byte ptr es:[di], cl     ; store to dst (es segment)
    test cl, cl
    jz   strcat_done
    inc  si
    inc  di
    jmp  strcat_copy

strcat_done:
    pop  di
    pop  si
    mov  sp, bp
    pop  bp
    ret
strcat endp

;char* strdup(char* string)
strdup proc near
    push bp
    mov bp, sp
    push di
    push si
    push bx
    push es

    push  word ptr [bp+arg1]
    call _strlen
    add sp, 2

    test ax, ax
    jz strdup_done

    inc ax
    push ax
    call AllocMem
    add sp, 2

    test ax, ax 
    jz strdup_failed
    mov es, ax

    mov si, word ptr [bp+arg1]
    xor di, di
strdup_loop:
    mov bl, byte ptr [si]
    mov byte ptr es:[di], bl
    inc si
    inc di
    test bl, bl
    jnz strdup_loop

    mov ax, es

strdup_done:
    pop es
    pop bx
    pop si
    pop di
    mov sp, bp
    pop bp
    ret

strdup_failed:
    xor ax, ax
    jmp strdup_done

strdup endp

;bool is_white_space(char ch)
is_white_space proc near
    push bp
    mov bp, sp
    
    mov al, byte ptr [bp+arg1]

    cmp  al, ' '
    je   is_white_space_detected
    cmp  al, 09h   ; tab
    je   is_white_space_detected
    cmp  al, 0Ah   ; line feed
    je   is_white_space_detected
    cmp  al, 0Bh   ; vertical tab
    je   is_white_space_detected
    cmp  al, 0Ch   ; form feed
    je   is_white_space_detected
    cmp  al, 0Dh   ; carriage return
    je   is_white_space_detected

    mov ax, 0
    jmp is_white_space_done

is_white_space_detected:
    mov ax, 1

is_white_space_done:
    mov sp, bp
    pop bp
    ret
is_white_space endp

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

    push bx
    call is_white_space
    add sp, 2

    test ax, ax
    jz strtos_sign

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

; ds:string1
; es:string2
; int stricmp (const char* string1, const char* string2)
stricmp proc near
    push bp
    mov  bp, sp

    mov  si, word ptr [bp+4]   ; pointer to first string
    mov  di, word ptr [bp+6]   ; pointer to second string
    xor  ax, ax

loop_stricmp:
    mov  al, byte ptr [si]     ; load current char from str1
    mov  ah, byte ptr es:[di]     ; load current char from str2

    ; convert al to uppercase if it is a lowercase letter
    cmp  al, 'a'
    jb   stricmp_skip_al
    cmp  al, 'z'
    ja   stricmp_skip_al
    sub  al, 20h               ; 'a'..'z' -> 'A'..'Z'
stricmp_skip_al:

    ; convert ah to uppercase if it is a lowercase letter
    cmp  ah, 'a'
    jb   stricmp_skip_ah
    cmp  ah, 'z'
    ja   stricmp_skip_ah
    sub  ah, 20h
stricmp_skip_ah:

    cmp  al, ah                ; compare the (possibly converted) characters
    jne  diff_stricmp

    test al, al                ; if both are NUL, strings are equal
    jz   equal_stricmp

    inc  si                    ; move to next character
    inc  di
    jmp  loop_stricmp

diff_stricmp:
    mov cx, ax

    movzx ax, ch               ; zero-extend both chars to 16 bits
    movzx bx, cl
    sub   ax, bx               ; return difference (signed)
    jmp   end_stricmp

equal_stricmp:
    xor  ax, ax

end_stricmp:
    mov  sp, bp
    pop  bp
    ret
stricmp endp

; char* strchr(char* string, int c)
strchr proc near
    push bp
    mov bp, sp
    push bx
    mov di, word ptr [bp+4]
    mov al, byte ptr [bp+6]

strchr_loop:
    mov bx, word ptr [di]
    cmp bl, al 
    je strchr_success

    test bl, bl 
    jz strchr_not_found

    inc di
    jmp strchr_loop

strchr_success:
    mov ax, di

    pop bx
    mov sp, bp
    pop bp
    ret

strchr_not_found:
    xor ax, ax

    pop bx
    mov sp, bp
    pop bp
    ret

strchr endp 

; ds:string
; es:needle
; char* strstr(char* string, char* needle)
strstr proc near
    push bp
    mov  bp, sp
    pushregs 
    push si
    push di
    push bx

    mov  di, word ptr [bp+arg2]        ; DI = string
    mov  si, word ptr [bp+arg1]        ; SI = needle
    mov  bx, di        

    cmp  byte ptr [di], 0
    je   strstr_di_zero
    cmp  byte ptr es:[si], 0 
    jne  strstr_not_empty
    mov  ax, di        
    jmp  strstr_done

strstr_not_empty:
strstr_search_loop:
    mov  di, bx          
    mov  si, word ptr [bp+arg1]      

strstr_compare:
    mov  cl, byte ptr [di]   
    mov  ch, byte ptr es:[si]     

    test ch, ch
    jz   strstr_success 

    cmp  cl, ch
    jne  strstr_next_position    

    test cl, cl         
    jz   strstr_success

    inc  di  
    inc  si 
    jmp  strstr_compare

strstr_next_position:
    inc  bx 
    cmp  byte ptr [bx], 0
    jnz  strstr_search_loop

    xor  ax, ax 
    jmp  strstr_done

strstr_success:
    mov  ax, bx  

strstr_done:
    pop  bx
    pop  di
    pop  si
    mov  sp, bp
    pop  bp
    ret

strstr_di_zero:
    mov ax, di
    jmp strstr_done
strstr endp

; ds:string1
; es:string2
; int strcmp(const char* string1, const char* string2)
strcmp proc near
    push bp
    mov bp, sp
    ; to do fix cdecl arguments
    mov si, word ptr [bp + arg1]
    mov di, word ptr [bp + arg2]
    xor ax, ax
    
cmploop:
    mov al, byte ptr es:[si]
    mov ah, byte ptr [di]
    
    cmp al, ah
    jne cmp_diff
    
    test al, al
    jz cmp_equal
    
    inc si
    inc di
    jmp cmploop
    
cmp_diff:
    mov cx, ax
    movzx ax, ch
    movzx bx, cl
    sub ax, bx
    jmp cmp_end

cmp_equal:
    xor ax, ax
    
cmp_end:
    mov sp, bp
    pop bp
    ret
strcmp endp

; es:dest
; ds:src
;char* strcpy(char* dest, const char* src)
strcpy proc near
    push bp
    mov bp, sp
    
    push di
    
    mov di, word ptr [bp + arg1]
    mov si, word ptr [bp + arg2]
    mov ax, di
    
cpyloop:
    mov dl, byte ptr [si]
    mov byte ptr es:[di], dl
    test dl, dl
    jz cpy_end
    
    inc si
    inc di
    jmp cpyloop
    
cpy_end:
    mov ax, di
    pop di
    mov sp, bp
    pop bp
    ret
strcpy endp

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

