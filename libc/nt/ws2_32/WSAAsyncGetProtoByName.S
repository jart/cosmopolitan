#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSAAsyncGetProtoByName,WSAAsyncGetProtoByName,105

#ifdef __x86_64__
	.text.windows
WSAAsyncGetProtoByName:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSAAsyncGetProtoByName(%rip),%rax
	jmp	__sysv2nt6
	.endfn	WSAAsyncGetProtoByName,globl
	.previous
#endif
