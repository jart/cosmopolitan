#include "libc/nt/ntdllimport.h"
.ntimp	NtSetIntervalProfile,NtSetIntervalProfile

	.text.windows
NtSetIntervalProfile:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtSetIntervalProfile(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtSetIntervalProfile,globl
	.previous
