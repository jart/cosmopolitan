#include "libc/nt/ntdllimport.h"
.ntimp	NtDelayExecution,NtDelayExecution

#ifdef __x86_64__
	.text.windows
NtDelayExecution:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtDelayExecution(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtDelayExecution,globl
	.previous
#endif
