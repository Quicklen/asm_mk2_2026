
; bool InitMem(void* segProgramStartAddress, void* lastSegAddr, void* offsetProgramEnd);
InitMem proc near 
	push bp
	mov bp, sp
	push es
	
	; смещение внутри сегмента переводим в количество параграфов
	; выравниваем смещение по границе параграфа
	mov bx, 0fff0h	; маска для выравнивания
	mov ax, [bp+arg3]
	add ax, 0fh		; граница выравнивания
	and bx, ax
	; байты в параграфы (bytes / 16)
	shr bx, 4
	
	; переводим смещение внутри сегмента(в параграфах) в сегментный адрес
	; адресСегмента + количествоПараграфов = новыйСегментныйАдрес
	mov ax, [bp+arg2]
	add bx, ax
	; segProgStart - segProgEnd
	mov ax, [bp+arg1]
	sub bx, ax
	mov ah, 4ah
	int 21h
	jnc _IM_success
	; handle error
	_IM_fail:
	xor ax, ax
	jmp _IM_exit
	
	_IM_success:
	mov ax, 1
	_IM_exit:
	pop es
	mov sp, bp
	pop bp
	ret
InitMem endp

; void* AllocMem(unsigned word sizeInBytes) -> сегментный адрес выделенного блока;
AllocMem proc near	
	push bp
	mov bp, sp
	
	; Выравнивание вверх по границе параграфа
	mov ax, [bp+arg1]
	add ax, 0fh
	mov bx, 0fff0h
	and bx, ax
	; Байты в параграфы (bytes / 16)
	shr bx, 4
	
	; Выделяем память
	mov ah, 48h
	int 21h
	jnc _AM_exit
	
	; Ошибка
	_AM_fail:
	xor ax, ax
	
	_AM_exit:
	mov sp, bp
	pop bp
	ret
AllocMem endp

; void FreeMem(void* segAddr)
FreeMem proc near
	push bp
	mov bp, sp
	push es
	
	mov es, [bp+arg1]    ; load segment address to free
	mov ah, 49h          ; DOS function: free memory block
	int 21h              ; call DOS
	jnc _FM_exit         ; if CF=0, success, skip error handling

	; error handling: ensure CF is set on return
	stc                  ; explicitly set CF (already set by INT 21h)
	
_FM_exit:
	pop es
	mov sp, bp
	pop bp
	ret
FreeMem endp


; word ReallocMem(void* pointer, unsigned word newSizeInBytes)
ReallocMem proc near
	_ReallocMem_localSize equ 1*2
	push bp
	mov bp, sp
	sub sp, _ReallocMem_localSize
	push es
	
	; выравнивание вверх по границе параграфа
	mov bx, 0fff0h
	mov ax, [bp+arg2]
	add ax, 0fh
	and bx, ax
	; байты в параграфы (bytes / 16)
	shr bx, 4
	
	mov [bp+var1], bx
	mov es, [bp+arg1]
	mov ah, 4ah
	int 21h
	jnc _RM_exit
	
	; handle
	
	_RM_exit:
	pop es
	add sp, _ReallocMem_localSize
	mov sp, bp
	pop bp
	ret
ReallocMem endp