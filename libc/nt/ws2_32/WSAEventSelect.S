#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSAEventSelect,WSAEventSelect,67

	.text.windows
WSAEventSelect:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSAEventSelect(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	WSAEventSelect,globl
	.previous
