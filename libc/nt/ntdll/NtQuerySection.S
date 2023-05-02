#include "libc/nt/ntdllimport.h"
.ntimp	NtQuerySection,NtQuerySection

#ifdef __x86_64__
	.text.windows
NtQuerySection:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtQuerySection(%rip),%rax
	jmp	__sysv2nt6
	.endfn	NtQuerySection,globl
	.previous
#endif
