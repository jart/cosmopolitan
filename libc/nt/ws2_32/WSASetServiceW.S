#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSASetServiceW,WSASetServiceW,118

#ifdef __x86_64__
	.text.windows
WSASetService:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSASetServiceW(%rip),%rax
	jmp	__sysv2nt
	.endfn	WSASetService,globl
	.previous
#endif
