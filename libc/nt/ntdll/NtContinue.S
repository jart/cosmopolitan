#include "libc/nt/ntdllimport.h"
.ntimp	NtContinue,NtContinue

	.text.windows
	.ftrace1
NtContinue:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_NtContinue(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtContinue,globl
	.previous
