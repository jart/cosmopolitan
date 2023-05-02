#include "libc/nt/ntdllimport.h"
.ntimp	NtQueryPerformanceCounter,NtQueryPerformanceCounter

#ifdef __x86_64__
	.text.windows
NtQueryPerformanceCounter:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtQueryPerformanceCounter(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtQueryPerformanceCounter,globl
	.previous
#endif
