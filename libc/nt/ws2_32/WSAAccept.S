#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSAAccept,WSAAccept,41

	.text.windows
WSAAccept:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSAAccept(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	WSAAccept,globl
	.previous
