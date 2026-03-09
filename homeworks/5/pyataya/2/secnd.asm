; 2. Написать программу для проверки каждого символа введенной строки 
; (кроме завершающего) на принадлежность к определенному диапазону в таблице ASCII. 
; Диапазон символов запрашивать у пользователя на старте программы.
; При первом несовпадении выводить сообщение в формате:
; "Some character in the string is not within the specified range." 
; И завершать программу с кодом -1.
; Или, если все совпало:
; "All characters in a string within the specified range." 
; И завершать программу с кодом 0.

stack segment para stack
db 256 dup(?)
stack ends

data segment para public
    msg_first db "Input first character of range: $"
    msg_second db "Input last character of range: $"
    msg_string db "Input string: $"
    msg_error db "Some character in the string is not within the specified range.$"
    msg_success db "All characters in a string within the specified range.$"
    crlf db 13, 10, "$"
    first_diap db ?
    second_diap db ?
    str1 db 255 dup(?)      
    real_len db ?  
data ends

code segment para public 

assume cs:code, ds:data, ss:stack

start:
    mov ax, data
    mov ds, ax
    mov ax, stack
    mov ss, ax
    
    mov dx, offset msg_first
    mov ah, 09h
    int 21h
    
    mov ah, 01h   
    int 21h
    mov [first_diap], al

    mov dx, offset crlf
    mov ah, 09h
    int 21h

    mov dx, offset msg_second
    mov ah, 09h
    int 21h
    
    mov ah, 01h      
    int 21h
    mov [second_diap], al

    mov dx, offset crlf
    mov ah, 09h
    int 21h

    mov al, [first_diap]
    mov bl, [second_diap]
    cmp al, bl
    jbe range_ok     
	
	mov dx, offset msg_error
    mov ah, 09h
    int 21h
    
    mov dx, offset crlf
    mov ah, 09h
    int 21h

    mov ax, 4c01h
	int 21h 
    
range_ok:
    mov dx, offset msg_string
    mov ah, 09h
    int 21h

    mov dx, offset str1
    mov byte ptr [str1], 254   
    mov ah, 0ah
    int 21h

    mov bl, [str1+1]
    mov [real_len], bl

    mov dx, offset crlf
    mov ah, 09h
    int 21h

    mov cl, [real_len]      
    xor ch, ch
    xor si, si             
    
check_loop:
    mov al, [str1 + 2 + si] 

    cmp al, [first_diap]
    jb  error_found        

    cmp al, [second_diap]
    ja  error_found         
    
    inc si                 
    loop check_loop

    mov dx, offset msg_success
    mov ah, 09h
    int 21h

    mov dx, offset crlf
    mov ah, 09h
    int 21h
    
    mov ax, 4c00h         
    int 21h
    
error_found:
    mov dx, offset msg_error
    mov ah, 09h
    int 21h
    
    mov dx, offset crlf
    mov ah, 09h
    int 21h
    
    mov ax, 4c01h          
    int 21h

code ends
end start