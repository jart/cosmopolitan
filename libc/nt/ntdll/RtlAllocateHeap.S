#include "libc/nt/ntdllimport.h"
.ntimp	RtlAllocateHeap,RtlAllocateHeap

	.text.windows
	.ftrace1
RtlAllocateHeap:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_RtlAllocateHeap(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	RtlAllocateHeap,globl
	.previous
