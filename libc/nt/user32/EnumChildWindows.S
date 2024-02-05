#include "libc/nt/codegen.h"
.imp	user32,__imp_EnumChildWindows,EnumChildWindows

	.text.windows
	.ftrace1
EnumChildWindows:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_EnumChildWindows(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	EnumChildWindows,globl
	.previous
