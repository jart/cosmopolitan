#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSAEnumNetworkEvents,WSAEnumNetworkEvents,64

#ifdef __x86_64__
	.text.windows
WSAEnumNetworkEvents:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSAEnumNetworkEvents(%rip),%rax
	jmp	__sysv2nt
	.endfn	WSAEnumNetworkEvents,globl
	.previous
#endif
