#include "libc/nt/codegen.h"
.imp	user32,__imp_GetAsyncKeyState,GetAsyncKeyState

	.text.windows
	.ftrace1
GetAsyncKeyState:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetAsyncKeyState(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	GetAsyncKeyState,globl
	.previous
