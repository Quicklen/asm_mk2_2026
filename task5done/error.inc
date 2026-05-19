
fio_error_handler proc near 
    push bp
    mov bp, sp

    mov  bx, word ptr [bp+arg1]        ; BX = error_code
    shl  bx, 1
    mov  dx, word ptr [fio_err_vec + bx] 

    push dx
    call _putstr
    add sp, 2

    push 2
    call exit
    add sp, 2

    mov sp, bp
    pop bp
    ret
fio_error_handler endp 