; Функции для ввода-вывода строк/символов (используется соглашение cdecl)
.386

ACCESS_READ     EQU 0

SEEK_START equ 0
SEEK_CURRENT_POS equ 1
SEEK_END equ 2

arg1 equ 4
arg2 equ 6
arg3 equ 8
arg4 equ 10
arg5 equ 12
arg6 equ 14

var1 equ -2
var2 equ -4
var3 equ -6
var4 equ -8
var5 equ -10
var6 equ -12

SHRT_MAX equ 32767
SHRT_MIN equ -32768

stack segment para stack use16
db 65530 dup(?)
stack ends

data segment para public use16

    ERROR_ALLOC equ 0
    ERROR_OPEN equ 1
    ERROR_LSEEK equ 2

    err_alloc db "failed to allocate memory", 0dh, 0ah, 0
    err_open db "failed to open file", 0dh, 0ah, 0
    err_lseek db "failed to lseek file", 0dh, 0ah, 0
    err_vec dw offset err_alloc, offset err_open, offset err_lseek

    fio_err_no_error        db " ", 0
    fio_err_invalid_func    db "The specified function is not supported", 0dh, 0ah, 0
    fio_err_not_found       db "The requested file could not be located", 0dh, 0ah, 0
    fio_err_path_not_found  db "The specified directory path does not exist", 0dh, 0ah, 0
    fio_err_too_many_open   db "The system has reached the limit of simultaneously open files", 0dh, 0ah, 0
    fio_err_access_denied   db "You do not have permission to perform this operation", 0dh, 0ah, 0
    fio_err_invalid_handle  db "The provided file handle is not recognized", 0dh, 0ah, 0
    fio_err_mcb_destroyed   db "The internal memory control structure has been corrupted", 0dh, 0ah, 0
    fio_err_insufficient_mem db "There is not enough free memory to complete this request", 0dh, 0ah, 0
    fio_err_invalid_mem_block db "The memory block address supplied is incorrect", 0dh, 0ah, 0
    fio_err_invalid_env     db "The program environment block appears to be invalid", 0dh, 0ah, 0
    fio_err_invalid_format  db "The file or data format is not recognized", 0dh, 0ah, 0
    fio_err_invalid_access  db "The specified access mode code is invalid", 0dh, 0ah, 0
    fio_err_invalid_data    db "The provided data is malformed or corrupted", 0dh, 0ah, 0
    fio_err_reserved        db "A reserved system error has occurred", 0dh, 0ah, 0
    fio_err_invalid_drive   db "The drive letter you specified is not valid", 0dh, 0ah, 0
    fio_err_remove_cur_dir  db "It is not possible to delete the current working directory", 0dh, 0ah, 0
    fio_err_not_same_device db "The source and destination paths refer to different devices", 0dh, 0ah, 0
    fio_err_no_more_files   db "There are no additional files matching the search criteria", 0dh, 0ah, 0
    fio_err_write_protected db "The target disk or media is currently write-protected", 0dh, 0ah, 0
    fio_err_unknown_unit    db "The requested disk drive or unit does not exist", 0dh, 0ah, 0
    fio_err_drive_not_ready db "The drive is not ready; please check the disk and try again", 0dh, 0ah, 0
    fio_err_unknown_cmd     db "The device does not recognize the command that was sent", 0dh, 0ah, 0
    fio_err_crc_error       db "A cyclic redundancy check (CRC) error has been detected", 0dh, 0ah, 0
    fio_err_bad_req_len     db "The length of the request structure is invalid", 0dh, 0ah, 0
    fio_err_seek_error      db "Unable to reposition the file pointer to the desired location", 0dh, 0ah, 0
    fio_err_unknown_media   db "The type of media in the drive cannot be determined", 0dh, 0ah, 0
    fio_err_sector_not_found db "The requested disk sector was not found on the media", 0dh, 0ah, 0
    fio_err_printer_out_paper db "The printer is out of paper and cannot continue", 0dh, 0ah, 0
    fio_err_invalid_device_req db "The requested operation is not supported for this device type", 0dh, 0ah, 0
    fio_err_read_fault      db "A critical error occurred while attempting to read data", 0dh, 0ah, 0
    fio_err_general_failure db "An unspecified general hardware or system failure has occurred", 0dh, 0ah, 0
    fio_err_unknown         db "An unrecognized DOS error condition has been encountered", 0dh, 0ah, 0

    fio_err_vec dw offset fio_err_no_error
                dw offset fio_err_invalid_func
                dw offset fio_err_not_found
                dw offset fio_err_path_not_found
                dw offset fio_err_too_many_open
                dw offset fio_err_access_denied
                dw offset fio_err_invalid_handle
                dw offset fio_err_mcb_destroyed
                dw offset fio_err_insufficient_mem
                dw offset fio_err_invalid_mem_block
                dw offset fio_err_invalid_env
                dw offset fio_err_invalid_format
                dw offset fio_err_invalid_access
                dw offset fio_err_invalid_data
                dw offset fio_err_reserved
                dw offset fio_err_invalid_drive
                dw offset fio_err_remove_cur_dir
                dw offset fio_err_not_same_device
                dw offset fio_err_no_more_files
                dw offset fio_err_write_protected
                dw offset fio_err_unknown_unit
                dw offset fio_err_drive_not_ready
                dw offset fio_err_unknown_cmd
                dw offset fio_err_crc_error
                dw offset fio_err_bad_req_len
                dw offset fio_err_seek_error
                dw offset fio_err_unknown_media
                dw offset fio_err_sector_not_found
                dw offset fio_err_printer_out_paper
                dw offset fio_err_invalid_device_req
                dw offset fio_err_read_fault
                dw offset fio_err_general_failure

    str1 db 256 dup(?)
    str2 db "Hello, World!", 0

    test_str1       db "Hello", 0
    test_str2       db "Hello, World!", 0
    test_str3       db "world", 0
    test_str4       db 0
    test_str5       db "abc", 0
    test_str6       db "abd", 0
    test_str7       db "ab", 0
    test_str8       db "abcdef", 0
    test_str9       db "cde", 0
    test_buffer     db 256 dup(?)

    test_str_upper db "ABC", 0
    test_str_apple db "apple", 0
    test_str_banana db "BANANA", 0
    test_str_cat_res db "Helloabd", 0

    test_strtol1   db "123",0
    test_strtol2   db "-456",0
    test_strtol3   db "  +789",0
    test_strtol4   db "FF",0
    test_strtol5   db "0x10",0
    test_strtol6   db "077",0
    test_strtol7   db "10",0
    test_strtol8   db "123abc",0
    test_strtol9   db "abc",0
    test_strtol10  db "32767",0
    test_strtol11  db "-32768",0
    test_end_ptr   dw ?

    msg_pass        db " PASS", 13, 10, 0
    msg_fail        db " FAIL", 13, 10, 0
    msg_strlen      db "Test strlen", 0
    msg_strchr      db "Test strchr", 0
    msg_strstr      db "Test strstr", 0
    msg_strcmp      db "Test strcmp", 0
    msg_strcpy      db "Test strcpy", 0
    msg_stricmp     db "Test stricmp", 0
    msg_strtol      db "Test strtol ", 0
    msg_strdup      db "Test strdup", 0
    msg_strcat      db "Test strcat", 0

    msg_loaded_str  db "Loaded string from file: ", 13, 10, 0
    msg_filed_to_find_delimiter  db "failed to find delimiter '|' in string!", 13, 10, 0

    filepath1 db 128 dup(?)
    filepath2 db 128 dup(?)
    
    msg_enter_file1 db "Enter path to first file: ", 0
    msg_enter_file2 db "Enter path to second file: ", 0
    msg_reading_file db "Reading file...", 13, 10, 0
    msg_file_size db "File size: ", 0
    msg_lines_count db " lines", 13, 10, 0
    msg_case_sensitive db 13, 10, "--- Case-sensitive comparison (strcmp) ---", 13, 10, 0
    msg_case_insensitive db 13, 10, "--- Case-insensitive comparison (stricmp) ---", 13, 10, 0
    msg_diff_line db "Line ", 0
    msg_diff_char db ", char ", 0
    msg_no_diff db "Files are identical!", 13, 10, 0
    msg_error_reading db "Error reading file!", 13, 10, 0
    msg_error_empty db "File is empty!", 13, 10, 0
    msg_diff_line_count db "Files have different number of lines!", 13, 10, 0
    msg_bytes_read db " bytes read", 13, 10, 0
    
    num_buffer db 16 dup(?)

data ends

code segment para public use16

assume cs:code,ds:data,ss:stack, es:data

include macro.inc

include strings.inc
include memory.inc
include misc.inc

include io.inc
include fio.inc
include error.inc

include tests.inc

; ============================================================================
; void print_number(int num)
; ============================================================================
print_number proc near
    push bp
    mov bp, sp
    push ax
    push bx
    push cx
    push dx
    push si
    
    mov ax, word ptr [bp+arg1]
    mov si, offset num_buffer
    add si, 15
    mov byte ptr [si], 0
    dec si
    
    test ax, ax
    jge pn_convert
    neg ax
    push ax
    push '-'
    call putchar
    add sp, 2
    pop ax
    
pn_convert:
    mov bx, 10
    
pn_loop:
    xor dx, dx
    div bx
    add dl, '0'
    mov byte ptr [si], dl
    dec si
    test ax, ax
    jnz pn_loop
    
    inc si
    push si
    call putstr
    add sp, 2
    
    pop si
    pop dx
    pop cx
    pop bx
    pop ax
    mov sp, bp
    pop bp
    ret
print_number endp

; ============================================================================
; word split_text_to_lines(word seg_text)
; Модифицирует текст: заменяет \r и \n на \0
; Возвращает: ax = сегмент массива смещений строк, cx = количество строк
; ============================================================================
split_text_to_lines proc near
    push bp
    mov bp, sp
    sub sp, 6                      ; [bp-2] = line_count
                                   ; [bp-4] = array_seg
                                   ; [bp-6] = text_len
    push es
    push ds
    push si
    push di
    push bx
    push dx
    
    mov es, word ptr [bp+arg1]     ; es = сегмент текста
    
    cmp byte ptr es:[0], 0
    jne stl_not_empty
    jmp stl_empty
    
stl_not_empty:
    ; Первый проход: заменяем \r и \n на \0, считаем строки
    xor si, si
    xor cx, cx                     ; счетчик строк
    xor dx, dx                     ; длина текста
    
stl_clean_loop:
    mov al, byte ptr es:[si]
    test al, al
    jz stl_clean_done
    
    inc dx                         ; считаем длину
    
    cmp al, 0Dh                    ; \r
    jne stl_check_nl
    mov byte ptr es:[si], 0        ; заменяем \r на 0
    inc si
    jmp stl_clean_loop
    
stl_check_nl:
    cmp al, 0Ah                    ; \n
    jne stl_clean_next
    mov byte ptr es:[si], 0        ; заменяем \n на 0
    inc cx                         ; считаем строку
    
stl_clean_next:
    inc si
    jmp stl_clean_loop
    
stl_clean_done:
    mov word ptr [bp-6], dx        ; text_len
    inc cx                         ; +1 за последнюю строку
    mov word ptr [bp-2], cx        ; line_count
    
    ; Выделяем память под массив смещений (по 2 байта на строку)
    shl cx, 1
    push cx
    call AllocMem
    add sp, 2
    test ax, ax
    jz stl_empty
    
    mov word ptr [bp-4], ax        ; array_seg
    
    ; Второй проход: находим начала строк
    xor si, si
    xor di, di                     ; di = индекс в массиве
    
    ; Первая строка всегда начинается с 0 (если текст не пустой)
    push ds
    mov ds, word ptr [bp-4]
    mov word ptr [di], 0           ; первая строка - смещение 0
    add di, 2
    pop ds
    
    ; Ищем остальные строки
    mov si, 1
    
stl_find_loop:
    ; Проверяем, не вышли ли за пределы текста
    mov ax, si
    cmp ax, word ptr [bp-6]        ; cmp si, text_len
    jae stl_done
    
    ; Проверяем, что текущий символ не 0, а предыдущий - 0
    cmp byte ptr es:[si], 0
    je stl_find_next
    
    cmp byte ptr es:[si-1], 0
    jne stl_find_next
    
    ; Нашли начало новой строки
    push ds
    mov ds, word ptr [bp-4]
    mov word ptr [di], si          ; сохраняем смещение
    add di, 2
    pop ds
    
stl_find_next:
    inc si
    jmp stl_find_loop
    
stl_done:
    mov cx, word ptr [bp-2]
    mov ax, word ptr [bp-4]
    jmp stl_exit
    
stl_empty:
    xor ax, ax
    xor cx, cx
    
stl_exit:
    pop dx
    pop bx
    pop di
    pop si
    pop ds
    pop es
    mov sp, bp
    pop bp
    ret
split_text_to_lines endp

; ============================================================================
; int compare_lines_by_offset(word seg1, word off1, word seg2, word off2, int case_insensitive)
; ============================================================================
compare_lines_by_offset proc near
    push bp
    mov bp, sp
    sub sp, 2                      ; [bp-2] = char_index
    push si
    push di
    push es
    push ds
    
    mov es, word ptr [bp+arg1]     ; seg1
    mov si, word ptr [bp+arg2]     ; off1
    
    mov ds, word ptr [bp+arg3]     ; seg2
    mov di, word ptr [bp+arg4]     ; off2
    
    mov word ptr [bp-2], 0         ; char_index = 0
    
clbo_loop:
    mov al, byte ptr es:[si]
    mov ah, byte ptr [di]
    
    cmp word ptr [bp+arg5], 0      ; case_insensitive?
    jne clbo_case_insensitive
    
    cmp al, ah
    jne clbo_diff
    jmp clbo_check_end
    
clbo_case_insensitive:
    cmp al, 'a'
    jb clbo_check_ah
    cmp al, 'z'
    ja clbo_check_ah
    sub al, 20h
clbo_check_ah:
    cmp ah, 'a'
    jb clbo_compare
    cmp ah, 'z'
    ja clbo_compare
    sub ah, 20h
    
clbo_compare:
    cmp al, ah
    jne clbo_diff
    
clbo_check_end:
    test al, al                    ; конец строки?
    jz clbo_equal
    
    inc si
    inc di
    inc word ptr [bp-2]
    jmp clbo_loop
    
clbo_diff:
    mov ax, word ptr [bp-2]
    jmp clbo_exit
    
clbo_equal:
    mov ax, -1
    
clbo_exit:
    pop ds
    pop es
    pop di
    pop si
    mov sp, bp
    pop bp
    ret
compare_lines_by_offset endp

; ============================================================================
; int read_file_to_memory(char* filename)
; ============================================================================
read_file_to_memory proc near
    push bp
    mov bp, sp
    sub sp, 4
    push bx
    push cx
    push dx
    push si
    push di
    push ds
    
    push word ptr [bp+arg1]
    push ACCESS_READ
    call fopen
    add sp, 4
    jc rfm_error
    
    mov word ptr [bp-2], ax
    
    push ax
    call fsize
    add sp, 2
    jc rfm_error_close
    
    cmp dx, 0
    jne rfm_too_big
    test ax, ax
    jz rfm_empty
    
    mov word ptr [bp-4], ax
    
    push offset msg_file_size
    call putstr
    add sp, 2
    push word ptr [bp-4]
    call print_number
    add sp, 2
    push offset msg_bytes_read
    call putstr
    add sp, 2
    
    mov ax, word ptr [bp-4]
    add ax, 1
    push ax
    call AllocMem
    add sp, 2
    test ax, ax
    jz rfm_error_close
    
    push ax
    push ds
    mov ds, ax
    
    push word ptr [bp-4]
    push 0
    push word ptr [bp-2]
    call fread
    add sp, 6
    
    pop ds
    pop es
    
    mov di, word ptr [bp-4]
    mov byte ptr es:[di], 0
    
    push word ptr [bp-2]
    call fclose
    add sp, 2
    
    mov ax, es
    jmp rfm_exit
    
rfm_too_big:
    push offset msg_error_reading
    call putstr
    add sp, 2
    jmp rfm_error_close
    
rfm_empty:
    push offset msg_error_empty
    call putstr
    add sp, 2
    jmp rfm_error_close
    
rfm_error_close:
    push word ptr [bp-2]
    call fclose
    add sp, 2
    
rfm_error:
    xor ax, ax
    
rfm_exit:
    pop ds
    pop di
    pop si
    pop dx
    pop cx
    pop bx
    mov sp, bp
    pop bp
    ret
read_file_to_memory endp

; ============================================================================
; void free_lines_array(void* lines_array)
; ============================================================================
free_lines_array proc near
    push bp
    mov bp, sp
    
    push word ptr [bp+arg1]
    call FreeMem
    add sp, 2
    
    mov sp, bp
    pop bp
    ret
free_lines_array endp

; ============================================================================
; void compare_files(char* file1_path, char* file2_path)
; ============================================================================
compare_files proc near
    push bp
    mov bp, sp
    sub sp, 14
    push si
    push di
    push bx
    
    mov word ptr [bp-2], 0
    mov word ptr [bp-4], 0
    mov word ptr [bp-6], 0
    mov word ptr [bp-8], 0
    mov word ptr [bp-10], 0
    mov word ptr [bp-12], 0
    
    ; Читаем первый файл
    push offset msg_reading_file
    call putstr
    add sp, 2
    
    push word ptr [bp+arg1]
    call read_file_to_memory
    add sp, 2
    test ax, ax
    jz cf_error
    mov word ptr [bp-2], ax
    
    ; Читаем второй файл
    push offset msg_reading_file
    call putstr
    add sp, 2
    
    push word ptr [bp+arg2]
    call read_file_to_memory
    add sp, 2
    test ax, ax
    jz cf_error
    mov word ptr [bp-4], ax
    
    ; Разбираем первый файл
    push word ptr [bp-2]
    call split_text_to_lines
    add sp, 2
    mov word ptr [bp-6], ax
    mov word ptr [bp-10], cx
    
    ; Разбираем второй файл
    push word ptr [bp-4]
    call split_text_to_lines
    add sp, 2
    mov word ptr [bp-8], ax
    mov word ptr [bp-12], cx
    
    ; Выводим количество строк
    push word ptr [bp-10]
    call print_number
    add sp, 2
    push offset msg_lines_count
    call putstr
    add sp, 2
    
    push word ptr [bp-12]
    call print_number
    add sp, 2
    push offset msg_lines_count
    call putstr
    add sp, 2
    
    cmp word ptr [bp-10], 0
    je cf_no_lines
    cmp word ptr [bp-12], 0
    je cf_no_lines
    
    ; --- Case-sensitive ---
    push offset msg_case_sensitive
    call putstr
    add sp, 2
    
    mov cx, word ptr [bp-10]
    cmp cx, word ptr [bp-12]
    jbe cf_cs_start
    mov cx, word ptr [bp-12]
    
cf_cs_start:
    xor bx, bx
    mov word ptr [bp-14], 0
    
cf_cs_loop:
    cmp bx, cx
    jae cf_cs_done
    
    push cx
    push bx
    
    mov si, bx
    shl si, 1
    
    push ds
    mov ds, word ptr [bp-6]
    mov di, word ptr [si]          ; off1
    pop ds
    
    push es
    mov es, word ptr [bp-8]
    mov si, word ptr es:[si]       ; off2
    pop es
    
    push 0
    push si
    push word ptr [bp-4]
    push di
    push word ptr [bp-2]
    call compare_lines_by_offset
    add sp, 10
    
    cmp ax, -1
    je cf_cs_next
    
    mov word ptr [bp-14], 1
    
    push offset msg_diff_line
    call putstr
    add sp, 2
    push bx
    call print_number
    add sp, 2
    push offset msg_diff_char
    call putstr
    add sp, 2
    push ax
    call print_number
    add sp, 2
    call putnewline
    
cf_cs_next:
    pop bx
    inc bx
    pop cx
    jmp cf_cs_loop
    
cf_cs_done:
    mov ax, word ptr [bp-10]
    cmp ax, word ptr [bp-12]
    je cf_cs_check_no_diff
    
    mov word ptr [bp-14], 1
    push offset msg_diff_line_count
    call putstr
    add sp, 2
    
cf_cs_check_no_diff:
    cmp word ptr [bp-14], 0
    jne cf_ci_section
    
    push offset msg_no_diff
    call putstr
    add sp, 2
    
    ; --- Case-insensitive ---
cf_ci_section:
    push offset msg_case_insensitive
    call putstr
    add sp, 2
    
    mov cx, word ptr [bp-10]
    cmp cx, word ptr [bp-12]
    jbe cf_ci_start
    mov cx, word ptr [bp-12]
    
cf_ci_start:
    xor bx, bx
    mov word ptr [bp-14], 0
    
cf_ci_loop:
    cmp bx, cx
    jae cf_ci_done
    
    push cx
    push bx
    
    mov si, bx
    shl si, 1
    
    push ds
    mov ds, word ptr [bp-6]
    mov di, word ptr [si]
    pop ds
    
    push es
    mov es, word ptr [bp-8]
    mov si, word ptr es:[si]
    pop es
    
    push 1
    push si
    push word ptr [bp-4]
    push di
    push word ptr [bp-2]
    call compare_lines_by_offset
    add sp, 10
    
    cmp ax, -1
    je cf_ci_next
    
    mov word ptr [bp-14], 1
    
    push offset msg_diff_line
    call putstr
    add sp, 2
    push bx
    call print_number
    add sp, 2
    push offset msg_diff_char
    call putstr
    add sp, 2
    push ax
    call print_number
    add sp, 2
    call putnewline
    
cf_ci_next:
    pop bx
    inc bx
    pop cx
    jmp cf_ci_loop
    
cf_ci_done:
    mov ax, word ptr [bp-10]
    cmp ax, word ptr [bp-12]
    je cf_ci_check_no_diff
    
    mov word ptr [bp-14], 1
    push offset msg_diff_line_count
    call putstr
    add sp, 2
    jmp cf_cleanup
    
cf_ci_check_no_diff:
    cmp word ptr [bp-14], 0
    jne cf_cleanup
    
    push offset msg_no_diff
    call putstr
    add sp, 2
    jmp cf_cleanup
    
cf_no_lines:
    push offset msg_error_empty
    call putstr
    add sp, 2
    jmp cf_cleanup
    
cf_cleanup:
    cmp word ptr [bp-6], 0
    je cf_free2
    push word ptr [bp-6]
    call free_lines_array
    add sp, 2
    
cf_free2:
    cmp word ptr [bp-8], 0
    je cf_free3
    push word ptr [bp-8]
    call free_lines_array
    add sp, 2
    
cf_free3:
    cmp word ptr [bp-2], 0
    je cf_free4
    push word ptr [bp-2]
    call FreeMem
    add sp, 2
    
cf_free4:
    cmp word ptr [bp-4], 0
    je cf_done
    push word ptr [bp-4]
    call FreeMem
    add sp, 2
    jmp cf_done
    
cf_error:
    cmp word ptr [bp-2], 0
    je cf_err1
    push word ptr [bp-2]
    call FreeMem
    add sp, 2
cf_err1:
    cmp word ptr [bp-4], 0
    je cf_done
    push word ptr [bp-4]
    call FreeMem
    add sp, 2
    
cf_done:
    pop bx
    pop di
    pop si
    mov sp, bp
    pop bp
    ret
compare_files endp

; ============================================================================
; void main_process()
; ============================================================================
main_process proc near
    push bp
    mov bp, sp
    
    call putnewline
    
    push offset msg_enter_file1
    call putstr
    add sp, 2
    push 128
    push offset filepath1
    call getstr
    add sp, 4
    
    push offset msg_enter_file2
    call putstr
    add sp, 2
    push 128
    push offset filepath2
    call getstr
    add sp, 4
    
    call putnewline
    
    cmp byte ptr filepath1, 0
    je mp_empty
    cmp byte ptr filepath2, 0
    je mp_empty
    
    push offset filepath2
    push offset filepath1
    call compare_files
    add sp, 4
    jmp mp_done
    
mp_empty:
    push offset msg_error_empty
    call putstr
    add sp, 2
    
mp_done:
    mov sp, bp
    pop bp
    ret
main_process endp

_test proc near
    push bp
    mov bp, sp   

    call putnewline

    call test_strtol
    call test_strlen
    call test_strchr
    call test_strstr
    call test_strcmp
    call test_stricmp
    call test_strcpy
    call test_strcat 
    call putnewline

    mov sp, bp
    pop bp
    ret
_test endp
 
start:
    mov ax, data
    mov ds, ax
    mov es, ax
    mov ax, stack
    mov ss, ax
    nop

    call main_process
    
    call exit0
end_code_seg:
code ends

end start