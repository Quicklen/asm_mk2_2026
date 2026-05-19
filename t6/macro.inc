arg1 equ 4
arg2 equ 6
arg3 equ 8
arg4 equ 10
arg5 equ 12

var1 equ -2
var2 equ -4
var3 equ -6
var4 equ -8

; макрос для сохранения регистров, указанных в качестве агрументов
; доступно передать до 10-ти регистров (7 общего назначения и 3 сегментных)
pushregs macro reg1, reg2, reg3, reg4, reg5, reg6, reg7, reg8, reg9, reg10
	ifnb <reg1>
		push reg1
		pushregs reg2, reg3, reg4, reg5, reg6, reg7, reg8, reg9, reg10
	endif
endm

; макрос для восстановления регистров из стека
popregs macro reg1, reg2, reg3, reg4, reg5, reg6, reg7, reg8, reg9, reg10
	ifnb <reg1>
		pop reg1
		popregs reg2, reg3, reg4, reg5, reg6, reg7, reg8, reg9, reg10
	endif
endm

; break point
bkp macro
	int 3
endm

app_init macro
    mov ax, data
    mov ds, ax
    mov es, ax
    mov ax, stack
    mov ss, ax
endm

cdecl_call_c macro function, function_call_failed_label, farg1, farg2, farg3, farg4, farg5
	ifnb <function_call_failed_label>
	push bx
	endif

	ifnb <farg5>
        push farg5
    endif
	ifnb <farg4>
        push farg4
    endif
	ifnb <farg3>
        push farg3
    endif
	ifnb <farg2>
        push farg2
    endif
	ifnb <farg1>
        push farg1
    endif

	ifnb <function>
		call function
	endif
	
	setc bl

	ifnb <farg5>
        add sp, 2
    endif
	ifnb <farg4>
        add sp, 2
    endif
	ifnb <farg3>
        add sp, 2
    endif
	ifnb <farg2>
        add sp, 2
    endif
	ifnb <farg1>
        add sp, 2
    endif

ifnb <function_call_failed_label>
	cmp bl, 1
	pop bx ; restore ax
	je <function_call_failed_label>

endif
endm

cdecl_call macro _func, _arg1, _arg2, _arg3, _arg4, _arg5, _arg6, _arg7, _arg8, _arg9 

 ifnb <_arg9>
    push _arg9
  endif
  ifnb <_arg8>
    push _arg8
  endif
  ifnb <_arg7>
    push _arg7
  endif
  ifnb <_arg6>
    push _arg6
  endif
  ifnb <_arg5>
    push _arg5
  endif
  ifnb <_arg4>
    push _arg4
  endif
  ifnb <_arg3>
    push _arg3
  endif
  ifnb <_arg2>
    push _arg2
  endif
  ifnb <_arg1>
    push _arg1
  endif

  ifnb <_func>
    call _func
  endif

  ifnb <_arg9>
    add sp, 18
  elseifnb <_arg8>
    add sp, 16
  elseifnb <_arg7>
    add sp, 14
  elseifnb <_arg6>
    add sp, 12
  elseifnb <_arg5>
    add sp, 10
  elseifnb <_arg4>
    add sp, 8
  elseifnb <_arg3>
    add sp, 6
  elseifnb <_arg2>
    add sp, 4
  elseifnb <_arg1>
    add sp, 2
  endif

endm
