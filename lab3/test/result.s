
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
mov eax, DWORD PTR [ebp-4]  # right operand a
neg eax
push eax  # push result
mov ebx, DWORD PTR [ebp-8]  # right operand b
pop eax  # left operand from stack
add eax, ebx
push eax  # push result
pop eax  # expression result
mov DWORD PTR [ebp-4], eax  # a = eax

# Expression
mov eax, DWORD PTR [ebp-4]  # expression result a
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
