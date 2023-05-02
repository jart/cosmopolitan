#include "libc/nt/ntdllimport.h"
.ntimp	NtQueryFullAttributesFile,NtQueryFullAttributesFile

#ifdef __x86_64__
	.text.windows
NtQueryFullAttributesFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtQueryFullAttributesFile(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtQueryFullAttributesFile,globl
	.previous
#endif
