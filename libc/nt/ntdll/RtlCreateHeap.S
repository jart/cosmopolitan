#include "libc/nt/ntdllimport.h"
.ntimp	RtlCreateHeap,RtlCreateHeap

#ifdef __x86_64__
	.text.windows
RtlCreateHeap:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RtlCreateHeap(%rip),%rax
	jmp	__sysv2nt6
	.endfn	RtlCreateHeap,globl
	.previous
#endif
