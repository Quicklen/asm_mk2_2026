_get_mode proc near
	push bp
	mov bp, sp

	mov ah, 0Fh
	int 10h

	movzx ax, al

	mov sp, bp
	pop bp
	ret
_get_mode endp

_set_mode proc near
	push bp
	mov bp, sp

	mov ax, word ptr [bp + arg1]
	mov ah, 00h
	int 10h

	mov sp, bp
	pop bp
	ret
_set_mode endp

_setpixel proc near
	push bp
	mov bp, sp
	pusha

	mov cx, word ptr [bp + arg1]
	test cl, 1
	jz draw_pixel_raw_even_row
	mov bx, 2000h
	jmp short draw_pixel_raw_continue

draw_pixel_raw_even_row:
	xor bx, bx

draw_pixel_raw_continue:
	shr cx, 1
	mov al, 80
	mul cl

	mov dx, word ptr [bp + arg2]
	mov cx, dx
	not cl
	and cl, 00000011b
	shl cl, 1

	shr dx, 1
	shr dx, 1

	add ax, dx
	add bx, ax

	mov ah, es:[bx]
	ror ah, cl
	and ah, 11111100b
	mov al, byte ptr [bp + arg3]
	or ah, al
	rol ah, cl
	mov es:[bx], ah

	popa
	mov sp, bp
	pop bp
	ret
_setpixel endp

_clear_screen	proc	near
	push	bp
	mov	bp,sp
	push	si
	push	di
   ;	
   ;	    short x, y;
   ;	    for (y = 0; y < SCREEN_H; ++y) {
   ;	
	xor	di,di
	jmp	short y_loop_check
y_loop_body:
   ;	
   ;	        for (x = 0; x < SCREEN_W; ++x) {
   ;	
	xor	si,si
	jmp	short x_loop_check
x_loop_body:
   ;	
   ;	            setpixel(x, y, color);
   ;	
	push	word ptr [bp+4]
	push	di
	push	si
	call	near ptr _setpixel
	add	sp,6
	inc	si
x_loop_check:
	cmp	si,320
	jl	short x_loop_body
	inc	di
y_loop_check:
	cmp	di,240
	jl	short y_loop_body
   ;	
   ;	        }
   ;	    }
   ;	}
   ;	
	pop	di
	pop	si
	pop	bp
	ret	
_clear_screen	endp

_getKey2	proc	near
	push	bp
	mov	bp,sp
	sub	sp,2
   ;	
   ;	{
   ;	    unsigned short result;
   ;	    asm {
   ;	        mov ah, 1
   ;	
	mov	 ah, 1
   ;	
   ;	        int 16h
   ;	
	int	 16h
   ;	
   ;	        jz no_key
   ;	
	je	short no_key
   ;	
   ;	        mov result, ax
   ;	
	mov	 [bp-2], ax
   ;	
   ;	        jmp dne
   ;	
	jmp	short dne
no_key:
   ;	
   ;	    }
   ;	
   ;	no_key:
   ;	    return 0;
   ;	
	xor	ax,ax
	jmp	short return_result
dne:
   ;	
   ;	
   ;	dne:
   ;	    return result;
   ;	
	mov	ax,word ptr [bp-2]
	jmp	short return_result
return_result:
   ;	
   ;	}
   ;	
	mov	sp,bp
	pop	bp
	ret	
_getKey2	endp

_getKey	proc	near
	push	bp
	mov	bp,sp
	sub	sp,2
   ;	
   ;	{
   ;	    unsigned short result;
   ;	    asm {
   ;	        mov ah, 0
   ;	
	mov	 ah, 0
   ;	
   ;	        int 16h
   ;	
	int	 16h
   ;	
   ;	        mov result, ax
   ;	
	mov	 [bp-2], ax
   ;	
   ;	    }
   ;	    return result;
   ;	
	mov	ax,word ptr [bp-2]
	jmp	short return_result_key
return_result_key:
   ;	
   ;	}
   ;	
	mov	sp,bp
	pop	bp
	ret	
_getKey	endp

printKeys proc
	
	curr_char equ var1
	isExit	equ var2
	
	push bp
	mov bp, sp
	sub sp, 4*2
	
	mov ax, 0B800h
	mov es, ax
	
	mov byte ptr [bp+curr_char], ' '
	mov byte ptr [bp+isExit], 0
	printKeys_while_b:
		xor bx, bx
		mov cx, 80*25
		print_b: ; пишем 80*25 символов
			mov al, byte ptr [bp+curr_char]
			mov ah, byte ptr [bp+arg1]
			mov word ptr es:[bx], ax
			add bx, 2
			loop print_b
		
		cmp byte ptr [bp+isExit], 1
		je printKeys_while_e
		
		call _getKey

		cmp ah, 01h	; ESC scan code
		jne next_check
		mov byte ptr [bp+isExit], 1
		jmp set_key
		
		next_check:
		cmp al, 0	;если нажатая клавиша (комбинация клавиш) не имеет символьного отображения
		je printKeys_while_b
		
		set_key:
		mov byte ptr [bp+curr_char], al
		jmp printKeys_while_b
	printKeys_while_e:
	
	mov sp, bp
	pop bp
	ret
printKeys endp