#include "libc/nt/codegen.h"
.imp	user32,__imp_GetWindowTextW,GetWindowTextW

	.text.windows
	.ftrace1
GetWindowText:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_GetWindowTextW(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	GetWindowText,globl
	.previous
