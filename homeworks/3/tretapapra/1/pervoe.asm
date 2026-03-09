.386
stack_seg segment para stack use16
    db 256 dup(?)
stack_seg ends

data_seg segment para public use16
    first   db  241  
            db  0    
            db  241 dup(?) 

    second  db  241  
            db  0    
			db  241 dup(?) 

    third   db  241  
            db  0    
            db  241 dup(?) 


data_seg ends

code_seg segment para public use16
    assume cs:code_seg, ds:data_seg, ss:stack_seg

start:      
            mov ax, data_seg
            mov ds, ax
			mov ax, stack_seg
			mov ss, ax
            
            mov dx,  offset first
            mov ah, 0ah
            int 21h

            mov ah, 02h
            mov dx, 0Dh
            int 21h
            mov dx, 0Ah
            int 21h

            mov bl, byte ptr [first+1]
            lea si, [first+2]
            mov bh, 0h
            add si, bx

            mov byte ptr [si], '$'

            mov dx, offset second
            mov ah, 0ah
            int 21h

            mov ah, 02h
            mov dx, 0Dh
            int 21h
            mov dx, 0Ah
            int 21h

            mov bl, byte ptr [second+1]
            lea si, [second+2]
            mov bh, 0h
            add si, bx

            mov byte ptr [si], '$'

            mov dx, offset third
            mov ah, 0ah
            int 21h

            mov ah, 02h
            mov dx, 0Dh
            int 21h
            mov dx, 0Ah
            int 21h

            mov bl, byte ptr [third+1]
            lea si, [third+2]
            mov bh, 0h
            add si, bx

            mov byte ptr [si], '$'

            mov dx, offset [first + 2]
            mov ah, 9 
            int 21h

            mov ah, 02h
            mov dx, 0Dh
            int 21h
            mov dx, 0Ah
            int 21h

            mov dx, offset [second + 2]
            mov ah, 9 
            int 21h

            mov ah, 02h
            mov dx, 0Dh
            int 21h
            mov dx, 0Ah
            int 21h

            mov dx, offset [third + 2]
            mov ah, 9 
            int 21h

            mov ah, 02h
            mov dx, 0Dh
            int 21h
            mov dx, 0Ah
            int 21h

            mov ax, 4c00h           
            int 21h

code_seg ends

end start