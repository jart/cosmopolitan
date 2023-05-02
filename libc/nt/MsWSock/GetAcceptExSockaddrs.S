#include "libc/nt/codegen.h"
.imp	MsWSock,__imp_GetAcceptExSockaddrs,GetAcceptExSockaddrs,0

#ifdef __x86_64__
	.text.windows
GetAcceptExSockaddrs:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetAcceptExSockaddrs(%rip),%rax
	jmp	__sysv2nt8
	.endfn	GetAcceptExSockaddrs,globl
	.previous
#endif
