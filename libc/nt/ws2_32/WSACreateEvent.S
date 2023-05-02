#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSACreateEvent,WSACreateEvent,50

#ifdef __x86_64__
	.text.windows
WSACreateEvent:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_WSACreateEvent(%rip)
	leave
	ret
	.endfn	WSACreateEvent,globl
	.previous
#endif
