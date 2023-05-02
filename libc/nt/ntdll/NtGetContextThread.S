#include "libc/nt/ntdllimport.h"
.ntimp	NtGetContextThread,NtGetContextThread

#ifdef __x86_64__
	.text.windows
NtGetContextThread:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtGetContextThread(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtGetContextThread,globl
	.previous
#endif
