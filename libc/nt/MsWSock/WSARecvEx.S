#include "libc/nt/codegen.h"
.imp	MsWSock,__imp_WSARecvEx,WSARecvEx,0

	.text.windows
WSARecvEx:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSARecvEx(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	WSARecvEx,globl
	.previous
