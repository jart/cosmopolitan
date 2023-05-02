#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSARecvFrom,WSARecvFrom,93

#ifdef __x86_64__
	.text.windows
__WSARecvFrom:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSARecvFrom(%rip),%rax
	jmp	__sysv2nt10
	.endfn	__WSARecvFrom,globl
	.previous
#endif
