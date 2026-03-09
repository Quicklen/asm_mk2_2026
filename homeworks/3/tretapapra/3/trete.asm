stack_seg segment para stack 'stack' 
db 256 dup(?)
stack_seg ends 

data_seg segment para public 'data'  
x dd 2
y dd 2
z dd ?

a dd 2
b dd 2
d dw ?
data_seg ends

code_seg segment para 'code'     

assume cs:code_seg,ds:data_seg,ss:stack_seg

start:
    mov ax, stack_seg
    mov ss, ax
    mov ax, data_seg
    mov ds, ax
    
    ; z = (x*y)/(x+y)
    mov ax, word ptr[y]        
    imul word ptr[x]           
    
    mov bx, word ptr[x]       
    add bx, word ptr[y]        
    
    xor dx, dx
    idiv bx
    
    mov word ptr[z], ax  
	
	mov dl, byte ptr[z]
	mov ah, 02h
	int 21h

	; ( с = (a + b)^2 )
	xor bx, bx
	mov bx, word ptr[a]
	add bx, word ptr[b]
	
	mov ax, bx
	imul bx
	mov word ptr [d], ax
	
	; ( с = (a + b)^3 )
	xor bx, bx
	mov bx, word ptr[a]
	add bx, word ptr[b]
	
	mov ax, bx
	imul bx
	imul bx
	mov word ptr [d], ax
	
	mov word ptr[d], ax
    
    mov ax, 4c00h
    int 21h
    
code_seg ends
end start