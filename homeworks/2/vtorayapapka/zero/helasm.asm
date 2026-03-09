data_seg segment para public
str db "Hello, asm!",0Dh,0Ah,"$"	;0Dh - \r, 0Ah - \n
index equ 2
data_seg ends


stack_seg segment para stack
db 256 dup("?")
stack_seg ends


code_seg segment para
assume cs:code_seg,ss:stack_seg,ds:data_seg

start:
	
	mov ax, data_seg
	mov ds, ax
	mov ax, stack_seg
	mov ss, ax
	
	lea bx, [str + index]
	mov byte ptr[bx], "J"
	
	mov dl, [bx+1]
	mov ah, 02h
	int 21h
	
	mov dl, 0Ah
	int 21h
	
	mov dx, offset str
	mov ah, 09h
	int 21h
	
	mov ax, 4c00h
	int 21h
	
code_seg ends

end start
	
	

