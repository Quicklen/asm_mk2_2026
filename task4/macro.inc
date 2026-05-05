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