.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_WSASetEvent,WSASetEvent,100

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
