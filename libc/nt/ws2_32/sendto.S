#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_sendto,sendto

	.text.windows
        .ftrace1
__sys_sendto_nt:
        .ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_sendto(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	__sys_sendto_nt,globl
	.previous
