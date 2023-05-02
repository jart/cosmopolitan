#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSAStartup,WSAStartup,115

#ifdef __x86_64__
	.text.windows
WSAStartup:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSAStartup(%rip),%rax
	jmp	__sysv2nt
	.endfn	WSAStartup,globl
	.previous
#endif
