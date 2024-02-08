#include "libc/nt/codegen.h"
.imp	user32,__imp_FindWindowExW,FindWindowExW

	.text.windows
	.ftrace1
FindWindowEx:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_FindWindowExW(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	FindWindowEx,globl
	.previous
