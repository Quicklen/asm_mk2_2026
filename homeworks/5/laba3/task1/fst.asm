; 1. Написать программу считывания строки с клавиатуры и 
; а) вывода этой строки в обратном порядке, 
; б) вывода этой строки n раз подряд в цикле.
; Все циклы реализовать с помощью инструкций переходов.

.386
stack segment para stack
db 256 dup(?)
stack ends 

data segment para public
    str1 db 241 dup(?)      
    real_len db ?
    n db 3                
    crlf db 13, 10, "$"    
    count dw ?             
data ends

code segment para public use16

assume cs:code, ds:data, ss:stack

start:
    mov ax, data
    mov ds, ax

    mov dx, offset str1
    mov byte ptr [str1], 240   
    mov ah, 0ah
    int 21h
    
    xor bx, bx
    mov bl, [str1+1]  
    mov [real_len], bl
    
  
    mov dx, offset crlf
    mov ah, 09h
    int 21h

    mov ch, 0
    mov cl, [real_len]    
    mov si, cx             
    
reverse_loop:
    dec si                
    mov dl, [str1 + 2 + si] 
    mov ah, 02h
    int 21h                 
    
    dec cl                
    cmp cl, 0
    jne reverse_loop      

    mov dx, offset crlf
    mov ah, 09h
    int 21h
    
    mov al, [n]             
    mov ah, 0
    mov [count], ax         
    
outer_loop:
    cmp word ptr [count], 0 
    je exit                 

    mov ch, 0
    mov cl, [real_len]      
    xor si, si              
    
inner_loop:
    mov dl, [str1 + 2 + si] 
    mov ah, 02h
    int 21h               
    
    inc si                
    dec cl                 
    cmp cl, 0
    jne inner_loop        

    mov dx, offset crlf
    mov ah, 09h
    int 21h
    
    dec word ptr [count]    
    jmp outer_loop          
    
exit:
    mov ax, 4c00h
    int 21h
    
code ends
end start