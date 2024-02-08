#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetNumberOfConsoleInputEvents,GetNumberOfConsoleInputEvents

	.text.windows
	.ftrace1
GetNumberOfConsoleInputEvents:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_GetNumberOfConsoleInputEvents(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	GetNumberOfConsoleInputEvents,globl
	.previous
