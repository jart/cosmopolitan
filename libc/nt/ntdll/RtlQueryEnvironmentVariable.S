#include "libc/nt/ntdllimport.h"
.ntimp	RtlQueryEnvironmentVariable,RtlQueryEnvironmentVariable

	.text.windows
	.ftrace1
RtlQueryEnvironmentVariable:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_RtlQueryEnvironmentVariable(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	RtlQueryEnvironmentVariable,globl
	.previous
