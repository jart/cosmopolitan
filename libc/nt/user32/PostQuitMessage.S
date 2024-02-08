#include "libc/nt/codegen.h"
.imp	user32,__imp_PostQuitMessage,PostQuitMessage

	.text.windows
	.ftrace1
PostQuitMessage:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_PostQuitMessage(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	PostQuitMessage,globl
	.previous
