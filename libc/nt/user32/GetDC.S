#include "libc/nt/codegen.h"
.imp	user32,__imp_GetDC,GetDC

	.text.windows
	.ftrace1
GetDC:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetDC(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	GetDC,globl
	.previous
