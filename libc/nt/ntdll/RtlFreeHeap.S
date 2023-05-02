#include "libc/nt/ntdllimport.h"
.ntimp	RtlFreeHeap,RtlFreeHeap

#ifdef __x86_64__
	.text.windows
RtlFreeHeap:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RtlFreeHeap(%rip),%rax
	jmp	__sysv2nt
	.endfn	RtlFreeHeap,globl
	.previous
#endif
