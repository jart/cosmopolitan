#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSACleanup,WSACleanup,116

#ifdef __x86_64__
	.text.windows
WSACleanup:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_WSACleanup(%rip)
	leave
	ret
	.endfn	WSACleanup,globl
	.previous
#endif
