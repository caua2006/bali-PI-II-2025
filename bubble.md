org 100h

start:
    
    mov dx, offset msg_inicio
    call print_str
    call print_crlf

    mov cx, tamVet         ; número de elementos

menos_um:
    dec cx
    jz fim_sort            ; se cx chegou a zero ? terminou

    xor si, si             ; SI = índice (0)
    mov bx, cx             ; BX = limite interno

loop_interno:
    mov al, [vetor + si]
    mov dl, [vetor + si + 1]

    cmp al, dl
    jbe nao_troca

    mov [vetor + si], dl
    mov [vetor + si + 1], al

nao_troca:
    inc si
    dec bx
    jnz loop_interno

    jmp menos_um

fim_sort:
    call print_crlf
    mov dx, offset msg_final
    call print_str
    call print_crlf

    mov si, 0
    mov cx, tamVet

loop_exibe:
    mov al, [vetor + si]
    call print_u8

    mov dl, ' '
    call print_char

    inc si
    loop loop_exibe

    call print_crlf

fim_prog:
    mov ah, 4Ch
    int 21h


; -------- DADOS --------

msg_inicio db 'Ordenando vetor usando Bubble Sort...', '$'
msg_final  db 'Vetor ordenado:', '$'

vetor db 8, 3, 2, 2, 9, 7, 6, 4
tamVet equ ($ - vetor)


; -------- ROTINAS --------

print_str:
    mov ah, 09h
    int 21h
    ret

print_char:
    mov ah, 02h
    int 21h
    ret

print_crlf:
    mov dl, 13
    call print_char
    mov dl, 10
    call print_char
    ret

print_u8:               ; AL = 0..99
    cmp al, 10
    jb um_dig
    xor ah, ah
    mov bl, 10
    div bl
    mov bh, ah
    add al, '0'
    mov dl, al
    call print_char
    mov dl, bh
    add dl, '0'
    call print_char
    ret

um_dig:
    add al, '0'
    mov dl, al
    call print_char
    ret


end start
