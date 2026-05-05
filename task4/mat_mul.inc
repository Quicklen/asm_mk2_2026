_mat_mul proc near
    push bp
    mov  bp, sp
    sub  sp, 18
    push si
    push di

    mov  byte ptr [bp-15], ' '
    mov  byte ptr [bp-18], 13   ; '\r'
    mov  byte ptr [bp-17], 10   ; '\n'
    mov  byte ptr [bp-16], 0

    ; Проверка совместимости: cols1 == rows2
    mov  ax, word ptr ds:[MAT_DATA_OFFSET+2]  ; cols1
    cmp  ax, word ptr es:[MAT_DATA_OFFSET]    ; rows2
    je   mat_mul_continue
    mov  ax, -1          ; возвращаем -1 = ошибка
    jmp  mat_mul_exit

mat_mul_continue:
    ; Внешний цикл по строкам результата (r = 0..rows1-1)
    xor  di, di                 ; r
    jmp  check_r

row_loop:
    ; Цикл по столбцам результата (c = 0..cols2-1)
    xor  si, si                 ; c
    jmp  check_c

col_loop:
    mov  word ptr [bp-2], 0     ; sum = 0

    ; Цикл по k (0..cols1-1)
    xor  bx, bx                 ; k
    jmp  check_k

k_loop:
    ; A[r][k] = ds:[ r*cols1*2 + k*2 ]
    mov  ax, di                 ; r
    mov  dx, word ptr ds:[MAT_DATA_OFFSET+2]  ; cols1
    shl  dx, 1
    imul dx                     ; ax = r * cols1 * 2
    mov  dx, bx                 ; k
    shl  dx, 1                  ; dx = k * 2
    add  ax, dx                 ; ax = итоговое смещение
    push si                     ; сохраняем c (si)
    mov  si, ax                 ; временно помещаем смещение в si
    mov  ax, word ptr ds:[si]   ; ax = mat1[r][k]
    mov  cx, ax                 ; сохраняем в cx
    pop  si                     ; восстанавливаем c

    ; B[k][c] = es:[ k*cols2*2 + c*2 ]
    mov  ax, bx                 ; k
    mov  dx, word ptr es:[MAT_DATA_OFFSET+2]  ; cols2
    shl  dx, 1
    imul dx                     ; ax = k * cols2 * 2
    mov  dx, si                 ; c
    shl  dx, 1                  ; dx = c * 2
    add  ax, dx                 ; ax = итоговое смещение
    push di                     ; сохраняем r (di)
    mov  di, ax                 ; помещаем смещение в di
    mov  ax, word ptr es:[di]   ; ax = mat2[k][c]
    pop  di                     ; восстанавливаем r

    imul cx                     ; ax = mat1 * mat2
    add  word ptr [bp-2], ax    ; sum += произведение

    inc  bx                     ; k++
check_k:
    cmp  bx, word ptr ds:[MAT_DATA_OFFSET+2]  ; k < cols1 ?
    jl   k_loop

    ; stoa(sum, num_buf)
    push ds
    mov  ax, ss
    mov  ds, ax
    lea  ax, word ptr [bp-10]   ; num_buf
    push ax
    push word ptr [bp-2]        ; sum
    call near ptr _stoa
    pop  cx
    pop  cx
    pop  ds

    ; strlen(num_buf)
    push ds
    mov  ax, ss
    mov  ds, ax
    lea  ax, word ptr [bp-10]
    push ax
    call near ptr _strlen
    pop  cx
    mov  word ptr [bp-4], ax    ; len
    pop  ds

    ; fwrite(fd, num_buf, len)
    push ds
    mov  ax, ss
    mov  ds, ax
    push word ptr [bp-4]
    lea  ax, word ptr [bp-10]
    push ax
    push word ptr [bp+4]        ; fd
    call near ptr _fwrite
    add  sp, 6
    pop  ds

    inc  si                     ; c++
check_c:
    cmp  si, word ptr es:[MAT_DATA_OFFSET+2]  ; c < cols2 ?
    jge  end_col_loop

    ; Пробел между элементами (если не последний столбец)
    push ds
    mov  ax, ss
    mov  ds, ax
    push 1
    lea  ax, word ptr [bp-15]   ; space
    push ax
    push word ptr [bp+4]
    call near ptr _fwrite
    add  sp, 6
    pop  ds
    jmp  col_loop

end_col_loop:
    ; В конце строки – перевод строки
    push ds
    mov  ax, ss
    mov  ds, ax
    push 2
    lea  ax, word ptr [bp-18]   ; newline
    push ax
    push word ptr [bp+4]
    call near ptr _fwrite
    add  sp, 6
    pop  ds

    inc  di                     ; r++
check_r:
    cmp  di, word ptr ds:[MAT_DATA_OFFSET]  ; r < rows ?
    jl   row_loop

    xor  ax, ax                 ; возвращаем 0 = успех
mat_mul_exit:
    pop  di
    pop  si
    mov  sp, bp
    pop  bp
    ret
_mat_mul endp