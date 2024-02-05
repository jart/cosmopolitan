#include "libc/nt/codegen.h"
.imp	user32,__imp_PeekMessageW,PeekMessageW

	.text.windows
	.ftrace1
PeekMessage:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_PeekMessageW(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	PeekMessage,globl
	.previous
