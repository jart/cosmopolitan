#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetACP,GetACP

	.text.windows
	.ftrace1
GetACP:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	sub	$32,%rsp
	call	*__imp_GetACP(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	GetACP,globl
	.previous
