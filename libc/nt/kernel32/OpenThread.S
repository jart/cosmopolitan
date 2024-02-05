#include "libc/nt/codegen.h"
.imp	kernel32,__imp_OpenThread,OpenThread

	.text.windows
	.ftrace1
OpenThread:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_OpenThread(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	OpenThread,globl
	.previous
