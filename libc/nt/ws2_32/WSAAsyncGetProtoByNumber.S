#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSAAsyncGetProtoByNumber,WSAAsyncGetProtoByNumber,104

	.text.windows
WSAAsyncGetProtoByNumber:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSAAsyncGetProtoByNumber(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	WSAAsyncGetProtoByNumber,globl
	.previous
