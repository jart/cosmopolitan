#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSARecvFrom,WSARecvFrom

	.text.windows
        .ftrace1
__WSARecvFrom:
        .ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_WSARecvFrom(%rip),%rax
	jmp	__sysv2nt10
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	__WSARecvFrom,globl
	.previous
