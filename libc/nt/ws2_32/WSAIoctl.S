#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSAIoctl,WSAIoctl

	.text.windows
	.ftrace1
WSAIoctl:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_WSAIoctl(%rip),%rax
	jmp	__sysv2nt10
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	WSAIoctl,globl
	.previous
