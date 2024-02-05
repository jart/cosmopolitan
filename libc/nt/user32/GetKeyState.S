#include "libc/nt/codegen.h"
.imp	user32,__imp_GetKeyState,GetKeyState

	.text.windows
	.ftrace1
GetKeyState:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetKeyState(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	GetKeyState,globl
	.previous
