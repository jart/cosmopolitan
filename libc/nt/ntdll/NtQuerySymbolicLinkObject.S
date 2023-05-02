#include "libc/nt/ntdllimport.h"
.ntimp	NtQuerySymbolicLinkObject,NtQuerySymbolicLinkObject

#ifdef __x86_64__
	.text.windows
NtQuerySymbolicLinkObject:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtQuerySymbolicLinkObject(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtQuerySymbolicLinkObject,globl
	.previous
#endif
