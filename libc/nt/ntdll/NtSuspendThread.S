#include "libc/nt/ntdllimport.h"
.ntimp	NtSuspendThread,NtSuspendThread

	.text.windows
	.ftrace1
NtSuspendThread:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_NtSuspendThread(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtSuspendThread,globl
	.previous
