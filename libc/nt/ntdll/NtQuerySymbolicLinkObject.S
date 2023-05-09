#include "libc/nt/ntdllimport.h"
.ntimp	NtQuerySymbolicLinkObject,NtQuerySymbolicLinkObject

	.text.windows
NtQuerySymbolicLinkObject:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtQuerySymbolicLinkObject(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtQuerySymbolicLinkObject,globl
	.previous
