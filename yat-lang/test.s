.text
	.globl main
main:
	leaq      .L0, %rbx
	movq      %rbx, system.io.Print
	leaq      .L1, %rbx
	movq      %rbx, system.io.Read
	leaq      .L3, %r8
	movq      %r8, program.main
	call      *(program.main)
	ret

.data
.L2:
	.hword 0
system.io.tempmessage:
	.zero 8
system.io.Print:
	.zero 8
system.io.Read:
	.zero 8
program.a:
	.zero 4
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
	leaq      .L2, %rax
	leave
	ret       
.L3:
	pushq     %rbp
	movq      %rsp, %rbp
	subq      $36, %rsp
	movl      $0, %ebx
	movl      %ebx, -4(%rbp)
	#while start
.L4:
	movb      -4(%rbp), %bl
	cmpb      $10, %bl
	jg        .L5
	jmp       .L6
.L6:
	movl      -4(%rbp), %r8d
	movl      %r8d, -4(%rbp)
	movl      -4(%rbp), %r8d
	addl      $1, %r8d
	movl      %r8d, -4(%rbp)
	jmp       .L4
	#while end
.L5:
	movl      $0, %eax
	leave
	ret       

