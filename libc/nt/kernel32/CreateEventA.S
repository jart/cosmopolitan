#include "libc/nt/codegen.h"
.imp	kernel32,__imp_CreateEventA,CreateEventA

	.text.windows
	.ftrace1
CreateEventA:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_CreateEventA(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	CreateEventA,globl
	.previous
