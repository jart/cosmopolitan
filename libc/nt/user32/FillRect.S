#include "libc/nt/codegen.h"
.imp	user32,__imp_FillRect,FillRect

	.text.windows
	.ftrace1
FillRect:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_FillRect(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	FillRect,globl
	.previous
