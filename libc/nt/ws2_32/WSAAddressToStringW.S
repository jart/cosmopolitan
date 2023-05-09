#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSAAddressToStringW,WSAAddressToStringW,43

	.text.windows
WSAAddressToString:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSAAddressToStringW(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	WSAAddressToString,globl
	.previous
