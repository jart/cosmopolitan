#include "libc/nt/codegen.h"
.imp	gdi32,__imp_SaveDC,SaveDC

	.text.windows
	.ftrace1
SaveDC:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_SaveDC(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	SaveDC,globl
	.previous
