#include "libc/nt/ntdllimport.h"
.ntimp	NtSetContextThread,NtSetContextThread

#ifdef __x86_64__
	.text.windows
NtSetContextThread:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtSetContextThread(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtSetContextThread,globl
	.previous
#endif
