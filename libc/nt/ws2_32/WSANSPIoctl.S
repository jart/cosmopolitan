#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSANSPIoctl,WSANSPIoctl,85

	.text.windows
WSANSPIoctl:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSANSPIoctl(%rip),%rax
	jmp	__sysv2nt8
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	WSANSPIoctl,globl
	.previous
