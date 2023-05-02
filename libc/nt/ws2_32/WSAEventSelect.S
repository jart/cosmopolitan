#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSAEventSelect,WSAEventSelect,67

#ifdef __x86_64__
	.text.windows
WSAEventSelect:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSAEventSelect(%rip),%rax
	jmp	__sysv2nt
	.endfn	WSAEventSelect,globl
	.previous
#endif
