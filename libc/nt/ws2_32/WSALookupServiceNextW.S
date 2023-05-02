#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSALookupServiceNextW,WSALookupServiceNextW,84

#ifdef __x86_64__
	.text.windows
WSALookupServiceNext:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSALookupServiceNextW(%rip),%rax
	jmp	__sysv2nt
	.endfn	WSALookupServiceNext,globl
	.previous
#endif
