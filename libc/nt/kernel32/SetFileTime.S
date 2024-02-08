#include "libc/nt/codegen.h"
.imp	kernel32,__imp_SetFileTime,SetFileTime

	.text.windows
	.ftrace1
SetFileTime:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_SetFileTime(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	SetFileTime,globl
	.previous
