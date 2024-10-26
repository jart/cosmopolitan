#include "libc/nt/codegen.h"
.imp	shell32,__imp_CommandLineToArgvW,CommandLineToArgvW

	.text.windows
	.ftrace1
CommandLineToArgv:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_CommandLineToArgvW(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	CommandLineToArgv,globl
	.previous
