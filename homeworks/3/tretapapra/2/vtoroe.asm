stack_seg segment para stack
    db 256 dup(?)
stack_seg ends

data_seg segment para public
    adres db "y.txt", 0  
    str db 241 dup(?)  
    from_file dw 240 dup(?)
	from_file_str dw ?
    reserv dw ?
    adres_res db ?
    real_len db ?
	read_buf dw ?
data_seg ends

code_seg segment para

assume cs:code_seg, ds:data_seg, ss:stack_seg

start:
    mov ax, data_seg
    mov ds, ax
    mov ax, stack_seg
    mov ss, ax 

    mov dx, offset str
    mov byte ptr [str], 240  
    mov ah, 0ah
    int 21h

    mov bl, [str+1]  
    mov [real_len], bl

    mov dx, offset adres
    mov cx, 0  
    mov ah, 3ch
    int 21h

    mov [reserv], ax  
	
	mov dx, offset adres
    mov al, 2
    mov ah, 3dh
    int 21h

    mov bx, [reserv]  
    mov dx, offset str+2 
    xor ch, ch
    mov cl, [real_len]  
    mov ah, 40h
    int 21h

    mov bx, [reserv]
    mov ah, 3eh
    int 21h

    mov dx, offset adres
    mov al, 0
    mov ah, 3dh
    int 21h
    
    mov [reserv], ax  

    mov bx, [reserv]
    mov dx, offset from_file
	xor ch, ch
    mov cl, [real_len] 
    mov ah, 3fh
    int 21h
	
	mov [read_buf], ax

	mov bx, [reserv]
    mov ah, 3eh
    int 21h
	
    lea si, [from_file]
	mov bx, [read_buf]      
	mov byte ptr [si+bx], '$'
    
	mov dx, 0Dh
	mov ah, 02h
	int 21h
	
	mov dx, 0Ah
	mov ah, 02h
	int 21h
	
	xor dx, dx
    mov dx, offset from_file
    mov ah, 09h 
    int 21h
    
    
    mov ax, 4c00h
    int 21h

code_seg ends
end start