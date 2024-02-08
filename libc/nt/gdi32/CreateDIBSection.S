#include "libc/nt/codegen.h"
.imp	gdi32,__imp_CreateDIBSection,CreateDIBSection

	.text.windows
	.ftrace1
CreateDIBSection:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_CreateDIBSection(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	CreateDIBSection,globl
	.previous
