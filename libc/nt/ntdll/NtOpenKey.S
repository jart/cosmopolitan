#include "libc/nt/ntdllimport.h"
.ntimp	NtOpenKey,NtOpenKey

#ifdef __x86_64__
	.text.windows
NtOpenKey:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtOpenKey(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtOpenKey,globl
	.previous
#endif
