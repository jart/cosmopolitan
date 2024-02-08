#include "libc/nt/codegen.h"
.imp	kernel32,__imp_AddVectoredExceptionHandler,AddVectoredExceptionHandler

	.text.windows
	.ftrace1
AddVectoredExceptionHandler:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_AddVectoredExceptionHandler(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	AddVectoredExceptionHandler,globl
	.previous
