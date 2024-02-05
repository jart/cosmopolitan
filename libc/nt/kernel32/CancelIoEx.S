#include "libc/nt/codegen.h"
.imp	kernel32,__imp_CancelIoEx,CancelIoEx

	.text.windows
	.ftrace1
CancelIoEx:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_CancelIoEx(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	CancelIoEx,globl
	.previous
