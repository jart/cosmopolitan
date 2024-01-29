#include "libc/nt/codegen.h"
.imp	user32,__imp_GetCursor,GetCursor

	.text.windows
	.ftrace1
GetCursor:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	sub	$32,%rsp
	call	*__imp_GetCursor(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	GetCursor,globl
	.previous
