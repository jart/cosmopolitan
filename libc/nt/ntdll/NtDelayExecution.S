#include "libc/nt/ntdllimport.h"
.ntimp	NtDelayExecution,NtDelayExecution

	.text.windows
NtDelayExecution:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtDelayExecution(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtDelayExecution,globl
	.previous
