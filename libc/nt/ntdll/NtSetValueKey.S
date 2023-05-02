#include "libc/nt/ntdllimport.h"
.ntimp	NtSetValueKey,NtSetValueKey

#ifdef __x86_64__
	.text.windows
NtSetValueKey:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtSetValueKey(%rip),%rax
	jmp	__sysv2nt6
	.endfn	NtSetValueKey,globl
	.previous
#endif
