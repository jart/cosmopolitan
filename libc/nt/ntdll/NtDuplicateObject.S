#include "libc/nt/ntdllimport.h"
.ntimp	NtDuplicateObject,NtDuplicateObject

#ifdef __x86_64__
	.text.windows
NtDuplicateObject:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtDuplicateObject(%rip),%rax
	jmp	__sysv2nt8
	.endfn	NtDuplicateObject,globl
	.previous
#endif
