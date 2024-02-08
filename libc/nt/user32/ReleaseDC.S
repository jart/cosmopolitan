#include "libc/nt/codegen.h"
.imp	user32,__imp_ReleaseDC,ReleaseDC

	.text.windows
	.ftrace1
ReleaseDC:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_ReleaseDC(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	ReleaseDC,globl
	.previous
