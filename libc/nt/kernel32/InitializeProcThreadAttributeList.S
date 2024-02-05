#include "libc/nt/codegen.h"
.imp	kernel32,__imp_InitializeProcThreadAttributeList,InitializeProcThreadAttributeList

	.text.windows
	.ftrace1
InitializeProcThreadAttributeList:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_InitializeProcThreadAttributeList(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	InitializeProcThreadAttributeList,globl
	.previous
