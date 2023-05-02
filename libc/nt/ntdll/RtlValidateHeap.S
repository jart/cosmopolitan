#include "libc/nt/ntdllimport.h"
.ntimp	RtlValidateHeap,RtlValidateHeap

#ifdef __x86_64__
	.text.windows
RtlValidateHeap:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RtlValidateHeap(%rip),%rax
	jmp	__sysv2nt
	.endfn	RtlValidateHeap,globl
	.previous
#endif
