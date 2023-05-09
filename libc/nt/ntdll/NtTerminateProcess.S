#include "libc/nt/ntdllimport.h"
.ntimp	NtTerminateProcess,NtTerminateProcess

	.text.windows
NtTerminateProcess:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtTerminateProcess(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtTerminateProcess,globl
	.previous
