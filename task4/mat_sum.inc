; ds - mat1 
; es - mat2
_mat_sum proc near
    push bp
    mov  bp, sp
    sub  sp, 16
    push si
    push di

    mov  byte ptr [bp-13], ' '
    mov byte ptr [bp-16], 13
    mov byte ptr [bp-15], 10
    mov byte ptr [bp-14], 0

    ; Проверка размеров
    mov  ax, word ptr ds:[MAT_DATA_OFFSET+2]  ; cols1
    cmp  ax, word ptr es:[MAT_DATA_OFFSET+2]  ; cols2
    jne  mat_sum_dim_mismatch
    mov  ax, word ptr ds:[MAT_DATA_OFFSET]    ; rows1
    cmp  ax, word ptr es:[MAT_DATA_OFFSET]    ; rows2
    je   mat_sum_continue

mat_sum_dim_mismatch:
    mov  ax, -1          ; возвращаем -1 как признак ошибки
    jmp  mat_sum_return

mat_sum_continue:
    ; Вывод матрицы
    xor  di, di                 ; r = 0

mat_sum_row_loop:
    xor  si, si                 ; c = 0

mat_sum_col_loop:
    ; sum = mat1[r][c] + mat2[r][c]
    ; offset = r * cols * 2 + c * 2
    mov  ax, di                 ; r
    mov  dx, word ptr ds:[MAT_DATA_OFFSET+2]  ; cols
    shl  dx, 1                  ; cols * 2
    imul dx                     ; r * cols * 2
    mov  dx, si                 ; c
    shl  dx, 1                  ; c * 2
    add  ax, dx
    mov  bx, ax
    mov  ax, word ptr ds:[bx]   ; mat1[r][c]
    push ax

    mov  ax, di                 ; r
    mov  dx, word ptr es:[MAT_DATA_OFFSET+2]  ; cols
    shl  dx, 1
    imul dx
    mov  dx, si                 ; c
    shl  dx, 1
    add  ax, dx
    mov  bx, ax
    pop  ax
    add  ax, word ptr es:[bx]   ; mat2[r][c]
    mov  word ptr [bp-2], ax    ; sum

    ; stoa(sum, num_buf)
    push ds
    mov ax, ss
    mov ds, ax
    lea  ax, word ptr [bp-12]
    push ax
    push word ptr [bp-2]
    call near ptr _stoa
    pop  cx
    pop  cx
    pop ds

    ; strlen(num_buf)
    push ds
    mov ax, ss
    mov ds, ax
    lea  ax, word ptr [bp-12]
    push ax
    call near ptr _strlen
    pop  cx
    mov  word ptr [bp-4], ax
    pop ds

    ; fwrite
    push ds
    mov ax, ss
    mov ds, ax
    push word ptr [bp-4]
    lea  ax, word ptr [bp-12]
    push ax
    push word ptr [bp+4]
    call near ptr _fwrite
    add  sp, 6
    pop ds

    inc  si
    cmp  si, word ptr ds:[MAT_DATA_OFFSET+2]  ; c < cols ?
    jge  mat_sum_end_row

    ; пробел
    push ds
    mov ax, ss
    mov ds, ax
    push 1
    lea  ax, word ptr [bp-13]
    push ax
    push word ptr [bp+4]
    call near ptr _fwrite
    add  sp, 6
    pop ds
    jmp  mat_sum_col_loop

mat_sum_end_row:
    ; newline
    push ds
    mov ax, ss
    mov ds, ax
    push 2
    lea  ax, word ptr [bp-16]
    push ax
    push word ptr [bp+4]
    call near ptr _fwrite
    add  sp, 6
    pop ds

    inc  di
    cmp  di, word ptr ds:[MAT_DATA_OFFSET]  ; r < rows ?
    jl   mat_sum_row_loop

    xor  ax, ax          ; возвращаем 0 = успех

mat_sum_return:
    pop  di
    pop  si
    mov  sp, bp
    pop  bp
    ret
_mat_sum endp