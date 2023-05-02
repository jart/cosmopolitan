#include "libc/nt/ntdllimport.h"
.ntimp	NtWaitForSingleObject,NtWaitForSingleObject

#ifdef __x86_64__
	.text.windows
NtWaitForSingleObject:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtWaitForSingleObject(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtWaitForSingleObject,globl
	.previous
#endif
