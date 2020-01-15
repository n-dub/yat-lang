.text
	.globl main
main:
	leaq      .L0, %rbx
	movq      %rbx, system.io.print
	leaq      .L1, %rbx
	movq      %rbx, program.min
	leaq      .L2, %rdx
	movq      %rdx, program.main
	call      *(program.main)
	ret

.data
.L5:
	.hword 97, 32, 61, 32, 51, 10, 0
.L8:
	.hword 97, 32, 61, 32, 45, 51, 10, 0
system.io.tempmessage:
	.zero 8
system.io.print:
	.zero 8
program.min:
	.zero 8
program.main:
	.zero 8
.text
.L0:
	pushq     %rbp
	movq      %rsp, %rbp
	subq      $32, %rsp
	movq      16(%rbp), %rbx
	movq      %rbx, system.io.tempmessage
	
.data
m0:
	.ascii "\0"
fmtstr:
	.ascii "%ls\0"
.text
	leaq      m0, %rdx
	movl      $0, %ecx
	call      setlocale
	
	movq      system.io.tempmessage, %rdx
	leaq      fmtstr, %rcx
	call      printf
	leave
	ret
.L1:
	pushq     %rbp
	movq      %rsp, %rbp
	subq      $32, %rsp
	movq      24(%rbp), %rbx
	subq      16(%rbp), %rbx
	movq      %rbx, %rax
	leave
	ret       
.L2:
	pushq     %rbp
	movq      %rsp, %rbp
	subq      $40, %rsp
	movl      $5, %ebx
	movslq    %ebx, %rdx
	movq      %rdx, %rbx
	movq      %rbx, -8(%rsp)
	movl      $2, %ebx
	movslq    %ebx, %rdx
	movq      %rdx, %rbx
	movq      %rbx, -16(%rsp)
	subq      $16, %rsp
	call      *(program.min)
	addq      $16, %rsp
	movq      %rax, %rbx
	movq      %rbx, -8(%rbp)
	movq      -8(%rbp), %rbx
	movl      %ebx, %edx
	movl      %edx, %ebx
	cmpl      $3, %ebx
	jne       .L3
	jmp       .L4
.L4:
	leaq      .L5, %rdx
	movq      %rdx, -8(%rsp)
	subq      $8, %rsp
	call      *(system.io.print)
	addq      $8, %rsp
.L3:
	#if-then end
	movl      $3, %edx
	negl      %edx
	movq      -8(%rbp), %r8
	movl      %r8d, %r9d
	movl      %r9d, %r8d
	cmpl      %edx, %r8d
	jne       .L6
	jmp       .L7
.L7:
	leaq      .L8, %rdx
	movq      %rdx, -8(%rsp)
	subq      $8, %rsp
	call      *(system.io.print)
	addq      $8, %rsp
.L6:
	#if-then end
	movl      $0, %eax
	leave
	ret       

