#include "libc/nt/ntdllimport.h"
.ntimp	NtSuspendThread,NtSuspendThread

#ifdef __x86_64__
	.text.windows
NtSuspendThread:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtSuspendThread(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtSuspendThread,globl
	.previous
#endif
