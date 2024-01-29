#include "libc/nt/ntdllimport.h"
.ntimp	RtlCloneUserProcess,RtlCloneUserProcess

	.text.windows
	.ftrace1
RtlCloneUserProcess:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_RtlCloneUserProcess(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	RtlCloneUserProcess,globl
	.previous
