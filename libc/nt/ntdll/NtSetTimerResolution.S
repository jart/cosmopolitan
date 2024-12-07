#include "libc/nt/ntdllimport.h"
.ntimp	NtSetTimerResolution,NtSetTimerResolution

	.text.windows
	.ftrace1
NtSetTimerResolution:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_NtSetTimerResolution(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtSetTimerResolution,globl
	.previous
