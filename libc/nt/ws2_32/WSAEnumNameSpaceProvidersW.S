#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSAEnumNameSpaceProvidersW,WSAEnumNameSpaceProvidersW,63

#ifdef __x86_64__
	.text.windows
WSAEnumNameSpaceProviders:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSAEnumNameSpaceProvidersW(%rip),%rax
	jmp	__sysv2nt
	.endfn	WSAEnumNameSpaceProviders,globl
	.previous
#endif
