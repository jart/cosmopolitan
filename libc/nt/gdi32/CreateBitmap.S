#include "libc/nt/codegen.h"
.imp	gdi32,__imp_CreateBitmap,CreateBitmap

	.text.windows
	.ftrace1
CreateBitmap:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_CreateBitmap(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	CreateBitmap,globl
	.previous
