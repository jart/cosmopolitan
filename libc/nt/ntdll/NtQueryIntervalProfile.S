#include "libc/nt/ntdllimport.h"
.ntimp	NtQueryIntervalProfile,NtQueryIntervalProfile

	.text.windows
NtQueryIntervalProfile:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtQueryIntervalProfile(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtQueryIntervalProfile,globl
	.previous
