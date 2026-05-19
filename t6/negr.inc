    fio_err_no_error        db " ", 0                                ; code 0 (no error, rarely used)
    fio_err_invalid_func    db "Invalid function", 0dh, 0ah, 0      ; code 1
    fio_err_not_found       db "File not found", 0dh, 0ah, 0        ; code 2
    fio_err_path_not_found  db "Path not found", 0dh, 0ah, 0        ; code 3
    fio_err_too_many_open   db "Too many open files", 0dh, 0ah, 0   ; code 4
    fio_err_access_denied   db "Access denied", 0dh, 0ah, 0         ; code 5
    fio_err_invalid_handle  db "Invalid handle", 0dh, 0ah, 0        ; code 6
    fio_err_mcb_destroyed   db "Memory control blocks destroyed", 0dh, 0ah, 0 ; code 7
    fio_err_insufficient_mem db "Insufficient memory", 0dh, 0ah, 0  ; code 8
    fio_err_invalid_mem_block db "Invalid memory block address", 0dh, 0ah, 0 ; code 9
    fio_err_invalid_env     db "Invalid environment", 0dh, 0ah, 0   ; code 10
    fio_err_invalid_format  db "Invalid format", 0dh, 0ah, 0        ; code 11
    fio_err_invalid_access  db "Invalid access code", 0dh, 0ah, 0   ; code 12
    fio_err_invalid_data    db "Invalid data", 0dh, 0ah, 0          ; code 13
    fio_err_reserved        db "Reserved error", 0dh, 0ah, 0        ; code 14
    fio_err_invalid_drive   db "Invalid drive", 0dh, 0ah, 0         ; code 15
    fio_err_remove_cur_dir  db "Cannot remove current directory", 0dh, 0ah, 0 ; code 16
    fio_err_not_same_device db "Not same device", 0dh, 0ah, 0       ; code 17
    fio_err_no_more_files   db "No more files", 0dh, 0ah, 0         ; code 18
    fio_err_write_protected db "Write protected", 0dh, 0ah, 0       ; code 19
    fio_err_unknown_unit    db "Unknown unit", 0dh, 0ah, 0          ; code 20
    fio_err_drive_not_ready db "Drive not ready", 0dh, 0ah, 0       ; code 21
    fio_err_unknown_cmd     db "Unknown command", 0dh, 0ah, 0       ; code 22
    fio_err_crc_error       db "CRC error", 0dh, 0ah, 0             ; code 23
    fio_err_bad_req_len     db "Bad request structure length", 0dh, 0ah, 0 ; code 24
    fio_err_seek_error      db "Seek error", 0dh, 0ah, 0            ; code 25
    fio_err_unknown_media   db "Unknown media type", 0dh, 0ah, 0    ; code 26
    fio_err_sector_not_found db "Sector not found", 0dh, 0ah, 0     ; code 27
    fio_err_printer_out_paper db "Printer out of paper", 0dh, 0ah, 0 ; code 28
    fio_err_invalid_device_req db "Invalid device request", 0dh, 0ah, 0 ; code 29 (returned by read/write on unsuitable device)
    fio_err_read_fault      db "Read fault", 0dh, 0ah, 0            ; code 30
    fio_err_general_failure db "General failure", 0dh, 0ah, 0       ; code 31
    fio_err_unknown         db "Unknown DOS error", 0dh, 0ah, 0     ; for codes >3

    fio_err_vec dw offset fio_err_no_error          ; 0
                dw offset fio_err_invalid_func      ; 1
                dw offset fio_err_not_found         ; 2
                dw offset fio_err_path_not_found    ; 3
                dw offset fio_err_too_many_open     ; 4
                dw offset fio_err_access_denied     ; 5
                dw offset fio_err_invalid_handle    ; 6
                dw offset fio_err_mcb_destroyed     ; 7
                dw offset fio_err_insufficient_mem  ; 8
                dw offset fio_err_invalid_mem_block ; 9
                dw offset fio_err_invalid_env       ; 10
                dw offset fio_err_invalid_format    ; 11
                dw offset fio_err_invalid_access    ; 12
                dw offset fio_err_invalid_data      ; 13
                dw offset fio_err_reserved          ; 14
                dw offset fio_err_invalid_drive     ; 15
                dw offset fio_err_remove_cur_dir    ; 16
                dw offset fio_err_not_same_device   ; 17
                dw offset fio_err_no_more_files     ; 18
                dw offset fio_err_write_protected   ; 19
                dw offset fio_err_unknown_unit      ; 20
                dw offset fio_err_drive_not_ready   ; 21
                dw offset fio_err_unknown_cmd       ; 22
                dw offset fio_err_crc_error         ; 23
                dw offset fio_err_bad_req_len       ; 24
                dw offset fio_err_seek_error        ; 25
                dw offset fio_err_unknown_media     ; 26
                dw offset fio_err_sector_not_found  ; 27
                dw offset fio_err_printer_out_paper ; 28
                dw offset fio_err_invalid_device_req; 29
                dw offset fio_err_read_fault        ; 30
                dw offset fio_err_general_failure   ; 31