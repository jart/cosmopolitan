#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSACleanup,WSACleanup,116

	.text.windows
WSACleanup:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_WSACleanup(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	WSACleanup,globl
	.previous
