#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSAIoctl,WSAIoctl,78

#ifdef __x86_64__
	.text.windows
WSAIoctl:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSAIoctl(%rip),%rax
	jmp	__sysv2nt10
	.endfn	WSAIoctl,globl
	.previous
#endif
