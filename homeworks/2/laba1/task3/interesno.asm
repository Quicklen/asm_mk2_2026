stack segment para stack
db 256 dup(?)
stack ends

data segment para public

	str_max db 240         	
	str_len db ?           
	str_str db 256 dup(?)  
	new_line db 0ah,0dh,'$'	

data ends

code segment para public 

assume cs:code,ds:data,ss:stack

start:
    mov ax, data
    mov ds, ax
    mov ax, stack
    mov ss, ax
    
    mov ah, 0ah
    mov dx, offset str_max
    int 21h
    
    mov ah, 09h
    mov dx, offset new_line
    int 21h
    
    mov bx, 0 ; 
    mov bl, byte ptr[str_len] 
    mov si, offset str_str
    mov byte ptr[si+bx], '$' 
    mov ah, 09h
    mov dx, offset str_str
    int 21h
    
    mov ah, 4ch
    mov al, 00h
	int 21h
code ends

end start