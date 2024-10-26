#include "libc/nt/codegen.h"
.imp	user32,__imp_UnregisterClassW,UnregisterClassW

	.text.windows
	.ftrace1
UnregisterClass:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_UnregisterClassW(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	UnregisterClass,globl
	.previous
