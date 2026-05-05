; short read_mat(short fd)
_read_mat proc near
        push    bp
        mov     bp, sp
        sub     sp, 34
        push    si
        push    di

        ; Local variable offsets
        ; buffer[20]  at [bp-34]
        ; c (byte)    at [bp-1]
        ; idx         si (register)
        ; end_ptr     at [bp-4]   (word)
        ; rows        at [bp-6]   (word)
        ; cols        at [bp-8]   (word)
        ; i           at [bp-10]  (word)
        ; j           at [bp-12]  (word)
        ; val         at [bp-14]  (word)

        mov     di, word ptr [bp+4]    ; fd (argument offset adjusted)

        ;----------------------------------------------------------------------
        ; Skip whitespace and read first number (rows)
        ;----------------------------------------------------------------------
skip_ws1:
        push    di
        call    _fget_ch
        pop     cx
        mov     byte ptr [bp-1], al
        cmp     byte ptr [bp-1], EOF
        je      error_mat               ; EOF while expecting rows
        cmp     byte ptr [bp-1], ' '
        je      skip_ws1
        cmp     byte ptr [bp-1], 10
        je      skip_ws1
        cmp     byte ptr [bp-1], 13
        je      skip_ws1

        xor     si, si                  ; idx = 0

read_rows_loop:
        cmp     byte ptr [bp-1], ' '
        je      end_rows
        cmp     byte ptr [bp-1], 10
        je      end_rows
        cmp     byte ptr [bp-1], 13
        je      end_rows
        cmp     byte ptr [bp-1], EOF
        je      error_mat               ; EOF in middle of number

        cmp     si, 19
        jge     skip_store1
        mov     al, byte ptr [bp-1]
        mov     byte ptr [bp+si-34], al
        inc     si
skip_store1:
        push    di
        call    _fget_ch
        pop     cx
        mov     byte ptr [bp-1], al
        jmp     read_rows_loop

end_rows:
        mov     byte ptr [bp+si-34], 0   ; null terminate

        push    ds
        mov     ax, ss
        mov     ds, ax
        push    10
        lea     ax, word ptr [bp-4]
        push    ax
        lea     ax, word ptr [bp-34]
        push    ax
        call    _strtos
        add     sp, 6
        mov     word ptr [bp-6], ax       ; rows
        pop     ds

        ;----------------------------------------------------------------------
        ; Skip whitespace and read second number (cols)
        ;----------------------------------------------------------------------
skip_ws2:
        push    di
        call    _fget_ch
        pop     cx
        mov     byte ptr [bp-1], al
        cmp     byte ptr [bp-1], EOF
        je      error_mat               ; EOF while expecting cols
        cmp     byte ptr [bp-1], ' '
        je      skip_ws2
        cmp     byte ptr [bp-1], 10
        je      skip_ws2
        cmp     byte ptr [bp-1], 13
        je      skip_ws2

        xor     si, si                  ; idx = 0

read_cols_loop:
        cmp     byte ptr [bp-1], ' '
        je      end_cols
        cmp     byte ptr [bp-1], 10
        je      end_cols
        cmp     byte ptr [bp-1], 13
        je      end_cols
        cmp     byte ptr [bp-1], EOF
        je      error_mat               ; EOF in middle of number

        cmp     si, 19
        jge     skip_store2
        mov     al, byte ptr [bp-1]
        mov     byte ptr [bp+si-34], al
        inc     si
skip_store2:
        push    di
        call    _fget_ch
        pop     cx
        mov     byte ptr [bp-1], al
        jmp     read_cols_loop

end_cols:
        push    ds
        mov     ax, ss
        mov     ds, ax
        mov     byte ptr [bp+si-34], 0
        push    10
        lea     ax, word ptr [bp-4]
        push    ax
        lea     ax, word ptr [bp-34]
        push    ax
        call    _strtos
        add     sp, 6
        pop     ds
        mov     word ptr [bp-8], ax       ; cols

        ;----------------------------------------------------------------------
        ; Validate dimensions (1..100)
        ;----------------------------------------------------------------------
        cmp     word ptr [bp-6], 0
        jle     error_mat
        cmp     word ptr [bp-6], 100
        jg      error_mat
        cmp     word ptr [bp-8], 0
        jle     error_mat
        cmp     word ptr [bp-8], 100
        jg      error_mat

        ; store global dimensions
        push    di
        mov     di, MAT_DATA_OFFSET
        mov     ax, word ptr [bp-6]
        mov     word ptr es:[di], ax
        mov     ax, word ptr [bp-8]
        mov     word ptr es:[di+2], ax
        pop     di

        ;----------------------------------------------------------------------
        ; Read matrix elements (rows × cols numbers)
        ;----------------------------------------------------------------------
        mov     word ptr [bp-10], 0      ; i = 0
        jmp     check_i

outer_loop:
        mov     word ptr [bp-12], 0      ; j = 0
        jmp     check_j

inner_loop:
        ; skip whitespace before number
skip_ws3:
        push    di
        call    _fget_ch
        pop     cx
        mov     byte ptr [bp-1], al
        cmp     byte ptr [bp-1], EOF
        je      error_mat               ; EOF while expecting next element
        cmp     byte ptr [bp-1], ' '
        je      skip_ws3
        cmp     byte ptr [bp-1], 10
        je      skip_ws3
        cmp     byte ptr [bp-1], 13
        je      skip_ws3

        xor     si, si                  ; idx = 0

read_val_loop:
        cmp     byte ptr [bp-1], ' '
        je      end_val
        cmp     byte ptr [bp-1], 10
        je      end_val
        cmp     byte ptr [bp-1], 13
        je      end_val
        cmp byte ptr [bp-1], EOF
		je end_val               ; EOF in middle of number

        cmp     si, 19
        jge     skip_store3
        mov     al, byte ptr [bp-1]
        mov     byte ptr [bp+si-34], al
        inc     si
skip_store3:
        push    di
        call    _fget_ch
        pop     cx
        mov     byte ptr [bp-1], al
        jmp     read_val_loop

end_val:
        push    ds
        mov     ax, ss
        mov     ds, ax
        mov     byte ptr [bp+si-34], 0
        push    10
        lea     ax, word ptr [bp-4]
        push    ax
        lea     ax, word ptr [bp-34]
        push    ax
        call    _strtos
        add     sp, 6
        mov     word ptr [bp-14], ax     ; val
        pop     ds

        ; mat_data[i][j] = val
        ; ИСПРАВЛЕНО: динамический шаг строки = cols * 2
        mov     ax, word ptr [bp-10]     ; i (строка)
        mov     dx, word ptr [bp-8]      ; cols
        shl     dx, 1                    ; cols * 2 (байт на строку)
        imul    dx                        ; ax = i * cols * 2
        mov     dx, word ptr [bp-12]     ; j (столбец)
        shl     dx, 1                    ; j * 2
        add     ax, dx                    ; ax = i*cols*2 + j*2
        mov     bx, ax
        mov     dx, word ptr [bp-14]     ; val

        push    di
        mov     di, bx
        mov     word ptr es:[di], dx
        pop     di

        inc     word ptr [bp-12]         ; j++

check_j:
        mov     ax, word ptr [bp-12]
        cmp     ax, word ptr [bp-8]      ; cols
        jl      inner_loop

        inc     word ptr [bp-10]         ; i++

check_i:
        mov     ax, word ptr [bp-10]
        cmp     ax, word ptr [bp-6]      ; rows
        jl      outer_loop

        ; success
        xor     ax, ax
        jmp     done_mat

error_mat:
        mov     ax, 0FFFFh

done_mat:
        pop     di
        pop     si
        mov     sp, bp
        pop     bp
        ret

_read_mat endp

; es - matrix segment
_mat_print	proc	near
	push	bp
	mov	bp,sp
	sub	sp,16
	push	si
	push	di
   ;	
   ;	{
   ;	    short i, j;
   ;	    char buf[16];               /* enough for any short integer */
   ;	    char space = ' ';
   ;	
	mov	byte ptr [bp-1], 32
   ;	
   ;	    char* newline = "\n\r";
   ;	
        mov byte ptr [bp-4], 10   ; newline[0]
        mov byte ptr [bp-3], 13   ; newline[1]
        mov byte ptr [bp-2], 0    ; newline[2]
   ;	
   ;	
   ;	    for (i = 0; i < mat_rows1; ++i) {
   ;	
	xor	di,di
	jmp	short _mat_print_i_test
_mat_print_i_body:
   ;	
   ;	        for (j = 0; j < mat_cols1; ++j) {
   ;	
	xor	si,si
	jmp	short _mat_print_j_test
_mat_print_j_body:
   ;	
   ;	            stoa(mat_data1[i][j], buf);          /* convert to string */
   ;	
	lea	ax,word ptr [bp-16]
	push	ax
	mov	ax, di                   ; i (строка)
	mov	dx, word ptr es:[MAT_DATA_OFFSET+2]  ; cols
	shl	dx, 1                    ; cols * 2
	imul dx                      ; i * cols * 2
	mov	dx, si                   ; j (столбец)
	shl	dx, 1                    ; j * 2
	add	ax, dx                   ; i*cols*2 + j*2
	mov	bx, ax
	push	word ptr es:[bx]
	call	near ptr _stoa
	pop	cx
	pop	cx
   ;	
   ;	            fwrite(fd, buf, strlen(buf));        /* write the number */
   ;	
	lea	ax,word ptr [bp-16]
	push	ax
	call	near ptr _strlen
	pop	cx
	push	ax
	lea	ax,word ptr [bp-16]
	push	ax
	push	word ptr [bp+4]
	call	near ptr _fwrite
	add	sp,6
   ;	
   ;	            if (j < mat_cols1 - 1) {
   ;	
        
	mov	ax,word ptr es:[MAT_DATA_OFFSET+2]
	dec	ax
	cmp	ax,si
	jle	short _mat_print_j_next
   ;	
   ;	                fwrite(fd, &space, 1);           /* write a space between elements */
   ;	
        push    ds
        mov     ax, ss
        mov     ds, ax

	mov	ax,1
	push	ax
	lea	ax,word ptr [bp-1]
	push	ax
	push	word ptr [bp+4]
	call	near ptr _fwrite
	add	sp,6
        
        pop     ds
_mat_print_j_next:
	inc	si
_mat_print_j_test:
	cmp	si,word ptr es:[MAT_DATA_OFFSET+2]
	jl	short _mat_print_j_body
   ;	
   ;	            }
   ;	        }
   ;	        fwrite(fd, &newline, 1);                 /* write newline after each row */
   ;	
        push    ds
        mov     ax, ss
        mov     ds, ax
	mov	ax,1
	push	ax
	lea	ax,word ptr [bp-4]
	push	ax
	push	word ptr [bp+4]
	call	near ptr _fwrite
	add	sp,6
        pop     ds

	inc	di
_mat_print_i_test:
	cmp	di,word ptr es:[MAT_DATA_OFFSET]
	jl	short _mat_print_i_body
   ;	
   ;	    }
   ;	}
   ;	
	pop	di
	pop	si
	mov	sp,bp
	pop	bp
	ret	
_mat_print	endp