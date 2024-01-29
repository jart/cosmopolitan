#include "libc/nt/ntdllimport.h"
.ntimp	RtlCreateProcessParameters,RtlCreateProcessParameters

	.text.windows
	.ftrace1
RtlCreateProcessParameters:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_RtlCreateProcessParameters(%rip),%rax
	jmp	__sysv2nt10
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	RtlCreateProcessParameters,globl
	.previous
