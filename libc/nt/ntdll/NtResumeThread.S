#include "libc/nt/ntdllimport.h"
.ntimp	NtResumeThread,NtResumeThread

	.text.windows
	.ftrace1
NtResumeThread:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_NtResumeThread(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtResumeThread,globl
	.previous
