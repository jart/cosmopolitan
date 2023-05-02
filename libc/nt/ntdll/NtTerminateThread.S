#include "libc/nt/ntdllimport.h"
.ntimp	NtTerminateThread,NtTerminateThread

#ifdef __x86_64__
	.text.windows
NtTerminateThread:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtTerminateThread(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtTerminateThread,globl
	.previous
#endif
