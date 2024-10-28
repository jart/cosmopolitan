#include "libc/nt/codegen.h"
.imp	gdi32,__imp_DescribePixelFormat,DescribePixelFormat

	.text.windows
	.ftrace1
DescribePixelFormat:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_DescribePixelFormat(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	DescribePixelFormat,globl
	.previous
