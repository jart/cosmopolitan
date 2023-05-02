#include "libc/nt/ntdllimport.h"
.ntimp	NtContinue,NtContinue

#ifdef __x86_64__
	.text.windows
NtContinue:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtContinue(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtContinue,globl
	.previous
#endif
