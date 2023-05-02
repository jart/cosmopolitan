#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSASetEvent,WSASetEvent,100

#ifdef __x86_64__
	.text.windows
WSASetEvent:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_WSASetEvent(%rip)
	leave
	ret
	.endfn	WSASetEvent,globl
	.previous
#endif
