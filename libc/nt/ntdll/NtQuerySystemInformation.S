#include "libc/nt/ntdllimport.h"
.ntimp	NtQuerySystemInformation,NtQuerySystemInformation

	.text.windows
NtQuerySystemInformation:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtQuerySystemInformation(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtQuerySystemInformation,globl
	.previous
