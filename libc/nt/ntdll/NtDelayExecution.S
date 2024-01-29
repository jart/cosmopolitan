#include "libc/nt/ntdllimport.h"
.ntimp	NtDelayExecution,NtDelayExecution

	.text.windows
	.ftrace1
NtDelayExecution:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_NtDelayExecution(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtDelayExecution,globl
	.previous
