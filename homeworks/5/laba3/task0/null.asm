.386

stack segment para stack
db 256 dup (?)
stack ends 

data segment para public
  src_string db "Try find symbol!"
  new_line db 0dh, 0ah, "$"
  src_len dw ?
  success_str db " - Symbol was found!", 0dh, 0ah, "$"
  error_str db " - Symbol wasn't found (((", 0dh, 0ah, "$"
  symbol_again db "GO NOVI SYMBOL", 0Dh, 0Ah, "$"
  schet db 0 ; счетчик запросов
  schet2 db 0 ; счетчик пустых символов
  reserved db 256 dup (?) 
data ends

code segment para public use16

assume cs:code,ds:data,ss:stack

start:
  ; инициализация сегментных регистров
	mov ax, data
    mov ds, ax
    mov ax, stack
    mov ss, ax
	nop
  
again:
	mov dx, offset symbol_again
	mov ah, 09h
	int 21h 
	  
	mov ah, 01h
	int 21h
	mov byte ptr [reserved], al
	  
	mov dx, offset new_line
	mov ah, 09h
	int 21h
	  
	mov al, byte ptr [reserved]
	mov cx, offset new_line
	mov bx, offset src_string
	sub cx, bx  ; cx = длина строки
	mov word ptr [src_len], cx
	  
	dec bx
search:
	inc bx
	cmp al, byte ptr [bx]
	loopne search      
		
	je found
	
	xor dx, dx
	mov dl, al
	mov ah, 02h
	int 21h
	
	mov dx, offset error_str
	jmp print
found:
	xor dx, dx
	mov dl, al
	mov ah, 02h
	int 21h
	
	mov dx, offset success_str
	
print:
	mov ah, 09h
	int 21h
	
	cmp [reserved], 0Dh
	je increment
	mov [schet2], 0

incremnt:
	inc [schet]
	cmp [schet], 5
	je obnul
	jmp again

obnul:
	mov [schet], 0
	mov dx, offset src_string
	mov ah, 09h
	int 21h
	jmp again
increment:
	inc [schet2]
	cmp [schet2], 2
	je exit
	jmp incremnt
exit:
	mov ax, 4c00h
	int 21h
  
code ends

end start