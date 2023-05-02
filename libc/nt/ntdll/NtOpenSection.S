#include "libc/nt/ntdllimport.h"
.ntimp	NtOpenSection,NtOpenSection

#ifdef __x86_64__
	.text.windows
NtOpenSection:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtOpenSection(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtOpenSection,globl
	.previous
#endif
