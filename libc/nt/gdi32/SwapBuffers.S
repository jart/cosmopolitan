#include "libc/nt/codegen.h"
.imp	gdi32,__imp_SwapBuffers,SwapBuffers

	.text.windows
	.ftrace1
SwapBuffers:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_SwapBuffers(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	SwapBuffers,globl
	.previous
