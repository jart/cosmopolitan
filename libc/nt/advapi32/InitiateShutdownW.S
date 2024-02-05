#include "libc/nt/codegen.h"
.imp	advapi32,__imp_InitiateShutdownW,InitiateShutdownW

	.text.windows
	.ftrace1
InitiateShutdown:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_InitiateShutdownW(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	InitiateShutdown,globl
	.previous
