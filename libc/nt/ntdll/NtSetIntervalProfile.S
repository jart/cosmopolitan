#include "libc/nt/ntdllimport.h"
.ntimp	NtSetIntervalProfile,NtSetIntervalProfile

#ifdef __x86_64__
	.text.windows
NtSetIntervalProfile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtSetIntervalProfile(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtSetIntervalProfile,globl
	.previous
#endif
