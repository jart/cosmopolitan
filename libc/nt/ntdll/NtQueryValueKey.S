#include "libc/nt/ntdllimport.h"
.ntimp	NtQueryValueKey,NtQueryValueKey

#ifdef __x86_64__
	.text.windows
NtQueryValueKey:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtQueryValueKey(%rip),%rax
	jmp	__sysv2nt6
	.endfn	NtQueryValueKey,globl
	.previous
#endif
