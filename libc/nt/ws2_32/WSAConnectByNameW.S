#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSAConnectByNameW,WSAConnectByNameW

	.text.windows
	.ftrace1
WSAConnectByName:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_WSAConnectByNameW(%rip),%rax
	jmp	__sysv2nt10
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	WSAConnectByName,globl
	.previous
