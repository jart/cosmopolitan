#include "libc/nt/ntdllimport.h"
.ntimp	NtQueryAttributesFile,NtQueryAttributesFile

#ifdef __x86_64__
	.text.windows
NtQueryAttributesFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtQueryAttributesFile(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtQueryAttributesFile,globl
	.previous
#endif
