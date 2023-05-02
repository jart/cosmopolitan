#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSALookupServiceBeginW,WSALookupServiceBeginW,81

#ifdef __x86_64__
	.text.windows
WSALookupServiceBegin:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSALookupServiceBeginW(%rip),%rax
	jmp	__sysv2nt
	.endfn	WSALookupServiceBegin,globl
	.previous
#endif
