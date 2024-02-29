#include "libc/nt/codegen.h"
.imp	BCryptPrimitives,__imp_ProcessPrng,ProcessPrng

	.text.windows
	.ftrace1
ProcessPrng:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_ProcessPrng(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	ProcessPrng,globl
	.previous
