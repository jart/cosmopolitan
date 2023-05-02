#include "libc/nt/ntdllimport.h"
.ntimp	RtlReAllocateHeap,RtlReAllocateHeap

#ifdef __x86_64__
	.text.windows
RtlReAllocateHeap:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RtlReAllocateHeap(%rip),%rax
	jmp	__sysv2nt
	.endfn	RtlReAllocateHeap,globl
	.previous
#endif
