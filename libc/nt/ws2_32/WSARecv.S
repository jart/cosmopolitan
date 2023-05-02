#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSARecv,WSARecv,91

#ifdef __x86_64__
	.text.windows
__WSARecv:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSARecv(%rip),%rax
	jmp	__sysv2nt8
	.endfn	__WSARecv,globl
	.previous
#endif
