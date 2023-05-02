#include "libc/nt/ntdllimport.h"
.ntimp	NtSetTimer,NtSetTimer

#ifdef __x86_64__
	.text.windows
NtSetTimer:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtSetTimer(%rip),%rax
	jmp	__sysv2nt8
	.endfn	NtSetTimer,globl
	.previous
#endif
