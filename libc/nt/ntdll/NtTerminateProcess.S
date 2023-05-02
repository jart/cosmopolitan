#include "libc/nt/ntdllimport.h"
.ntimp	NtTerminateProcess,NtTerminateProcess

#ifdef __x86_64__
	.text.windows
NtTerminateProcess:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtTerminateProcess(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtTerminateProcess,globl
	.previous
#endif
