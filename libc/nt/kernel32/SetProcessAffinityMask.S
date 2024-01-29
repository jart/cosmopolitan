#include "libc/nt/codegen.h"
.imp	kernel32,__imp_SetProcessAffinityMask,SetProcessAffinityMask

	.text.windows
	.ftrace1
SetProcessAffinityMask:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_SetProcessAffinityMask(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	SetProcessAffinityMask,globl
	.previous
