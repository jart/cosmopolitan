#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_getpeername,getpeername

	.text.windows
        .ftrace1
__sys_getpeername_nt:
        .ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_getpeername(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	__sys_getpeername_nt,globl
	.previous
