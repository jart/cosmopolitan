#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSAGetServiceClassInfoW,WSAGetServiceClassInfoW,71

	.text.windows
WSAGetServiceClassInfo:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSAGetServiceClassInfoW(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	WSAGetServiceClassInfo,globl
	.previous
