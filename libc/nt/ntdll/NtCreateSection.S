#include "libc/nt/ntdllimport.h"
.ntimp	NtCreateSection,NtCreateSection

#ifdef __x86_64__
	.text.windows
NtCreateSection:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtCreateSection(%rip),%rax
	jmp	__sysv2nt8
	.endfn	NtCreateSection,globl
	.previous
#endif
