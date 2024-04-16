.intel_syntax noprefix
.global main
.extern printf

.data
format_str:
    .asciz "%d\n"

.text
main:
    push ebp
    mov ebp, esp
    sub esp, 0x100

    ##REPLACEME##

    push eax
    push offset format_str
    call printf
    add esp, 8

    pop eax
    leave
    ret
