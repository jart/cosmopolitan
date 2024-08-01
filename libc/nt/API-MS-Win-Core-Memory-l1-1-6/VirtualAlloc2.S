#include "libc/nt/codegen.h"
.imp	API-MS-Win-Core-Memory-l1-1-6,__imp_VirtualAlloc2,VirtualAlloc2

	.text.windows
	.ftrace1
VirtualAlloc2:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_VirtualAlloc2(%rip),%rax
	jmp	__sysv2nt8
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	VirtualAlloc2,globl
	.previous
