#include "libc/nt/ntdllimport.h"
.ntimp	RtlReAllocateHeap,RtlReAllocateHeap

	.text.windows
RtlReAllocateHeap:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RtlReAllocateHeap(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	RtlReAllocateHeap,globl
	.previous
