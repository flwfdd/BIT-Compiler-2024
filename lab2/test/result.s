
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

    mov DWORD PTR [ebp-4], 0  # int a
mov DWORD PTR [ebp-8], 0  # int b
mov DWORD PTR [ebp-12], 0  # int c

# Expression
mov eax, 1  # expression result 1
mov DWORD PTR [ebp-4], eax  # a = eax

# Expression
mov eax, DWORD PTR [ebp-4]  # expression result a
push eax
push offset format_str
call printf
add esp, 8
pop eax


# Expression
mov eax, 2  # expression result 2
mov DWORD PTR [ebp-8], eax  # b = eax

# Expression
mov eax, DWORD PTR [ebp-8]  # expression result b
push eax
push offset format_str
call printf
add esp, 8
pop eax


# Expression
mov eax, 114514  # expression result 114514
mov DWORD PTR [ebp-12], eax  # c = eax

# Expression
mov eax, DWORD PTR [ebp-12]  # expression result c
push eax
push offset format_str
call printf
add esp, 8
pop eax


# Expression
mov eax, 0  # expression result 0
# return eax
#

    pop eax
    leave
    ret
