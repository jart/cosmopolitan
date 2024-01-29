#include "libc/nt/codegen.h"
.imp	user32,__imp_MoveWindow,MoveWindow

	.text.windows
	.ftrace1
MoveWindow:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_MoveWindow(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	MoveWindow,globl
	.previous
