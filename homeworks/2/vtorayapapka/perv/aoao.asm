stack segment para stack 
db 256 dup(?)
stack ends

data segment para public
data ends

code segment para 

assume cs:code,ds:data,ss:stack

start:
  mov ax,data
  mov ds,ax
  mov ax,stack
  mov ss,ax

  mov ah,01h
  int 21h
  
  mov bl,al
  
  mov ah, 02h
  mov dl, 0Dh
  int 21h

  mov ah, 02h
  mov dl, 0Ah
  int 21h
  
  mov dl, bl
  mov ah,02h
  int 21h
  
  
  
  mov ax,4c00h
  int 21h
  
code ends

end start
