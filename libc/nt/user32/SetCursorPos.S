#include "libc/nt/codegen.h"
.imp	user32,__imp_SetCursorPos,SetCursorPos

	.text.windows
	.ftrace1
SetCursorPos:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_SetCursorPos(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	SetCursorPos,globl
	.previous
