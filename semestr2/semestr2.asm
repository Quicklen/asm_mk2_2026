.386

OVERFLOW_H equ 32767
OVERFLOW_L equ 32768

ERROR_SUCCESS 			equ 0
ERROR_DIV_ZERO			equ 1
INTEGER_OVERFLOW  		equ 2
ERROR_FORMAT_MISMATCH 	equ 3
AN_UNACCEPTABLE_SIGN	equ 4

arg1 equ 4
arg2 equ 6
arg3 equ 8
arg4 equ 10

var1 equ -2
var2 equ -4
var3 equ -6
var4 equ -8

stack segment para stack
db 65530 dup(?)
stack ends

data segment para public
	str1 db 256 dup(?)
	max db 254
	len db ?
	mul_index db 0
	
	result_buf db 16 dup (?)
	result_buf_hex db 9 dup(?)
	
	buf_num1 db 10 dup(?)
	num1_len db ?
	buf_num2 db 10 dup(?)
	num2_len db ?
	
	msg_expression db "Expression: ", "$"
	msg_result db "Result: ", "$"
	str_hex_prefix db " (0x$"
	str_hex_suffix db ")$"
	str_error_mismatch db "Error: format mismatch", "$"
	str_error_div_zero db "Error: div zero", "$"
	str_error_overflow db "Error: overflow", "$"
	str_error_operation db "Error: unknow operation", "$"
	
	fst_num dw ?
	sec_num dw ?
	operation db ?
	
	result_if_mul dd ?
	result_else dw ?
	
	temp_buf db 10 dup(?)
data ends

code segment para public use16

assume cs:code,ds:data,ss:stack

_putnewline:
    push bp
    mov bp, sp
    
    mov dx, 10
    push dx
    call _putchar
    add sp, 2
    
    mov dx, 13
    push dx
    call _putchar
    add sp, 2
    
    mov sp, bp
    pop bp
    ret

_exit:
    push bp
    mov bp, sp
    
	mov al, 00h
    mov ah, 4ch
	int 21h
    
    mov sp, bp
    pop bp
    ret
	
_exit_div_zero:
    push bp
    mov bp, sp

    mov dx, offset str_error_div_zero
    mov ah, 9
    int 21h

    mov al, ERROR_DIV_ZERO
    mov ah, 4ch
    int 21h
	
	mov sp, bp
    pop bp
    ret

_exit_overflow:
    push bp
    mov bp, sp

    mov dx, offset str_error_overflow
    mov ah, 9
    int 21h

    mov al, INTEGER_OVERFLOW
    mov ah, 4ch
    int 21h
	
	mov sp, bp
    pop bp
    ret

_exit_error_mismatch:
    push bp
    mov bp, sp

    mov dx, offset str_error_mismatch
    mov ah, 9
    int 21h

    mov al, ERROR_FORMAT_MISMATCH
    mov ah, 4ch
    int 21h
	
	mov sp, bp
    pop bp
    ret

_exit_error_operation:
    push bp
    mov bp, sp

    mov dx, offset str_error_operation
    mov ah, 9
    int 21h

    mov al, AN_UNACCEPTABLE_SIGN
    mov ah, 4ch
    int 21h
	
	mov sp, bp
    pop bp
    ret
	
_exit0:
    push bp
    mov bp, sp
    
    mov dx, 0
    push dx
    call _exit
    add sp, 2
    
    mov sp, bp
    pop bp
    ret
    
_uatoi:
	push bp
	mov bp, sp
	push si
	mov si, word ptr [bp + arg1]

	mov bx, si
	cmp byte ptr [bx], '-'
	je __ua_skip_s
	cmp byte ptr [bx], '+'
	jne __ua_check_hex

__ua_skip_s:
	inc bx

__ua_check_hex:
	cmp byte ptr [bx], '0'
	jne __ua_call_dec
	mov al, byte ptr [bx + 1]
	cmp al, 'x'
	je __ua_call_hex
	cmp al, 'X'
	je __ua_call_hex

__ua_call_dec:
	push si
	call _atoi
	pop cx
	jmp __ua_done

__ua_call_hex:
	xor dx, dx
	cmp byte ptr [si], '-'
	jne __ua_h_pos
	mov dx, 1

__ua_h_pos:
	push bx
	call _ahtoi
	pop cx
	jc __ua_done
	cmp dx, 0
	jz __ua_done

	cmp ax, 8000h
	je __ua_min_int
	neg ax
	jo __ua_overflow
	clc
	jmp __ua_done

__ua_min_int:
	mov ax, 8000h
	clc
	jmp __ua_done

__ua_overflow:
	mov ax, INTEGER_OVERFLOW
	call _exit_overflow
	stc

__ua_done:
	pop si
	mov sp, bp
	pop bp
	ret

_ahtoi:
	push bp
	mov bp, sp
	push si
	push dx
	mov si, word ptr [bp + arg1]
	add si, 2
	xor ax, ax

__ah_loop:
	mov cl, byte ptr [si]
	cmp cl, 0
	jz __ah_done

	cmp ah, 0
	jne __ah_check_overflow
	jmp __ah_shift_ok

__ah_check_overflow:
	and ah, 0F0h
	cmp ah, 0
	je __ah_shift_ok
	jmp __ah_overflow

__ah_shift_ok:
	shl ax, 4
	cmp cl, '9'
	jbe __ah_dig
	and cl, 0DFh
	sub cl, 'A'-10
	jmp __ah_acc

__ah_dig:
	sub cl, '0'

__ah_acc:
	or al, cl
	inc si
	jmp __ah_loop

__ah_overflow:
	mov ax, INTEGER_OVERFLOW
	stc
	call _exit_overflow
	jmp __ah_end
	

__ah_done:
	clc

__ah_end:
	pop dx
	pop si
	mov sp, bp
	pop bp
	ret

_atoi:
	push bp
	mov bp, sp
	push si
	push bx
	mov si, word ptr [bp + arg1]
	xor ax, ax
	xor bx, bx

__at_skip:
	cmp byte ptr [si], ' '
	jne __at_sign
	inc si
	jmp __at_skip

__at_sign:
	cmp byte ptr [si], '-'
	jne __at_plus
	mov bx, 1
	inc si
	jmp __at_loop

__at_plus:
	cmp byte ptr [si], '+'
	jne __at_loop
	inc si

__at_loop:
	mov cl, byte ptr [si]
	cmp cl, '0'
	jb __at_done
	cmp cl, '9'
	ja __at_done
	sub cl, '0'

	cmp ax, 3276
	jg __at_overflow_err

	imul ax, 10
	jo __at_overflow_err

	xor ch, ch
	add ax, cx
	jo __at_overflow_err

	inc si
	jmp __at_loop

__at_done:
	cmp bx, 0
	jz __at_check_positive

	cmp ax, 8000h
	je __at_is_min_int
	neg ax
	jo __at_overflow_err
	clc
	jmp __at_end

__at_check_positive:
	cmp ax, 0
	jge __at_positive_ok
	jmp __at_overflow_err

__at_positive_ok:
	clc
	jmp __at_end

__at_is_min_int:
	mov ax, 8000h
	clc
	jmp __at_end

__at_overflow_err:
	mov ax, INTEGER_OVERFLOW
	call _exit_overflow
	stc

__at_end:
	pop bx
	pop si
	mov sp, bp
	pop bp
	ret

itoa_dec:
    push bp
    mov bp, sp

    mov di, [bp+4] 
    mov ax, [bp+6]

    cmp ax, 0
    jns itoa_positive

    mov byte ptr [di], '-'
    inc di
    neg ax

itoa_positive:
    cmp ax, 0
    jne itoa_not_zero

    mov byte ptr [di], '0'
    inc di
    mov byte ptr [di], 0
    jmp itoa_done

itoa_not_zero:
    xor cx, cx
    mov bx, 10

itoa_loop:
    xor dx, dx
    div bx
    push dx
    inc cx
    cmp ax, 0
    jnz itoa_loop

itoa_write:
    pop dx
    add dl, '0'
    mov [di], dl
    inc di
    loop itoa_write

    mov byte ptr [di], 0

itoa_done:
    mov sp, bp
    pop bp
    ret
	
itoa_hex:
    push bp
    mov bp, sp
    push di
    push ax
    push bx
    push cx
    
    mov di, [bp+4]    
    mov ax, [bp+6]    
    
    mov bl, ah
    mov bh, al
    mov al, bl
    call _byte_to_hex
    
    mov al, bh
    call _byte_to_hex
    
    mov byte ptr [di], 0
    
    pop cx
    pop bx
    pop ax
    pop di
    mov sp, bp
    pop bp
    ret

_byte_to_hex:
    push cx
    mov cl, al
    shr al, 4
    call _nibble_to_hex
    mov al, cl
    and al, 0Fh
    call _nibble_to_hex
    pop cx
    ret

_nibble_to_hex:
    cmp al, 9
    jbe _nibble_digit
    add al, 7
_nibble_digit:
    add al, '0'
    mov [di], al
    inc di
    ret
	
_check_hex_format:
    push bp
    mov bp, sp
    push si
    push bx
    
    mov si, [bp+4]
    xor bx, bx
    
    cmp byte ptr [si], '-'
    je _chf_skip_sign
    cmp byte ptr [si], '+'
    jne _chf_check_hex_start
    
_chf_skip_sign:
    inc si
    
_chf_check_hex_start:
    cmp byte ptr [si], '0'
    jne _chf_not_hex
    mov al, byte ptr [si+1]
    cmp al, 'x'
    je _chf_hex_found
    cmp al, 'X'
    je _chf_hex_found
    jmp _chf_not_hex
    
_chf_hex_found:
    add si, 2
    mov bx, si
    
    mov al, [si]
    cmp al, 0
    je _chf_error
    cmp al, ' '
    je _chf_error
    
_chf_hex_loop:
    mov al, [si]
    cmp al, 0
    je _chf_ok
    cmp al, ' '
    je _chf_ok
    
    cmp al, '0'
    jb _chf_error
    cmp al, '9'
    jbe _chf_next

    and al, 0DFh
    cmp al, 'A'
    jb _chf_error
    cmp al, 'F'
    ja _chf_error
    
_chf_next:
    inc si
    jmp _chf_hex_loop
    
_chf_not_hex:
    mov ax, 0
    clc
    jmp _chf_end
    
_chf_ok:
    mov ax, 1
    clc
    jmp _chf_end
    
_chf_error:
    mov ax, 0
    stc
    
_chf_end:
    pop bx
    pop si
    mov sp, bp
    pop bp
    ret

_check: 
    push bp
    mov bp, sp
    
    mov sp, bp
    pop bp
    ret


_input:
    push bp
    mov bp, sp
    push si
    push di
    push bx

    mov dx, offset str1
    mov byte ptr [str1], 254
    mov ah, 0Ah
    int 21h
	
	call _putnewline

    mov cl, [str1+1]
    cmp cl, 0
    je _input_error

    xor si, si
    add si, 2          

    mov di, offset buf_num1

parse_num1:
    mov al, [str1+si]
    cmp al, ' '
    je num1_done
    cmp al, 13             
    je _input_error

    mov [di], al
    inc di
    inc si
    jmp parse_num1

num1_done:
    mov byte ptr [di], 0 

skip_spaces1:
    inc si
    mov al, [str1+si]
    cmp al, ' '
    je skip_spaces1

    mov [operation], al
    inc si

skip_spaces2:
    mov al, [str1+si]
    cmp al, ' '
    jne parse_num2
    inc si
    jmp skip_spaces2

parse_num2:
    mov di, offset buf_num2

parse_num2_loop:
    mov al, [str1+si]
    cmp al, ' '
    je num2_done
    cmp al, 13
    je num2_done

    mov [di], al
    inc di
    inc si
    jmp parse_num2_loop

num2_done:
    mov byte ptr [di], 0

    mov ax, 1
    jmp _input_exit

_input_error:
    xor ax, ax

_input_exit:
    pop bx
    pop di
    pop si
    mov sp, bp
    pop bp
    ret

	
_check_format_num1:
    push bp
    mov bp, sp
    push si
    push bx
    
    mov si, offset buf_num1  
    xor bx, bx                 

    push si
    call _check_hex_format
    pop cx
    jnc _cfn1_hex_check
    
_cfn1_hex_check:
    cmp ax, 1
    je _cfn1_hex_valid

    mov al, [si]              
    cmp al, '-'               
    je _check_sign1
    cmp al, '+'                
    je _check_sign1
    jmp _check_digit1_start
    
_check_sign1:
    inc bx                   
    
_check_digit1_start:

    mov al, [si + bx]
    cmp al, 0
    je _calc_error            
    cmp al, 13
    je _calc_error
    
_check_loop1:
    mov al, [si + bx]
    cmp al, 0
    je _check_ok1              
    cmp al, 13
    je _check_ok1            
    cmp al, ' '
    je _check_ok1              

    cmp al, '0'
    jl _calc_error
    cmp al, '9'
    jg _calc_error
    
    inc bx
    jmp _check_loop1
    
_check_ok1:

    cmp bx, 0
    je _calc_error
    cmp bx, 1
    jne _check_ok1_exit

    mov al, [si]
    cmp al, '-'
    je _calc_error
    cmp al, '+'
    je _calc_error
    
_check_ok1_exit:
    mov ax, 1                 
    pop bx
    pop si
    mov sp, bp
    pop bp
    ret
    
_cfn1_hex_valid:

    mov ax, 1
    pop bx
    pop si
    mov sp, bp
    pop bp
    ret
    
_check_format_num2:
    push bp
    mov bp, sp
    push si
    push bx
    
    mov si, offset buf_num2   
    xor bx, bx                 

    push si
    call _check_hex_format
    pop cx
    jnc _cfn2_hex_check
    
_cfn2_hex_check:
    cmp ax, 1
    je _cfn2_hex_valid

    mov al, [si]               
    cmp al, '-'                
    je _check_sign2
    cmp al, '+'                
    je _check_sign2
    jmp _check_digit2_start
    
_check_sign2:
    inc bx                  
    
_check_digit2_start:
    mov al, [si + bx]
    cmp al, 0
    je _calc_error              
    cmp al, 13
    je _calc_error
    
_check_loop2:
    mov al, [si + bx]
    cmp al, 0
    je _check_ok2               
    cmp al, 13
    je _check_ok2            
    cmp al, ' '
    je _check_ok2            

    cmp al, '0'
    jl _calc_error
    cmp al, '9'
    jg _calc_error
    
    inc bx
    jmp _check_loop2
    
_check_ok2:

    cmp bx, 0
    je _calc_error
    cmp bx, 1
    jne _check_ok2_exit

    mov al, [si]
    cmp al, '-'
    je _calc_error
    cmp al, '+'
    je _calc_error
    
_check_ok2_exit:
    mov ax, 1                
    pop bx
    pop si
    mov sp, bp
    pop bp
    ret
    
_cfn2_hex_valid:
    mov ax, 1
    pop bx
    pop si
    mov sp, bp
    pop bp
    ret
	

itoa32:
    push bp
    mov bp, sp
    push ax
    push bx
    push cx
    push dx
    push si
    push di

    mov di, [bp+4] 
    mov ax, [bp+6]
    mov dx, [bp+8]

    cmp dx, 0
    jne not_zero32
    cmp ax, 0
    jne not_zero32

    mov byte ptr [di], '0'
    inc di
    mov byte ptr [di], 0
    jmp done32

not_zero32:
    cmp dx, 0
    jge positive32

    not ax
    not dx
    add ax, 1
    adc dx, 0

    mov byte ptr [di], '-'
    inc di

positive32:
    xor cx, cx
    mov bx, 10

convert_loop:
    mov si, ax
    mov ax, dx
    xor dx, dx
    div bx       

    xchg ax, si
    div bx       

    push dx      
    inc cx

    xor dx, dx
    cmp ax, 0
    jne convert_loop

write_loop:
    pop dx
    add dl, '0'
    mov [di], dl
    inc di
    loop write_loop

    mov byte ptr [di], 0

done32:
    pop di
    pop si
    pop dx
    pop cx
    pop bx
    pop ax
    mov sp, bp
    pop bp
    ret

itoa32_hex:
    push bp
    mov bp, sp
    push di
    push ax
    push dx
    push bx
    push cx

    mov di, [bp+4]     
    mov ax, [bp+6]    
    mov dx, [bp+8]      

    mov bx, ax
    mov cx, dx

    mov ax, cx
    call _write_hex_digit  

    mov ax, bx
    call _write_hex_digit
    
    mov byte ptr [di], 0  

    pop cx
    pop bx
    pop dx
    pop ax
    pop di
    mov sp, bp
    pop bp
    ret

_write_hex_digit:
    push cx
    mov cx, 4
_whd_loop:
    rol ax, 4
    mov dl, al
    and dl, 0Fh
    cmp dl, 9
    jbe _whd_digit
    add dl, 7
_whd_digit:
    add dl, '0'
    mov [di], dl
    inc di
    loop _whd_loop
    pop cx
    ret
	
_calc:
    push bp
    mov bp, sp
	clc
	
	call _check_format_num1
    push offset buf_num1
    call _uatoi
    add sp, 2
    jc _calc_error
    mov [fst_num], ax
	
sec_num_push:
	call _check_format_num2
	
    push offset buf_num2
    call _uatoi
    add sp, 2
    jc _calc_error
    mov [sec_num], ax

next_step:

    mov ax, [fst_num]
    mov bx, [sec_num]

    cmp byte ptr [operation], '+'
    je add_op

    cmp byte ptr [operation], '-'
    je sub_op

    cmp byte ptr [operation], '*'
    je mul_op

    cmp byte ptr [operation], '/'
    je div_op

    cmp byte ptr [operation], '%'
    je mod_op

    jmp _operation_error

add_op:
    add ax, bx
    jo _overflow
    mov [result_else], ax
	cmp [result_else], 32767
	jg _overflow
    jmp done

sub_op:
    sub ax, bx
    jo _overflow
    mov [result_else], ax
	cmp [result_else], 32767
	jg _overflow
    jmp done

mul_op:
	mov byte ptr[mul_index], 1
    imul bx
    mov word ptr [result_if_mul], ax
    mov word ptr [result_if_mul+2], dx
    jmp done

div_op:
    cmp bx, 0
    je _div_zero
    cwd
    idiv bx
    mov [result_else], ax
	cmp [result_else], 32767
	jg _overflow
    jmp done

mod_op:
    cmp bx, 0
    je _div_zero
    cwd
    idiv bx
    mov [result_else], dx
	cmp [result_else], 32767
	jg _overflow
    jmp done

_div_zero:
    call _exit_div_zero

_overflow:
    call _exit_overflow

_calc_error:
    call _exit_error_mismatch

_operation_error:
    call _exit_error_operation
	
done:
    mov sp, bp
    pop bp
    ret
	
_processing_res:
    push bp
    mov bp, sp

    cmp [mul_index], 0
    je _else_mult_res

_if_mult_res:
    mov ax, word ptr [result_if_mul]
    mov dx, word ptr [result_if_mul+2]

    push dx
    push ax
    push offset result_buf
    call itoa32
    add sp, 6

    push dx
    push ax
    push offset result_buf_hex
    call itoa32_hex
    add sp, 6

    jmp _print

_else_mult_res:
    mov ax, [result_else]

    mov cx, 9
    mov di, offset result_buf_hex
    mov al, 0
    rep stosb
	
	mov ax, [result_else]  
    push ax
    push offset result_buf
    call itoa_dec
    add sp, 4
 
    mov ax, [result_else]  
    push ax
    push offset result_buf_hex
    call itoa_hex
    add sp, 4

_print:
    mov dx, offset msg_result
    mov ah, 9
    int 21h

    push offset result_buf
    call _putstr
    add sp, 2

    mov dx, offset str_hex_prefix
    mov ah, 9
    int 21h

    push offset result_buf_hex
    call _putstr
    add sp, 2

    mov dx, offset str_hex_suffix
    mov ah, 9
    int 21h

    call _putnewline

    mov sp, bp
    pop bp
    ret
	

_putstr:
    push bp
    mov bp, sp
    push si
    mov si, [bp+4]
    
_ps_loop:
    lodsb
    cmp al, 0
    je _ps_end
    mov dl, al
    mov ah, 2
    int 21h
    jmp _ps_loop
    
_ps_end:
    pop si
    mov sp, bp
    pop bp
    ret
	

_putchar:
    push bp
    mov bp, sp
    mov dl, [bp+4]
    mov ah, 2
    int 21h
    mov sp, bp
    pop bp
    ret
	
start: 
    mov ax, data
    mov ds, ax
    mov ax, stack
    mov ss, ax
	
	call _input

    call _calc
	
	call _processing_res

	call _exit0
code ends

end start