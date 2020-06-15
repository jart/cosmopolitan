.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_WSACleanup,WSACleanup,116

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
