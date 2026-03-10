.386

stack_seg segment para stack use16
    db 256 dup(?)
stack_seg ends

data_seg segment para public use16
	hex db "0123456789ABCDEF"
	space db ' $'
	colon db ': $'
	nl db 0Dh,0Ah,'$'
	spec_nul db "NUL$"
    spec_soh db "SOH$"
    spec_stx db "STX$"
    spec_etx db "ETX$"
    spec_eot db "EOT$"
    spec_enq db "ENQ$"
    spec_ack db "ACK$"
    spec_bel db "BEL$"
    spec_bs db "BS$"
    spec_tab db "TAB$"
    spec_lf db "LF$"
    spec_vt db "VT$"
    spec_ff db "FF$"
    spec_cr db "CR$"
    spec_so db "SO$"
    spec_si db "SI$"
    spec_dle db "DLE$"
    spec_dc1 db "DC1$"
    spec_dc2 db "DC2$"
    spec_dc3 db "DC3$"
    spec_dc4 db "DC4$"
    spec_nak db "NAK$"
    spec_syn db "SYN$"
    spec_etb db "ETB$"
    spec_can db "CAN$"
    spec_em db "EM$"
    spec_sub db "SUB$"
    spec_esc db "ESC$"
    spec_fs db "FS$"
    spec_gs db "GS$"
    spec_rs db "RS$"
    spec_us db "US$"
    spec_del db "DEL$"
    spec_spc db "SPC$"
data_seg ends

code_seg segment para use16
assume cs:code_seg, ds:data_seg, ss:stack_seg

start:

    mov ax,data_seg
    mov ds,ax
	mov ax,stack_seg
	mov ss,ax

    mov cx,256        ; 256 символов
    xor si,si         ; текущий ASCII код
    xor di,di         ; счетчик 8 символов

next_symbol:

    mov ax, si   ; скопировать весь SI в AX
    mov al, al   ; AL уже содержит младший байт SI
	

    ; ---- печать символа ----
    cmp al, 0Eh
	jb nepechat
vivo:
    mov dl,al
    mov ah,02h
    int 21h
    jmp after_char
nepechat:
	cmp al, 07h
	jb vivo
	mov dl, '-'
    mov ah,02h
    int 21h
	jmp after_char
after_char:

    ; печать ":"
    mov dx,offset colon
    mov ah,09h
    int 21h

    ; ---- HEX вывод ----

    mov ax,si
    mov bl,al

    mov bh,bl
    shr bh,4
    mov bl,bh
    xor bh,bh
    mov dl,hex[bx]
    mov ah,02h
    int 21h

    mov ax,si
	mov bl,al
	
	mov bh, bl
	shl bh, 4
	shr bh, 4
	mov bl,bh
    xor bh,bh
    mov dl,hex[bx]
    mov ah,02h
    int 21h
	
    ; пробел
    mov dx,offset space
    mov ah,09h
    int 21h

    ; следующий символ
    inc si
    inc di

    cmp di,8
    jne no_newline

    mov di,0
    mov dx,offset nl
    mov ah,09h
    int 21h

no_newline:

    loop next_symbol

    mov ax,4C00h
    int 21h

code_seg ends
end start