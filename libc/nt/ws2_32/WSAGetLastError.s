.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_WSAGetLastError,WSAGetLastError,111

	.text.windows
WSAGetLastError:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_WSAGetLastError(%rip)
	leave
	ret
	.endfn	WSAGetLastError,globl
	.previous
