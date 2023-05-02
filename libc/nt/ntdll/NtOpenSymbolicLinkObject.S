#include "libc/nt/ntdllimport.h"
.ntimp	NtOpenSymbolicLinkObject,NtOpenSymbolicLinkObject

#ifdef __x86_64__
	.text.windows
NtOpenSymbolicLinkObject:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtOpenSymbolicLinkObject(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtOpenSymbolicLinkObject,globl
	.previous
#endif
