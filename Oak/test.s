.data
abc:
.zero 76
var:
.zero 8
_main:
.zero 8
.text
.globl main
L0:

    pushq %rbp
    movq %rsp, %rbp
    subq $48, %rsp

movq $2, var2
imulq $2, var2
movq $2, %rax
eaq  