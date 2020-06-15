.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_WSAInstallServiceClassA,WSAInstallServiceClassA,76

	.text.windows
WSAInstallServiceClassA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_WSAInstallServiceClassA(%rip)
	leave
	ret
	.endfn	WSAInstallServiceClassA,globl
	.previous
