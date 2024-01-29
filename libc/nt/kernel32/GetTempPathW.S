#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetTempPathW,GetTempPathW

	.text.windows
	.ftrace1
GetTempPath:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_GetTempPathW(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	GetTempPath,globl
	.previous
