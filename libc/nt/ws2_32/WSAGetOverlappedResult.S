#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSAGetOverlappedResult,WSAGetOverlappedResult,68

#ifdef __x86_64__
	.text.windows
WSAGetOverlappedResult:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSAGetOverlappedResult(%rip),%rax
	jmp	__sysv2nt6
	.endfn	WSAGetOverlappedResult,globl
	.previous
#endif
