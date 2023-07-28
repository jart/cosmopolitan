#include "libc/nt/codegen.h"
.imp	MsWSock,__imp_AcceptEx,AcceptEx

	.text.windows
        .ftrace1
AcceptEx:
        .ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_AcceptEx(%rip),%rax
	jmp	__sysv2nt8
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	AcceptEx,globl
	.previous
