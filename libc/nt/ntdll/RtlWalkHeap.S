#include "libc/nt/ntdllimport.h"
.ntimp	RtlWalkHeap,RtlWalkHeap

	.text.windows
	.ftrace1
RtlWalkHeap:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_RtlWalkHeap(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	RtlWalkHeap,globl
	.previous
