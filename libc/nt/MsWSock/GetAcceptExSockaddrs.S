#include "libc/nt/codegen.h"
.imp	MsWSock,__imp_GetAcceptExSockaddrs,GetAcceptExSockaddrs,0

	.text.windows
GetAcceptExSockaddrs:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetAcceptExSockaddrs(%rip),%rax
	jmp	__sysv2nt8
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	GetAcceptExSockaddrs,globl
	.previous
