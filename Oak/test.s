.text
	.globl main
main:
	leaq	.L0, %rbx
	movq	%rbx, program.square
	leaq	.L1, %rbx
	movq	%rbx, program.main
	call	*(program.main)
	ret

.data
program.square:
	.zero 8
program.main:
	.zero 8
.text
.L0:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movl	16(%rbp), %ebx
	imull	16(%rbp), %ebx
	movl	%ebx, %eax
	leave
	ret	
.L1:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$37, %rsp
	movl	$123, %ebx
	movl	%ebx, -4(%rbp)
	movzlb -4(%rbp), %ah
	movb	%ah, %bh
	movb	%bh, -5(%rbp)
	movsbl -5(%rbp), %eax
	movl	%eax, %ebx
	movl	%ebx, -4(%rsp)
	subq	$4, %rsp
	call	*(program.square)
	addq	$4, %rsp
	movzlb %eax, %ah
	movb	%ah, %bh
	movb	%bh, -5(%rbp)
	movl	$0, %eax
	leave
	ret	

