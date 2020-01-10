.text
	.globl main
main:
	leaq      .L0, %rbx
	movq      %rbx, program.main
	call      *(program.main)
	ret

.data
program.main:
	.zero 8
.text
.L0:
	pushq     %rbp
	movq      %rsp, %rbp
	subq      $41, %rsp
	movb      $123, %bl
	movb      %bl, -1(%rbp)
	movb      -1(%rbp), %bl
	movsbq    %bl, %rdx
	movq      %rdx, %rbx
	movq      %rbx, -9(%rbp)
	movl      $0, %eax
	leave
	ret       

