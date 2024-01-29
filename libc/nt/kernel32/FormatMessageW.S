#include "libc/nt/codegen.h"
.imp	kernel32,__imp_FormatMessageW,FormatMessageW

	.text.windows
	.ftrace1
FormatMessage:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_FormatMessageW(%rip),%rax
	jmp	__sysv2nt8
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	FormatMessage,globl
	.previous
