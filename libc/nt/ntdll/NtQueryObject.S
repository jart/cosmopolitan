#include "libc/nt/ntdllimport.h"
.ntimp	NtQueryObject,NtQueryObject

#ifdef __x86_64__
	.text.windows
NtQueryObject:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtQueryObject(%rip),%rax
	jmp	__sysv2nt6
	.endfn	NtQueryObject,globl
	.previous
#endif
