#include "libc/nt/codegen.h"
.imp	kernel32,__imp_LoadResource,LoadResource

	.text.windows
	.ftrace1
LoadResource:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_LoadResource(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	LoadResource,globl
	.previous
