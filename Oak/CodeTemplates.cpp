#include "CodeTemplates.h"

// TODO: change hard-coded allocated stack size

const String fn_header =
LR"(
    pushq %rbp
    movq %rsp, %rbp
    subq $48, %rsp
)";

const String fn_footer =
LR"(
    movl	$0, %eax
    addq	$48, %rsp
    popq	%rbp
    ret
)";

