BLINK_DURATION equ 120
BLINK_COLOR equ 2
BUTTONS_COUNT equ 17

BUTTONS_PER_LINE equ 5

_buttons	label	word
	db	91
	db	49
	db	93
	db	2 dup (0)
	db	1
	db	0
	db	49
	db	91
	db	50
	db	93
	db	2 dup (0)
	db	1
	db	0
	db	50
	db	91
	db	51
	db	93
	db	2 dup (0)
	db	1
	db	0
	db	51
	db	91
	db	52
	db	93
	db	2 dup (0)
	db	1
	db	0
	db	52
	db	91
	db	53
	db	93
	db	2 dup (0)
	db	1
	db	0
	db	53
	db	91
	db	54
	db	93
	db	2 dup (0)
	db	1
	db	0
	db	54
	db	91
	db	55
	db	93
	db	2 dup (0)
	db	1
	db	0
	db	55
	db	91
	db	56
	db	93
	db	2 dup (0)
	db	1
	db	0
	db	56
	db	91
	db	57
	db	93
	db	2 dup (0)
	db	1
	db	0
	db	57
	db	91
	db	48
	db	93
	db	2 dup (0)
	db	1
	db	0
	db	48
	db	91
	db	43
	db	93
	db	2 dup (0)
	db	1
	db	0
	db	43
	db	91
	db	45
	db	93
	db	2 dup (0)
	db	1
	db	0
	db	45
	db	91
	db	42
	db	93
	db	2 dup (0)
	db	1
	db	0
	db	42
	db	91
	db	47
	db	93
	db	2 dup (0)
	db	1
	db	0
	db	47
	db	91
	db	94
	db	93
	db	2 dup (0)
	db	1
	db	0
	db	94
	db	91
	db	67
	db	93
	db	2 dup (0)
	db	1
	db	0
	db	99
	db	91
	db	61
	db	93
	db	2 dup (0)
	db	1
	db	0
	db	61