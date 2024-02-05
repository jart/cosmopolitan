#include "libc/nt/ntdllimport.h"
.ntimp	NtQueryIntervalProfile,NtQueryIntervalProfile

	.text.windows
	.ftrace1
NtQueryIntervalProfile:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_NtQueryIntervalProfile(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtQueryIntervalProfile,globl
	.previous
