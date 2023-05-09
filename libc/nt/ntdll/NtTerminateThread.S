#include "libc/nt/ntdllimport.h"
.ntimp	NtTerminateThread,NtTerminateThread

	.text.windows
NtTerminateThread:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtTerminateThread(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtTerminateThread,globl
	.previous
