#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSAAsyncGetHostByAddr,WSAAsyncGetHostByAddr,102

#ifdef __x86_64__
	.text.windows
WSAAsyncGetHostByAddr:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSAAsyncGetHostByAddr(%rip),%rax
	jmp	__sysv2nt8
	.endfn	WSAAsyncGetHostByAddr,globl
	.previous
#endif
