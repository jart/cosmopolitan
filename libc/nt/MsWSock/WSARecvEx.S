#include "libc/nt/codegen.h"
.imp	MsWSock,__imp_WSARecvEx,WSARecvEx,0

#ifdef __x86_64__
	.text.windows
WSARecvEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSARecvEx(%rip),%rax
	jmp	__sysv2nt
	.endfn	WSARecvEx,globl
	.previous
#endif
