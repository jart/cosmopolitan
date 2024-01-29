#include "libc/nt/ntdllimport.h"
.ntimp	NtYieldExecution,NtYieldExecution

	.text.windows
	.ftrace1
NtYieldExecution:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	sub	$32,%rsp
	call	*__imp_NtYieldExecution(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	NtYieldExecution,globl
	.previous
