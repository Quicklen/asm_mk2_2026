; access_mode = 
;   0000 - read 
;   0001 - write
;   0010 - read/write 
; short fopen(short access_mode, const char* filename)
; cf flag - as error and return -1
fopen proc near 
    push bp
    mov bp, sp
    push cx
    push dx

    mov cx, word ptr[bp+arg1]

    mov dx, word ptr[bp+arg2] ; filename
    mov al, cl                ; access mode
    mov ah, 3Dh               ; 3dh - open file
    int 21h
    jc fopen_error ; если CF = 1 — ошибка

fopen_done:
    pop dx
    pop cx
    mov sp, bp 
    pop bp
    ret

fopen_error:
    jmp fopen_done
fopen endp


; bool fclose(short d)
; закрыть файловый дескриптор.
; cf - flag and return -1
fclose proc near
    push bp
    mov bp, sp
    push bx

    mov ah, 3eh
    mov bx, word ptr [bp+arg1]
    int 21h
    jc fclose_error

    xor ax, ax 
fclose_done:
    pop bx
    mov sp, bp
    pop bp
    ret

fclose_error:
    mov ax, -1
    jmp fclose_done

fclose endp

; rel = 
; 0 - from file start
; 1 - current pos
; 2 - end of file
; int lseek(short d, short offset_high_word, short offset_low_words, short rel)
; dx:ax - new file pos
; on error cf flag

; Назначение: позиционировать файловый указатель.
; Возвращает: dx:ax = новая позиция; CF=1 при ошибке.

; Назначение: перемещает файловый указатель (позицию чтения/записи).
; Аргументы:
;   [bp+arg1] = rel              — точка отсчёта (0, 1 или 2)
;   [bp+arg2] = offset_high_word — старшее слово смещения (обычно 0 для файлов < 64К)
;   [bp+arg3] = offset_low_word  — младшее слово смещения
;   [bp+arg4] = d                — файловый дескриптор
lseek proc near 
    push bp
    mov bp, sp
    push cx
    push bx

    mov bx, word ptr [bp+arg4]; BX = file handle
    mov dx, word ptr[bp+arg2] ; offset high
    mov cx, word ptr[bp+arg3] ; offset low

    mov ah, 42h
    mov al, byte ptr[bp+arg1]

    int 21h

    pop bx
    pop cx
    mov sp, bp
    pop bp
    ret
lseek endp

; long fsize(int d)
; узнать размер файла в байтах.
; Аргумент:
;   [bp+arg1] = d — файловый дескриптор
; Возвращает:
;   dx:ax = размер файла (dx — старшее слово, ax — младшее)
;   dx:ax = -1 при ошибке (и CF=1)

; short fsize(int d)
fsize proc near
    push bp
    mov bp, sp
    sub sp, 4                      ; [bp-2] = current_pos_low
                                   ; [bp-4] = current_pos_high

    ;получаем текущую позицию 

    push word ptr [bp+arg1]        
    push 0                          
    push 0                         
    push SEEK_CURRENT_POS           
    call lseek                      
    add sp, 8                       
    jc fsize_error

    mov word ptr [bp-2], ax        ; сохраняем текущую позицию (младшее слово)
    mov word ptr [bp-4], dx        ; сохраняем текущую позицию (старшее слово)

    ; получаем размер файла 
    push word ptr [bp+arg1]         
    push 0                          ; offset_high = 0
    push 0                          ; offset_low = 0
    push SEEK_END                   ; rel = 2 
    call lseek                      ; перемещаемся в конец: dx:ax = размер файла
    jc fsize_error                  ; если CF=1 — ошибка
    add sp, 8                       

    push dx                         ; сохраняем размер файла
    push ax

    ; возвращаем указатель в исходную позицию
    push word ptr [bp+arg1]         
    push word ptr [bp-4]            ; original high
    push word ptr [bp-2]            ; original low
    push SEEK_START                 ; rel = 0 (от начала)
    call lseek                      ; перемещаемся в исходную позицию
    add sp, 8                       

    pop ax                          ; размер файла (младшее слово)
    pop dx                          ; размер файла (старшее слово)

fsize_done:
    mov sp, bp
    pop bp
    ret

fsize_error:                        
    add sp, 8                       
    mov ax, -1                     
    cwd                             
                                    
    jmp fsize_done

fsize endp

; ds:buffer
; short fread(short d, char* buffer, const short len)
;   [bp+arg1] = d      — файловый дескриптор
;   [bp+arg2] = buffer — адрес буфера (в сегменте данных ds)
;   [bp+arg3] = len    — максимальное количество байт для чтения
; Возвращает:
;   ax = количество реально прочитанных байт
;   ax = 0 при ошибке чтения или конце файла

fread proc near 
    push bp
    mov bp, sp
    push bx
    push cx
    push dx
    
    mov ah, 3Fh                  
    mov bx, word ptr [bp+arg1]   ; file handle
    mov cx, word ptr [bp+arg3]   ; количество байт
    mov dx, word ptr [bp+arg2]   ; буфер (смещение в ds)
    int 21h                      
    
    jc fread_error               ; если CF=1 - ошибка
    
    ; ax содержит количество прочитанных байт
    jmp fread_done
    
fread_error:
    xor ax, ax                   ; возвращаем 0 при ошибке
    
fread_done:
    pop dx
    pop cx
    pop bx
    mov sp, bp
    pop bp
    ret
fread endp

; ds:buffer
; short fwrite(short d, char* buffer, short size) 
; Аргументы:
;   [bp+arg1] = d      — файловый дескриптор
;   [bp+arg2] = buffer — адрес буфера с данными (ds:dx)
;   [bp+arg3] = size   — количество байт для записи
; Возвращает:
;   ax = количество фактически записанных байт
;   CF = 1 при ошибке, ax = код ошибки

fwrite proc near
    push bp
    mov bp, sp
    
    mov ah, 40h
    mov bx, word ptr[bp+arg1]
    mov cx, word ptr[bp+arg3]
    mov dx, word ptr[bp+arg2]
    int 21h

    mov sp, bp
    pop bp
    ret
fwrite endp

; fread to string
; char* freadtos(short d, short count_of_bytes)
; читает данные из файла в динамически выделенную память
; возвращает указатель на строку (сегмент).
; Аргументы:
;   [bp+arg1] = d              — файловый дескриптор
;   [bp+arg2] = count_of_bytes — количество байт для чтения
; Возвращает:
;   ax = сегментный адрес строки с прочитанными данными (при успехе)
;   ax = 0, CF = 1 при ошибке

freadtos proc near
    push bp
    mov bp, sp
    push ds
    push bx
    push cx
    push dx

    mov bx, word ptr [bp+arg1]

    inc bx
    push bx 
    call AllocMem
    jc freadtos_alloc_failed
    add sp, 2

    mov es, ax

    push ds
    push es
    pop ds ; ds = es

    push word ptr [bp+arg1]
    push 0 ; es:[0]
    push word ptr [bp+arg2]
    call fread
    add sp, 6
    jc freadtos_read_failed

    pop ds ; ds = ds
    mov ax, es
    clc
freadtos_done:
    pop dx
    pop cx
    pop bx
    pop ds
    mov sp, bp
    pop bp
    ret
freadtos_read_failed:
    pop ds
    push es
    call FreeMem
    add sp, 2
    xor ax, ax
    stc
    jmp freadtos_done

freadtos_alloc_failed:
    add sp, 2
    xor ax, ax
    stc
    jmp freadtos_done
freadtos endp