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

mstack segment para stack use16
db 65530 dup(?)
mstack ends

stack segment para stack use16
db 65530 dup(?)
stack ends

data segment para public use16

; errors
    ERROR_ALLOC equ 0
    ERROR_OPEN equ 1
    ERROR_LSEEK equ 2

    err_unknown_operation   db "Unknown operation!",0    
    err_invalid_matrix_size db "Invalid matrix size for operation!", 0

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

	err_overflow   db 'Overflow', 0
    err_div_zero   db 'Div by 0', 0
    err_unknown_op db 'Bad op', 0
    err_empty      db 'Empty', 0
    font_filename db 'font.bin', 0

	orig_mode db 00h

include font16.inc

include buttons.inc


    calc_str    db 'calculator',0

    str_init    db '0',0               ; used as init_text

    sep_line    db '-----------', 0
    _output_field db '0', 260 dup(0), 0

OUTPUT_FIELD_SIZE equ 15

    _output_field_i dw 0

    _blink_index dw -1
    _blink_orig_color db 0
    _blink_counter db 0
    _need_refresh db 1 

    handler_ss dw 0
    handler_sp dw 0 
;----

data ends

code segment para public use16

assume cs:code,ds:data,ss:stack, es:data

include misc.inc
include macro.inc

include strings.inc
include memory.inc

;io headers
include io.inc
include fio.inc

; error handling function 
include error.inc

include grap.inc
include mouse.inc



_main proc near 
    push bp
    mov bp, sp

    call _get_mode
	mov byte ptr ds:[orig_mode], al

	mov dx, 04h
	push dx
	call _set_mode
	add sp, 2

	mov ax, 0         ; инициализировать мышь
	int 33h
	
	mov ax, 1         ; показать курсор мыши
	int 33h
	
    mov ax, cs
    mov es, ax
	mov ax, 000Ch     ; установить обработчик событий мыши
	mov cx, 0002h     ; событие - нажатие левой кнопки
	mov dx, offset mouse_handler ; ES:DX - адрес обработчика
	int 33h

	mov ax, 0B800H
	mov es, ax

    push offset font_filename
    push ACCESS_READ
    call _fopen
    jc skip_font_loading
    add sp, 4

    push ax
    call _load_font
    add sp,2

skip_font_loading:

    call _maing

    call getchar

	movzx dx, byte ptr ds:[orig_mode]
	push dx
	call _set_mode
	add sp, 2
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

    ; code here
    call _main

    call exit0
end_code_seg:
code ends

end start
