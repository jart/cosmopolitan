#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSAConnectByNameW,WSAConnectByNameW,49

#ifdef __x86_64__
	.text.windows
WSAConnectByName:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSAConnectByNameW(%rip),%rax
	jmp	__sysv2nt10
	.endfn	WSAConnectByName,globl
	.previous
#endif
