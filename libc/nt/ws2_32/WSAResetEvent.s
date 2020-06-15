.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_WSAResetEvent,WSAResetEvent,95

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
