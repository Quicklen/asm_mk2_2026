; ds - mat1 
; es - mat2
_mat_diff proc near
    push bp
    mov  bp, sp
    sub  sp, 16
    push si
    push di

    mov  byte ptr [bp-13], ' '
    mov byte ptr [bp-16], 13   ; '\r'
    mov byte ptr [bp-15], 10   ; '\n'
    mov byte ptr [bp-14], 0

    ; ѕроверка размеров
    mov  ax, word ptr ds:[MAT_DATA_OFFSET+2]  ; cols1
    cmp  ax, word ptr es:[MAT_DATA_OFFSET+2]  ; cols2
    jne  mat_diff_dim_mismatch
    mov  ax, word ptr ds:[MAT_DATA_OFFSET]    ; rows1
    cmp  ax, word ptr es:[MAT_DATA_OFFSET]    ; rows2
    je   mat_diff_continue

mat_diff_dim_mismatch:
    mov  ax, -1          ; возвращаем -1 как признак ошибки
    jmp  mat_diff_return

mat_diff_continue:
    xor  di, di                 ; r = 0

mat_diff_row_loop:
    xor  si, si                 ; c = 0

mat_diff_col_loop:
    ; diff = mat1[r][c] - mat2[r][c]
    mov  ax, di                 ; r
    mov  dx, word ptr ds:[MAT_DATA_OFFSET+2]  ; cols
    shl  dx, 1                  ; cols * 2
    imul dx                     ; r * cols * 2
    mov  dx, si                 ; c
    shl  dx, 1                  ; c * 2
    add  ax, dx
    mov  bx, ax
    mov  ax, word ptr ds:[bx]   ; mat1
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
    sub  ax, word ptr es:[bx]   ; mat2
    mov  word ptr [bp-2], ax    ; diff

    ; stoa(diff, num_buf)
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
    jge  mat_diff_end_row

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
    jmp  mat_diff_col_loop

mat_diff_end_row:
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
    jl   mat_diff_row_loop

    xor  ax, ax          ; возвращаем 0 = успех

mat_diff_return:
    pop  di
    pop  si
    mov  sp, bp
    pop  bp
    ret
_mat_diff endp