#include "libc/nt/codegen.h"
.imp	pdh,__imp_PdhGetFormattedCounterValue,PdhGetFormattedCounterValue

	.text.windows
	.ftrace1
PdhGetFormattedCounterValue:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_PdhGetFormattedCounterValue(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	PdhGetFormattedCounterValue,globl
	.previous
