#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_bind,bind

	.text.windows
        .ftrace1
__sys_bind_nt:
        .ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_bind(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	__sys_bind_nt,globl
	.previous
