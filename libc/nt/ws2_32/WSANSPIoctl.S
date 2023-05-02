#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSANSPIoctl,WSANSPIoctl,85

#ifdef __x86_64__
	.text.windows
WSANSPIoctl:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSANSPIoctl(%rip),%rax
	jmp	__sysv2nt8
	.endfn	WSANSPIoctl,globl
	.previous
#endif
