#include "libc/nt/ntdllimport.h"
.ntimp	NtQueryTimerResolution,NtQueryTimerResolution

	.text.windows
	.ftrace1
NtQueryTimerResolution:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_NtQueryTimerResolution(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtQueryTimerResolution,globl
	.previous
