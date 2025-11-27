org 100h

start:
    ;seu codigo aqui 
    
    

; ---- dados ----
texto1  db 'SEU TEXTO', '$'
texto2 db 'SEU TEXTO$', '$'


; ---- rotinas ----

; print_str: DS:DX ? string terminada em '$'
print_str:
    mov     ah, 09h
    int     21h
    ret

; print_char: DL = caractere
print_char:
    mov     ah, 02h
    int     21h
    ret

; print_crlf
print_crlf:
    mov     dl, 13
    call    print_char
    mov     dl, 10
    call    print_char
    ret

; print_u8: AL = 0..99
print_u8:
    cmp     al, 10
    jb      um_dig
    xor     ah, ah
    mov     bl, 10
    div     bl
    mov     bh, ah
    add     al, '0'
    mov     dl, al
    call    print_char
    mov     dl, bh
    add     dl, '0'
    call    print_char
    ret
um_dig:
    add     al, '0'
    mov     dl, al
    call    print_char
    ret

; len_dollar: DS:SI ? string terminada em '$'
; retorna CX = numero de caracteres antes do '$'
len_dollar:
    push    ax
    xor     cx, cx
ld_next:
    lodsb
    cmp     al, '$'
    je      fim
    inc     cx
    jmp     ld_next
fim:
    pop     ax
    ret



end start
