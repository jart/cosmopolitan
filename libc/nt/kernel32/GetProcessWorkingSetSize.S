#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetProcessWorkingSetSize,GetProcessWorkingSetSize

	.text.windows
	.ftrace1
GetProcessWorkingSetSize:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_GetProcessWorkingSetSize(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	GetProcessWorkingSetSize,globl
	.previous
