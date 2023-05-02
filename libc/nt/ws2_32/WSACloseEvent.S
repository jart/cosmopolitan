#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSACloseEvent,WSACloseEvent,45

#ifdef __x86_64__
	.text.windows
WSACloseEvent:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_WSACloseEvent(%rip)
	leave
	ret
	.endfn	WSACloseEvent,globl
	.previous
#endif
