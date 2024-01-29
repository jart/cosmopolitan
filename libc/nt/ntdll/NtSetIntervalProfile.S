#include "libc/nt/ntdllimport.h"
.ntimp	NtSetIntervalProfile,NtSetIntervalProfile

	.text.windows
	.ftrace1
NtSetIntervalProfile:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_NtSetIntervalProfile(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtSetIntervalProfile,globl
	.previous
