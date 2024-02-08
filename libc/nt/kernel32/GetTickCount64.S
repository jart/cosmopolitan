#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetTickCount64,GetTickCount64

	.text.windows
	.ftrace1
GetTickCount64:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	sub	$32,%rsp
	call	*__imp_GetTickCount64(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	GetTickCount64,globl
	.previous
