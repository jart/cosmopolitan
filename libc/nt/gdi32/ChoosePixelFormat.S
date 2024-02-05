#include "libc/nt/codegen.h"
.imp	gdi32,__imp_ChoosePixelFormat,ChoosePixelFormat

	.text.windows
	.ftrace1
ChoosePixelFormat:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_ChoosePixelFormat(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	ChoosePixelFormat,globl
	.previous
