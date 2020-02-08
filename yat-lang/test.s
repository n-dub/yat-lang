.text
	.globl main
main:
	leaq      .L0, %rbx
	movq      %rbx, system.io.PrintStr
	leaq      .L1, %rbx
	movq      %rbx, system.io.PrintInt
	leaq      .L2, %rbx
	movq      %rbx, system.io.ReadStr
	leaq      .L4, %rbx
	movq      %rbx, system.io.ReadInt
	leaq      .L5, %r10
	movq      %r10, program.main
	call      *(program.main)
	ret

.data
.L3:
	.hword 0
.L9:
	.hword 105, 32, 61, 32, 0
.L10:
	.hword 10, 97, 91, 105, 93, 32, 61, 32, 0
.L11:
	.hword 10, 0
system.io.PrintStr:
	.zero 8
system.io.PrintInt:
	.zero 8
system.io.ReadStr:
	.zero 8
system.io.ReadInt:
	.zero 8
program.a:
	.zero 4000
program.main:
	.zero 8
.text
.L0:
	pushq     %rbp
	movq      %rsp, %rbp
	subq      $36, %rsp
	movl      $0, %ebx
	movl      %ebx, -4(%rbp)
	
.data
m0:
	.ascii "\0"
fmtstr0:
	.ascii "%ls\0"
.text
	leaq      m0, %rdx
	movl      $0, %ecx
	call      setlocale
	
	movq      16(%rbp), %rdx
	leaq      fmtstr0, %rcx
	call      printf
	xor       %rax, %rax
	leave
	ret
.L1:
	pushq     %rbp
	movq      %rsp, %rbp
	subq      $36, %rsp
	movl      $0, %ebx
	movl      %ebx, -4(%rbp)
	
.data
fmtstr1:
	.ascii "%lli\0"
.text
	movq      16(%rbp), %rdx
	leaq      fmtstr1, %rcx
	call      printf
	xor       %rax, %rax
	leave
	ret
.L2:
	pushq     %rbp
	movq      %rsp, %rbp
	subq      $32, %rsp
	leaq      .L3, %rax
	leave
	ret       
.L4:
	pushq     %rbp
	movq      %rsp, %rbp
	subq      $40, %rsp
	movq      $0, %rbx
	movq      %rbx, -8(%rbp)
	
.data
fmtstr2:
	.ascii "%lli\0"
.text
	leaq      -8(%rbp), %rdx
	leaq      fmtstr2, %rcx
	call      scanf
	xor       %rax, %rax
	movq      -8(%rbp), %rax
	leave
	ret       
.L5:
	pushq     %rbp
	movq      %rsp, %rbp
	subq      $44, %rsp
	subq      $0, %rsp
	call      *(system.io.ReadInt)
	addq      $0, %rsp
	movq      %rax, %rbx
	movl      %ebx, %r8d
	movl      %r8d, -4(%rbp)
	movq      $1, %rbx
	movq      %rbx, -12(%rbp)
	#while start
.L6:
	movq      -12(%rbp), %rbx
	movl      %ebx, %r8d
	cmpl      -4(%rbp), %r8d
	jg        .L7
	jmp       .L8
.L8:
	movq      -12(%rbp), %rbx
	subq      $1, %rbx
	movq      -12(%rbp), %r9
	movl      %r9d, %r10d
	movl      %r10d, program.a(, %rbx, 4)
	leaq      .L9, %r9
	movq      %r9, -8(%rsp)
	subq      $8, %rsp
	call      *(system.io.PrintStr)
	addq      $8, %rsp
	movq      -12(%rbp), %r9
	movq      %r9, -8(%rsp)
	subq      $8, %rsp
	call      *(system.io.PrintInt)
	addq      $8, %rsp
	leaq      .L10, %r9
	movq      %r9, -8(%rsp)
	subq      $8, %rsp
	call      *(system.io.PrintStr)
	addq      $8, %rsp
	movq      -12(%rbp), %r9
	subq      $1, %r9
	movl      program.a(, %r9, 4), %r10d
	movslq    %r10d, %r11
	movq      %r11, %r10
	movq      %r10, -8(%rsp)
	subq      $8, %rsp
	call      *(system.io.PrintInt)
	addq      $8, %rsp
	leaq      .L11, %r10
	movq      %r10, -8(%rsp)
	subq      $8, %rsp
	call      *(system.io.PrintStr)
	addq      $8, %rsp
	movq      -12(%rbp), %r10
	addq      $1, %r10
	movq      %r10, -12(%rbp)
	jmp       .L6
	#while end
.L7:
	movl      $0, %eax
	leave
	ret       

