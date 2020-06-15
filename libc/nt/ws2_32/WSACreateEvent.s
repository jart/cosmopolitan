.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_WSACreateEvent,WSACreateEvent,50

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
