.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_WSACloseEvent,WSACloseEvent,45

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
