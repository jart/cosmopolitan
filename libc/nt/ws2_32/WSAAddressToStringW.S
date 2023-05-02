#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSAAddressToStringW,WSAAddressToStringW,43

#ifdef __x86_64__
	.text.windows
WSAAddressToString:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSAAddressToStringW(%rip),%rax
	jmp	__sysv2nt6
	.endfn	WSAAddressToString,globl
	.previous
#endif
