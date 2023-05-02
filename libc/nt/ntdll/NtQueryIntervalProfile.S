#include "libc/nt/ntdllimport.h"
.ntimp	NtQueryIntervalProfile,NtQueryIntervalProfile

#ifdef __x86_64__
	.text.windows
NtQueryIntervalProfile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtQueryIntervalProfile(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtQueryIntervalProfile,globl
	.previous
#endif
