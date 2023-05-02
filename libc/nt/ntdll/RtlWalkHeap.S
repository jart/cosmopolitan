#include "libc/nt/ntdllimport.h"
.ntimp	RtlWalkHeap,RtlWalkHeap

#ifdef __x86_64__
	.text.windows
RtlWalkHeap:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RtlWalkHeap(%rip),%rax
	jmp	__sysv2nt
	.endfn	RtlWalkHeap,globl
	.previous
#endif
