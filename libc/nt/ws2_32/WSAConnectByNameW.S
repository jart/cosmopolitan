#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSAConnectByNameW,WSAConnectByNameW,49

	.text.windows
WSAConnectByName:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSAConnectByNameW(%rip),%rax
	jmp	__sysv2nt10
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	WSAConnectByName,globl
	.previous
