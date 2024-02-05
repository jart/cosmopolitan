#include "libc/nt/codegen.h"
.imp	user32,__imp_GetCursorPos,GetCursorPos

	.text.windows
	.ftrace1
GetCursorPos:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetCursorPos(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	GetCursorPos,globl
	.previous
