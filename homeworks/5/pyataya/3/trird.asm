.386
stack_seg segment para stack use16
    db 256 dup(?)
stack_seg ends

data_seg segment para public use16
    num_str db 0      
    edinica db 0    
data_seg ends

code_seg segment para use16
    assume cs:code_seg, ds:data_seg, ss:stack_seg

start:
    mov ax, data_seg
    mov ds, ax

	mov dl, 0Dh
	mov ah, 02h
	int 21h
	
	mov dl, 0Ah
	mov ah, 02h
	int 21h
	
    mov [num_str], 0

outer_loop_left:
    cmp [num_str], 9
    jg done_left       

    mov [edinica], 0

inner_loop_left:

    cmp [num_str], 0
    jne print_tens_left

    mov dl, [edinica]
    add dl, '0'
    mov ah, 02h
    int 21h
    mov dl, ' '
	mov ah, 02h
    int 21h
    jmp after_print_left
print_tens_left:
    mov dl, [num_str]
    add dl, '0'
    int 21h
    mov dl, [edinica]
    add dl, '0'
    int 21h
after_print_left:
    cmp [edinica], 9
    je skip_space_left
    mov dl, ' '
	mov ah, 02h
    int 21h
skip_space_left:
    inc [edinica]
    cmp [edinica], 10
    jnge inner_loop_left

    mov dl, 0Dh
	mov ah, 02h
	int 21h
	
	mov dl, 0Ah
	mov ah, 02h
	int 21h

    inc [num_str]
    jmp outer_loop_left

done_left:
	
	mov dl, 0Dh
	mov ah, 02h
	int 21h
	
	mov dl, 0Ah
	mov ah, 02h
	int 21h
	
    mov [num_str], 0
outer_loop_right:
    cmp [num_str], 9
    jg done_right

    mov [edinica], 0

inner_loop_right:
    cmp [num_str], 0
    jne print_tens_right
    mov dl, ' '
	mov ah, 02h
    int 21h
    mov dl, [edinica]
    add dl, '0'
	mov ah, 02h
    int 21h
    jmp after_print_right
print_tens_right:
    mov dl, [num_str]
    add dl, '0'
	mov ah, 02h
    int 21h
    mov dl, [edinica]
    add dl, '0'
	mov ah, 02h
    int 21h
after_print_right:
    cmp [edinica], 9
    je skip_space_right
    mov dl, ' '
	mov ah, 02h
    int 21h
skip_space_right:
    inc [edinica]
    cmp [edinica], 10
    jnge inner_loop_right

	mov dl, 0Dh
	mov ah, 02h
	int 21h
	
	mov dl, 0Ah
	mov ah, 02h
	int 21h

    inc [num_str]
    jmp outer_loop_right

done_right:
    mov ax, 4C00h
    int 21h

code_seg ends
end start