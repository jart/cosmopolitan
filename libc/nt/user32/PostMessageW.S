#include "libc/nt/codegen.h"
.imp	user32,__imp_PostMessageW,PostMessageW

	.text.windows
	.ftrace1
PostMessage:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_PostMessageW(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	PostMessage,globl
	.previous
