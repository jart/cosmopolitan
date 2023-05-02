#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSAGetServiceClassInfoW,WSAGetServiceClassInfoW,71

#ifdef __x86_64__
	.text.windows
WSAGetServiceClassInfo:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSAGetServiceClassInfoW(%rip),%rax
	jmp	__sysv2nt
	.endfn	WSAGetServiceClassInfo,globl
	.previous
#endif
