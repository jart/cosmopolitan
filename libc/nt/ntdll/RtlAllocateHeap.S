#include "libc/nt/ntdllimport.h"
.ntimp	RtlAllocateHeap,RtlAllocateHeap

#ifdef __x86_64__
	.text.windows
RtlAllocateHeap:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RtlAllocateHeap(%rip),%rax
	jmp	__sysv2nt
	.endfn	RtlAllocateHeap,globl
	.previous
#endif
