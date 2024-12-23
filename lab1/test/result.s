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
mov DWORD PTR [ebp-16], 0  # int d

# Expression
mov eax, 1  # expression result 1
mov DWORD PTR [ebp-4], eax  # a = eax

# Expression
mov eax, 2  # expression result 2
mov DWORD PTR [ebp-8], eax  # b = eax

# Expression
mov eax, 3  # expression result 3
mov DWORD PTR [ebp-12], eax  # c = eax

# Expression
mov ebx, 2  # right operand 2
mov eax, DWORD PTR [ebp-8]  # left operand b
imul eax, ebx
push eax  # push result
pop ebx  # right operand from stack
mov eax, DWORD PTR [ebp-4]  # left operand a
add eax, ebx
push eax  # push result
mov ebx, DWORD PTR [ebp-12]  # right operand c
pop eax  # left operand from stack
cdq
idiv ebx
push eax  # push result
mov ebx, 3  # right operand 3
pop eax  # left operand from stack
sub eax, ebx
push eax  # push result
pop eax  # expression result
mov DWORD PTR [ebp-16], eax  # d = eax

# Expression
mov eax, DWORD PTR [ebp-16]  # expression result d
# return eax

    push eax
    push offset format_str
    call printf
    add esp, 8

    pop eax
    leave
    ret
