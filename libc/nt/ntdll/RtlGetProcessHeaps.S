#include "libc/nt/ntdllimport.h"
.ntimp	RtlGetProcessHeaps,RtlGetProcessHeaps

#ifdef __x86_64__
	.text.windows
RtlGetProcessHeaps:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RtlGetProcessHeaps(%rip),%rax
	jmp	__sysv2nt
	.endfn	RtlGetProcessHeaps,globl
	.previous
#endif
