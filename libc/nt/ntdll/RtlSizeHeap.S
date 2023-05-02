#include "libc/nt/ntdllimport.h"
.ntimp	RtlSizeHeap,RtlSizeHeap

#ifdef __x86_64__
	.text.windows
RtlSizeHeap:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RtlSizeHeap(%rip),%rax
	jmp	__sysv2nt
	.endfn	RtlSizeHeap,globl
	.previous
#endif
