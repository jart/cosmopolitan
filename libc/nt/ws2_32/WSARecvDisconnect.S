#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSARecvDisconnect,WSARecvDisconnect,92

#ifdef __x86_64__
	.text.windows
WSARecvDisconnect:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSARecvDisconnect(%rip),%rax
	jmp	__sysv2nt
	.endfn	WSARecvDisconnect,globl
	.previous
#endif
