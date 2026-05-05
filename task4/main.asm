; Функции для ввода-вывода строк/символов (используется соглашение cdecl)
.386

MAT_DATA_OFFSET EQU 20000
EOF EQU 0FFh

ACCESS_READ     EQU 0
ACCESS_WRITE    EQU 1

SEEK_START equ 0
SEEK_CURRENT_POS equ 1
SEEK_END equ 2

arg1 equ 4
arg2 equ 6
arg3 equ 8
arg4 equ 10

var1 equ -2
var2 equ -4
var3 equ -6
var4 equ -8

SHRT_MAX equ 32767
SHRT_MIN equ -32768

stack segment para stack use16
db 65530 dup(?)
stack ends

data segment para public use16

; errors
    ERROR_ALLOC equ 0
    ERROR_OPEN equ 1
    ERROR_LSEEK equ 2

    err_unknown_operation   db "Unknown operation!",0    
	err_dim_mismatch        db "Matrix dimensions are incompatible for this operation!", 0
    fio_err_no_error        db " ", 0                                ; code 0 (no error, rarely used)
    fio_err_invalid_func    db "Invalid function", 0dh, 0ah, 0      ; code 1
    fio_err_not_found       db "File not found", 0dh, 0ah, 0        ; code 2
    fio_err_path_not_found  db "Path not found", 0dh, 0ah, 0        ; code 3
    fio_err_too_many_open   db "Too many open files", 0dh, 0ah, 0   ; code 4
    fio_err_access_denied   db "Access denied", 0dh, 0ah, 0         ; code 5
    fio_err_invalid_handle  db "Invalid handle", 0dh, 0ah, 0        ; code 6
    fio_err_mcb_destroyed   db "Memory control blocks destroyed", 0dh, 0ah, 0 ; code 7
    fio_err_insufficient_mem db "Insufficient memory", 0dh, 0ah, 0  ; code 8
    fio_err_invalid_mem_block db "Invalid memory block address", 0dh, 0ah, 0 ; code 9
    fio_err_invalid_env     db "Invalid environment", 0dh, 0ah, 0   ; code 10
    fio_err_invalid_format  db "Invalid format", 0dh, 0ah, 0        ; code 11
    fio_err_invalid_access  db "Invalid access code", 0dh, 0ah, 0   ; code 12
    fio_err_invalid_data    db "Invalid data", 0dh, 0ah, 0          ; code 13
    fio_err_reserved        db "Reserved error", 0dh, 0ah, 0        ; code 14
    fio_err_invalid_drive   db "Invalid drive", 0dh, 0ah, 0         ; code 15
    fio_err_remove_cur_dir  db "Cannot remove current directory", 0dh, 0ah, 0 ; code 16
    fio_err_not_same_device db "Not same device", 0dh, 0ah, 0       ; code 17
    fio_err_no_more_files   db "No more files", 0dh, 0ah, 0         ; code 18
    fio_err_write_protected db "Write protected", 0dh, 0ah, 0       ; code 19
    fio_err_unknown_unit    db "Unknown unit", 0dh, 0ah, 0          ; code 20
    fio_err_drive_not_ready db "Drive not ready", 0dh, 0ah, 0       ; code 21
    fio_err_unknown_cmd     db "Unknown command", 0dh, 0ah, 0       ; code 22
    fio_err_crc_error       db "CRC error", 0dh, 0ah, 0             ; code 23
    fio_err_bad_req_len     db "Bad request structure length", 0dh, 0ah, 0 ; code 24
    fio_err_seek_error      db "Seek error", 0dh, 0ah, 0            ; code 25
    fio_err_unknown_media   db "Unknown media type", 0dh, 0ah, 0    ; code 26
    fio_err_sector_not_found db "Sector not found", 0dh, 0ah, 0     ; code 27
    fio_err_printer_out_paper db "Printer out of paper", 0dh, 0ah, 0 ; code 28
    fio_err_invalid_device_req db "Invalid device request", 0dh, 0ah, 0 ; code 29 (returned by read/write on unsuitable device)
    fio_err_read_fault      db "Read fault", 0dh, 0ah, 0            ; code 30
    fio_err_general_failure db "General failure", 0dh, 0ah, 0       ; code 31
    fio_err_unknown         db "Unknown DOS error", 0dh, 0ah, 0     ; for codes >3

    fio_err_vec dw offset fio_err_no_error          ; 0
                dw offset fio_err_invalid_func      ; 1
                dw offset fio_err_not_found         ; 2
                dw offset fio_err_path_not_found    ; 3
                dw offset fio_err_too_many_open     ; 4
                dw offset fio_err_access_denied     ; 5
                dw offset fio_err_invalid_handle    ; 6
                dw offset fio_err_mcb_destroyed     ; 7
                dw offset fio_err_insufficient_mem  ; 8
                dw offset fio_err_invalid_mem_block ; 9
                dw offset fio_err_invalid_env       ; 10
                dw offset fio_err_invalid_format    ; 11
                dw offset fio_err_invalid_access    ; 12
                dw offset fio_err_invalid_data      ; 13
                dw offset fio_err_reserved          ; 14
                dw offset fio_err_invalid_drive     ; 15
                dw offset fio_err_remove_cur_dir    ; 16
                dw offset fio_err_not_same_device   ; 17
                dw offset fio_err_no_more_files     ; 18
                dw offset fio_err_write_protected   ; 19
                dw offset fio_err_unknown_unit      ; 20
                dw offset fio_err_drive_not_ready   ; 21
                dw offset fio_err_unknown_cmd       ; 22
                dw offset fio_err_crc_error         ; 23
                dw offset fio_err_bad_req_len       ; 24
                dw offset fio_err_seek_error        ; 25
                dw offset fio_err_unknown_media     ; 26
                dw offset fio_err_sector_not_found  ; 27
                dw offset fio_err_printer_out_paper ; 28
                dw offset fio_err_invalid_device_req; 29
                dw offset fio_err_read_fault        ; 30
                dw offset fio_err_general_failure   ; 31
;

    newline db 10, 13

    filename1       db 200 dup(0)
    filename2       db 200 dup(0)
    result_filename db 200 dup(0)

    mat_ob dw 0

    filename1_ask_str       db "please enter first matrix filename: ", 0
    filename2_ask_str       db "please enter second matrix filename: ", 0
    result_filename_ask_str db "please enter result matrix filename: ", 0
    ask_op                  db "please enter operation(+,-,*): ", 0

    tmp_buffer              db 3 dup (0)

data ends

mat1_seg segment para public use16
    _mat_data1   DW  10000 DUP (?)
    _mat_rows1   DW  0               ; number of rows in matrix
    _mat_cols1   DW  0               ; number of columns in matrix
mat1_seg ends

mat2_seg segment para public use16
    _mat_data2   DW  10000 DUP (?)
    _mat_rows2   DW  0               ; number of rows in matrix
    _mat_cols2   DW  0               ; number of columns in matrix
mat2_seg ends

code segment para public use16

assume cs:code,ds:data,ss:stack, es:data

include misc.inc
include macro.inc
include strings.inc
include memory.inc
include io.inc
include fio.inc
include error.inc
include mat.inc
include mat_sum.inc
include mat_diff.inc
include mat_mul.inc

_bo_mat_input proc near
    push bp
    mov bp, sp 

    push offset filename1_ask_str
    call _putstr
    add sp, 2

    push offset filename1
    push 200
    call getstr
    add sp, 4

    push offset filename2_ask_str
    call _putstr
    add sp, 2

    push offset filename2
    push 200
    call getstr
    add sp, 4

    push offset result_filename_ask_str
    call _putstr
    add sp, 2

    push offset result_filename
    push 200
    call getstr
    add sp, 4

    mov sp, bp
    pop bp
    ret
_bo_mat_input endp

_select_op proc near
    push bp
    mov bp, sp 

    mov ax, word ptr [bp+arg1]

    cmp al, '+'
    je _select_op_plus
    cmp al, '-'
    je _select_op_minus
    cmp al, '*'
    je _select_op_mul

    push offset err_unknown_operation
    call _putstr
    add sp, 2

    mov sp, bp
    pop bp
    stc
    ret

_select_op_done:
    mov sp, bp
    pop bp
    clc
    ret

_select_op_minus:
    mov word ptr [mat_ob], offset _mat_diff
    jmp _select_op_done

_select_op_plus:
    mov word ptr [mat_ob], offset _mat_sum
    jmp _select_op_done

_select_op_mul:
    mov word ptr [mat_ob], offset _mat_mul
    jmp _select_op_done

_select_op endp

; _bo_mat_op_calc proc near 
    ; push bp
    ; mov bp, sp
    ; sub sp, 4

    ; mov bx, word ptr [mat_ob]
    ; mov word ptr [bp+var2], bx

    ; mov ax, mat1_seg
    ; mov es, ax

    ; push offset filename1
    ; push ACCESS_READ
    ; call _fopen
    ; jc _bo_mat_op_calc_open_failed
    ; add sp, 4

    ; mov word ptr [bp+var1], ax

    ; push ax
    ; call _read_mat
    ; add sp, 2

    ; push word ptr [bp+var1]
    ; call _fclose
    ; add sp, 2

    ; mov ax, mat2_seg
    ; mov es, ax

    ; push offset filename2
    ; push ACCESS_READ
    ; call _fopen
    ; jc _bo_mat_op_calc_open_failed
    ; add sp, 4

    ; mov word ptr [bp+var1], ax

    ; push ax
    ; call _read_mat
    ; add sp, 2

    ; push word ptr [bp+var1]
    ; call _fclose
    ; add sp, 2

    ; push offset result_filename
    ; push ACCESS_WRITE
    ; call _fopen
    ; jc _bo_mat_op_calc_open_failed
    ; add sp, 4
    
    ; mov word ptr [bp+var1], ax

    ; push ds
    ; mov ax, mat1_seg
    ; mov ds, ax
    ; mov ax, mat2_seg
    ; mov es, ax
    ; push word ptr [bp+var1]
    ; mov bx, word ptr [bp+var2]

    ; call bx
    ; add sp, 2    

    ; pop ds

    ; push word ptr [bp+var1]
    ; call _fclose
    ; add sp, 2

; _bo_mat_op_calc_done:
    ; mov sp, bp
    ; pop bp
    ; ret

; _bo_mat_op_calc_open_failed:
    ; add sp, 4

    ; mov cx, ax

    ; push cx
    ; call fio_error_handler
    ; add sp, 2

    ; jmp _bo_mat_op_calc_done
; _bo_mat_op_calc endp

_bo_mat_op_calc proc near 
    push bp
    mov bp, sp
    sub sp, 4

    mov bx, word ptr [mat_ob]
    mov word ptr [bp+var2], bx

    mov ax, mat1_seg
    mov es, ax

    push offset filename1
    push ACCESS_READ
    call _fopen
    jc _bo_mat_op_calc_open_failed
    add sp, 4

    mov word ptr [bp+var1], ax

    push ax
    call _read_mat
    add sp, 2

    push word ptr [bp+var1]
    call _fclose
    add sp, 2

    mov ax, mat2_seg
    mov es, ax

    push offset filename2
    push ACCESS_READ
    call _fopen
    jc _bo_mat_op_calc_open_failed
    add sp, 4

    mov word ptr [bp+var1], ax

    push ax
    call _read_mat
    add sp, 2

    push word ptr [bp+var1]
    call _fclose
    add sp, 2

    push offset result_filename
    push ACCESS_WRITE
    call _fopen
    jc _bo_mat_op_calc_open_failed
    add sp, 4
    
    mov word ptr [bp+var1], ax

    ; СОХРАНЯЕМ ds в стеке ПЕРЕД переключением на сегменты матриц
    push ds
    
    ; Настраиваем сегменты для операции
    mov ax, mat1_seg
    mov ds, ax          ; ds -> матрица 1
    mov ax, mat2_seg
    mov es, ax          ; es -> матрица 2
    
    ; Вызываем операцию - теперь она возвращает результат в AX
    push word ptr [bp+var1]   ; дескриптор файла результата
    mov bx, word ptr [bp+var2] ; адрес процедуры операции
    call bx
    add sp, 2
    
    ; Проверяем результат в AX (0 = успех, -1 = ошибка)
    cmp ax, -1
    je _bo_mat_op_calc_dim_error
    
    ; Успех - восстанавливаем ds и закрываем файл
    pop ds
    push word ptr [bp+var1]
    call _fclose
    add sp, 2

_bo_mat_op_calc_done:
    mov sp, bp
    pop bp
    ret

_bo_mat_op_calc_dim_error:
    ; Восстанавливаем ds из стека
    pop ds
    
    ; Закрываем файл результата
    push word ptr [bp+var1]
    call _fclose
    add sp, 2
    
    ; Выводим сообщение об ошибке
    push offset err_dim_mismatch
    call _putstr
    add sp, 2
    
    jmp _bo_mat_op_calc_done

_bo_mat_op_calc_open_failed:
    add sp, 4
    mov cx, ax
    push cx
    call fio_error_handler
    add sp, 2
    jmp _bo_mat_op_calc_done
_bo_mat_op_calc endp

_main proc near 
    push bp
    mov bp, sp 

    call _bo_mat_input

    push offset ask_op
    call _putstr
    add sp, 2

    call getchar
    xor ah, ah
    push ax
    call _select_op
    jc main_done

    call _bo_mat_op_calc

main_done:
    mov sp, bp
    pop bp
    ret

_main endp 

start:
    mov ax, data
    mov ds, ax
    mov es, ax
    mov ax, stack
    mov ss, ax
    nop

    push offset end_code_seg
    push cs
    push es
    call InitMem
    add sp, 6 

    call _main

    call exit0
end_code_seg:
code ends

end start