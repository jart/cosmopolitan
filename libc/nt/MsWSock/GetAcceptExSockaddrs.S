#include "libc/nt/codegen.h"
.imp	MsWSock,__imp_GetAcceptExSockaddrs,GetAcceptExSockaddrs

	.text.windows
        .ftrace1
GetAcceptExSockaddrs:
        .ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_GetAcceptExSockaddrs(%rip),%rax
	jmp	__sysv2nt8
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	GetAcceptExSockaddrs,globl
	.previous
