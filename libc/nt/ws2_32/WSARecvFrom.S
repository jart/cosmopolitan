#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSARecvFrom,WSARecvFrom,93

	.text.windows
__WSARecvFrom:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSARecvFrom(%rip),%rax
	jmp	__sysv2nt10
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	__WSARecvFrom,globl
	.previous
