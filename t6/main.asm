.386
mstack segment para stack use16
    db 65530 dup(?)
mstack ends
stack segment para stack use16
    db 65530 dup(?)
stack ends

data segment para public use16
    ; === Переменные для кейлоггера ===
    win_top         db 4
    win_left        db 2
    win_height      db 18
    win_width       db 76
    cur_row         dw 0
    cur_col         dw 0
    log_handle      dw 0
    orig_mode       db 00h

    char_to_write   db 0          

    ; Строки для спецклавиш
    str_esc         db '[ESC]', 0
    str_enter       db '[ENTER]', 0
    str_f1          db '[F1]', 0
    str_f2          db '[F2]', 0
    str_bs          db '[BS]', 0
    str_tab         db '[TAB]', 0
    str_shift       db '[SHIFT]', 0
    str_ctrl        db '[CTRL]', 0
    str_alt         db '[ALT]', 0
    str_caps        db '[CAPS]', 0
    str_unknown     db '[UNK]', 0

    str_prompt      db 'Enter log file path: $'
    str_err_create  db 'Error creating file! Press any key...$'
    str_success     db 'File OK! Press any key...$'
    str_welcome     db 'KeyLogger v1.0 | F1-Clear | F2-Reset | ESC-Exit$'
    default_name    db 'KEYLOG.DAT', 0
    prompt_buf      db 80 dup(0)
data ends

code segment para public use16
    assume cs:code, ds:data, ss:stack, es:data

; Макросы
pushregs macro r1,r2,r3,r4,r5,r6,r7,r8,r9,r10
    ifnb <r1>
        push r1
        pushregs r2,r3,r4,r5,r6,r7,r8,r9,r10
    endif
endm
popregs macro r1,r2,r3,r4,r5,r6,r7,r8,r9,r10
    ifnb <r1>
        pop r1
        popregs r2,r3,r4,r5,r6,r7,r8,r9,r10
    endif
endm


clear_screen proc near
    push bp
    mov bp, sp
    pushregs ax, cx, di
    mov ax, 0B800h
    mov es, ax
    xor di, di
    mov cx, 80*25
    mov ax, 1F00h + ' '
    rep stosw
    popregs di, cx, ax
    pop bp
    ret
clear_screen endp


draw_h_line proc near
    push bp
    mov bp, sp
    pushregs ax, cx, dx, di
    ; [bp+4] = flag, [bp+6] = width, [bp+8] = left
    mov ax, bx                 ; строка
    mov dl, 80
    mul dl
    add ax, [bp+8]             ; + left
    mov di, ax
    shl di, 1
    mov cx, [bp+6]
    add cx, 2                  ; общая ширина с уголками
    cmp word ptr [bp+4], 1
    jne draw_bottom_corner_left
    mov word ptr es:[di], 201 + 1Eh*256
    jmp draw_h_body
draw_bottom_corner_left:
    mov word ptr es:[di], 200 + 1Eh*256
draw_h_body:
    add di, 2
    dec cx
    dec cx
draw_h_loop:
    cmp cx, 0
    jle draw_h_end
    mov word ptr es:[di], 205 + 1Eh*256
    add di, 2
    dec cx
    jmp draw_h_loop
draw_h_end:
    cmp word ptr [bp+4], 1
    jne draw_bottom_corner_right
    mov word ptr es:[di], 187 + 1Eh*256
    jmp draw_h_done
draw_bottom_corner_right:
    mov word ptr es:[di], 188 + 1Eh*256
draw_h_done:
    popregs di, dx, cx, ax
    pop bp
    ret 6
draw_h_line endp


draw_v_line proc near
    push bp
    mov bp, sp
    pushregs ax, bx, cx, dx, di
    ; [bp+4]=row, [bp+6]=left, [bp+8]=width, [bp+10]=attr
    mov ax, [bp+4]
    mov dl, 80
    mul dl
    add ax, [bp+6]
    mov di, ax
    shl di, 1
    mov cx, [bp+8]
    add cx, 2
    mov word ptr es:[di], 186 + 1Eh*256
    add di, 2
    dec cx
    dec cx
    mov ah, byte ptr [bp+10]
    mov al, ' '
draw_v_loop:
    cmp cx, 0
    jle draw_v_end
    mov es:[di], ax
    add di, 2
    dec cx
    jmp draw_v_loop
draw_v_end:
    mov word ptr es:[di], 186 + 1Eh*256
    popregs di, dx, cx, bx, ax
    pop bp
    ret 8
draw_v_line endp

draw_window proc near
    push bp
    mov bp, sp
    pushregs ax, bx, cx, dx, di
    ; [bp+4]=top, [bp+6]=left, [bp+8]=height, [bp+10]=width, [bp+12]=attr
    mov ax, 0B800h
    mov es, ax

    ; сохраняем параметры окна в глобальные переменные
    mov al, byte ptr [bp+4]
    mov win_top, al
    mov al, byte ptr [bp+6]
    mov win_left, al
    mov al, byte ptr [bp+8]
    mov win_height, al
    mov al, byte ptr [bp+10]
    mov win_width, al

    ; верхняя линия
    mov bx, [bp+4]               ; row = top
    push word ptr [bp+6]         ; left
    push word ptr [bp+10]        ; width
    push 1                       ; flag (верх)
    call draw_h_line

    ; нижняя линия
    mov bx, [bp+4]
    add bx, [bp+8]
    dec bx                       ; row = top + height - 1
    push word ptr [bp+6]         ; left
    push word ptr [bp+10]        ; width
    push 0                       ; flag (низ)
    call draw_h_line

    ; вертикальные линии
    mov bx, [bp+4]
    inc bx                       ; начинаем со второй строки
draw_v_win_loop:
    mov ax, [bp+4]
    add ax, [bp+8]
    dec ax
    cmp bx, ax                   ; последняя строка перед нижней гранью
    jge draw_win_done
    push word ptr [bp+12]        ; attr
    push word ptr [bp+10]        ; width
    push word ptr [bp+6]         ; left
    push bx                      ; row
    call draw_v_line
    inc bx
    jmp draw_v_win_loop
draw_win_done:
    popregs di, dx, cx, bx, ax
    pop bp
    ret 10
draw_window endp


scroll_log_up proc near
    push bp
    mov bp, sp
    pushregs ax, bx, cx, dx, si, di, ds
    mov ax, 0B800h
    mov ds, ax
    mov es, ax

    ; источник: вторая строка содержимого
    mov al, win_top
    add al, 2
    mov dl, 80
    mul dl
    add al, win_left
    inc al
    shl ax, 1
    mov si, ax

    ; приёмник: первая строка содержимого
    mov al, win_top
    inc al
    mov dl, 80
    mul dl
    add al, win_left
    inc al
    shl ax, 1
    mov di, ax

    mov al, win_height
    sub al, 2
    mov dl, win_width
    mul dl
    mov cx, ax
    cld
    rep movsw

    ; очистка последней строки
    mov al, win_top
    add al, win_height
    dec al
    mov dl, 80
    mul dl
    add al, win_left
    inc al
    shl ax, 1
    mov di, ax
    mov cl, win_width
clear_last_line_loop:
    cmp cl, 0
    jle scroll_done
    mov word ptr es:[di], 20h + 1Fh*256
    add di, 2
    dec cl
    jmp clear_last_line_loop
scroll_done:
    mov al, win_height
    sub al, 2
    mov ah, 0
    mov cur_row, ax
    mov cur_col, 0
    popregs ds, di, si, dx, cx, bx, ax
    pop bp
    ret
scroll_log_up endp

print_char_to_log proc near
    push bp
    mov bp, sp
    pushregs ax, bx, cx, dx, di, es

    ; ----- ОТЛАДКА: вывести '*' в (0,0) ярко-жёлтым -----
    push 0B800h
    pop es
    mov di, 0
    mov word ptr es:[di], 0E00h + '*'
    ; -------------------------------------------------

    mov al, [bp+4]      ; символ
    mov ah, [bp+6]      ; атрибут

    ; проверка переполнения строки
    mov bl, win_width
    mov bh, 0
    cmp cur_col, bx
    jne check_row_overflow
    mov cur_col, 0
    inc cur_row

check_row_overflow:
    mov bl, win_height
    sub bl, 2
    mov bh, 0
    cmp cur_row, bx
    jl place_char
    call scroll_log_up

place_char:
    ; вычисляем позицию БЕЗОПАСНО
    movzx bx, win_top
    add bx, cur_row       ; bx = top + row
    inc bx                ; +1
    mov ax, 80
    mul bx                ; ax = (top+row+1)*80
    movzx bx, win_left
    add ax, bx            ; + left
    movzx bx, byte ptr cur_col
    add ax, bx            ; + col
    inc ax                ; +1
    shl ax, 1             ; *2
    mov di, ax

    mov ax, 0B800h
    mov es, ax
    mov al, [bp+4]
    mov ah, [bp+6]
    mov es:[di], ax

    ; запись в файл (оставляем как было)
    cmp log_handle, 0
    je skip_file_write
    push ds
    mov ax, data
    mov ds, ax
    mov al, [bp+4]
    mov char_to_write, al
    mov ah, 40h
    mov bx, log_handle
    mov cx, 1
    lea dx, char_to_write
    int 21h
    pop ds
skip_file_write:

    inc cur_col
    popregs es, di, dx, cx, bx, ax
    pop bp
    ret 4
print_char_to_log endp

print_string_to_log proc near
    push bp
    mov bp, sp
    pushregs si, ax
    mov si, [bp+4]
print_str_loop:
    mov al, [si]
    test al, al
    jz print_str_done
    push 1Fh
    mov ah, 0
    push ax
    call print_char_to_log
    inc si
    jmp print_str_loop
print_str_done:
    popregs ax, si
    pop bp
    ret 2
print_string_to_log endp


clear_log_window proc near
    push bp
    mov bp, sp
    pushregs ax, bx, cx, dx, di
    mov ax, 0B800h
    mov es, ax

    mov cl, win_height
    sub cl, 2
    mov ch, 0
    mov bl, win_top
    inc bl
clear_rows_loop:
    push cx
    mov al, 80
    mul bl
    add al, win_left
    inc al
    shl ax, 1
    mov di, ax
    mov cl, win_width
    mov ch, 0
clear_cols_loop:
    mov word ptr es:[di], 20h + 1Fh*256
    add di, 2
    loop clear_cols_loop
    inc bl
    pop cx
    loop clear_rows_loop

    mov cur_row, 0
    mov cur_col, 0
    popregs di, dx, cx, bx, ax
    pop bp
    ret
clear_log_window endp


get_special_key_str proc near
    push bp
    mov bp, sp
    pushregs bx
    mov bl, [bp+4]
    cmp bl, 01h
    je return_esc
    cmp bl, 0Eh
    je return_bs
    cmp bl, 0Fh
    je return_tab
    cmp bl, 1Ch
    je return_enter
    cmp bl, 3Bh
    je return_f1
    cmp bl, 3Ch
    je return_f2
    cmp bl, 2Ah
    je return_shift
    cmp bl, 1Dh
    je return_ctrl
    cmp bl, 38h
    je return_alt
    cmp bl, 3Ah
    je return_caps
    lea ax, str_unknown
    jmp ret_name
return_esc:     lea ax, str_esc
return_bs:      lea ax, str_bs
return_tab:     lea ax, str_tab
return_enter:   lea ax, str_enter
return_f1:      lea ax, str_f1
return_f2:      lea ax, str_f2
return_shift:   lea ax, str_shift
return_ctrl:    lea ax, str_ctrl
return_alt:     lea ax, str_alt
return_caps:    lea ax, str_caps
ret_name:
    popregs bx
    pop bp
    ret 2
get_special_key_str endp


input_string proc near
    push bp
    mov bp, sp
    pushregs ax, bx, cx, dx, si
    mov bx, [bp+4]          ; буфер
    mov cx, [bp+6]          ; макс. длина
    xor si, si
    dec cx
input_loop:
    mov ah, 08h
    int 21h
    cmp al, 0Dh
    je input_done
    cmp al, 08h
    je backspace_pressed
    cmp si, cx
    jae input_loop
    mov [bx+si], al
    inc si
    push ax
    mov ah, 02h
    mov dl, al
    int 21h
    pop ax
    jmp input_loop
backspace_pressed:
    test si, si
    jz input_loop
    dec si
    push ax
    mov ah, 02h
    mov dl, 08h
    int 21h
    mov dl, ' '
    int 21h
    mov dl, 08h
    int 21h
    pop ax
    jmp input_loop
input_done:
    mov byte ptr [bx+si], 0
    push ax
    mov ah, 02h
    mov dl, 0Dh
    int 21h
    mov dl, 0Ah
    int 21h
    pop ax
    mov ax, si
    popregs si, dx, cx, bx, ax
    pop bp
    ret 4
input_string endp


create_log_file proc near
    push bp
    mov bp, sp
    pushregs ax, bx, cx, dx, si, di, ds, es
    cmp byte ptr prompt_buf, 0
    jne try_create
    lea si, default_name
    lea di, prompt_buf
    mov cx, 11
    cld
    rep movsb
try_create:
    push ds
    mov ax, data
    mov ds, ax
    mov ah, 3Ch
    mov cx, 20h
    lea dx, prompt_buf
    int 21h
    jnc file_created
    mov ah, 3Dh
    mov al, 2
    lea dx, prompt_buf
    int 21h
    jnc file_opened
    xor ax, ax
    jmp create_done
file_created:
    mov log_handle, ax
    mov ax, 1
    jmp create_done
file_opened:
    mov log_handle, ax
    mov ah, 42h
    mov al, 2
    mov bx, log_handle
    xor cx, cx
    xor dx, dx
    int 21h
    mov ax, 1
create_done:
    pop ds
    popregs es, ds, di, si, dx, cx, bx, ax
    pop bp
    ret
create_log_file endp

reset_log_file proc near
    push bp
    mov bp, sp
    pushregs ax, bx, cx, dx
    cmp log_handle, 0
    je reset_done
    mov ah, 3Eh
    mov bx, log_handle
    int 21h
    mov log_handle, 0
    mov ah, 3Ch
    mov cx, 20h
    lea dx, prompt_buf
    int 21h
    jc reset_done
    mov log_handle, ax
reset_done:
    call clear_log_window
    popregs dx, cx, bx, ax
    pop bp
    ret
reset_log_file endp

get_key proc near
    push bp
    mov bp, sp
    mov ah, 00h
    int 16h
    pop bp
    ret
get_key endp


log_newline proc near
    push bp
    mov bp, sp
    pushregs ax
    mov cur_col, 0
    inc word ptr cur_row
    mov al, win_height
    sub al, 2
    cmp byte ptr cur_row, al
    jl newline_ok
    call scroll_log_up
newline_ok:
    popregs ax
    pop bp
    ret
log_newline endp


main proc near
    push bp
    mov bp, sp

    mov ah, 0Fh
    int 10h
    mov orig_mode, al
    mov ax, 0003h
    int 10h
    call clear_screen

    ; ======== Окно запроса пути ========
    push 1Fh
    push 40
    push 5
    push 20
    push 8
    call draw_window

    mov ah, 02h
    mov bh, 0
    mov dh, 10
    mov dl, 22
    int 10h
    lea dx, str_prompt
    mov ah, 09h
    int 21h

    mov ah, 02h
    mov bh, 0
    mov dh, 12
    mov dl, 22
    int 10h
    push 60
    lea dx, prompt_buf
    push dx
    call input_string
    add sp, 4

    call create_log_file
    test ax, ax
    jnz file_ok

    mov ah, 02h
    mov bh, 0
    mov dh, 14
    mov dl, 18
    int 10h
    lea dx, str_err_create
    mov ah, 09h
    int 21h
    call get_key
    jmp exit_app

file_ok:
    mov ah, 02h
    mov bh, 0
    mov dh, 14
    mov dl, 20
    int 10h
    lea dx, str_success
    mov ah, 09h
    int 21h
    call get_key

    ; ======== Основное окно ========
    call clear_screen
    push 1Fh
    push 76
    push 18
    push 2
    push 4
    call draw_window

    mov ah, 02h
    mov bh, 0
    mov dh, 23
    mov dl, 2
    int 10h
    lea dx, str_welcome
    mov ah, 09h
    int 21h

    ; Явная установка параметров (как в оригинале)
    mov win_top, 4
    mov win_left, 2
    mov win_height, 18
    mov win_width, 76
    mov cur_row, 0
    mov cur_col, 0

    ; === ТЕСТОВАЯ ПРЯМАЯ ЗАПИСЬ (красная 'T') ===
    push 0B800h
    pop es
    mov di, (5*80 + 3)*2
    mov word ptr es:[di], 4F00h + 'T'

    ; === ТЕСТОВЫЙ СИМВОЛ 'A' (через процедуру) ===
    push 1Fh
    push 'A'
    call print_char_to_log   


main_loop:
    call get_key
    push ax

    cmp al, 0
    je show_special_key
    cmp al, 13
    je show_enter_key
    cmp al, 8
    je show_bs_key
    cmp al, 9
    je show_tab_key

    push 1Fh
    mov ah, 0
    push ax
    call print_char_to_log
    jmp display_done

show_enter_key:
    push offset str_enter
    call print_string_to_log
    call log_newline
    jmp display_done
show_bs_key:
    push offset str_bs
    call print_string_to_log
    jmp display_done
show_tab_key:
    push offset str_tab
    call print_string_to_log
    jmp display_done
show_special_key:
    mov al, ah
    mov ah, 0
    push ax
    call get_special_key_str
    push ax
    call print_string_to_log
    add sp, 2

display_done:
    pop ax

    cmp ah, 01h
    je exit_app
    cmp ah, 3Bh
    jne check_f2
    call clear_log_window
    jmp main_loop
check_f2:
    cmp ah, 3Ch
    jne main_loop
    call reset_log_file
    jmp main_loop

exit_app:
    cmp log_handle, 0
    je restore_mode
    mov ah, 3Eh
    mov bx, log_handle
    int 21h

restore_mode:
    mov al, orig_mode
    mov ah, 0
    int 10h

    pop bp
    ret
main endp

start:
    mov ax, data
    mov ds, ax
    mov es, ax
    mov ax, stack
    mov ss, ax
    call main
    mov ah, 4Ch
    mov al, 0
    int 21h

code ends
end start