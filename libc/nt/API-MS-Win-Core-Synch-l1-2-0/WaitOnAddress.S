#include "libc/nt/codegen.h"
.imp	API-MS-Win-Core-Synch-l1-2-0,__imp_WaitOnAddress,WaitOnAddress

	.text.windows
	.ftrace1
WaitOnAddress:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_WaitOnAddress(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	WaitOnAddress,globl
	.previous
