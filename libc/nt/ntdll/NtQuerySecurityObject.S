#include "libc/nt/ntdllimport.h"
.ntimp	NtQuerySecurityObject,NtQuerySecurityObject

#ifdef __x86_64__
	.text.windows
NtQuerySecurityObject:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtQuerySecurityObject(%rip),%rax
	jmp	__sysv2nt6
	.endfn	NtQuerySecurityObject,globl
	.previous
#endif
