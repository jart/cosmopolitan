#include "libc/nt/ntdllimport.h"
.ntimp	NtQuerySystemInformation,NtQuerySystemInformation

#ifdef __x86_64__
	.text.windows
NtQuerySystemInformation:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtQuerySystemInformation(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtQuerySystemInformation,globl
	.previous
#endif
