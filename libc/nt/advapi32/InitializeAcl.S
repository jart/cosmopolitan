#include "libc/nt/codegen.h"
.imp	advapi32,__imp_InitializeAcl,InitializeAcl

	.text.windows
	.ftrace1
InitializeAcl:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_InitializeAcl(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	InitializeAcl,globl
	.previous
