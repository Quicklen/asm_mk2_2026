stack_seg segment para stack 
    db 256 dup(?)
stack_seg ends

data_seg segment para public
    string_buffer       db 241             
                        db 0                   
                        db 241 dup(?)         
data_seg ends

code_seg segment para 
    assume cs:code_seg, ds:data_seg, ss:stack_seg



start proc

    mov ax, data_seg
    mov ds, ax

    mov dx, offset string_buffer
    mov ah, 0ah
    int 21h

    mov dl, 0dh   
    mov ah, 02h
    int 21h
    mov dl, 0ah   
	mov ah, 02h
    int 21h

    mov bl, string_buffer+1  
    mov bh, 0                
    lea si, string_buffer+2 
    add si, bx
    mov byte ptr [si], '$'

    mov dx, offset [string_buffer+2]
    mov ah, 09h
    int 21h

    mov dl, 0dh
    mov ah, 02h
    int 21h
    mov dl, 0ah
    int 21h

    mov ax, 4c00h           
    int 21h

start endp

code_seg ends

end start