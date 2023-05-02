#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSAResetEvent,WSAResetEvent,95

#ifdef __x86_64__
	.text.windows
WSAResetEvent:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_WSAResetEvent(%rip)
	leave
	ret
	.endfn	WSAResetEvent,globl
	.previous
#endif
