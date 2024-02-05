#include "libc/nt/codegen.h"
.imp	kernel32,__imp_SetConsoleCtrlHandler,SetConsoleCtrlHandler

	.text.windows
	.ftrace1
SetConsoleCtrlHandler:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_SetConsoleCtrlHandler(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	SetConsoleCtrlHandler,globl
	.previous
